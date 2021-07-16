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
 * @brief Functions to generate block size descriptor and decimation tables.
 */

#include "astcenc_internal.h"

/**
 * @brief Decode the properties of an encoded 2D block mode.
 *
 * @param      block_mode      The encoded block mode.
 * @param[out] x_weights       The number of weights in the X dimension.
 * @param[out] y_weights       The number of weights in the Y dimension.
 * @param[out] is_dual_plane   True if this block mode has two weight planes.
 * @param[out] quant_mode      The quantization level for the weights.
 *
 * @return Returns true of valid mode, false otherwise.
 */
static bool decode_block_mode_2d(
	unsigned int block_mode,
	unsigned int& x_weights,
	unsigned int& y_weights,
	bool& is_dual_plane,
	unsigned int& quant_mode
) {
	unsigned int base_quant_mode = (block_mode >> 4) & 1;
	unsigned int H = (block_mode >> 9) & 1;
	unsigned int D = (block_mode >> 10) & 1;
	unsigned int A = (block_mode >> 5) & 0x3;

	x_weights = 0;
	y_weights = 0;

	if ((block_mode & 3) != 0)
	{
		base_quant_mode |= (block_mode & 3) << 1;
		unsigned int B = (block_mode >> 7) & 3;
		switch ((block_mode >> 2) & 3)
		{
		case 0:
			x_weights = B + 4;
			y_weights = A + 2;
			break;
		case 1:
			x_weights = B + 8;
			y_weights = A + 2;
			break;
		case 2:
			x_weights = A + 2;
			y_weights = B + 8;
			break;
		case 3:
			B &= 1;
			if (block_mode & 0x100)
			{
				x_weights = B + 2;
				y_weights = A + 2;
			}
			else
			{
				x_weights = A + 2;
				y_weights = B + 6;
			}
			break;
		}
	}
	else
	{
		base_quant_mode |= ((block_mode >> 2) & 3) << 1;
		if (((block_mode >> 2) & 3) == 0)
		{
			return false;
		}

		unsigned int B = (block_mode >> 9) & 3;
		switch ((block_mode >> 7) & 3)
		{
		case 0:
			x_weights = 12;
			y_weights = A + 2;
			break;
		case 1:
			x_weights = A + 2;
			y_weights = 12;
			break;
		case 2:
			x_weights = A + 6;
			y_weights = B + 6;
			D = 0;
			H = 0;
			break;
		case 3:
			switch ((block_mode >> 5) & 3)
			{
			case 0:
				x_weights = 6;
				y_weights = 10;
				break;
			case 1:
				x_weights = 10;
				y_weights = 6;
				break;
			case 2:
			case 3:
				return false;
			}
			break;
		}
	}

	unsigned int weight_count = x_weights * y_weights * (D + 1);
	quant_mode = (base_quant_mode - 2) + 6 * H;
	is_dual_plane = D != 0;

	unsigned int weight_bits = get_ise_sequence_bitcount(weight_count, (quant_method)quant_mode);
	return (weight_count <= BLOCK_MAX_WEIGHTS &&
	        weight_bits >= BLOCK_MIN_WEIGHT_BITS &&
	        weight_bits <= BLOCK_MAX_WEIGHT_BITS);
}

/**
 * @brief Decode the properties of an encoded 3D block mode.
 *
 * @param      block_mode      The encoded block mode.
 * @param[out] x_weights       The number of weights in the X dimension.
 * @param[out] y_weights       The number of weights in the Y dimension.
 * @param[out] z_weights       The number of weights in the Z dimension.
 * @param[out] is_dual_plane   True if this block mode has two weight planes.
 * @param[out] quant_mode      The quantization level for the weights.
 *
 * @return Returns true of valid mode, false otherwise.
 */
static bool decode_block_mode_3d(
	unsigned int block_mode,
	unsigned int& x_weights,
	unsigned int& y_weights,
	unsigned int& z_weights,
	bool& is_dual_plane,
	unsigned int& quant_mode
) {
	unsigned int base_quant_mode = (block_mode >> 4) & 1;
	unsigned int H = (block_mode >> 9) & 1;
	unsigned int D = (block_mode >> 10) & 1;
	unsigned int A = (block_mode >> 5) & 0x3;

	x_weights = 0;
	y_weights = 0;
	z_weights = 0;

	if ((block_mode & 3) != 0)
	{
		base_quant_mode |= (block_mode & 3) << 1;
		unsigned int B = (block_mode >> 7) & 3;
		unsigned int C = (block_mode >> 2) & 0x3;
		x_weights = A + 2;
		y_weights = B + 2;
		z_weights = C + 2;
	}
	else
	{
		base_quant_mode |= ((block_mode >> 2) & 3) << 1;
		if (((block_mode >> 2) & 3) == 0)
		{
			return false;
		}

		int B = (block_mode >> 9) & 3;
		if (((block_mode >> 7) & 3) != 3)
		{
			D = 0;
			H = 0;
		}
		switch ((block_mode >> 7) & 3)
		{
		case 0:
			x_weights = 6;
			y_weights = B + 2;
			z_weights = A + 2;
			break;
		case 1:
			x_weights = A + 2;
			y_weights = 6;
			z_weights = B + 2;
			break;
		case 2:
			x_weights = A + 2;
			y_weights = B + 2;
			z_weights = 6;
			break;
		case 3:
			x_weights = 2;
			y_weights = 2;
			z_weights = 2;
			switch ((block_mode >> 5) & 3)
			{
			case 0:
				x_weights = 6;
				break;
			case 1:
				y_weights = 6;
				break;
			case 2:
				z_weights = 6;
				break;
			case 3:
				return false;
			}
			break;
		}
	}

	unsigned int weight_count = x_weights * y_weights * z_weights * (D + 1);
	quant_mode = (base_quant_mode - 2) + 6 * H;
	is_dual_plane = D != 0;

	unsigned int weight_bits = get_ise_sequence_bitcount(weight_count, (quant_method)quant_mode);
	return (weight_count <= BLOCK_MAX_WEIGHTS &&
	        weight_bits >= BLOCK_MIN_WEIGHT_BITS &&
	        weight_bits <= BLOCK_MAX_WEIGHT_BITS);
}

/**
 * @brief Create a 2D decimation entry for a block-size and weight-decimation pair.
 *
 * @param      x_texels    The number of texels in the X dimension.
 * @param      y_texels    The number of texels in the Y dimension.
 * @param      x_weights   The number of weights in the X dimension.
 * @param      y_weights   The number of weights in the Y dimension.
 * @param[out] di          The decimation info structure to populate.
 */
static void init_decimation_info_2d(
	unsigned int x_texels,
	unsigned int y_texels,
	unsigned int x_weights,
	unsigned int y_weights,
	decimation_info& di
) {
	unsigned int texels_per_block = x_texels * y_texels;
	unsigned int weights_per_block = x_weights * y_weights;

	uint8_t weight_count_of_texel[BLOCK_MAX_TEXELS];
	uint8_t grid_weights_of_texel[BLOCK_MAX_TEXELS][4];
	uint8_t weights_of_texel[BLOCK_MAX_TEXELS][4];

	uint8_t texel_count_of_weight[BLOCK_MAX_WEIGHTS];
	uint8_t max_texel_count_of_weight = 0;
	uint8_t texels_of_weight[BLOCK_MAX_WEIGHTS][BLOCK_MAX_TEXELS];
	uint8_t texel_weights_of_weight[BLOCK_MAX_WEIGHTS][BLOCK_MAX_TEXELS];

	promise(weights_per_block > 0);
	promise(texels_per_block > 0);
	promise(x_texels > 0);
	promise(y_texels > 0);

	for (unsigned int i = 0; i < weights_per_block; i++)
	{
		texel_count_of_weight[i] = 0;
	}

	for (unsigned int i = 0; i < texels_per_block; i++)
	{
		weight_count_of_texel[i] = 0;
	}

	for (unsigned int y = 0; y < y_texels; y++)
	{
		for (unsigned int x = 0; x < x_texels; x++)
		{
			unsigned int texel = y * x_texels + x;

			unsigned int x_weight = (((1024 + x_texels / 2) / (x_texels - 1)) * x * (x_weights - 1) + 32) >> 6;
			unsigned int y_weight = (((1024 + y_texels / 2) / (y_texels - 1)) * y * (y_weights - 1) + 32) >> 6;

			unsigned int x_weight_frac = x_weight & 0xF;
			unsigned int y_weight_frac = y_weight & 0xF;
			unsigned int x_weight_int = x_weight >> 4;
			unsigned int y_weight_int = y_weight >> 4;

			unsigned int qweight[4];
			qweight[0] = x_weight_int + y_weight_int * x_weights;
			qweight[1] = qweight[0] + 1;
			qweight[2] = qweight[0] + x_weights;
			qweight[3] = qweight[2] + 1;

			// Truncated-precision bilinear interpolation
			unsigned int prod = x_weight_frac * y_weight_frac;

			unsigned int weight[4];
			weight[3] = (prod + 8) >> 4;
			weight[1] = x_weight_frac - weight[3];
			weight[2] = y_weight_frac - weight[3];
			weight[0] = 16 - x_weight_frac - y_weight_frac + weight[3];

			for (unsigned int i = 0; i < 4; i++)
			{
				if (weight[i] != 0)
				{
					grid_weights_of_texel[texel][weight_count_of_texel[texel]] = static_cast<uint8_t>(qweight[i]);
					weights_of_texel[texel][weight_count_of_texel[texel]] = static_cast<uint8_t>(weight[i]);
					weight_count_of_texel[texel]++;
					texels_of_weight[qweight[i]][texel_count_of_weight[qweight[i]]] = static_cast<uint8_t>(texel);
					texel_weights_of_weight[qweight[i]][texel_count_of_weight[qweight[i]]] = static_cast<uint8_t>(weight[i]);
					texel_count_of_weight[qweight[i]]++;
					max_texel_count_of_weight = astc::max(max_texel_count_of_weight, texel_count_of_weight[qweight[i]]);
				}
			}
		}
	}

	for (unsigned int i = 0; i < texels_per_block; i++)
	{
		di.texel_weight_count[i] = weight_count_of_texel[i];

		for (unsigned int j = 0; j < weight_count_of_texel[i]; j++)
		{
			di.texel_weights_int_4t[j][i] = weights_of_texel[i][j];
			di.texel_weights_float_4t[j][i] = ((float)weights_of_texel[i][j]) * (1.0f / WEIGHTS_TEXEL_SUM);
			di.texel_weights_4t[j][i] = grid_weights_of_texel[i][j];
		}

		// Init all 4 entries so we can rely on zeros for vectorization
		for (unsigned int j = weight_count_of_texel[i]; j < 4; j++)
		{
			di.texel_weights_int_4t[j][i] = 0;
			di.texel_weights_float_4t[j][i] = 0.0f;
			di.texel_weights_4t[j][i] = 0;
		}
	}

	for (unsigned int i = 0; i < weights_per_block; i++)
	{
		unsigned int texel_count_wt = texel_count_of_weight[i];
		di.weight_texel_count[i] = (uint8_t)texel_count_wt;

		for (unsigned int j = 0; j < texel_count_wt; j++)
		{
			uint8_t texel = texels_of_weight[i][j];

			// Create transposed versions of these for better vectorization
			di.weight_texel[j][i] = texel;
			di.weights_flt[j][i] = (float)texel_weights_of_weight[i][j];

			// perform a layer of array unrolling. An aspect of this unrolling is that
			// one of the texel-weight indexes is an identity-mapped index; we will use this
			// fact to reorder the indexes so that the first one is the identity index.
			int swap_idx = -1;
			for (unsigned int k = 0; k < 4; k++)
			{
				uint8_t dttw = di.texel_weights_4t[k][texel];
				float dttwf = di.texel_weights_float_4t[k][texel];
				if (dttw == i && dttwf != 0.0f)
				{
					swap_idx = k;
				}
				di.texel_weights_texel[i][j][k] = dttw;
				di.texel_weights_float_texel[i][j][k] = dttwf;
			}

			if (swap_idx != 0)
			{
				uint8_t vi = di.texel_weights_texel[i][j][0];
				float vf = di.texel_weights_float_texel[i][j][0];
				di.texel_weights_texel[i][j][0] = di.texel_weights_texel[i][j][swap_idx];
				di.texel_weights_float_texel[i][j][0] = di.texel_weights_float_texel[i][j][swap_idx];
				di.texel_weights_texel[i][j][swap_idx] = vi;
				di.texel_weights_float_texel[i][j][swap_idx] = vf;
			}
		}

		// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
		// Match last texel in active lane in SIMD group, for better gathers
		uint8_t last_texel = di.weight_texel[texel_count_wt - 1][i];
		for (unsigned int j = texel_count_wt; j < max_texel_count_of_weight; j++)
		{
			di.weight_texel[j][i] = last_texel;
			di.weights_flt[j][i] = 0.0f;
		}
	}

	// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
	unsigned int texels_per_block_simd = round_up_to_simd_multiple_vla(texels_per_block);
	for (unsigned int i = texels_per_block; i < texels_per_block_simd; i++)
	{
		di.texel_weight_count[i] = 0;

		for (unsigned int j = 0; j < 4; j++)
		{
			di.texel_weights_float_4t[j][i] = 0;
			di.texel_weights_4t[j][i] = 0;
			di.texel_weights_int_4t[j][i] = 0;
		}
	}

	// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
	// Match last texel in active lane in SIMD group, for better gathers
	unsigned int last_texel_count_wt = texel_count_of_weight[weights_per_block - 1];
	uint8_t last_texel = di.weight_texel[last_texel_count_wt - 1][weights_per_block - 1];

	unsigned int weights_per_block_simd = round_up_to_simd_multiple_vla(weights_per_block);
	for (unsigned int i = weights_per_block; i < weights_per_block_simd; i++)
	{
		di.weight_texel_count[i] = 0;

		for (unsigned int j = 0; j < max_texel_count_of_weight; j++)
		{
			di.weight_texel[j][i] = last_texel;
			di.weights_flt[j][i] = 0.0f;
		}
	}

	di.texel_count = static_cast<uint8_t>(texels_per_block);
	di.weight_count = static_cast<uint8_t>(weights_per_block);
	di.weight_x = static_cast<uint8_t>(x_weights);
	di.weight_y = static_cast<uint8_t>(y_weights);
	di.weight_z = 1;
}

/**
 * @brief Create a 3D decimation entry for a block-size and weight-decimation pair.
 *
 * @param      x_texels    The number of texels in the X dimension.
 * @param      y_texels    The number of texels in the Y dimension.
 * @param      z_texels    The number of texels in the Z dimension.
 * @param      x_weights   The number of weights in the X dimension.
 * @param      y_weights   The number of weights in the Y dimension.
 * @param      z_weights   The number of weights in the Z dimension.
 * @param[out] di          The decimation info structure to populate.
 */
static void init_decimation_info_3d(
	unsigned int x_texels,
	unsigned int y_texels,
	unsigned int z_texels,
	unsigned int x_weights,
	unsigned int y_weights,
	unsigned int z_weights,
	decimation_info& di
) {
	unsigned int texels_per_block = x_texels * y_texels * z_texels;
	unsigned int weights_per_block = x_weights * y_weights * z_weights;

	uint8_t weight_count_of_texel[BLOCK_MAX_TEXELS];
	uint8_t grid_weights_of_texel[BLOCK_MAX_TEXELS][4];
	uint8_t weights_of_texel[BLOCK_MAX_TEXELS][4];

	uint8_t texel_count_of_weight[BLOCK_MAX_WEIGHTS];
	uint8_t max_texel_count_of_weight = 0;
	uint8_t texels_of_weight[BLOCK_MAX_WEIGHTS][BLOCK_MAX_TEXELS];
	uint8_t texel_weights_of_weight[BLOCK_MAX_WEIGHTS][BLOCK_MAX_TEXELS];

	promise(weights_per_block > 0);
	promise(texels_per_block > 0);

	for (unsigned int i = 0; i < weights_per_block; i++)
	{
		texel_count_of_weight[i] = 0;
	}

	for (unsigned int i = 0; i < texels_per_block; i++)
	{
		weight_count_of_texel[i] = 0;
	}

	for (unsigned int z = 0; z < z_texels; z++)
	{
		for (unsigned int y = 0; y < y_texels; y++)
		{
			for (unsigned int x = 0; x < x_texels; x++)
			{
				int texel = (z * y_texels + y) * x_texels + x;

				int x_weight = (((1024 + x_texels / 2) / (x_texels - 1)) * x * (x_weights - 1) + 32) >> 6;
				int y_weight = (((1024 + y_texels / 2) / (y_texels - 1)) * y * (y_weights - 1) + 32) >> 6;
				int z_weight = (((1024 + z_texels / 2) / (z_texels - 1)) * z * (z_weights - 1) + 32) >> 6;

				int x_weight_frac = x_weight & 0xF;
				int y_weight_frac = y_weight & 0xF;
				int z_weight_frac = z_weight & 0xF;
				int x_weight_int = x_weight >> 4;
				int y_weight_int = y_weight >> 4;
				int z_weight_int = z_weight >> 4;
				int qweight[4];
				int weight[4];
				qweight[0] = (z_weight_int * y_weights + y_weight_int) * x_weights + x_weight_int;
				qweight[3] = ((z_weight_int + 1) * y_weights + (y_weight_int + 1)) * x_weights + (x_weight_int + 1);

				// simplex interpolation
				int fs = x_weight_frac;
				int ft = y_weight_frac;
				int fp = z_weight_frac;

				int cas = ((fs > ft) << 2) + ((ft > fp) << 1) + ((fs > fp));
				int N = x_weights;
				int NM = x_weights * y_weights;

				int s1, s2, w0, w1, w2, w3;
				switch (cas)
				{
				case 7:
					s1 = 1;
					s2 = N;
					w0 = 16 - fs;
					w1 = fs - ft;
					w2 = ft - fp;
					w3 = fp;
					break;
				case 3:
					s1 = N;
					s2 = 1;
					w0 = 16 - ft;
					w1 = ft - fs;
					w2 = fs - fp;
					w3 = fp;
					break;
				case 5:
					s1 = 1;
					s2 = NM;
					w0 = 16 - fs;
					w1 = fs - fp;
					w2 = fp - ft;
					w3 = ft;
					break;
				case 4:
					s1 = NM;
					s2 = 1;
					w0 = 16 - fp;
					w1 = fp - fs;
					w2 = fs - ft;
					w3 = ft;
					break;
				case 2:
					s1 = N;
					s2 = NM;
					w0 = 16 - ft;
					w1 = ft - fp;
					w2 = fp - fs;
					w3 = fs;
					break;
				case 0:
					s1 = NM;
					s2 = N;
					w0 = 16 - fp;
					w1 = fp - ft;
					w2 = ft - fs;
					w3 = fs;
					break;
				default:
					s1 = NM;
					s2 = N;
					w0 = 16 - fp;
					w1 = fp - ft;
					w2 = ft - fs;
					w3 = fs;
					break;
				}

				qweight[1] = qweight[0] + s1;
				qweight[2] = qweight[1] + s2;
				weight[0] = w0;
				weight[1] = w1;
				weight[2] = w2;
				weight[3] = w3;

				for (unsigned int i = 0; i < 4; i++)
				{
					if (weight[i] != 0)
					{
						grid_weights_of_texel[texel][weight_count_of_texel[texel]] = static_cast<uint8_t>(qweight[i]);
						weights_of_texel[texel][weight_count_of_texel[texel]] = static_cast<uint8_t>(weight[i]);
						weight_count_of_texel[texel]++;
						texels_of_weight[qweight[i]][texel_count_of_weight[qweight[i]]] = static_cast<uint8_t>(texel);
						texel_weights_of_weight[qweight[i]][texel_count_of_weight[qweight[i]]] = static_cast<uint8_t>(weight[i]);
						texel_count_of_weight[qweight[i]]++;
						max_texel_count_of_weight = astc::max(max_texel_count_of_weight, texel_count_of_weight[qweight[i]]);
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < texels_per_block; i++)
	{
		di.texel_weight_count[i] = weight_count_of_texel[i];

		// Init all 4 entries so we can rely on zeros for vectorization
		for (unsigned int j = 0; j < 4; j++)
		{
			di.texel_weights_int_4t[j][i] = 0;
			di.texel_weights_float_4t[j][i] = 0.0f;
			di.texel_weights_4t[j][i] = 0;
		}

		for (unsigned int j = 0; j < weight_count_of_texel[i]; j++)
		{
			di.texel_weights_int_4t[j][i] = weights_of_texel[i][j];
			di.texel_weights_float_4t[j][i] = ((float)weights_of_texel[i][j]) * (1.0f / WEIGHTS_TEXEL_SUM);
			di.texel_weights_4t[j][i] = grid_weights_of_texel[i][j];
		}
	}

	for (unsigned int i = 0; i < weights_per_block; i++)
	{
		unsigned int texel_count_wt = texel_count_of_weight[i];
		di.weight_texel_count[i] = (uint8_t)texel_count_wt;

		for (unsigned int j = 0; j < texel_count_wt; j++)
		{
			unsigned int texel = texels_of_weight[i][j];

			// Create transposed versions of these for better vectorization
			di.weight_texel[j][i] = static_cast<uint8_t>(texel);
			di.weights_flt[j][i] = static_cast<float>(texel_weights_of_weight[i][j]);

			// perform a layer of array unrolling. An aspect of this unrolling is that
			// one of the texel-weight indexes is an identity-mapped index; we will use this
			// fact to reorder the indexes so that the first one is the identity index.
			int swap_idx = -1;
			for (unsigned int k = 0; k < 4; k++)
			{
				uint8_t dttw = di.texel_weights_4t[k][texel];
				float dttwf = di.texel_weights_float_4t[k][texel];
				if (dttw == i && dttwf != 0.0f)
				{
					swap_idx = k;
				}
				di.texel_weights_texel[i][j][k] = dttw;
				di.texel_weights_float_texel[i][j][k] = dttwf;
			}

			if (swap_idx != 0)
			{
				uint8_t vi = di.texel_weights_texel[i][j][0];
				float vf = di.texel_weights_float_texel[i][j][0];
				di.texel_weights_texel[i][j][0] = di.texel_weights_texel[i][j][swap_idx];
				di.texel_weights_float_texel[i][j][0] = di.texel_weights_float_texel[i][j][swap_idx];
				di.texel_weights_texel[i][j][swap_idx] = vi;
				di.texel_weights_float_texel[i][j][swap_idx] = vf;
			}
		}

		// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
		// Match last texel in active lane in SIMD group, for better gathers
		uint8_t last_texel = di.weight_texel[texel_count_wt - 1][i];
		for (unsigned int j = texel_count_wt; j < max_texel_count_of_weight; j++)
		{
			di.weight_texel[j][i] = last_texel;
			di.weights_flt[j][i] = 0.0f;
		}
	}

	// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
	unsigned int texels_per_block_simd = round_up_to_simd_multiple_vla(texels_per_block);
	for (unsigned int i = texels_per_block; i < texels_per_block_simd; i++)
	{
		di.texel_weight_count[i] = 0;

		for (unsigned int j = 0; j < 4; j++)
		{
			di.texel_weights_float_4t[j][i] = 0;
			di.texel_weights_4t[j][i] = 0;
			di.texel_weights_int_4t[j][i] = 0;
		}
	}

	// Initialize array tail so we can over-fetch with SIMD later to avoid loop tails
	// Match last texel in active lane in SIMD group, for better gathers
	int last_texel_count_wt = texel_count_of_weight[weights_per_block - 1];
	uint8_t last_texel = di.weight_texel[last_texel_count_wt - 1][weights_per_block - 1];

	unsigned int weights_per_block_simd = round_up_to_simd_multiple_vla(weights_per_block);
	for (unsigned int i = weights_per_block; i < weights_per_block_simd; i++)
	{
		di.weight_texel_count[i] = 0;

		for (int j = 0; j < max_texel_count_of_weight; j++)
		{
			di.weight_texel[j][i] = last_texel;
			di.weights_flt[j][i] = 0.0f;
		}
	}

	di.texel_count = static_cast<uint8_t>(texels_per_block);
	di.weight_count = static_cast<uint8_t>(weights_per_block);
	di.weight_x = static_cast<uint8_t>(x_weights);
	di.weight_y = static_cast<uint8_t>(y_weights);
	di.weight_z = static_cast<uint8_t>(z_weights);
}

/**
 * @brief Assign the texels to use for kmeans clustering.
 *
 * The max limit is @c BLOCK_MAX_KMEANS_TEXELS; above this a random selection is used.
 * The @c bsd.texel_count is an input and must be populated beforehand.
 *
 * @param[in,out] bsd   The block size descriptor to populate.
 */
static void assign_kmeans_texels(
	block_size_descriptor& bsd
) {
	// Use all texels for kmeans on a small block
	if (bsd.texel_count <= BLOCK_MAX_KMEANS_TEXELS)
	{
		for (uint8_t i = 0; i < bsd.texel_count; i++)
		{
			bsd.kmeans_texels[i] = i;
		}

		return;
	}

	// Select a random subset of BLOCK_MAX_KMEANS_TEXELS for kmeans on a large block
	uint64_t rng_state[2];
	astc::rand_init(rng_state);

	// Initialize array used for tracking used indices
	bool seen[BLOCK_MAX_TEXELS];
	for (uint8_t i = 0; i < bsd.texel_count; i++)
	{
		seen[i] = false;
	}

	// Assign 64 random indices, retrying if we see repeats
	unsigned int arr_elements_set = 0;
	while (arr_elements_set < BLOCK_MAX_KMEANS_TEXELS)
	{
		uint8_t texel = static_cast<uint8_t>(astc::rand(rng_state));
		texel = texel % bsd.texel_count;
		if (!seen[texel])
		{
			bsd.kmeans_texels[arr_elements_set++] = texel;
			seen[texel] = true;
		}
	}
}

/**
 * @brief Allocate a single 2D decimation table entry.
 *
 * @param x_texels    The number of texels in the X dimension.
 * @param y_texels    The number of texels in the Y dimension.
 * @param x_weights   The number of weights in the X dimension.
 * @param y_weights   The number of weights in the Y dimension.
 *
 * @return The new entry's index in the compacted decimation table array.
 */
static int construct_dt_entry_2d(
	unsigned int x_texels,
	unsigned int y_texels,
	unsigned int x_weights,
	unsigned int y_weights,
	block_size_descriptor& bsd
) {
	unsigned int dm_index = bsd.decimation_mode_count;
	unsigned int weight_count = x_weights * y_weights;
	assert(weight_count <= BLOCK_MAX_WEIGHTS);

	bool try_2planes = (2 * weight_count) <= BLOCK_MAX_WEIGHTS;

	decimation_info *di = aligned_malloc<decimation_info>(sizeof(decimation_info), ASTCENC_VECALIGN);
	init_decimation_info_2d(x_texels, y_texels, x_weights, y_weights, *di);

	int maxprec_1plane = -1;
	int maxprec_2planes = -1;
	for (int i = 0; i < 12; i++)
	{
		unsigned int bits_1plane = get_ise_sequence_bitcount(weight_count, (quant_method)i);
		if (bits_1plane >= BLOCK_MIN_WEIGHT_BITS && bits_1plane <= BLOCK_MAX_WEIGHT_BITS)
		{
			maxprec_1plane = i;
		}

		if (try_2planes)
		{
			unsigned int bits_2planes = get_ise_sequence_bitcount(2 * weight_count, (quant_method)i);
			if (bits_2planes >= BLOCK_MIN_WEIGHT_BITS && bits_2planes <= BLOCK_MAX_WEIGHT_BITS)
			{
				maxprec_2planes = i;
			}
		}
	}

	// At least one of the two should be valid ...
	assert(maxprec_1plane >= 0 || maxprec_2planes >= 0);
	bsd.decimation_modes[dm_index].maxprec_1plane = static_cast<int8_t>(maxprec_1plane);
	bsd.decimation_modes[dm_index].maxprec_2planes = static_cast<int8_t>(maxprec_2planes);

	// Default to not enabled - we'll populate these based on active block modes
	bsd.decimation_modes[dm_index].percentile_hit = false;

	bsd.decimation_tables[dm_index] = di;

	bsd.decimation_mode_count++;
	return dm_index;
}

/**
 * @brief Allocate block modes and decimation tables for a single 2D block size.
 *
 * @param      x_texels         The number of texels in the X dimension.
 * @param      y_texels         The number of texels in the Y dimension.
 * @param      can_omit_modes   Can we discard modes that astcenc won't use, even if legal?
 * @param      mode_cutoff      Percentile cutoff in range [0,1]. Low values more likely to be used.
 * @param[out] bsd              The block size descriptor to populate.
 */
static void construct_block_size_descriptor_2d(
	unsigned int x_texels,
	unsigned int y_texels,
	bool can_omit_modes,
	float mode_cutoff,
	block_size_descriptor& bsd
) {
	// Store a remap table for storing packed decimation modes.
	// Indexing uses [Y * 16 + X] and max size for each axis is 12.
	static const unsigned int MAX_DMI = 12 * 16 + 12;
	int decimation_mode_index[MAX_DMI];

	bsd.xdim = static_cast<uint8_t>(x_texels);
	bsd.ydim = static_cast<uint8_t>(y_texels);
	bsd.zdim = 1;
	bsd.texel_count = static_cast<uint8_t>(x_texels * y_texels);
	bsd.decimation_mode_count = 0;

	for (unsigned int i = 0; i < MAX_DMI; i++)
	{
		decimation_mode_index[i] = -1;
	}

	// Gather all the decimation grids that can be used with the current block
#if !defined(ASTCENC_DECOMPRESS_ONLY)
	const float *percentiles = get_2d_percentile_table(x_texels, y_texels);
#else
	// Unused in decompress-only builds
	(void)can_omit_modes;
	(void)mode_cutoff;
#endif

	// Construct the list of block formats referencing the decimation tables
	unsigned int packed_idx = 0;
	unsigned int always_block_mode_count = 0;
	unsigned int always_decimation_mode_count = 0;

	// Iterate twice; first time keep the "always" blocks, second time keep the "non-always" blocks.
	// This ensures that the always block modes and decimation modes are at the start of the list.
	for (unsigned int j = 0; j < 2; j ++)
	{
		for (unsigned int i = 0; i < WEIGHTS_MAX_BLOCK_MODES; i++)
		{
			unsigned int x_weights, y_weights;
			bool is_dual_plane;

			unsigned int quant_mode;

	#if !defined(ASTCENC_DECOMPRESS_ONLY)
			float percentile = percentiles[i];
			bool selected = (percentile <= mode_cutoff) || !can_omit_modes;

			if (j == 0 && percentile != 0.0f)
			{
				continue;
			}

			if (j == 1 && percentile == 0.0f)
			{
				continue;
			}

	#else
			// Decompressor builds can never discard modes, as we cannot make any
			// assumptions about the modes the original compressor used
			bool selected = true;

			if (j == 1)
			{
				continue;
			}
	#endif

			// ASSUMPTION: No compressor will use more weights in a dimension than
			// the block has actual texels, because it wastes bits. Decompression
			// of an image which violates this assumption will fail, even though it
			// is technically permitted by the specification.

			// Skip modes that are invalid, too large, or not selected by heuristic
			bool valid = decode_block_mode_2d(i, x_weights, y_weights, is_dual_plane, quant_mode);
			if (!selected || !valid || (x_weights > x_texels) || (y_weights > y_texels))
			{
				bsd.block_mode_packed_index[i] = BLOCK_BAD_BLOCK_MODE;
				continue;
			}

			// Allocate and initialize the decimation table entry if we've not used it yet
			int decimation_mode = decimation_mode_index[y_weights * 16 + x_weights];
			if (decimation_mode == -1)
			{
				decimation_mode = construct_dt_entry_2d(x_texels, y_texels, x_weights, y_weights, bsd);
				decimation_mode_index[y_weights * 16 + x_weights] = decimation_mode;

	#if !defined(ASTCENC_DECOMPRESS_ONLY)
				if (percentile == 0.0f)
				{
					always_decimation_mode_count++;
				}
	#endif
			}

	#if !defined(ASTCENC_DECOMPRESS_ONLY)
			// Flatten the block mode heuristic into some precomputed flags
			if (percentile == 0.0f)
			{
				always_block_mode_count++;
				bsd.block_modes[packed_idx].percentile_hit = true;
				bsd.decimation_modes[decimation_mode].percentile_hit = true;
			}
			else if (percentile <= mode_cutoff)
			{
				bsd.block_modes[packed_idx].percentile_hit = true;
				bsd.decimation_modes[decimation_mode].percentile_hit = true;
			}
			else
			{
				bsd.block_modes[packed_idx].percentile_hit = false;
			}
	#endif

			bsd.block_modes[packed_idx].decimation_mode = static_cast<uint8_t>(decimation_mode);
			bsd.block_modes[packed_idx].quant_mode = static_cast<uint8_t>(quant_mode);
			bsd.block_modes[packed_idx].is_dual_plane = static_cast<uint8_t>(is_dual_plane);
			bsd.block_modes[packed_idx].mode_index = static_cast<uint16_t>(i);
			bsd.block_mode_packed_index[i] = static_cast<uint16_t>(packed_idx);
			packed_idx++;
		}
	}

	bsd.block_mode_count = packed_idx;
	bsd.always_block_mode_count = always_block_mode_count;
	bsd.always_decimation_mode_count = always_decimation_mode_count;

	assert(bsd.always_block_mode_count > 0);
	assert(bsd.always_decimation_mode_count > 0);

#if !defined(ASTCENC_DECOMPRESS_ONLY)
	delete[] percentiles;
#endif

	// Ensure the end of the array contains valid data (should never get read)
	for (unsigned int i = bsd.decimation_mode_count; i < WEIGHTS_MAX_DECIMATION_MODES; i++)
	{
		bsd.decimation_modes[i].maxprec_1plane = -1;
		bsd.decimation_modes[i].maxprec_2planes = -1;
		bsd.decimation_modes[i].percentile_hit = false;
		bsd.decimation_tables[i] = nullptr;
	}

	// Determine the texels to use for kmeans clustering.
	assign_kmeans_texels(bsd);
}

/**
 * @brief Allocate block modes and decimation tables for a single £D block size.
 *
 * TODO: This function doesn't include all of the heuristics that we use for 2D block sizes such as
 * the percentile mode cutoffs. If 3D becomes more widely used we should look at this.
 *
 * @param      x_texels   The number of texels in the X dimension.
 * @param      y_texels   The number of texels in the Y dimension.
 * @param      z_texels   The number of texels in the Z dimension.
 * @param[out] bsd        The block size descriptor to populate.
 */
static void construct_block_size_descriptor_3d(
	unsigned int x_texels,
	unsigned int y_texels,
	unsigned int z_texels,
	block_size_descriptor& bsd
) {
	// Store a remap table for storing packed decimation modes.
	// Indexing uses [Z * 64 + Y *  8 + X] and max size for each axis is 6.
	static constexpr unsigned int MAX_DMI = 6 * 64 + 6 * 8 + 6;
	int decimation_mode_index[MAX_DMI];
	unsigned int decimation_mode_count = 0;

	bsd.xdim = static_cast<uint8_t>(x_texels);
	bsd.ydim = static_cast<uint8_t>(y_texels);
	bsd.zdim = static_cast<uint8_t>(z_texels);
	bsd.texel_count = static_cast<uint8_t>(x_texels * y_texels * z_texels);

	for (unsigned int i = 0; i < MAX_DMI; i++)
	{
		decimation_mode_index[i] = -1;
	}

	// gather all the infill-modes that can be used with the current block size
	for (unsigned int x_weights = 2; x_weights <= x_texels; x_weights++)
	{
		for (unsigned int y_weights = 2; y_weights <= y_texels; y_weights++)
		{
			for (unsigned int z_weights = 2; z_weights <= z_texels; z_weights++)
			{
				unsigned int weight_count = x_weights * y_weights * z_weights;
				if (weight_count > BLOCK_MAX_WEIGHTS)
				{
					continue;
				}

				decimation_info *di = aligned_malloc<decimation_info>(sizeof(decimation_info), ASTCENC_VECALIGN);
				decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights] = decimation_mode_count;
				init_decimation_info_3d(x_texels, y_texels, z_texels, x_weights, y_weights, z_weights, *di);

				int maxprec_1plane = -1;
				int maxprec_2planes = -1;
				for (unsigned int i = 0; i < 12; i++)
				{
					unsigned int bits_1plane = get_ise_sequence_bitcount(weight_count, (quant_method)i);
					if (bits_1plane >= BLOCK_MIN_WEIGHT_BITS && bits_1plane <= BLOCK_MAX_WEIGHT_BITS)
					{
						maxprec_1plane = i;
					}

					unsigned int bits_2planes = get_ise_sequence_bitcount(2 * weight_count, (quant_method)i);
					if (bits_2planes >= BLOCK_MIN_WEIGHT_BITS && bits_2planes <= BLOCK_MAX_WEIGHT_BITS)
					{
						maxprec_2planes = i;
					}
				}

				if ((2 * weight_count) > BLOCK_MAX_WEIGHTS)
				{
					maxprec_2planes = -1;
				}

				bsd.decimation_modes[decimation_mode_count].maxprec_1plane = static_cast<int8_t>(maxprec_1plane);
				bsd.decimation_modes[decimation_mode_count].maxprec_2planes = static_cast<int8_t>(maxprec_2planes);
				bsd.decimation_modes[decimation_mode_count].percentile_hit = false;
				bsd.decimation_tables[decimation_mode_count] = di;
				decimation_mode_count++;
			}
		}
	}

	// Ensure the end of the array contains valid data (should never get read)
	for (unsigned int i = decimation_mode_count; i < WEIGHTS_MAX_DECIMATION_MODES; i++)
	{
		bsd.decimation_modes[i].maxprec_1plane = -1;
		bsd.decimation_modes[i].maxprec_2planes = -1;
		bsd.decimation_modes[i].percentile_hit = false;
		bsd.decimation_tables[i] = nullptr;
	}

	bsd.decimation_mode_count = decimation_mode_count;

	// Construct the list of block formats
	unsigned int packed_idx = 0;
	for (unsigned int i = 0; i < WEIGHTS_MAX_BLOCK_MODES; i++)
	{
		unsigned int x_weights, y_weights, z_weights;
		bool is_dual_plane;
		unsigned int quant_mode;
		bool permit_encode = true;

		if (decode_block_mode_3d(i, x_weights, y_weights, z_weights, is_dual_plane, quant_mode))
		{
			if (x_weights > x_texels || y_weights > y_texels || z_weights > z_texels)
			{
				permit_encode = false;
			}
		}
		else
		{
			permit_encode = false;
		}

		if (!permit_encode)
		{
			bsd.block_mode_packed_index[i] = BLOCK_BAD_BLOCK_MODE;
			continue;
		}

		int decimation_mode = decimation_mode_index[z_weights * 64 + y_weights * 8 + x_weights];
		bsd.block_modes[packed_idx].decimation_mode = static_cast<uint8_t>(decimation_mode);
		bsd.block_modes[packed_idx].quant_mode = static_cast<uint8_t>(quant_mode);
		bsd.block_modes[packed_idx].is_dual_plane = static_cast<uint8_t>(is_dual_plane);
		bsd.block_modes[packed_idx].mode_index = static_cast<uint16_t>(i);

		// No percentile table, so enable everything all the time ...
		bsd.block_modes[packed_idx].percentile_hit = true;
		bsd.decimation_modes[decimation_mode].percentile_hit = true;

		bsd.block_mode_packed_index[i] = static_cast<uint16_t>(packed_idx);

		packed_idx++;
	}

	bsd.block_mode_count = packed_idx;

	// These are never used = the MODE0 fast path is skipped for 3D blocks
	bsd.always_block_mode_count = 0;
	bsd.always_decimation_mode_count = 0;

	// Determine the texels to use for kmeans clustering.
	assign_kmeans_texels(bsd);
}

/* See header for documentation. */
void init_block_size_descriptor(
	unsigned int x_texels,
	unsigned int y_texels,
	unsigned int z_texels,
	bool can_omit_modes,
	float mode_cutoff,
	block_size_descriptor& bsd
) {
	if (z_texels > 1)
	{
		construct_block_size_descriptor_3d(x_texels, y_texels, z_texels, bsd);
	}
	else
	{
		construct_block_size_descriptor_2d(x_texels, y_texels, can_omit_modes, mode_cutoff, bsd);
	}

	init_partition_tables(bsd);
}

/* See header for documentation. */
void term_block_size_descriptor(
	block_size_descriptor& bsd
) {
	for (unsigned int i = 0; i < bsd.decimation_mode_count; i++)
	{
		aligned_free<const decimation_info>(bsd.decimation_tables[i]);
	}
}
