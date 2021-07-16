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

#if !defined(ASTCENC_DECOMPRESS_ONLY)

/**
 * @brief Functions for computing color endpoints and texel weights.
 */

#include <cassert>

#include "astcenc_internal.h"
#include "astcenc_vecmathlib.h"

/**
 * @brief Compute the ideal endpoints and weights for 1 color component.
 *
 * @param      bsd         The block size information.
 * @param      blk         The image block color data to compress.
 * @param      ewb         The image block weighted error data.
 * @param      pi          The partition info for the current trial.
 * @param[out] ei          The computed ideal endpoints and weights.
 * @param      component   The color component to compute.
 */
static void compute_ideal_colors_and_weights_1_comp(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	endpoints_and_weights& ei,
	unsigned int component
) {
	int partition_count = pi.partition_count;
	ei.ep.partition_count = partition_count;
	promise(partition_count > 0);

	int texel_count = bsd.texel_count;
	promise(texel_count > 0);

	float lowvalues[BLOCK_MAX_PARTITIONS] { 1e10f, 1e10f, 1e10f, 1e10f };
	float highvalues[BLOCK_MAX_PARTITIONS] { -1e10f, -1e10f, -1e10f, -1e10f };

	float partition_error_scale[BLOCK_MAX_PARTITIONS];
	float linelengths_rcp[BLOCK_MAX_PARTITIONS];

	const float *error_weights = nullptr;
	const float* data_vr = nullptr;

	assert(component < BLOCK_MAX_COMPONENTS);
	switch (component)
	{
	case 0:
		error_weights = ewb.texel_weight_r;
		data_vr = blk.data_r;
		break;
	case 1:
		error_weights = ewb.texel_weight_g;
		data_vr = blk.data_g;
		break;
	case 2:
		error_weights = ewb.texel_weight_b;
		data_vr = blk.data_b;
		break;
	default:
		error_weights = ewb.texel_weight_a;
		data_vr = blk.data_a;
		break;
	}

	for (int i = 0; i < texel_count; i++)
	{
		if (error_weights[i] > 1e-10f)
		{
			float value = data_vr[i];
			int partition = pi.partition_of_texel[i];

			lowvalues[partition] = astc::min(value, lowvalues[partition]);
			highvalues[partition] = astc::max(value, highvalues[partition]);
		}
	}

	vmask4 sep_mask = vint4::lane_id() == vint4(component);
	for (int i = 0; i < partition_count; i++)
	{
		float diff = highvalues[i] - lowvalues[i];

		if (diff < 0)
		{
			lowvalues[i] = 0.0f;
			highvalues[i] = 0.0f;
		}

		diff = astc::max(diff, 1e-7f);

		partition_error_scale[i] = diff * diff;
		linelengths_rcp[i] = 1.0f / diff;

		ei.ep.endpt0[i] = select(blk.data_min, vfloat4(lowvalues[i]), sep_mask);
		ei.ep.endpt1[i] = select(blk.data_max, vfloat4(highvalues[i]), sep_mask);
	}

	bool is_constant_wes = true;
	float constant_wes = partition_error_scale[pi.partition_of_texel[0]] * error_weights[0];

	for (int i = 0; i < texel_count; i++)
	{
		float value = data_vr[i];
		int partition = pi.partition_of_texel[i];
		value -= lowvalues[partition];
		value *= linelengths_rcp[partition];
		value = astc::clamp1f(value);

		ei.weights[i] = value;
		ei.weight_error_scale[i] = partition_error_scale[partition] * error_weights[i];
		assert(!astc::isnan(ei.weight_error_scale[i]));

		is_constant_wes = is_constant_wes && ei.weight_error_scale[i] == constant_wes;
	}

	// Zero initialize any SIMD over-fetch
	int texel_count_simd = round_up_to_simd_multiple_vla(texel_count);
	for (int i = texel_count; i < texel_count_simd; i++)
	{
		ei.weights[i] = 0.0f;
		ei.weight_error_scale[i] = 0.0f;
	}

	ei.is_constant_weight_error_scale = is_constant_wes;
}

/**
 * @brief Compute the ideal endpoints and weights for 2 color components.
 *
 * @param      bsd          The block size information.
 * @param      blk          The image block color data to compress.
 * @param      ewb          The image block weighted error data.
 * @param      pi           The partition info for the current trial.
 * @param[out] ei           The computed ideal endpoints and weights.
 * @param      component1   The first color component to compute.
 * @param      component2   The second color component to compute.
 */
static void compute_ideal_colors_and_weights_2_comp(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	endpoints_and_weights& ei,
	int component1,
	int component2
) {
	int partition_count = pi.partition_count;
	ei.ep.partition_count = partition_count;
	promise(partition_count > 0);

	int texel_count = bsd.texel_count;
	promise(texel_count > 0);

	partition_metrics pms[BLOCK_MAX_PARTITIONS];

	const float *error_weights;
	const float* data_vr = nullptr;
	const float* data_vg = nullptr;
	if (component1 == 0 && component2 == 1)
	{
		error_weights = ewb.texel_weight_rg;
		data_vr = blk.data_r;
		data_vg = blk.data_g;
	}
	else if (component1 == 0 && component2 == 2)
	{
		error_weights = ewb.texel_weight_rb;
		data_vr = blk.data_r;
		data_vg = blk.data_b;
	}
	else // (component1 == 1 && component2 == 2)
	{
		error_weights = ewb.texel_weight_gb;
		data_vr = blk.data_g;
		data_vg = blk.data_b;
	}

	float lowparam[BLOCK_MAX_PARTITIONS] { 1e10f, 1e10f, 1e10f, 1e10f };
	float highparam[BLOCK_MAX_PARTITIONS] { -1e10f, -1e10f, -1e10f, -1e10f };

	line2 lines[BLOCK_MAX_PARTITIONS];
	float scale[BLOCK_MAX_PARTITIONS];
	float length_squared[BLOCK_MAX_PARTITIONS];

	compute_avgs_and_dirs_2_comp(pi, blk, ewb, component1, component2, pms);

	for (int i = 0; i < partition_count; i++)
	{
		vfloat4 dir = pms[i].dir.swz<0, 1>();
		if (hadd_s(dir) < 0.0f)
		{
			dir = vfloat4::zero() - dir;
		}

		lines[i].a = pms[i].avg.swz<0, 1>();
		lines[i].b = normalize_safe(dir, unit2());
	}

	for (int i = 0; i < texel_count; i++)
	{
		if (error_weights[i] > 1e-10f)
		{
			int partition = pi.partition_of_texel[i];
			vfloat4 point = vfloat2(data_vr[i], data_vg[i]) * pms[partition].color_scale.swz<0, 1>();
			line2 l = lines[partition];
			float param = dot_s(point - l.a, l.b);
			ei.weights[i] = param;

			lowparam[partition] = astc::min(param, lowparam[partition]);
			highparam[partition] = astc::max(param, highparam[partition]);
		}
		else
		{
			ei.weights[i] = -1e38f;
		}
	}

	vfloat4 lowvalues[BLOCK_MAX_PARTITIONS];
	vfloat4 highvalues[BLOCK_MAX_PARTITIONS];

	for (int i = 0; i < partition_count; i++)
	{
		float length = highparam[i] - lowparam[i];
		if (length < 0.0f) // Case for when none of the texels had any weight
		{
			lowparam[i] = 0.0f;
			highparam[i] = 1e-7f;
		}

		// It is possible for a uniform-color partition to produce length=0; this causes NaN issues
		// so set to a small value to avoid this problem.
		length = astc::max(length, 1e-7f);
		length_squared[i] = length * length;
		scale[i] = 1.0f / length;

		vfloat4 ep0 = lines[i].a + lines[i].b * lowparam[i];
		vfloat4 ep1 = lines[i].a + lines[i].b * highparam[i];

		ep0 = ep0.swz<0, 1>() / pms[i].color_scale;

		ep1 = ep1.swz<0, 1>() / pms[i].color_scale;

		lowvalues[i] = ep0;
		highvalues[i] = ep1;
	}

	vmask4 comp1_mask = vint4::lane_id() == vint4(component1);
	vmask4 comp2_mask = vint4::lane_id() == vint4(component2);
	for (int i = 0; i < partition_count; i++)
	{
		vfloat4 ep0 = select(blk.data_min, vfloat4(lowvalues[i].lane<0>()), comp1_mask);
		vfloat4 ep1 = select(blk.data_max, vfloat4(highvalues[i].lane<0>()), comp1_mask);

		ei.ep.endpt0[i] = select(ep0, vfloat4(lowvalues[i].lane<1>()), comp2_mask);
		ei.ep.endpt1[i] = select(ep1, vfloat4(highvalues[i].lane<1>()), comp2_mask);
	}

	bool is_constant_wes = true;
	float constant_wes = length_squared[pi.partition_of_texel[0]] * error_weights[0];

	for (int i = 0; i < texel_count; i++)
	{
		int partition = pi.partition_of_texel[i];
		float idx = (ei.weights[i] - lowparam[partition]) * scale[partition];
		idx = astc::clamp1f(idx);

		ei.weights[i] = idx;
		ei.weight_error_scale[i] = length_squared[partition] * error_weights[i];
		assert(!astc::isnan(ei.weight_error_scale[i]));

		is_constant_wes = is_constant_wes && ei.weight_error_scale[i] == constant_wes;
	}

	// Zero initialize any SIMD over-fetch
	int texel_count_simd = round_up_to_simd_multiple_vla(texel_count);
	for (int i = texel_count; i < texel_count_simd; i++)
	{
		ei.weights[i] = 0.0f;
		ei.weight_error_scale[i] = 0.0f;
	}

	ei.is_constant_weight_error_scale = is_constant_wes;
}

/**
 * @brief Compute the ideal endpoints and weights for 3 color components.
 *
 * @param      bsd                 The block size information.
 * @param      blk                 The image block color data to compress.
 * @param      ewb                 The image block weighted error data.
 * @param      pi                  The partition info for the current trial.
 * @param[out] ei                  The computed ideal endpoints and weights.
 * @param      omitted_component   The color component excluded from the calculation.
 */
static void compute_ideal_colors_and_weights_3_comp(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	endpoints_and_weights& ei,
	unsigned int omitted_component
) {
	unsigned int partition_count = pi.partition_count;
	ei.ep.partition_count = partition_count;
	promise(partition_count > 0);

	unsigned int texel_count= bsd.texel_count;
	promise(texel_count > 0);

	partition_metrics pms[BLOCK_MAX_PARTITIONS];

	const float *error_weights;
	const float* data_vr = nullptr;
	const float* data_vg = nullptr;
	const float* data_vb = nullptr;
	if (omitted_component == 0)
	{
		error_weights = ewb.texel_weight_gba;
		data_vr = blk.data_g;
		data_vg = blk.data_b;
		data_vb = blk.data_a;
	}
	else if (omitted_component == 1)
	{
		error_weights = ewb.texel_weight_rba;
		data_vr = blk.data_r;
		data_vg = blk.data_b;
		data_vb = blk.data_a;
	}
	else if (omitted_component == 2)
	{
		error_weights = ewb.texel_weight_rga;
		data_vr = blk.data_r;
		data_vg = blk.data_g;
		data_vb = blk.data_a;
	}
	else
	{
		error_weights = ewb.texel_weight_rgb;
		data_vr = blk.data_r;
		data_vg = blk.data_g;
		data_vb = blk.data_b;
	}

	float lowparam[BLOCK_MAX_PARTITIONS] { 1e10f, 1e10f, 1e10f, 1e10f };
	float highparam[BLOCK_MAX_PARTITIONS] { -1e10f, -1e10f, -1e10f, -1e10f };

	line3 lines[BLOCK_MAX_PARTITIONS];
	float scale[BLOCK_MAX_PARTITIONS];
	float length_squared[BLOCK_MAX_PARTITIONS];

	compute_avgs_and_dirs_3_comp(pi, blk, ewb, omitted_component, pms);

	for (unsigned int i = 0; i < partition_count; i++)
	{
		vfloat4 dir = pms[i].dir;
		if (hadd_rgb_s(dir) < 0.0f)
		{
			dir = vfloat4::zero() - dir;
		}

		lines[i].a = pms[i].avg;
		lines[i].b = normalize_safe(dir, unit3());
	}

	for (unsigned int i = 0; i < texel_count; i++)
	{
		if (error_weights[i] > 1e-10f)
		{
			int partition = pi.partition_of_texel[i];
			vfloat4 point = vfloat3(data_vr[i], data_vg[i], data_vb[i]) * pms[partition].color_scale;
			line3 l = lines[partition];
			float param = dot3_s(point - l.a, l.b);
			ei.weights[i] = param;

			lowparam[partition] = astc::min(param, lowparam[partition]);
			highparam[partition] = astc::max(param, highparam[partition]);
		}
		else
		{
			ei.weights[i] = -1e38f;
		}
	}

	for (unsigned int i = 0; i < partition_count; i++)
	{
		float length = highparam[i] - lowparam[i];
		if (length < 0)			// Case for when none of the texels had any weight
		{
			lowparam[i] = 0.0f;
			highparam[i] = 1e-7f;
		}

		// It is possible for a uniform-color partition to produce length=0; this causes NaN issues
		// so set to a small value to avoid this problem.
		length = astc::max(length, 1e-7f);

		length_squared[i] = length * length;
		scale[i] = 1.0f / length;

		vfloat4 ep0 = lines[i].a + lines[i].b * lowparam[i];
		vfloat4 ep1 = lines[i].a + lines[i].b * highparam[i];

		ep0 = ep0 * pms[i].icolor_scale;
		ep1 = ep1 * pms[i].icolor_scale;

		vfloat4 bmin = blk.data_min;
		vfloat4 bmax = blk.data_max;

		assert(omitted_component < BLOCK_MAX_COMPONENTS);
		switch (omitted_component)
		{
			case 0:
				ei.ep.endpt0[i] = vfloat4(bmin.lane<0>(), ep0.lane<0>(), ep0.lane<1>(), ep0.lane<2>());
				ei.ep.endpt1[i] = vfloat4(bmax.lane<0>(), ep1.lane<0>(), ep1.lane<1>(), ep1.lane<2>());
				break;
			case 1:
				ei.ep.endpt0[i] = vfloat4(ep0.lane<0>(), bmin.lane<1>(), ep0.lane<1>(), ep0.lane<2>());
				ei.ep.endpt1[i] = vfloat4(ep1.lane<0>(), bmax.lane<1>(), ep1.lane<1>(), ep1.lane<2>());
				break;
			case 2:
				ei.ep.endpt0[i] = vfloat4(ep0.lane<0>(), ep0.lane<1>(), bmin.lane<2>(), ep0.lane<2>());
				ei.ep.endpt1[i] = vfloat4(ep1.lane<0>(), ep1.lane<1>(), bmax.lane<2>(), ep1.lane<2>());
				break;
			default:
				ei.ep.endpt0[i] = vfloat4(ep0.lane<0>(), ep0.lane<1>(), ep0.lane<2>(), bmin.lane<3>());
				ei.ep.endpt1[i] = vfloat4(ep1.lane<0>(), ep1.lane<1>(), ep1.lane<2>(), bmax.lane<3>());
				break;
		}
	}


	bool is_constant_wes = true;
	float constant_wes = length_squared[pi.partition_of_texel[0]] * error_weights[0];

	for (unsigned int i = 0; i < texel_count; i++)
	{
		int partition = pi.partition_of_texel[i];
		float idx = (ei.weights[i] - lowparam[partition]) * scale[partition];
		idx = astc::clamp1f(idx);

		ei.weights[i] = idx;
		ei.weight_error_scale[i] = length_squared[partition] * error_weights[i];
		assert(!astc::isnan(ei.weight_error_scale[i]));

		is_constant_wes = is_constant_wes && ei.weight_error_scale[i] == constant_wes;
	}

	// Zero initialize any SIMD over-fetch
	unsigned int texel_count_simd = round_up_to_simd_multiple_vla(texel_count);
	for (unsigned int i = texel_count; i < texel_count_simd; i++)
	{
		ei.weights[i] = 0.0f;
		ei.weight_error_scale[i] = 0.0f;
	}

	ei.is_constant_weight_error_scale = is_constant_wes;
}

/**
 * @brief Compute the ideal endpoints and weights for 4 color components.
 *
 * @param      bsd                 The block size information.
 * @param      blk                 The image block color data to compress.
 * @param      ewb                 The image block weighted error data.
 * @param      pi                  The partition info for the current trial.
 * @param[out] ei                  The computed ideal endpoints and weights.
 */
static void compute_ideal_colors_and_weights_4_comp(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	endpoints_and_weights& ei
) {
	const float *error_weights = ewb.texel_weight;

	int partition_count = pi.partition_count;

	int texel_count= bsd.texel_count;
	promise(texel_count > 0);
	promise(partition_count > 0);

	float lowparam[BLOCK_MAX_PARTITIONS] { 1e10, 1e10, 1e10, 1e10 };
	float highparam[BLOCK_MAX_PARTITIONS] { -1e10, -1e10, -1e10, -1e10 };

	line4 lines[BLOCK_MAX_PARTITIONS];

	float scale[BLOCK_MAX_PARTITIONS];
	float length_squared[BLOCK_MAX_PARTITIONS];

	partition_metrics pms[BLOCK_MAX_PARTITIONS];

	compute_avgs_and_dirs_4_comp(pi, blk, ewb, pms);

	// If the direction points from light to dark then flip so ep0 is darkest
	for (int i = 0; i < partition_count; i++)
	{
		vfloat4 dir = pms[i].dir;
		if (hadd_rgb_s(dir) < 0.0f)
		{
			dir = vfloat4::zero() - dir;
		}

		lines[i].a = pms[i].avg;
		lines[i].b = normalize_safe(dir, unit4());
	}

	for (int i = 0; i < texel_count; i++)
	{
		if (error_weights[i] > 1e-10f)
		{
			int partition = pi.partition_of_texel[i];

			vfloat4 point = blk.texel(i) * pms[partition].color_scale;
			line4 l = lines[partition];

			float param = dot_s(point - l.a, l.b);
			ei.weights[i] = param;

			lowparam[partition] = astc::min(param, lowparam[partition]);
			highparam[partition] = astc::max(param, highparam[partition]);
		}
		else
		{
			ei.weights[i] = -1e38f;
		}
	}

	for (int i = 0; i < partition_count; i++)
	{
		float length = highparam[i] - lowparam[i];
		if (length < 0)
		{
			lowparam[i] = 0.0f;
			highparam[i] = 1e-7f;
		}

		// It is possible for a uniform-color partition to produce length=0; this causes NaN issues
		// so set to a small value to avoid this problem.
		length = astc::max(length, 1e-7f);

		length_squared[i] = length * length;
		scale[i] = 1.0f / length;

		vfloat4 ep0 = lines[i].a + lines[i].b * lowparam[i];
		vfloat4 ep1 = lines[i].a + lines[i].b * highparam[i];

		ei.ep.endpt0[i] = ep0 * pms[i].icolor_scale;
		ei.ep.endpt1[i] = ep1 * pms[i].icolor_scale;
	}

	bool is_constant_wes = true;
	float constant_wes = length_squared[pi.partition_of_texel[0]] * error_weights[0];

	for (int i = 0; i < texel_count; i++)
	{
		int partition = pi.partition_of_texel[i];
		float idx = (ei.weights[i] - lowparam[partition]) * scale[partition];
		idx = astc::clamp1f(idx);

		ei.weights[i] = idx;
		ei.weight_error_scale[i] = error_weights[i] * length_squared[partition];
		assert(!astc::isnan(ei.weight_error_scale[i]));

		is_constant_wes = is_constant_wes && ei.weight_error_scale[i] == constant_wes;
	}

	// Zero initialize any SIMD over-fetch
	int texel_count_simd = round_up_to_simd_multiple_vla(texel_count);
	for (int i = texel_count; i < texel_count_simd; i++)
	{
		ei.weights[i] = 0.0f;
		ei.weight_error_scale[i] = 0.0f;
	}

	ei.is_constant_weight_error_scale = is_constant_wes;
}

/* See header for documentation. */
void compute_ideal_colors_and_weights_1plane(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	endpoints_and_weights& ei
) {
	bool uses_alpha = !blk.is_constant_channel(3);

	if (uses_alpha)
	{
		compute_ideal_colors_and_weights_4_comp(bsd, blk, ewb, pi, ei);
	}
	else
	{
		compute_ideal_colors_and_weights_3_comp(bsd, blk, ewb,  pi, ei, 3);
	}
}

/* See header for documentation. */
void compute_ideal_colors_and_weights_2planes(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	unsigned int plane2_component,
	endpoints_and_weights& ei1,
	endpoints_and_weights& ei2
) {
	const auto& pi = bsd.get_partition_info(1, 0);
	bool uses_alpha = !blk.is_constant_channel(3);

	assert(plane2_component < BLOCK_MAX_COMPONENTS);
	switch (plane2_component)
	{
	case 0: // Separate weights for red
		if (uses_alpha)
		{
			compute_ideal_colors_and_weights_3_comp(bsd, blk, ewb, pi, ei1, 0);
		}
		else
		{
			compute_ideal_colors_and_weights_2_comp(bsd, blk, ewb, pi, ei1, 1, 2);
		}
		compute_ideal_colors_and_weights_1_comp(bsd, blk, ewb, pi, ei2, 0);
		break;

	case 1: // Separate weights for green
		if (uses_alpha)
		{
			compute_ideal_colors_and_weights_3_comp(bsd,blk, ewb,  pi, ei1, 1);
		}
		else
		{
			compute_ideal_colors_and_weights_2_comp(bsd, blk, ewb, pi, ei1, 0, 2);
		}
		compute_ideal_colors_and_weights_1_comp(bsd, blk, ewb, pi, ei2, 1);
		break;

	case 2: // Separate weights for blue
		if (uses_alpha)
		{
			compute_ideal_colors_and_weights_3_comp(bsd, blk, ewb, pi, ei1, 2);
		}
		else
		{
			compute_ideal_colors_and_weights_2_comp(bsd, blk, ewb, pi, ei1, 0, 1);
		}
		compute_ideal_colors_and_weights_1_comp(bsd, blk, ewb, pi, ei2, 2);
		break;

	default: // Separate weights for alpha
		assert(uses_alpha);
		compute_ideal_colors_and_weights_3_comp(bsd, blk, ewb, pi, ei1, 3);
		compute_ideal_colors_and_weights_1_comp(bsd, blk, ewb, pi, ei2, 3);
		break;
	}
}

/* See header for documentation. */
float compute_error_of_weight_set_1plane(
	const endpoints_and_weights& eai,
	const decimation_info& di,
	const float* dec_weight_quant_uvalue
) {
	vfloat4 error_summav = vfloat4::zero();
	float error_summa = 0.0f;
	unsigned int texel_count = di.texel_count;

	bool is_decimated = di.texel_count != di.weight_count;

	// Process SIMD-width chunks, safe to over-fetch - the extra space is zero initialized
	if (is_decimated)
	{
		for (unsigned int i = 0; i < texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			// Compute the bilinear interpolation of the decimated weight grid
			vfloat current_values = bilinear_infill_vla(di, dec_weight_quant_uvalue, i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values = loada(eai.weights + i);
			vfloat diff = current_values - actual_values;
			vfloat significance = loada(eai.weight_error_scale + i);
			vfloat error = diff * diff * significance;

			haccumulate(error_summav, error);
		}
	}
	else
	{
		for (unsigned int i = 0; i < texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			// Load the weight set directly, without interpolation
			vfloat current_values = loada(dec_weight_quant_uvalue + i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values = loada(eai.weights + i);
			vfloat diff = current_values - actual_values;
			vfloat significance = loada(eai.weight_error_scale + i);
			vfloat error = diff * diff * significance;

			haccumulate(error_summav, error);
		}
	}

	// Resolve the final scalar accumulator sum
	haccumulate(error_summa, error_summav);

	return error_summa;
}

/* See header for documentation. */
float compute_error_of_weight_set_2planes(
	const endpoints_and_weights& eai1,
	const endpoints_and_weights& eai2,
	const decimation_info& di,
	const float* dec_weight_quant_uvalue_plane1,
	const float* dec_weight_quant_uvalue_plane2
) {
	vfloat4 error_summav = vfloat4::zero();
	float error_summa = 0.0f;
	unsigned int texel_count = di.texel_count;
	bool is_decimated = di.texel_count != di.weight_count;

	// Process SIMD-width chunks, safe to over-fetch - the extra space is zero initialized
	if (is_decimated)
	{
		for (unsigned int i = 0; i < texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			// Plane 1
			// Compute the bilinear interpolation of the decimated weight grid
			vfloat current_values1 = bilinear_infill_vla(di, dec_weight_quant_uvalue_plane1, i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values1 = loada(eai1.weights + i);
			vfloat diff = current_values1 - actual_values1;
			vfloat error1 = diff * diff * loada(eai1.weight_error_scale + i);

			// Plane 2
			// Compute the bilinear interpolation of the decimated weight grid
			vfloat current_values2 = bilinear_infill_vla(di, dec_weight_quant_uvalue_plane2, i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values2 = loada(eai2.weights + i);
			diff = current_values2 - actual_values2;
			vfloat error2 = diff * diff * loada(eai2.weight_error_scale + i);

			haccumulate(error_summav, error1 + error2);
		}
	}
	else
	{
		for (unsigned int i = 0; i < texel_count; i += ASTCENC_SIMD_WIDTH)
		{
			// Plane 1
			// Load the weight set directly, without interpolation
			vfloat current_values1 = loada(dec_weight_quant_uvalue_plane1 + i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values1 = loada(eai1.weights + i);
			vfloat diff = current_values1 - actual_values1;
			vfloat error1 = diff * diff * loada(eai1.weight_error_scale + i);

			// Plane 2
			// Load the weight set directly, without interpolation
			vfloat current_values2 = loada(dec_weight_quant_uvalue_plane2 + i);

			// Compute the error between the computed value and the ideal weight
			vfloat actual_values2 = loada(eai2.weights + i);
			diff = current_values2 - actual_values2;
			vfloat error2 = diff * diff * loada(eai2.weight_error_scale + i);

			haccumulate(error_summav, error1 + error2);
		}
	}

	// Resolve the final scalar accumulator sum
	haccumulate(error_summa, error_summav);

	return error_summa;
}

/* See header for documentation. */
void compute_ideal_weights_for_decimation(
	const endpoints_and_weights& eai_in,
	endpoints_and_weights& eai_out,
	const decimation_info& di,
	float* dec_weight_ideal_value,
	float* dec_weight_ideal_sig
) {
	unsigned int texel_count = di.texel_count;
	unsigned int weight_count = di.weight_count;

	promise(texel_count > 0);
	promise(weight_count > 0);

	// This function includes a copy of the epw from eai_in to eai_out. We do it here because we
	// want to load the data anyway, so we can avoid loading it from memory twice.
	eai_out.ep = eai_in.ep;
	eai_out.is_constant_weight_error_scale = eai_in.is_constant_weight_error_scale;

	// Ensure that the end of the output arrays that are used for SIMD paths later are filled so we
	// can safely run SIMD elsewhere without a loop tail. Note that this is always safe as weight
	// arrays always contain space for 64 elements
	unsigned int weight_count_simd = round_up_to_simd_multiple_vla(weight_count);
	for (unsigned int i = weight_count; i < weight_count_simd; i++)
	{
		dec_weight_ideal_value[i] = 0.0f;
	}

	// If we have a 1:1 mapping just shortcut the computation - clone the weights into both the
	// weight set and the output epw copy.

	// Transfer enough to also copy zero initialized SIMD over-fetch region
	unsigned int texel_count_simd = round_up_to_simd_multiple_vla(texel_count);
	if (texel_count == weight_count)
	{
		for (unsigned int i = 0; i < texel_count_simd; i++)
		{
			// Assert it's an identity map for valid texels, and last valid value for any overspill
			assert(((i < texel_count) && (i == di.weight_texel[0][i])) ||
			       ((i >= texel_count) && (texel_count - 1 == di.weight_texel[0][i])));
			dec_weight_ideal_value[i] = eai_in.weights[i];
			dec_weight_ideal_sig[i] = eai_in.weight_error_scale[i];

			eai_out.weights[i] = eai_in.weights[i];
			eai_out.weight_error_scale[i] = eai_in.weight_error_scale[i];
		}

		return;
	}
	// If we don't have a 1:1 mapping just clone the weights into the output epw copy and then do
	// the full algorithm to decimate weights.
	else
	{
		for (unsigned int i = 0; i < texel_count_simd; i++)
		{
			eai_out.weights[i] = eai_in.weights[i];
			eai_out.weight_error_scale[i] = eai_in.weight_error_scale[i];
		}
	}

	// Otherwise compute an estimate and perform single refinement iteration
	alignas(ASTCENC_VECALIGN) float infilled_weights[BLOCK_MAX_TEXELS];

	// Compute an initial average for each decimated weight
	bool constant_wes = eai_in.is_constant_weight_error_scale;
	vfloat weight_error_scale(eai_in.weight_error_scale[0]);

	// This overshoots - this is OK as we initialize the array tails in the
	// decimation table structures to safe values ...
	for (unsigned int i = 0; i < weight_count; i += ASTCENC_SIMD_WIDTH)
	{
		// Start with a small value to avoid div-by-zero later
		vfloat weight_weight(1e-10f);
		vfloat initial_weight = vfloat::zero();

		// Accumulate error weighting of all the texels using this weight
		vint weight_texel_count(di.weight_texel_count + i);
		unsigned int max_texel_count = hmax(weight_texel_count).lane<0>();
		promise(max_texel_count > 0);

		for (unsigned int j = 0; j < max_texel_count; j++)
		{
			// Not all lanes may actually use j texels, so mask out if idle
			vmask active = weight_texel_count > vint(j);

			vint texel(di.weight_texel[j] + i);
			texel = select(vint::zero(), texel, active);

			vfloat weight = loada(di.weights_flt[j] + i);
			weight = select(vfloat::zero(), weight, active);

			if (!constant_wes)
			{
				weight_error_scale = gatherf(eai_in.weight_error_scale, texel);
			}

			vfloat contrib_weight = weight * weight_error_scale;

			weight_weight += contrib_weight;
			initial_weight += gatherf(eai_in.weights, texel) * contrib_weight;
		}

		storea(weight_weight, dec_weight_ideal_sig + i);
		storea(initial_weight / weight_weight, dec_weight_ideal_value + i);
	}

	// Populate the interpolated weight grid based on the initital average
	// Process SIMD-width texel coordinates at at time while we can. Safe to
	// over-process full SIMD vectors - the tail is zeroed.
	for (unsigned int i = 0; i < texel_count; i += ASTCENC_SIMD_WIDTH)
	{
		vfloat weight = bilinear_infill_vla(di, dec_weight_ideal_value, i);
		storea(weight, infilled_weights + i);
	}

	// Perform a single iteration of refinement
	// Empirically determined step size; larger values don't help but smaller drops image quality
	constexpr float stepsize = 0.25f;
	constexpr float chd_scale = -WEIGHTS_TEXEL_SUM;

	for (unsigned int i = 0; i < weight_count; i += ASTCENC_SIMD_WIDTH)
	{
		vfloat weight_val = loada(dec_weight_ideal_value + i);

		// Accumulate error weighting of all the texels using this weight
		// Start with a small value to avoid div-by-zero later
		vfloat error_change0(1e-10f);
		vfloat error_change1(0.0f);

		// Accumulate error weighting of all the texels using this weight
		vint weight_texel_count(di.weight_texel_count + i);
		unsigned int max_texel_count = hmax(weight_texel_count).lane<0>();
		promise(max_texel_count > 0);

		for (unsigned int j = 0; j < max_texel_count; j++)
		{
			// Not all lanes may actually use j texels, so mask out if idle
			vmask active = weight_texel_count > vint(j);

			vint texel(di.weight_texel[j] + i);
			texel = select(vint::zero(), texel, active);

			vfloat contrib_weight = loada(di.weights_flt[j] + i);
			contrib_weight = select(vfloat::zero(), contrib_weight, active);

			if (!constant_wes)
			{
 				weight_error_scale = gatherf(eai_in.weight_error_scale, texel);
			}

			vfloat scale = weight_error_scale * contrib_weight;
			vfloat old_weight = gatherf(infilled_weights, texel);
			vfloat ideal_weight = gatherf(eai_in.weights, texel);

			error_change0 += contrib_weight * scale;
			error_change1 += (old_weight - ideal_weight) * scale;
		}


		vfloat step = (error_change1 * chd_scale) / error_change0;
		step = clamp(-stepsize, stepsize, step);

		// Update the weight; note this can store negative values.
		storea(weight_val + step, dec_weight_ideal_value + i);
	}
}

/* See header for documentation. */
void compute_quantized_weights_for_decimation(
	const decimation_info& di,
	float low_bound,
	float high_bound,
	const float* dec_weight_ideal_value,
	float* weight_set_out,
	uint8_t* quantized_weight_set,
	quant_method quant_level
) {
	int weight_count = di.weight_count;
	promise(weight_count > 0);
	const quantization_and_transfer_table *qat = &(quant_and_xfer_tables[quant_level]);

	// The available quant levels, stored with a minus 1 bias
	static const float quant_levels_m1[12] {
		1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 7.0f, 9.0f, 11.0f, 15.0f, 19.0f, 23.0f, 31.0f
	};

	float quant_level_m1 = quant_levels_m1[quant_level];

	// Quantize the weight set using both the specified low/high bounds and standard 0..1 bounds

	// TODO: Oddity to investigate; triggered by test in issue #265.
	if (high_bound < low_bound)
	{
		low_bound = 0.0f;
		high_bound = 1.0f;
	}

	float rscale = high_bound - low_bound;
	float scale = 1.0f / rscale;

	float scaled_low_bound = low_bound * scale;
	rscale *= 1.0f / 64.0f;

	vfloat scalev(scale);
	vfloat scaled_low_boundv(scaled_low_bound);
	vfloat quant_level_m1v(quant_level_m1);
	vfloat rscalev(rscale);
	vfloat low_boundv(low_bound);

	// This runs to the rounded-up SIMD size, which is safe as the loop tail is filled with known
	// safe data in compute_ideal_weights_for_decimation and arrays are always 64 elements
	for (int i = 0; i < weight_count; i += ASTCENC_SIMD_WIDTH)
	{
		vfloat ix = loada(&dec_weight_ideal_value[i]) * scalev - scaled_low_boundv;
		ix = clampzo(ix);

		// Look up the two closest indexes and return the one that was closest
		vfloat ix1 = ix * quant_level_m1v;

		vint weightl = float_to_int(ix1);
		vint weighth = weightl + vint(1);

		vfloat ixl = gatherf(qat->unquantized_value_unsc, weightl);
		vfloat ixh = gatherf(qat->unquantized_value_unsc, weighth);

		vmask mask = (ixl + ixh) < (vfloat(128.0f) * ix);
		vint weight = select(weightl, weighth, mask);
		ixl = select(ixl, ixh, mask);

		// Invert the weight-scaling that was done initially
		storea(ixl * rscalev + low_boundv, &weight_set_out[i]);
		vint scm = gatheri(qat->scramble_map, weight);
		vint scn = pack_low_bytes(scm);
		store_nbytes(scn, &quantized_weight_set[i]);
	}
}

/**
 * @brief Compute the RGB + offset for a HDR endpoint mode #7.
 *
 * Since the matrix needed has a regular structure we can simplify the inverse calculation. This
 * gives us ~24 multiplications vs. 96 for a generic inverse.
 *
 *  mat[0] = vfloat4(rgba_ws.x,      0.0f,      0.0f, wght_ws.x);
 *  mat[1] = vfloat4(     0.0f, rgba_ws.y,      0.0f, wght_ws.y);
 *  mat[2] = vfloat4(     0.0f,      0.0f, rgba_ws.z, wght_ws.z);
 *  mat[3] = vfloat4(wght_ws.x, wght_ws.y, wght_ws.z,      psum);
 *  mat = invert(mat);
 *
 * @param rgba_weight_sum     Sum of partition component error weights.
 * @param weight_weight_sum   Sum of partition component error weights * texel weight.
 * @param rgbq_sum            Sum of partition component error weights * texel weight * color data.
 * @param psum                Sum of RGB color weights * texel weight^2.
 */
static inline vfloat4 compute_rgbo_vector(
	vfloat4 rgba_weight_sum,
	vfloat4 weight_weight_sum,
	vfloat4 rgbq_sum,
	float psum
) {
	float X = rgba_weight_sum.lane<0>();
	float Y = rgba_weight_sum.lane<1>();
	float Z = rgba_weight_sum.lane<2>();
	float P = weight_weight_sum.lane<0>();
	float Q = weight_weight_sum.lane<1>();
	float R = weight_weight_sum.lane<2>();
	float S = psum;

	float PP = P * P;
	float QQ = Q * Q;
	float RR = R * R;

	float SZmRR = S * Z - RR;
	float DT = SZmRR * Y - Z * QQ;
	float YP = Y * P;
	float QX = Q * X;
	float YX = Y * X;
	float mZYP = -Z * YP;
	float mZQX = -Z * QX;
	float mRYX = -R * YX;
	float ZQP = Z * Q * P;
	float RYP = R * YP;
	float RQX = R * QX;

	// Compute the reciprocal of matrix determinant
	float rdet = 1.0f / (DT * X + mZYP * P);

	// Actually compute the adjugate, and then apply 1/det separately
	vfloat4 mat0(DT, ZQP, RYP, mZYP);
	vfloat4 mat1(ZQP, SZmRR * X - Z * PP, RQX, mZQX);
	vfloat4 mat2(RYP, RQX, (S * Y - QQ) * X - Y * PP, mRYX);
	vfloat4 mat3(mZYP, mZQX, mRYX, Z * YX);
	vfloat4 vect = rgbq_sum * rdet;

	return vfloat4(dot_s(mat0, vect),
	               dot_s(mat1, vect),
	               dot_s(mat2, vect),
	               dot_s(mat3, vect));
}

/* See header for documentation. */
void recompute_ideal_colors_1plane(
	const image_block& blk,
	const error_weight_block& ewb,
	const partition_info& pi,
	const decimation_info& di,
	int weight_quant_mode,
	const uint8_t* dec_weights_quant_pvalue,
	endpoints& ep,
	vfloat4 rgbs_vectors[BLOCK_MAX_PARTITIONS],
	vfloat4 rgbo_vectors[BLOCK_MAX_PARTITIONS]
) {
	int weight_count = di.weight_count;
	int partition_count = pi.partition_count;
	bool is_decimated = di.weight_count != di.texel_count;

	promise(weight_count > 0);
	promise(partition_count > 0);

	const quantization_and_transfer_table& qat = quant_and_xfer_tables[weight_quant_mode];

	float dec_weight_quant_uvalue[BLOCK_MAX_WEIGHTS];
	for (int i = 0; i < weight_count; i++)
	{
		dec_weight_quant_uvalue[i] = qat.unquantized_value[dec_weights_quant_pvalue[i]] * (1.0f / 64.0f);
	}

	for (int i = 0; i < partition_count; i++)
	{
		vfloat4 rgba_sum(1e-17f);
		vfloat4 rgba_weight_sum(1e-17f);

		int texel_count = pi.partition_texel_count[i];
		const uint8_t *texel_indexes = pi.texels_of_partition[i];

		promise(texel_count > 0);
		for (int j = 0; j < texel_count; j++)
		{
			int tix = texel_indexes[j];

			vfloat4 rgba = blk.texel(tix);
			vfloat4 error_weight = ewb.error_weights[tix];

			rgba_sum += rgba * error_weight;
			rgba_weight_sum += error_weight;
		}

		vfloat4 scale_direction = normalize((rgba_sum * (1.0f / rgba_weight_sum)).swz<0, 1, 2>());

		float scale_max = 0.0f;
		float scale_min = 1e10f;

		float wmin1 = 1.0f;
		float wmax1 = 0.0f;

		vfloat4 left_sum    = vfloat4::zero();
		vfloat4 middle_sum  = vfloat4::zero();
		vfloat4 right_sum   = vfloat4::zero();
		vfloat4 lmrs_sum    = vfloat4::zero();

		vfloat4 color_vec_x = vfloat4::zero();
		vfloat4 color_vec_y = vfloat4::zero();

		vfloat4 scale_vec = vfloat4::zero();

		vfloat4 weight_weight_sum = vfloat4(1e-17f);
		float psum = 1e-17f;

		for (int j = 0; j < texel_count; j++)
		{
			int tix = texel_indexes[j];

			vfloat4 rgba = blk.texel(tix);
			vfloat4 color_weight = ewb.error_weights[tix];

			// TODO: Move this calculation out to the color block?
			float ls_weight = hadd_rgb_s(color_weight);

			float idx0 = dec_weight_quant_uvalue[tix];
			if (is_decimated)
			{
				idx0 = bilinear_infill(di, dec_weight_quant_uvalue, tix);
			}

			float om_idx0 = 1.0f - idx0;
			wmin1 = astc::min(idx0, wmin1);
			wmax1 = astc::max(idx0, wmax1);

			float scale = dot3_s(scale_direction, rgba);
			scale_min = astc::min(scale, scale_min);
			scale_max = astc::max(scale, scale_max);

			vfloat4 left   = color_weight * (om_idx0 * om_idx0);
			vfloat4 middle = color_weight * (om_idx0 * idx0);
			vfloat4 right  = color_weight * (idx0 * idx0);

			vfloat4 lmrs = vfloat3(om_idx0 * om_idx0,
			                       om_idx0 * idx0,
			                       idx0 * idx0) * ls_weight;

			left_sum   += left;
			middle_sum += middle;
			right_sum  += right;
			lmrs_sum   += lmrs;

			vfloat4 color_idx(idx0);
			vfloat4 cwprod = color_weight * rgba;
			vfloat4 cwiprod = cwprod * color_idx;

			color_vec_y += cwiprod;
			color_vec_x += cwprod - cwiprod;

			scale_vec += vfloat2(om_idx0, idx0) * (ls_weight * scale);
			weight_weight_sum += color_weight * color_idx;
			psum += dot3_s(color_weight * color_idx, color_idx);
		}

		// Calculations specific to mode #7, the HDR RGB-scale mode
		vfloat4 rgbq_sum = color_vec_x + color_vec_y;
		rgbq_sum.set_lane<3>(hadd_rgb_s(color_vec_y));

		vfloat4 rgbovec = compute_rgbo_vector(rgba_weight_sum, weight_weight_sum,
		                                  rgbq_sum, psum);
		rgbo_vectors[i] = rgbovec;

		// We will occasionally get a failure due to the use of a singular (non-invertible) matrix.
		// Record whether such a failure has taken place; if it did, compute rgbo_vectors[] with a
		// different method later
		float chkval = dot_s(rgbovec, rgbovec);
		int rgbo_fail = chkval != chkval;

		// Initialize the luminance and scale vectors with a reasonable default
		float scalediv = scale_min * (1.0f / astc::max(scale_max, 1e-10f));
		scalediv = astc::clamp1f(scalediv);

		vfloat4 sds = scale_direction * scale_max;

		rgbs_vectors[i] = vfloat4(sds.lane<0>(), sds.lane<1>(), sds.lane<2>(), scalediv);

		if (wmin1 >= wmax1 * 0.999f)
		{
			// If all weights in the partition were equal, then just take average of all colors in
			// the partition and use that as both endpoint colors
			vfloat4 avg = (color_vec_x + color_vec_y) * (1.0f / rgba_weight_sum);

			vmask4 notnan_mask = avg == avg;
			ep.endpt0[i] = select(ep.endpt0[i], avg, notnan_mask);
			ep.endpt1[i] = select(ep.endpt1[i], avg, notnan_mask);

			rgbs_vectors[i] = vfloat4(sds.lane<0>(), sds.lane<1>(), sds.lane<2>(), 1.0f);
		}
		else
		{
			// Otherwise, complete the analytic calculation of ideal-endpoint-values for the given
			// set of texel weights and pixel colors
			vfloat4 color_det1 = (left_sum * right_sum) - (middle_sum * middle_sum);
			vfloat4 color_rdet1 = 1.0f / color_det1;

			float ls_det1  = (lmrs_sum.lane<0>() * lmrs_sum.lane<2>()) - (lmrs_sum.lane<1>() * lmrs_sum.lane<1>());
			float ls_rdet1 = 1.0f / ls_det1;

			vfloat4 color_mss1 = (left_sum * left_sum)
			                   + (2.0f * middle_sum * middle_sum)
			                   + (right_sum * right_sum);

			float ls_mss1 = (lmrs_sum.lane<0>() * lmrs_sum.lane<0>())
			              + (2.0f * lmrs_sum.lane<1>() * lmrs_sum.lane<1>())
			              + (lmrs_sum.lane<2>() * lmrs_sum.lane<2>());

			vfloat4 ep0 = (right_sum * color_vec_x - middle_sum * color_vec_y) * color_rdet1;
			vfloat4 ep1 = (left_sum * color_vec_y - middle_sum * color_vec_x) * color_rdet1;

			vmask4 det_mask = abs(color_det1) > (color_mss1 * 1e-4f);
			vmask4 notnan_mask = (ep0 == ep0) & (ep1 == ep1);
			vmask4 full_mask = det_mask & notnan_mask;

			ep.endpt0[i] = select(ep.endpt0[i], ep0, full_mask);
			ep.endpt1[i] = select(ep.endpt1[i], ep1, full_mask);

			float scale_ep0 = (lmrs_sum.lane<2>() * scale_vec.lane<0>() - lmrs_sum.lane<1>() * scale_vec.lane<1>()) * ls_rdet1;
			float scale_ep1 = (lmrs_sum.lane<0>() * scale_vec.lane<1>() - lmrs_sum.lane<1>() * scale_vec.lane<0>()) * ls_rdet1;

			if (fabsf(ls_det1) > (ls_mss1 * 1e-4f) && scale_ep0 == scale_ep0 && scale_ep1 == scale_ep1 && scale_ep0 < scale_ep1)
			{
				float scalediv2 = scale_ep0 * (1.0f / scale_ep1);
				vfloat4 sdsm = scale_direction * scale_ep1;
				rgbs_vectors[i] = vfloat4(sdsm.lane<0>(), sdsm.lane<1>(), sdsm.lane<2>(), scalediv2);
			}
		}

		// If the calculation of an RGB-offset vector failed, try to compute a value another way
		if (rgbo_fail)
		{
			vfloat4 v0 = ep.endpt0[i];
			vfloat4 v1 = ep.endpt1[i];

			float avgdif = hadd_rgb_s(v1 - v0) * (1.0f / 3.0f);
			avgdif = astc::max(avgdif, 0.0f);

			vfloat4 avg = (v0 + v1) * 0.5f;
			vfloat4 ep0 = avg - vfloat4(avgdif) * 0.5f;

			rgbo_vectors[i] = vfloat4(ep0.lane<0>(), ep0.lane<1>(), ep0.lane<2>(), avgdif);
		}
	}
}

/* See header for documentation. */
void recompute_ideal_colors_2planes(
	const image_block& blk,
	const error_weight_block& ewb,
	const block_size_descriptor& bsd,
	const decimation_info& di,
	int weight_quant_mode,
	const uint8_t* dec_weights_quant_pvalue_plane1,
	const uint8_t* dec_weights_quant_pvalue_plane2,
	endpoints& ep,
	vfloat4& rgbs_vector,
	vfloat4& rgbo_vector,
	int plane2_component
) {
	int weight_count = di.weight_count;
	bool is_decimated = di.weight_count != di.texel_count;

	promise(weight_count > 0);

	const quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quant_mode]);

	float dec_weights_quant_uvalue_plane1[BLOCK_MAX_WEIGHTS_2PLANE];
	float dec_weights_quant_uvalue_plane2[BLOCK_MAX_WEIGHTS_2PLANE];

	for (int i = 0; i < weight_count; i++)
	{
		dec_weights_quant_uvalue_plane1[i] = qat->unquantized_value[dec_weights_quant_pvalue_plane1[i]] * (1.0f / 64.0f);
		dec_weights_quant_uvalue_plane2[i] = qat->unquantized_value[dec_weights_quant_pvalue_plane2[i]] * (1.0f / 64.0f);
	}

	vfloat4 rgba_sum = ewb.block_error_weighted_rgba_sum;
	vfloat4 rgba_weight_sum = ewb.block_error_weight_sum;

	int texel_count = bsd.texel_count;
	vfloat4 scale_direction = normalize((rgba_sum * (1.0f / rgba_weight_sum)).swz<0, 1, 2>());

	float scale_max = 0.0f;
	float scale_min = 1e10f;

	float wmin1 = 1.0f;
	float wmax1 = 0.0f;
	float wmin2 = 1.0f;
	float wmax2 = 0.0f;

	vfloat4 left_sum    = vfloat4::zero();
	vfloat4 middle_sum  = vfloat4::zero();
	vfloat4 right_sum   = vfloat4::zero();

	vfloat4 left2_sum   = vfloat4::zero();
	vfloat4 middle2_sum = vfloat4::zero();
	vfloat4 right2_sum  = vfloat4::zero();
	vfloat4 lmrs_sum    = vfloat4::zero();

	vfloat4 color_vec_x = vfloat4::zero();
	vfloat4 color_vec_y = vfloat4::zero();

	vfloat4 scale_vec = vfloat4::zero();

	vfloat4 weight_weight_sum = vfloat4(1e-17f);
	float psum = 1e-17f;

	for (int j = 0; j < texel_count; j++)
	{
		vfloat4 rgba = blk.texel(j);
		vfloat4 color_weight = ewb.error_weights[j];

		// TODO: Move this calculation out to the color block?
		float ls_weight = hadd_rgb_s(color_weight);

		float idx0 = dec_weights_quant_uvalue_plane1[j];
		if (is_decimated)
		{
			idx0 = bilinear_infill(di, dec_weights_quant_uvalue_plane1, j);
		}

		float om_idx0 = 1.0f - idx0;
		wmin1 = astc::min(idx0, wmin1);
		wmax1 = astc::max(idx0, wmax1);

		float scale = dot3_s(scale_direction, rgba);
		scale_min = astc::min(scale, scale_min);
		scale_max = astc::max(scale, scale_max);

		vfloat4 left   = color_weight * (om_idx0 * om_idx0);
		vfloat4 middle = color_weight * (om_idx0 * idx0);
		vfloat4 right  = color_weight * (idx0 * idx0);

		vfloat4 lmrs = vfloat3(om_idx0 * om_idx0,
		                       om_idx0 * idx0,
		                       idx0 * idx0) * ls_weight;

		left_sum   += left;
		middle_sum += middle;
		right_sum  += right;
		lmrs_sum   += lmrs;

		float idx1 = dec_weights_quant_uvalue_plane2[j];
		if (is_decimated)
		{
			idx1 = bilinear_infill(di, dec_weights_quant_uvalue_plane2, j);
		}

		float om_idx1 = 1.0f - idx1;
		wmin2 = astc::min(idx1, wmin2);
		wmax2 = astc::max(idx1, wmax2);

		vfloat4 left2   = color_weight * (om_idx1 * om_idx1);
		vfloat4 middle2 = color_weight * (om_idx1 * idx1);
		vfloat4 right2  = color_weight * (idx1 * idx1);

		left2_sum   += left2;
		middle2_sum += middle2;
		right2_sum  += right2;

		vmask4 p2_mask = vint4::lane_id() == vint4(plane2_component);
		vfloat4 color_idx = select(vfloat4(idx0), vfloat4(idx1), p2_mask);

		vfloat4 cwprod = color_weight * rgba;
		vfloat4 cwiprod = cwprod * color_idx;

		color_vec_y += cwiprod;
		color_vec_x += cwprod - cwiprod;

		scale_vec += vfloat2(om_idx0, idx0) * (ls_weight * scale);
		weight_weight_sum += (color_weight * color_idx);
		psum += dot3_s(color_weight * color_idx, color_idx);
	}

	// Calculations specific to mode #7, the HDR RGB-scale mode
	vfloat4 rgbq_sum = color_vec_x + color_vec_y;
	rgbq_sum.set_lane<3>(hadd_rgb_s(color_vec_y));

	rgbo_vector = compute_rgbo_vector(rgba_weight_sum, weight_weight_sum, rgbq_sum, psum);

	// We will occasionally get a failure due to the use of a singular (non-invertible) matrix.
	// Record whether such a failure has taken place; if it did, compute rgbo_vectors[] with a
	// different method later
	float chkval = dot_s(rgbo_vector, rgbo_vector);
	int rgbo_fail = chkval != chkval;

	// Initialize the luminance and scale vectors with a reasonable default
	float scalediv = scale_min * (1.0f / astc::max(scale_max, 1e-10f));
	scalediv = astc::clamp1f(scalediv);

	vfloat4 sds = scale_direction * scale_max;

	rgbs_vector = vfloat4(sds.lane<0>(), sds.lane<1>(), sds.lane<2>(), scalediv);

	if (wmin1 >= wmax1 * 0.999f)
	{
		// If all weights in the partition were equal, then just take average of all colors in
		// the partition and use that as both endpoint colors
		vfloat4 avg = (color_vec_x + color_vec_y) * (1.0f / rgba_weight_sum);

		vmask4 p1_mask = vint4::lane_id() != vint4(plane2_component);
		vmask4 notnan_mask = avg == avg;
		vmask4 full_mask = p1_mask & notnan_mask;

		ep.endpt0[0] = select(ep.endpt0[0], avg, full_mask);
		ep.endpt1[0] = select(ep.endpt1[0], avg, full_mask);

		rgbs_vector = vfloat4(sds.lane<0>(), sds.lane<1>(), sds.lane<2>(), 1.0f);
	}
	else
	{
		// Otherwise, complete the analytic calculation of ideal-endpoint-values for the given
		// set of texel weights and pixel colors
		vfloat4 color_det1 = (left_sum * right_sum) - (middle_sum * middle_sum);
		vfloat4 color_rdet1 = 1.0f / color_det1;

		float ls_det1  = (lmrs_sum.lane<0>() * lmrs_sum.lane<2>()) - (lmrs_sum.lane<1>() * lmrs_sum.lane<1>());
		float ls_rdet1 = 1.0f / ls_det1;

		vfloat4 color_mss1 = (left_sum * left_sum)
		                   + (2.0f * middle_sum * middle_sum)
		                   + (right_sum * right_sum);

		float ls_mss1 = (lmrs_sum.lane<0>() * lmrs_sum.lane<0>())
		              + (2.0f * lmrs_sum.lane<1>() * lmrs_sum.lane<1>())
		              + (lmrs_sum.lane<2>() * lmrs_sum.lane<2>());

		vfloat4 ep0 = (right_sum * color_vec_x - middle_sum * color_vec_y) * color_rdet1;
		vfloat4 ep1 = (left_sum * color_vec_y - middle_sum * color_vec_x) * color_rdet1;

		float scale_ep0 = (lmrs_sum.lane<2>() * scale_vec.lane<0>() - lmrs_sum.lane<1>() * scale_vec.lane<1>()) * ls_rdet1;
		float scale_ep1 = (lmrs_sum.lane<0>() * scale_vec.lane<1>() - lmrs_sum.lane<1>() * scale_vec.lane<0>()) * ls_rdet1;

		vmask4 p1_mask = vint4::lane_id() != vint4(plane2_component);
		vmask4 det_mask = abs(color_det1) > (color_mss1 * 1e-4f);
		vmask4 notnan_mask = (ep0 == ep0) & (ep1 == ep1);
		vmask4 full_mask = p1_mask & det_mask & notnan_mask;

		ep.endpt0[0] = select(ep.endpt0[0], ep0, full_mask);
		ep.endpt1[0] = select(ep.endpt1[0], ep1, full_mask);

		if (fabsf(ls_det1) > (ls_mss1 * 1e-4f) && scale_ep0 == scale_ep0 && scale_ep1 == scale_ep1 && scale_ep0 < scale_ep1)
		{
			float scalediv2 = scale_ep0 * (1.0f / scale_ep1);
			vfloat4 sdsm = scale_direction * scale_ep1;
			rgbs_vector = vfloat4(sdsm.lane<0>(), sdsm.lane<1>(), sdsm.lane<2>(), scalediv2);
		}
	}

	if (wmin2 >= wmax2 * 0.999f)
	{
		// If all weights in the partition were equal, then just take average of all colors in
		// the partition and use that as both endpoint colors
		vfloat4 avg = (color_vec_x + color_vec_y) * (1.0f / rgba_weight_sum);

		vmask4 p2_mask = vint4::lane_id() == vint4(plane2_component);
		vmask4 notnan_mask = avg == avg;
		vmask4 full_mask = p2_mask & notnan_mask;

		ep.endpt0[0] = select(ep.endpt0[0], avg, full_mask);
		ep.endpt1[0] = select(ep.endpt1[0], avg, full_mask);
	}
	else
	{
		// Otherwise, complete the analytic calculation of ideal-endpoint-values for the given
		// set of texel weights and pixel colors
		vfloat4 color_det2 = (left2_sum * right2_sum) - (middle2_sum * middle2_sum);
		vfloat4 color_rdet2 = 1.0f / color_det2;

		vfloat4 color_mss2 = (left2_sum * left2_sum)
		                   + (2.0f * middle2_sum * middle2_sum)
		                   + (right2_sum * right2_sum);

		vfloat4 ep0 = (right2_sum * color_vec_x - middle2_sum * color_vec_y) * color_rdet2;
		vfloat4 ep1 = (left2_sum * color_vec_y - middle2_sum * color_vec_x) * color_rdet2;

		vmask4 p2_mask = vint4::lane_id() == vint4(plane2_component);
		vmask4 det_mask = abs(color_det2) > (color_mss2 * 1e-4f);
		vmask4 notnan_mask = (ep0 == ep0) & (ep1 == ep1);
		vmask4 full_mask = p2_mask & det_mask & notnan_mask;

		ep.endpt0[0] = select(ep.endpt0[0], ep0, full_mask);
		ep.endpt1[0] = select(ep.endpt1[0], ep1, full_mask);
	}

	// If the calculation of an RGB-offset vector failed, try to compute a value another way
	if (rgbo_fail)
	{
		vfloat4 v0 = ep.endpt0[0];
		vfloat4 v1 = ep.endpt1[0];

		float avgdif = hadd_rgb_s(v1 - v0) * (1.0f / 3.0f);
		avgdif = astc::max(avgdif, 0.0f);

		vfloat4 avg = (v0 + v1) * 0.5f;
		vfloat4 ep0 = avg - vfloat4(avgdif) * 0.5f;

		rgbo_vector = vfloat4(ep0.lane<0>(), ep0.lane<1>(), ep0.lane<2>(), avgdif);
	}
}

#endif
