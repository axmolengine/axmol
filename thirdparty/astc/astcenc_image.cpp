// SPDX-License-Identifier: Apache-2.0
// ----------------------------------------------------------------------------
// Copyright 2011-2021 Arm Limited
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at:
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------------------------------------------------------

/**
 * @brief Functions for creating in-memory ASTC image structures.
 */

#include <cassert>
#include <cstring>

#include "astcenc_internal.h"

/**
 * @brief Loader pipeline function type for data fetch from memory.
 */
using pixel_loader = vfloat4(*)(const void*, int);

/**
 * @brief Loader pipeline function type for swizzling data in a vector.
 */
using pixel_swizzler = vfloat4(*)(vfloat4, const astcenc_swizzle&);

/**
 * @brief Loader pipeline function type for converting data in a vector to LNS.
 */
using pixel_converter = vfloat4(*)(vfloat4, vmask4);

/**
 * @brief Load a 8-bit UNORM texel from a data array.
 *
 * @param data          The data pointer.
 * @param base_offset   The index offset to the start of the pixel.
 */
static vfloat4 load_texel_u8(
	const void* data,
	int base_offset
) {
	const uint8_t* data8 = static_cast<const uint8_t*>(data);
	return int_to_float(vint4(data8 + base_offset)) / 255.0f;
}

/**
 * @brief Load a 16-bit fp16 texel from a data array.
 *
 * @param data          The data pointer.
 * @param base_offset   The index offset to the start of the pixel.
 */
static vfloat4 load_texel_f16(
	const void* data,
	int base_offset
) {
	const uint16_t* data16 = static_cast<const uint16_t*>(data);
	int r = data16[base_offset    ];
	int g = data16[base_offset + 1];
	int b = data16[base_offset + 2];
	int a = data16[base_offset + 3];
	return float16_to_float(vint4(r, g, b, a));
}

/**
 * @brief Load a 32-bit float texel from a data array.
 *
 * @param data          The data pointer.
 * @param base_offset   The index offset to the start of the pixel.
 */
static vfloat4 load_texel_f32(
	const void* data,
	int base_offset
) {
	const float* data32 = static_cast<const float*>(data);
	return vfloat4(data32 + base_offset);
}

/**
 * @brief Dummy no-op swizzle function.
 *
 * @param data   The source RGBA vector to swizzle.
 * @param swz    The swizzle to use.
 */
static vfloat4 swz_texel_skip(
	vfloat4 data,
	const astcenc_swizzle& swz
) {
	(void)swz;
	return data;
}

/**
 * @brief Swizzle a texel into a new arrangement.
 *
 * @param data   The source RGBA vector to swizzle.
 * @param swz    The swizzle to use.
 */
static vfloat4 swz_texel(
	vfloat4 data,
	const astcenc_swizzle& swz
) {
	alignas(16) float datas[6];

	storea(data, datas);
	datas[ASTCENC_SWZ_0] = 0.0f;
	datas[ASTCENC_SWZ_1] = 1.0f;

	return vfloat4(datas[swz.r], datas[swz.g], datas[swz.b], datas[swz.a]);
}

/**
 * @brief Encode a texel that is entirely LDR linear.
 *
 * @param data       The RGBA data to encode.
 * @param lns_mask   The mask for the HDR channels than need LNS encoding.
 */
static vfloat4 encode_texel_unorm(
	vfloat4 data,
	vmask4 lns_mask
) {
	(void)lns_mask;
	return data * 65535.0f;
}

/**
 * @brief Encode a texel that includes at least some HDR LNS texels.
 *
 * @param data       The RGBA data to encode.
 * @param lns_mask   The mask for the HDR channels than need LNS encoding.
 */
static vfloat4 encode_texel_lns(
	vfloat4 data,
	vmask4 lns_mask
) {
	vfloat4 datav_unorm = data * 65535.0f;
	vfloat4 datav_lns   = float_to_lns(data);
	return select(datav_unorm, datav_lns, lns_mask);
}

/* See header for documentation. */
void fetch_image_block(
	astcenc_profile decode_mode,
	const astcenc_image& img,
	image_block& blk,
	const block_size_descriptor& bsd,
	unsigned int xpos,
	unsigned int ypos,
	unsigned int zpos,
	const astcenc_swizzle& swz
) {
	unsigned int xsize = img.dim_x;
	unsigned int ysize = img.dim_y;
	unsigned int zsize = img.dim_z;

	blk.xpos = xpos;
	blk.ypos = ypos;
	blk.zpos = zpos;

	// True if any non-identity swizzle
	bool needs_swz = (swz.r != ASTCENC_SWZ_R) || (swz.g != ASTCENC_SWZ_G) ||
	                 (swz.b != ASTCENC_SWZ_B) || (swz.a != ASTCENC_SWZ_A);

	int idx = 0;

	vfloat4 data_min(1e38f);
	vfloat4 data_max(-1e38f);
	bool grayscale = true;

	// This works because we impose the same choice everywhere during encode
	uint8_t rgb_lns = (decode_mode == ASTCENC_PRF_HDR) ||
	                  (decode_mode == ASTCENC_PRF_HDR_RGB_LDR_A) ? 1 : 0;
	uint8_t a_lns = decode_mode == ASTCENC_PRF_HDR ? 1 : 0;
	vint4 use_lns(rgb_lns, rgb_lns, rgb_lns, a_lns);
	vmask4 lns_mask = use_lns != vint4::zero();

	// Set up the function pointers for loading pipeline as needed
	pixel_loader loader = load_texel_u8;
	if (img.data_type == ASTCENC_TYPE_F16)
	{
		loader = load_texel_f16;
	}
	else if  (img.data_type == ASTCENC_TYPE_F32)
	{
		loader = load_texel_f32;
	}

	pixel_swizzler swizzler = swz_texel_skip;
	if (needs_swz)
	{
		swizzler = swz_texel;
	}

	pixel_converter converter = encode_texel_unorm;
	if (any(lns_mask))
	{
		converter = encode_texel_lns;
	}

	for (unsigned int z = 0; z < bsd.zdim; z++)
	{
		unsigned int zi = astc::min(zpos + z, zsize - 1);
		void* plane = img.data[zi];

		for (unsigned int y = 0; y < bsd.ydim; y++)
		{
			unsigned int yi = astc::min(ypos + y, ysize - 1);

			for (unsigned int x = 0; x < bsd.xdim; x++)
			{
				unsigned int xi = astc::min(xpos + x, xsize - 1);

				vfloat4 datav = loader(plane, (4 * xsize * yi) + (4 * xi));
				datav = swizzler(datav, swz);
				datav = converter(datav, lns_mask);

				// Compute block metadata
				data_min = min(data_min, datav);
				data_max = max(data_max, datav);

				if (grayscale && (datav.lane<0>() != datav.lane<1>() || datav.lane<0>() != datav.lane<2>()))
				{
					grayscale = false;
				}

				blk.data_r[idx] = datav.lane<0>();
				blk.data_g[idx] = datav.lane<1>();
				blk.data_b[idx] = datav.lane<2>();
				blk.data_a[idx] = datav.lane<3>();

				blk.rgb_lns[idx] = rgb_lns;
				blk.alpha_lns[idx] = a_lns;

				idx++;
			}
		}
	}

	// Reverse the encoding so we store origin block in the original format
	vfloat4 data_enc = blk.texel(0);
	vfloat4 data_enc_unorm = data_enc / 65535.0f;
	vfloat4 data_enc_lns = vfloat4::zero();

	if (rgb_lns || a_lns)
	{
		data_enc_lns = float16_to_float(lns_to_sf16(float_to_int(data_enc)));
	}

	blk.origin_texel = select(data_enc_unorm, data_enc_lns, lns_mask);;

	// Store block metadata
	blk.data_min = data_min;
	blk.data_max = data_max;
	blk.grayscale = grayscale;
}

/* See header for documentation. */
void write_image_block(
	astcenc_image& img,
	const image_block& blk,
	const block_size_descriptor& bsd,
	unsigned int xpos,
	unsigned int ypos,
	unsigned int zpos,
	const astcenc_swizzle& swz
) {
	unsigned int xsize = img.dim_x;
	unsigned int ysize = img.dim_y;
	unsigned int zsize = img.dim_z;

	unsigned int x_start = xpos;
	unsigned int x_end = std::min(xsize, xpos + bsd.xdim);
	unsigned int x_nudge = bsd.xdim - (x_end - x_start);

	unsigned int y_start = ypos;
	unsigned int y_end = std::min(ysize, ypos + bsd.ydim);
	unsigned int y_nudge = (bsd.ydim - (y_end - y_start)) * bsd.xdim;

	unsigned int z_start = zpos;
	unsigned int z_end = std::min(zsize, zpos + bsd.zdim);

	float data[7];
	data[ASTCENC_SWZ_0] = 0.0f;
	data[ASTCENC_SWZ_1] = 1.0f;

	// True if any non-identity swizzle
	bool needs_swz = (swz.r != ASTCENC_SWZ_R) || (swz.g != ASTCENC_SWZ_G) ||
	                 (swz.b != ASTCENC_SWZ_B) || (swz.a != ASTCENC_SWZ_A);

	// True if any swizzle uses Z reconstruct
	bool needs_z = (swz.r == ASTCENC_SWZ_Z) || (swz.g == ASTCENC_SWZ_Z) ||
	               (swz.b == ASTCENC_SWZ_Z) || (swz.a == ASTCENC_SWZ_Z);

	int idx = 0;
	if (img.data_type == ASTCENC_TYPE_U8)
	{
		for (unsigned int z = z_start; z < z_end; z++)
		{
			// Fetch the image plane
			uint8_t* data8 = static_cast<uint8_t*>(img.data[z]);

			for (unsigned int y = y_start; y < y_end; y++)
			{
				for (unsigned int x = x_start; x < x_end; x++)
				{
					vint4 colori = vint4::zero();

					if (blk.data_r[idx] == std::numeric_limits<float>::quiet_NaN())
					{
						// Can't display NaN - show magenta error color
						colori = vint4(0xFF, 0x00, 0xFF, 0xFF);
					}
					else if (needs_swz)
					{
						data[ASTCENC_SWZ_R] = blk.data_r[idx];
						data[ASTCENC_SWZ_G] = blk.data_g[idx];
						data[ASTCENC_SWZ_B] = blk.data_b[idx];
						data[ASTCENC_SWZ_A] = blk.data_a[idx];

						if (needs_z)
						{
							float xcoord = (data[0] * 2.0f) - 1.0f;
							float ycoord = (data[3] * 2.0f) - 1.0f;
							float zcoord = 1.0f - xcoord * xcoord - ycoord * ycoord;
							if (zcoord < 0.0f)
							{
								zcoord = 0.0f;
							}
							data[ASTCENC_SWZ_Z] = (astc::sqrt(zcoord) * 0.5f) + 0.5f;
						}

						vfloat4 color = vfloat4(data[swz.r], data[swz.g], data[swz.b], data[swz.a]);
						colori = float_to_int_rtn(min(color, 1.0f) * 255.0f);
					}
					else
					{
						vfloat4 color = blk.texel(idx);
						colori = float_to_int_rtn(min(color, 1.0f) * 255.0f);
					}

					colori = pack_low_bytes(colori);
					store_nbytes(colori, data8 + (4 * xsize * y) + (4 * x    ));

					idx++;
				}
				idx += x_nudge;
			}
			idx += y_nudge;
		}
	}
	else if (img.data_type == ASTCENC_TYPE_F16)
	{
		for (unsigned int z = z_start; z < z_end; z++)
		{
			// Fetch the image plane
			uint16_t* data16 = static_cast<uint16_t*>(img.data[z]);

			for (unsigned int y = y_start; y < y_end; y++)
			{
				for (unsigned int x = x_start; x < x_end; x++)
				{
					vint4 color;

					if (blk.data_r[idx] == std::numeric_limits<float>::quiet_NaN())
					{
						color = vint4(0xFFFF);
					}
					else if (needs_swz)
					{
						data[ASTCENC_SWZ_R] = blk.data_r[idx];
						data[ASTCENC_SWZ_G] = blk.data_g[idx];
						data[ASTCENC_SWZ_B] = blk.data_b[idx];
						data[ASTCENC_SWZ_A] = blk.data_a[idx];

						if (needs_z)
						{
							float xN = (data[0] * 2.0f) - 1.0f;
							float yN = (data[3] * 2.0f) - 1.0f;
							float zN = 1.0f - xN * xN - yN * yN;
							if (zN < 0.0f)
							{
								zN = 0.0f;
							}
							data[ASTCENC_SWZ_Z] = (astc::sqrt(zN) * 0.5f) + 0.5f;
						}

						vfloat4 colorf(data[swz.r], data[swz.g], data[swz.b], data[swz.a]);
						color = float_to_float16(colorf);
					}
					else
					{
						vfloat4 colorf = blk.texel(idx);
						color = float_to_float16(colorf);
					}

					data16[(4 * xsize * y) + (4 * x    )] = (uint16_t)color.lane<0>();
					data16[(4 * xsize * y) + (4 * x + 1)] = (uint16_t)color.lane<1>();
					data16[(4 * xsize * y) + (4 * x + 2)] = (uint16_t)color.lane<2>();
					data16[(4 * xsize * y) + (4 * x + 3)] = (uint16_t)color.lane<3>();

					idx++;
				}
				idx += x_nudge;
			}
			idx += y_nudge;
		}
	}
	else // if (img.data_type == ASTCENC_TYPE_F32)
	{
		assert(img.data_type == ASTCENC_TYPE_F32);

		for (unsigned int z = z_start; z < z_end; z++)
		{
			// Fetch the image plane
			float* data32 = static_cast<float*>(img.data[z]);

			for (unsigned int y = y_start; y < y_end; y++)
			{
				for (unsigned int x = x_start; x < x_end; x++)
				{
					vfloat4 color = blk.texel(idx);

					if (color.lane<0>() == std::numeric_limits<float>::quiet_NaN())
					{
						color = vfloat4(std::numeric_limits<float>::quiet_NaN());
					}
					else if (needs_swz)
					{
						data[ASTCENC_SWZ_R] = color.lane<0>();
						data[ASTCENC_SWZ_G] = color.lane<1>();
						data[ASTCENC_SWZ_B] = color.lane<2>();
						data[ASTCENC_SWZ_A] = color.lane<3>();

						if (needs_z)
						{
							float xN = (data[0] * 2.0f) - 1.0f;
							float yN = (data[3] * 2.0f) - 1.0f;
							float zN = 1.0f - xN * xN - yN * yN;
							if (zN < 0.0f)
							{
								zN = 0.0f;
							}
							data[ASTCENC_SWZ_Z] = (astc::sqrt(zN) * 0.5f) + 0.5f;
						}

						color = vfloat4(data[swz.r], data[swz.g], data[swz.b], data[swz.a]);
					}

					store(color, data32 + (4 * xsize * y) + (4 * x    ));

					idx++;
				}
				idx += x_nudge;
			}
			idx += y_nudge;
		}
	}
}
