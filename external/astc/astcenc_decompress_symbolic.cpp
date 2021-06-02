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
 * @brief Functions to decompress a symbolic block.
 */

#include "astcenc_internal.h"

#include <stdio.h>
#include <assert.h>

/**
 * @brief Compute a vector of texel weights by interpolating the decimated weight grid.
 *
 * @param base_texel_index   The first texel to get; N (SIMD width) consecutive texels are loaded.
 * @param di                 The weight grid decimation to use.
 * @param weights            The raw weights.
 *
 * @return The undecimated weight for N (SIMD width) texels.
 */
static vint compute_value_of_texel_weight_int_vla(
	int base_texel_index,
	const decimation_info& di,
	const int* weights
) {
	vint summed_value(8);
	vint weight_count(di.texel_weight_count + base_texel_index);
	int max_weight_count = hmax(weight_count).lane<0>();

	promise(max_weight_count > 0);
	for (int i = 0; i < max_weight_count; i++)
	{
		vint texel_weights(di.texel_weights_4t[i] + base_texel_index);
		vint texel_weights_int(di.texel_weights_int_4t[i] + base_texel_index);

		summed_value += gatheri(weights, texel_weights) * texel_weights_int;
	}

	return lsr<4>(summed_value);
}

/**
 * @brief Compute the integer linear interpolation of two color endpoints.
 *
 * @param decode_mode   The ASTC profile (linear or sRGB)
 * @param color0        The endpoint0 color.
 * @param color1        The endpoint1 color.
 * @param weight_plane1 The interpolation weight (between 0 and 64) for plane 1.
 * @param weight_plane2 The interpolation weight (between 0 and 64) for plane 2.
 * @param plane2_mask   The mask pattern for the plane assignment (set = plane 2).
 *
 * @return The interpolated color.
 */
static vint4 lerp_color_int(
	astcenc_profile decode_mode,
	vint4 color0,
	vint4 color1,
	int weight_plane1,
	int weight_plane2,
	vmask4 plane2_mask
) {
	vint4 weight1 = select(vint4(weight_plane1), vint4(weight_plane2), plane2_mask);
	vint4 weight0 = vint4(64) - weight1;

	if (decode_mode == ASTCENC_PRF_LDR_SRGB)
	{
		color0 = asr<8>(color0);
		color1 = asr<8>(color1);
	}

	vint4 color = (color0 * weight0) + (color1 * weight1) + vint4(32);
	color = asr<6>(color);

	if (decode_mode == ASTCENC_PRF_LDR_SRGB)
	{
		color = color * vint4(257);
	}

	return color;
}

/**
 * @brief Convert integer color value into a float value for the decoder.
 *
 * @param data       The integer color value post-interpolation.
 * @param lns_mask   If set treat lane as HDR (LNS) else LDR (unorm16).
 *
 * @return The float color value.
 */
static inline vfloat4 decode_texel(
	vint4 data,
	vmask4 lns_mask
) {
	vint4 color_lns = vint4::zero();
	vint4 color_unorm = vint4::zero();

	// TODO: Why bounce this via fp16 first?
	if (any(lns_mask))
	{
		color_lns = lns_to_sf16(data);
	}

	if (!all(lns_mask))
	{
		color_unorm = unorm16_to_sf16(data);
	}

	// Pick components and then convert to FP16
	vint4 datai = select(color_unorm, color_lns, lns_mask);
	return float16_to_float(datai);
}

/* See header for documentation. */
void unpack_weights(
	const block_size_descriptor& bsd,
	const symbolic_compressed_block& scb,
	const decimation_info& di,
	bool is_dual_plane,
	quant_method quant_level,
	int weights_plane1[BLOCK_MAX_TEXELS],
	int weights_plane2[BLOCK_MAX_TEXELS]
) {
	// First, unquantize the weights ...
	int uq_plane1_weights[BLOCK_MAX_WEIGHTS];
	int uq_plane2_weights[BLOCK_MAX_WEIGHTS];
	unsigned int weight_count = di.weight_count;

	const quantization_and_transfer_table *qat = &(quant_and_xfer_tables[quant_level]);

	// Second, undecimate the weights ...
	// Safe to overshoot as all arrays are allocated to full size
	if (!is_dual_plane)
	{
		for (unsigned int i = 0; i < weight_count; i++)
		{
			uq_plane1_weights[i] = qat->unquantized_value[scb.weights[i]];
		}

		for (unsigned int i = 0; i < bsd.texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			store(compute_value_of_texel_weight_int_vla(i, di, uq_plane1_weights), weights_plane1 + i);
		}
	}
	else
	{
		for (unsigned int i = 0; i < weight_count; i++)
		{
			uq_plane1_weights[i] = qat->unquantized_value[scb.weights[i]];
			uq_plane2_weights[i] = qat->unquantized_value[scb.weights[i + WEIGHTS_PLANE2_OFFSET]];
		}

		for (unsigned int i = 0; i < bsd.texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			store(compute_value_of_texel_weight_int_vla(i, di, uq_plane1_weights), weights_plane1 + i);
			store(compute_value_of_texel_weight_int_vla(i, di, uq_plane2_weights), weights_plane2 + i);
		}
	}
}

/* See header for documentation. */
void decompress_symbolic_block(
	astcenc_profile decode_mode,
	const block_size_descriptor& bsd,
	int xpos,
	int ypos,
	int zpos,
	const symbolic_compressed_block& scb,
	image_block& blk
) {
	blk.xpos = xpos;
	blk.ypos = ypos;
	blk.zpos = zpos;

	blk.data_min = vfloat4::zero();
	blk.data_max = vfloat4::zero();
	blk.grayscale = false;

	// If we detected an error-block, blow up immediately.
	if (scb.block_type == SYM_BTYPE_ERROR)
	{
		for (unsigned int i = 0; i < bsd.texel_count; i++)
		{
			blk.data_r[i] = std::numeric_limits<float>::quiet_NaN();
			blk.data_g[i] = std::numeric_limits<float>::quiet_NaN();
			blk.data_b[i] = std::numeric_limits<float>::quiet_NaN();
			blk.data_a[i] = std::numeric_limits<float>::quiet_NaN();
			blk.rgb_lns[i] = 0;
			blk.alpha_lns[i] = 0;
		}

		return;
	}

	if ((scb.block_type == SYM_BTYPE_CONST_F16) ||
	    (scb.block_type == SYM_BTYPE_CONST_U16))
	{
		vfloat4 color;
		int use_lns = 0;

		// UNORM16 constant color block
		if (scb.block_type == SYM_BTYPE_CONST_U16)
		{
			vint4 colori(scb.constant_color);

			// For sRGB decoding a real decoder would just use the top 8 bits for color conversion.
			// We don't color convert, so rescale the top 8 bits into the full 16 bit dynamic range.
			if (decode_mode == ASTCENC_PRF_LDR_SRGB)
			{
				colori = asr<8>(colori) * 257;
			}

			vint4 colorf16 = unorm16_to_sf16(colori);
			color = float16_to_float(colorf16);
		}
		// FLOAT16 constant color block
		else
		{
			switch (decode_mode)
			{
			case ASTCENC_PRF_LDR_SRGB:
			case ASTCENC_PRF_LDR:
				color = vfloat4(std::numeric_limits<float>::quiet_NaN());
				break;
			case ASTCENC_PRF_HDR_RGB_LDR_A:
			case ASTCENC_PRF_HDR:
				// Constant-color block; unpack from FP16 to FP32.
				color = float16_to_float(vint4(scb.constant_color));
				use_lns = 1;
				break;
			}
		}

		// TODO: Skip this and add constant color transfer to img block?
		for (unsigned int i = 0; i < bsd.texel_count; i++)
		{
			blk.data_r[i] = color.lane<0>();
			blk.data_g[i] = color.lane<1>();
			blk.data_b[i] = color.lane<2>();
			blk.data_a[i] = color.lane<3>();
			blk.rgb_lns[i] = use_lns;
			blk.alpha_lns[i] = use_lns;
		}

		return;
	}

	// Get the appropriate partition-table entry
	int partition_count = scb.partition_count;
	const auto& pi = bsd.get_partition_info(partition_count, scb.partition_index);

	// Get the appropriate block descriptors
	const auto& bm = bsd.get_block_mode(scb.block_mode);
	const auto& di = bsd.get_decimation_info(bm.decimation_mode);

	int is_dual_plane = bm.is_dual_plane;

	// Unquantize and undecimate the weights
	int weights[BLOCK_MAX_TEXELS];
	int plane2_weights[BLOCK_MAX_TEXELS];
	unpack_weights(bsd, scb, di, is_dual_plane, bm.get_weight_quant_mode(), weights, plane2_weights);

	// Now that we have endpoint colors and weights, we can unpack texel colors
	int plane2_component = is_dual_plane ? scb.plane2_component : -1;
	vmask4 plane2_mask = vint4::lane_id() == vint4(plane2_component);

	for (int i = 0; i < partition_count; i++)
	{
		// Decode the color endpoints for this partition
		vint4 ep0;
		vint4 ep1;
		bool rgb_lns;
		bool a_lns;

		unpack_color_endpoints(decode_mode,
		                       scb.color_formats[i],
		                       scb.get_color_quant_mode(),
		                       scb.color_values[i],
		                       rgb_lns, a_lns,
		                       ep0, ep1);

		vmask4 lns_mask(rgb_lns, rgb_lns, rgb_lns, a_lns);

		int texel_count = pi.partition_texel_count[i];
		for (int j = 0; j < texel_count; j++)
		{
			int tix = pi.texels_of_partition[i][j];
			vint4 color = lerp_color_int(decode_mode,
			                             ep0,
			                             ep1,
			                             weights[tix],
			                             plane2_weights[tix],
			                             plane2_mask);

			vfloat4 colorf = decode_texel(color, lns_mask);

			blk.data_r[tix] = colorf.lane<0>();
			blk.data_g[tix] = colorf.lane<1>();
			blk.data_b[tix] = colorf.lane<2>();
			blk.data_a[tix] = colorf.lane<3>();
		}
	}
}

#if !defined(ASTCENC_DECOMPRESS_ONLY)

/* See header for documentation. */
float compute_symbolic_block_difference(
	const astcenc_config& config,
	const block_size_descriptor& bsd,
	const symbolic_compressed_block& scb,
	const image_block& blk,
	const error_weight_block& ewb
) {
	// If we detected an error-block, blow up immediately.
	if (scb.block_type == SYM_BTYPE_ERROR)
	{
		return 1e29f;
	}

	assert(scb.block_mode >= 0);

	// Get the appropriate partition-table entry
	int partition_count = scb.partition_count;
	const auto& pi = bsd.get_partition_info(partition_count, scb.partition_index);

	// Get the appropriate block descriptor
	const block_mode& bm = bsd.get_block_mode(scb.block_mode);
	const decimation_info& di = *(bsd.decimation_tables[bm.decimation_mode]);

	bool is_dual_plane = bm.is_dual_plane != 0;

	// Unquantize and undecimate the weights
	int weights[BLOCK_MAX_TEXELS];
	int plane2_weights[BLOCK_MAX_TEXELS];
	unpack_weights(bsd, scb, di, is_dual_plane, bm.get_weight_quant_mode(), weights, plane2_weights);

	int plane2_component = is_dual_plane ? scb.plane2_component : -1;
	vmask4 plane2_mask = vint4::lane_id() == vint4(plane2_component);

	float summa = 0.0f;
	for (int i = 0; i < partition_count; i++)
	{
		// Decode the color endpoints for this partition
		vint4 ep0;
		vint4 ep1;
		bool rgb_lns;
		bool a_lns;

		unpack_color_endpoints(config.profile,
		                       scb.color_formats[i],
		                       scb.get_color_quant_mode(),
		                       scb.color_values[i],
		                       rgb_lns, a_lns,
		                       ep0, ep1);

		vmask4 lns_mask(rgb_lns, rgb_lns, rgb_lns, a_lns);

		// Unpack and compute error for each texel in the partition
		int texel_count = pi.partition_texel_count[i];
		for (int j = 0; j < texel_count; j++)
		{
			int tix = pi.texels_of_partition[i][j];
			vint4 colori = lerp_color_int(config.profile,
			                              ep0, ep1,
			                              weights[tix],
			                              plane2_weights[tix], plane2_mask);

			vfloat4 color = int_to_float(colori);
			vfloat4 oldColor = blk.texel(tix);

			// Compare error using a perceptual decode metric for RGBM textures
			if (config.flags & ASTCENC_FLG_MAP_RGBM)
			{
				// Fail encodings that result in zero weight M pixels. Note that this can cause
				// "interesting" artifacts if we reject all useful encodings - we typically get max
				// brightness encodings instead which look just as bad. We recommend users apply a
				// bias to their stored M value, limiting the lower value to 16 or 32 to avoid
				// getting small M values post-quantization, but we can't prove it would never
				// happen, especially at low bit rates ...
				if (color.lane<3>() == 0.0f)
				{
					return -1e30f;
				}

				// Compute error based on decoded RGBM color
				color = vfloat4(
					color.lane<0>() * color.lane<3>() * config.rgbm_m_scale,
					color.lane<1>() * color.lane<3>() * config.rgbm_m_scale,
					color.lane<2>() * color.lane<3>() * config.rgbm_m_scale,
					1.0f
				);

				oldColor = vfloat4(
					oldColor.lane<0>() * oldColor.lane<3>() * config.rgbm_m_scale,
					oldColor.lane<1>() * oldColor.lane<3>() * config.rgbm_m_scale,
					oldColor.lane<2>() * oldColor.lane<3>() * config.rgbm_m_scale,
					1.0f
				);
			}

			vfloat4 error = oldColor - color;
			error = min(abs(error), 1e15f);
			error = error * error;

			float metric = dot_s(error, ewb.error_weights[tix]);
			summa += astc::min(metric, 1e30f);
		}
	}

	return summa;
}

#endif
