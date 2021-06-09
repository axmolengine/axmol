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
 * @brief Functions for angular-sum algorithm for weight alignment.
 *
 * This algorithm works as follows:
 * - we compute a complex number P as (cos s*i, sin s*i) for each weight,
 *   where i is the input value and s is a scaling factor based on the spacing between the weights.
 * - we then add together complex numbers for all the weights.
 * - we then compute the length and angle of the resulting sum.
 *
 * This should produce the following results:
 * - perfect alignment results in a vector whose length is equal to the sum of lengths of all inputs
 * - even distribution results in a vector of length 0.
 * - all samples identical results in perfect alignment for every scaling.
 *
 * For each scaling factor within a given set, we compute an alignment factor from 0 to 1. This
 * should then result in some scalings standing out as having particularly good alignment factors;
 * we can use this to produce a set of candidate scale/shift values for various quantization levels;
 * we should then actually try them and see what happens.
 */

#include "astcenc_internal.h"
#include "astcenc_vecmathlib.h"

#include <stdio.h>
#include <cassert>
#include <cstring>


static constexpr unsigned int ANGULAR_STEPS { 40 };

// Store a reduced sin/cos table for 64 possible weight values; this causes slight quality loss
// compared to using sin() and cos() directly. Must be 2^N.
static constexpr unsigned int SINCOS_STEPS { 64 };

static_assert((ANGULAR_STEPS % ASTCENC_SIMD_WIDTH) == 0,
              "ANGULAR_STEPS must be multiple of ASTCENC_SIMD_WIDTH");

static unsigned int max_angular_steps_needed_for_quant_level[13];

// The next-to-last entry is supposed to have the value 33. This because the 32-weight mode leaves a
// double-sized hole in the middle of the weight space, so we are better off matching 33 weights.
static const unsigned int quantization_steps_for_level[13] = {
	2, 3, 4, 5, 6, 8, 10, 12, 16, 20, 24, 33, 36
};

alignas(ASTCENC_VECALIGN) static float sin_table[SINCOS_STEPS][ANGULAR_STEPS];
alignas(ASTCENC_VECALIGN) static float cos_table[SINCOS_STEPS][ANGULAR_STEPS];

/* See header for documentation. */
void prepare_angular_tables()
{
	unsigned int max_angular_steps_needed_for_quant_steps[ANGULAR_STEPS + 1];
	for (unsigned int i = 0; i < ANGULAR_STEPS; i++)
	{
		float angle_step = (float)(i + 1);

		for (unsigned int j = 0; j < SINCOS_STEPS; j++)
		{
			sin_table[j][i] = static_cast<float>(sinf((2.0f * astc::PI / (SINCOS_STEPS - 1.0f)) * angle_step * static_cast<float>(j)));
			cos_table[j][i] = static_cast<float>(cosf((2.0f * astc::PI / (SINCOS_STEPS - 1.0f)) * angle_step * static_cast<float>(j)));
		}

		max_angular_steps_needed_for_quant_steps[i + 1] = astc::min(i + 1, ANGULAR_STEPS - 1);
	}

	for (unsigned int i = 0; i < 13; i++)
	{
		max_angular_steps_needed_for_quant_level[i] = max_angular_steps_needed_for_quant_steps[quantization_steps_for_level[i]];
	}
}

/**
 * @brief Compute the angular alignment factors and offsets.
 *
 * @param      weight_count              The number of (decimated) weights.
 * @param      dec_weight_quant_uvalue   The decimated and quantized weight values.
 * @param      dec_weight_quant_sig      The significance of each weight.
 * @param      max_angular_steps         The maximum number of steps to be tested.
 * @param[out] offsets                   The output angular offsets array.
 */
static void compute_angular_offsets(
	unsigned int weight_count,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	unsigned int max_angular_steps,
	float* offsets
) {
	promise(weight_count > 0);
	promise(max_angular_steps > 0);

	alignas(ASTCENC_VECALIGN) int isamplev[BLOCK_MAX_WEIGHTS] { 0 };

	// Precompute isample; arrays are always allocated 64 elements long
	for (unsigned int i = 0; i < weight_count; i += ASTCENC_SIMD_WIDTH)
	{
		// Add 2^23 and interpreting bits extracts round-to-nearest int
		vfloat sample = loada(dec_weight_quant_uvalue + i) * (SINCOS_STEPS - 1.0f) + vfloat(12582912.0f);
		vint isample = float_as_int(sample) & vint((SINCOS_STEPS - 1));
		storea(isample, isamplev + i);
	}

	// Arrays are multiple of SIMD width (ANGULAR_STEPS), safe to overshoot max
	vfloat mult = vfloat(1.0f / (2.0f * astc::PI));

	for (unsigned int i = 0; i < max_angular_steps; i += ASTCENC_SIMD_WIDTH)
	{
		vfloat anglesum_x = vfloat::zero();
		vfloat anglesum_y = vfloat::zero();

		for (unsigned int j = 0; j < weight_count; j++)
		{
			int isample = isamplev[j];
			vfloat sample_weightv(dec_weight_quant_sig[j]);
			anglesum_x += loada(cos_table[isample] + i) * sample_weightv;
			anglesum_y += loada(sin_table[isample] + i) * sample_weightv;
		}

		vfloat angle = atan2(anglesum_y, anglesum_x);
		vfloat ofs = angle * mult;
		storea(ofs, offsets + i);
	}
}

/**
 * @brief For a given step size compute the lowest and highest weight.
 *
 * Compute the lowest and highest weight that results from quantizing using the given stepsize and
 * offset, and then compute the resulting error. The cut errors indicate the error that results from
 * forcing samples that should have had one weight value one step up or down.
 *
 * @param      weight_count              The number of (decimated) weights.
 * @param      dec_weight_quant_uvalue   The decimated and quantized weight values.
 * @param      dec_weight_quant_sig      The significance of each weight.
 * @param      max_angular_steps         The maximum number of steps to be tested.
 * @param      max_quant_steps           The maximum quantization level to be tested.
 * @param      offsets                   The angular offsets array.
 * @param[out] lowest_weight             Per angular step, the lowest weight.
 * @param[out] weight_span               Per angular step, the span between lowest and highest weight.
 * @param[out] error                     Per angular step, the error.
 * @param[out] cut_low_weight_error      Per angular step, the low weight cut error.
 * @param[out] cut_high_weight_error     Per angular step, the high weight cut error.
 */
static void compute_lowest_and_highest_weight(
	unsigned int weight_count,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	unsigned int max_angular_steps,
	unsigned int max_quant_steps,
	const float* offsets,
	int* lowest_weight,
	int* weight_span,
	float* error,
	float* cut_low_weight_error,
	float* cut_high_weight_error
) {
	promise(weight_count > 0);
	promise(max_angular_steps > 0);

	vfloat rcp_stepsize = vfloat::lane_id() + vfloat(1.0f);

	// Arrays are ANGULAR_STEPS long, so always safe to run full vectors
	for (unsigned int sp = 0; sp < max_angular_steps; sp += ASTCENC_SIMD_WIDTH)
	{
		vint minidx(128);
		vint maxidx(-128);
		vfloat errval = vfloat::zero();
		vfloat cut_low_weight_err = vfloat::zero();
		vfloat cut_high_weight_err = vfloat::zero();
		vfloat offset = loada(&offsets[sp]);

		for (unsigned int j = 0; j < weight_count; ++j)
		{
			vfloat wt = load1(&dec_weight_quant_sig[j]);
			vfloat sval = load1(&dec_weight_quant_uvalue[j]) * rcp_stepsize - offset;
			vfloat svalrte = round(sval);
			vint idxv = float_to_int(svalrte);
			vfloat dif = sval - svalrte;
			vfloat dwt = dif * wt;
			errval += dwt * dif;

			// Reset tracker on min hit
			vmask mask = idxv < minidx;
			minidx = select(minidx, idxv, mask);
			cut_low_weight_err = select(cut_low_weight_err, vfloat::zero(), mask);

			// Accumulate on min hit
			mask = idxv == minidx;
			vfloat accum = cut_low_weight_err + wt - vfloat(2.0f) * dwt;
			cut_low_weight_err = select(cut_low_weight_err, accum, mask);

			// Reset tracker on max hit
			mask = idxv > maxidx;
			maxidx = select(maxidx, idxv, mask);
			cut_high_weight_err = select(cut_high_weight_err, vfloat::zero(), mask);

			// Accumulate on max hit
			mask = idxv == maxidx;
			accum = cut_high_weight_err + wt + vfloat(2.0f) * dwt;
			cut_high_weight_err = select(cut_high_weight_err, accum, mask);
		}

		// Write out min weight and weight span; clamp span to a usable range
		vint span = maxidx - minidx + vint(1);
		span = min(span, vint(max_quant_steps + 3));
		span = max(span, vint(2));
		storea(minidx, &lowest_weight[sp]);
		storea(span, &weight_span[sp]);

		// The cut_(lowest/highest)_weight_error indicate the error that results from  forcing
		// samples that should have had the weight value one step (up/down).
		vfloat ssize = 1.0f / rcp_stepsize;
		vfloat errscale = ssize * ssize;
		storea(errval * errscale, &error[sp]);
		storea(cut_low_weight_err * errscale, &cut_low_weight_error[sp]);
		storea(cut_high_weight_err * errscale, &cut_high_weight_error[sp]);

		rcp_stepsize = rcp_stepsize + vfloat(ASTCENC_SIMD_WIDTH);
	}
}

/**
 * @brief The main function for the angular algorithm.
 *
 * @param      weight_count              The number of (decimated) weights.
 * @param      dec_weight_quant_uvalue   The decimated and quantized weight value.
 * @param      dec_weight_quant_sig      The significance of each weight.
 * @param      max_quant_level           The maximum quantization level to be tested.
 * @param[out] low_value                 Per angular step, the lowest weight value.
 * @param[out] high_value                Per angular step, the highest weight value.
 */
static void compute_angular_endpoints_for_quant_levels(
	unsigned int weight_count,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	unsigned int max_quant_level,
	float low_value[12],
	float high_value[12]
) {
	unsigned int max_quant_steps = quantization_steps_for_level[max_quant_level];

	alignas(ASTCENC_VECALIGN) float angular_offsets[ANGULAR_STEPS];
	unsigned int max_angular_steps = max_angular_steps_needed_for_quant_level[max_quant_level];
	compute_angular_offsets(weight_count, dec_weight_quant_uvalue, dec_weight_quant_sig,
	                        max_angular_steps, angular_offsets);

	alignas(ASTCENC_VECALIGN) int32_t lowest_weight[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) int32_t weight_span[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) float error[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) float cut_low_weight_error[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) float cut_high_weight_error[ANGULAR_STEPS];

	compute_lowest_and_highest_weight(weight_count, dec_weight_quant_uvalue, dec_weight_quant_sig,
	                                  max_angular_steps, max_quant_steps,
	                                  angular_offsets, lowest_weight, weight_span, error,
	                                  cut_low_weight_error, cut_high_weight_error);

	// For each quantization level, find the best error terms. Use packed vectors so data-dependent
	// branches can become selects. This involves some integer to float casts, but the values are
	// small enough so they never round the wrong way.
	vfloat4 best_results[40];

	// Initialize the array to some safe defaults
	promise(max_quant_steps > 0);
	// TODO: Why the + 4 in the current code?
	for (unsigned int i = 0; i < (max_quant_steps + 4); i++)
	{
		// Lane<0> = Best error
		// Lane<1> = Best scale; -1 indicates no solution found
		// Lane<2> = Cut low weight
		best_results[i] = vfloat4(ERROR_CALC_DEFAULT, -1.0f, 0.0f, 0.0f);
	}

	promise(max_angular_steps > 0);
	for (unsigned int i = 0; i < max_angular_steps; i++)
	{
		int idx_span = weight_span[i];
		float error_cut_low = error[i] + cut_low_weight_error[i];
		float error_cut_high = error[i] + cut_high_weight_error[i];
		float error_cut_low_high = error[i] + cut_low_weight_error[i] + cut_high_weight_error[i];

		// Check best error against record N
		vfloat4 best_result = best_results[idx_span];
		vfloat4 new_result = vfloat4(error[i], (float)i, 0.0f, 0.0f);
		vmask4 mask1(best_result.lane<0>() > error[i]);
		best_results[idx_span] = select(best_result, new_result, mask1);

		// Check best error against record N-1 with either cut low or cut high
		best_result = best_results[idx_span - 1];

		new_result = vfloat4(error_cut_low, (float)i, 1.0f, 0.0f);
		vmask4 mask2(best_result.lane<0>() > error_cut_low);
		best_result = select(best_result, new_result, mask2);

		new_result = vfloat4(error_cut_high, (float)i, 0.0f, 0.0f);
		vmask4 mask3(best_result.lane<0>() > error_cut_high);
		best_results[idx_span - 1] = select(best_result, new_result, mask3);

		// Check best error against record N-2 with both cut low and high
		best_result = best_results[idx_span - 2];
		new_result = vfloat4(error_cut_low_high, (float)i, 1.0f, 0.0f);
		vmask4 mask4(best_result.lane<0>() > error_cut_low_high);
		best_results[idx_span - 2] = select(best_result, new_result, mask4);
	}

	for (unsigned int i = 0; i <= max_quant_level; i++)
	{
		unsigned int q = quantization_steps_for_level[i];
		int bsi = (int)best_results[q].lane<1>();

		// Did we find anything?
#if !defined(NDEBUG)
		if (bsi < 0)
		{
			printf("WARNING: Unable to find encoding within specified error limit\n");
		}
#endif

		bsi = astc::max(0, bsi);

		float stepsize = 1.0f / (1.0f + (float)bsi);
		int lwi = lowest_weight[bsi] + (int)best_results[q].lane<2>();
		int hwi = lwi + q - 1;

		float offset = angular_offsets[bsi] * stepsize;
		low_value[i] = offset + static_cast<float>(lwi) * stepsize;
		high_value[i] = offset + static_cast<float>(hwi) * stepsize;
	}
}

/**
 * @brief For a given step size compute the lowest and highest weight, variant for low weight count.
 *
 * Compute the lowest and highest weight that results from quantizing using the given stepsize and
 * offset, and then compute the resulting error. The cut errors indicate the error that results from
 * forcing samples that should have had one weight value one step up or down.
 *
 * @param      weight_count              The number of (decimated) weights.
 * @param      dec_weight_quant_uvalue   The decimated and quantized weight values.
 * @param      dec_weight_quant_sig      The significance of each weight.
 * @param      max_angular_steps         The maximum number of steps to be tested.
 * @param      max_quant_steps           The maximum quantization level to be tested.
 * @param      offsets                   The angular offsets array.
 * @param[out] lowest_weight             Per angular step, the lowest weight.
 * @param[out] weight_span               Per angular step, the span between lowest and highest weight.
 * @param[out] error                     Per angular step, the error.
 */
static void compute_lowest_and_highest_weight_lwc(
	unsigned int weight_count,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	unsigned int max_angular_steps,
	unsigned int max_quant_steps,
	const float* offsets,
	int* lowest_weight,
	int* weight_span,
	float* error
) {
	promise(weight_count > 0);
	promise(max_angular_steps > 0);

	vfloat rcp_stepsize = vfloat::lane_id() + vfloat(1.0f);

	// Arrays are ANGULAR_STEPS long, so always safe to run full vectors
	for (unsigned int sp = 0; sp < max_angular_steps; sp += ASTCENC_SIMD_WIDTH)
	{
		vint minidx(128);
		vint maxidx(-128);
		vfloat errval = vfloat::zero();
		vfloat offset = loada(&offsets[sp]);

		for (unsigned int j = 0; j < weight_count; ++j)
		{
			vfloat wt = load1(&dec_weight_quant_sig[j]);
			vfloat sval = load1(&dec_weight_quant_uvalue[j]) * rcp_stepsize - offset;
			vfloat svalrte = round(sval);
			vint idxv = float_to_int(svalrte);
			vfloat dif = sval - svalrte;
			vfloat dwt = dif * wt;
			errval += dwt * dif;

			// Reset tracker on min hit
			vmask mask = idxv < minidx;
			minidx = select(minidx, idxv, mask);

			// Reset tracker on max hit
			mask = idxv > maxidx;
			maxidx = select(maxidx, idxv, mask);
		}

		// Write out min weight and weight span; clamp span to a usable range
		vint span = maxidx - minidx + vint(1);
		span = min(span, vint(max_quant_steps + 3));
		span = max(span, vint(2));
		storea(minidx, &lowest_weight[sp]);
		storea(span, &weight_span[sp]);

		// The cut_(lowest/highest)_weight_error indicate the error that results from  forcing
		// samples that should have had the weight value one step (up/down).
		vfloat ssize = 1.0f / rcp_stepsize;
		vfloat errscale = ssize * ssize;
		storea(errval * errscale, &error[sp]);

		rcp_stepsize = rcp_stepsize + vfloat(ASTCENC_SIMD_WIDTH);
	}
}

/**
 * @brief The main function for the angular algorithm, variant for low weight count.
 *
 * @param      weight_count              The number of (decimated) weights.
 * @param      dec_weight_quant_uvalue   The decimated and quantized weight value.
 * @param      dec_weight_quant_sig      The significance of each weight.
 * @param      max_quant_level           The maximum quantization level to be tested.
 * @param[out] low_value                 Per angular step, the lowest weight value.
 * @param[out] high_value                Per angular step, the highest weight value.
 */
static void compute_angular_endpoints_for_quant_levels_lwc(
	unsigned int weight_count,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	unsigned int max_quant_level,
	float low_value[12],
	float high_value[12]
) {
	unsigned int max_quant_steps = quantization_steps_for_level[max_quant_level];
	unsigned int max_angular_steps = max_angular_steps_needed_for_quant_level[max_quant_level];

	alignas(ASTCENC_VECALIGN) float angular_offsets[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) int32_t lowest_weight[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) int32_t weight_span[ANGULAR_STEPS];
	alignas(ASTCENC_VECALIGN) float error[ANGULAR_STEPS];

	compute_angular_offsets(weight_count, dec_weight_quant_uvalue, dec_weight_quant_sig,
	                        max_angular_steps, angular_offsets);


	compute_lowest_and_highest_weight_lwc(weight_count, dec_weight_quant_uvalue, dec_weight_quant_sig,
	                                      max_angular_steps, max_quant_steps,
	                                      angular_offsets, lowest_weight, weight_span, error);

	// For each quantization level, find the best error terms. Use packed vectors so data-dependent
	// branches can become selects. This involves some integer to float casts, but the values are
	// small enough so they never round the wrong way.
	float best_error[ANGULAR_STEPS];
	int best_index[ANGULAR_STEPS];

	// Initialize the array to some safe defaults
	promise(max_quant_steps > 0);
	// TODO: Why the + 4 in the current code?
	for (unsigned int i = 0; i < (max_quant_steps + 4); i++)
	{
		best_error[i] = ERROR_CALC_DEFAULT;
		best_index[i] = -1;
	}

	promise(max_angular_steps > 0);
	for (unsigned int i = 0; i < max_angular_steps; i++)
	{
		int idx_span = weight_span[i];

		// Check best error against record N
		float current_best = best_error[idx_span];
		if (error[i] < current_best)
		{
			best_error[idx_span] = error[i];
			best_index[idx_span] = i;
		}
	}

	for (unsigned int i = 0; i <= max_quant_level; i++)
	{
		unsigned int q = quantization_steps_for_level[i];
		int bsi = best_index[q];

		// Did we find anything?
#if !defined(NDEBUG)
		if (bsi < 0)
		{
			printf("WARNING: Unable to find encoding within specified error limit\n");
		}
#endif

		bsi = astc::max(0, bsi);

		int lwi = lowest_weight[bsi];
		int hwi = lwi + q - 1;

		low_value[i]  = (angular_offsets[bsi] + static_cast<float>(lwi)) / (1.0f + (float)bsi);
		high_value[i] = (angular_offsets[bsi] + static_cast<float>(hwi)) / (1.0f + (float)bsi);
	}
}

/* See header for documentation. */
void compute_angular_endpoints_1plane(
	unsigned int tune_low_weight_limit,
	bool only_always,
	const block_size_descriptor& bsd,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	float low_value[WEIGHTS_MAX_BLOCK_MODES],
	float high_value[WEIGHTS_MAX_BLOCK_MODES]
) {
	float low_values[WEIGHTS_MAX_DECIMATION_MODES][12];
	float high_values[WEIGHTS_MAX_DECIMATION_MODES][12];

	promise(bsd.decimation_mode_count > 0);
	for (unsigned int i = 0; i < bsd.decimation_mode_count; i++)
	{
		const decimation_mode& dm = bsd.decimation_modes[i];
		if (dm.maxprec_1plane < 0 || (only_always && !dm.percentile_always) || !dm.percentile_hit)
		{
			continue;
		}

		unsigned int weight_count = bsd.decimation_tables[i]->weight_count;

		if (weight_count < tune_low_weight_limit)
		{
			compute_angular_endpoints_for_quant_levels_lwc(
				weight_count,
				dec_weight_quant_uvalue + i * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + i * BLOCK_MAX_WEIGHTS,
				dm.maxprec_1plane, low_values[i], high_values[i]);
		}
		else
		{
			compute_angular_endpoints_for_quant_levels(
				weight_count,
				dec_weight_quant_uvalue + i * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + i * BLOCK_MAX_WEIGHTS,
				dm.maxprec_1plane, low_values[i], high_values[i]);
		}
	}

	promise(bsd.block_mode_count > 0);
	for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
	{
		const block_mode& bm = bsd.block_modes[i];
		if (bm.is_dual_plane || (only_always && !bm.percentile_always) || !bm.percentile_hit)
		{
			continue;
		}

		unsigned int quant_mode = bm.quant_mode;
		unsigned int decim_mode = bm.decimation_mode;

		low_value[i] = low_values[decim_mode][quant_mode];
		high_value[i] = high_values[decim_mode][quant_mode];
	}
}

/* See header for documentation. */
void compute_angular_endpoints_2planes(
	unsigned int tune_low_weight_limit,
	const block_size_descriptor& bsd,
	const float* dec_weight_quant_uvalue,
	const float* dec_weight_quant_sig,
	float low_value1[WEIGHTS_MAX_BLOCK_MODES],
	float high_value1[WEIGHTS_MAX_BLOCK_MODES],
	float low_value2[WEIGHTS_MAX_BLOCK_MODES],
	float high_value2[WEIGHTS_MAX_BLOCK_MODES]
) {
	float low_values1[WEIGHTS_MAX_DECIMATION_MODES][12];
	float high_values1[WEIGHTS_MAX_DECIMATION_MODES][12];
	float low_values2[WEIGHTS_MAX_DECIMATION_MODES][12];
	float high_values2[WEIGHTS_MAX_DECIMATION_MODES][12];

	promise(bsd.decimation_mode_count > 0);
	for (unsigned int i = 0; i < bsd.decimation_mode_count; i++)
	{
		const decimation_mode& dm = bsd.decimation_modes[i];
		if (dm.maxprec_2planes < 0 || !dm.percentile_hit)
		{
			continue;
		}

		unsigned int weight_count = bsd.decimation_tables[i]->weight_count;

		if (weight_count < tune_low_weight_limit)
		{
			compute_angular_endpoints_for_quant_levels_lwc(
				weight_count,
				dec_weight_quant_uvalue + 2 * i * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + 2 * i * BLOCK_MAX_WEIGHTS,
				dm.maxprec_2planes, low_values1[i], high_values1[i]);

			compute_angular_endpoints_for_quant_levels_lwc(
				weight_count,
				dec_weight_quant_uvalue + (2 * i + 1) * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + (2 * i + 1) * BLOCK_MAX_WEIGHTS,
				dm.maxprec_2planes, low_values2[i], high_values2[i]);
		}
		else
		{
			compute_angular_endpoints_for_quant_levels(
				weight_count,
				dec_weight_quant_uvalue + 2 * i * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + 2 * i * BLOCK_MAX_WEIGHTS,
				dm.maxprec_2planes, low_values1[i], high_values1[i]);

			compute_angular_endpoints_for_quant_levels(
				weight_count,
				dec_weight_quant_uvalue + (2 * i + 1) * BLOCK_MAX_WEIGHTS,
				dec_weight_quant_sig + (2 * i + 1) * BLOCK_MAX_WEIGHTS,
				dm.maxprec_2planes, low_values2[i], high_values2[i]);
		}
	}

	promise(bsd.block_mode_count > 0);
	for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
	{
		const block_mode& bm = bsd.block_modes[i];
		if (!bm.is_dual_plane || !bm.percentile_hit)
		{
			continue;
		}

		unsigned int quant_mode = bm.quant_mode;
		unsigned int decim_mode = bm.decimation_mode;

		low_value1[i] = low_values1[decim_mode][quant_mode];
		high_value1[i] = high_values1[decim_mode][quant_mode];
		low_value2[i] = low_values2[decim_mode][quant_mode];
		high_value2[i] = high_values2[decim_mode][quant_mode];
	}
}

#endif
