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
 * @brief Functions to compress a symbolic block.
 */

#include "astcenc_internal.h"
#include "astcenc_diagnostic_trace.h"

#include <cassert>

/**
 * @brief Merge two planes of endpoints into a single vector.
 *
 * @param      ep_plane1          The endpoints for plane 1.
 * @param      ep_plane2          The endpoints for plane 2.
 * @param      component_plane2   The color component for plane 2.
 * @param[out] result             The merged output.
 */
static void merge_endpoints(
	const endpoints& ep_plane1,
	const endpoints& ep_plane2,
	unsigned int component_plane2,
	endpoints& result
) {
	unsigned int partition_count = ep_plane1.partition_count;
	vmask4 sep_mask = vint4::lane_id() == vint4(component_plane2);

	result.partition_count = partition_count;
	promise(partition_count > 0);
	for (unsigned int i = 0; i < partition_count; i++)
	{
		result.endpt0[i] = select(ep_plane1.endpt0[i], ep_plane2.endpt0[i], sep_mask);
		result.endpt1[i] = select(ep_plane1.endpt1[i], ep_plane2.endpt1[i], sep_mask);
	}
}

/**
 * @brief Attempt to improve weights given a chosen configuration.
 *
 * Given a fixed weight grid decimation and weight value quantization, iterate over all weights (per
 * partition and per plane) and attempt to improve image quality by moving each weight up by one or
 * down by one quantization step.
 *
 * @param      decode_mode          The decode mode (LDR, HDR).
 * @param      bsd                  The block size information.
 * @param      blk                  The image block color data to compress.
 * @param      ewb                  The image block weighted error data.
 * @param[out] scb                  The symbolic compressed block output.
 * @param[out] weight_set8_plane1   The quantized weights for plane 1.
 * @param[out] weight_set8_plane2   The quantized weights for plane 2, @c nullptr if 1 plane.
 */
static bool realign_weights(
	astcenc_profile decode_mode,
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	symbolic_compressed_block& scb,
	uint8_t* weight_set8_plane1,
	uint8_t* weight_set8_plane2
) {
	// Get the partition descriptor
	unsigned int partition_count = scb.partition_count;
	const auto& pi = bsd.get_partition_info(partition_count, scb.partition_index);

	// Get the quantization table
	const block_mode& bm = bsd.get_block_mode(scb.block_mode);
	unsigned int weight_quant_level = bm.quant_mode;
	const quantization_and_transfer_table *qat = &(quant_and_xfer_tables[weight_quant_level]);

	// Get the decimation table
	const decimation_info& di = *(bsd.decimation_tables[bm.decimation_mode]);
	unsigned int weight_count = di.weight_count;

	unsigned int max_plane = bm.is_dual_plane;
	int plane2_component = bm.is_dual_plane ? scb.plane2_component : -1;
	vmask4 plane_mask = vint4::lane_id() == vint4(plane2_component);

	// Decode the color endpoints
	bool rgb_hdr;
	bool alpha_hdr;
	vint4 endpnt0[BLOCK_MAX_PARTITIONS];
	vint4 endpnt1[BLOCK_MAX_PARTITIONS];
	vfloat4 endpnt0f[BLOCK_MAX_PARTITIONS];
	vfloat4 offset[BLOCK_MAX_PARTITIONS];

	promise(partition_count > 0);
	promise(weight_count > 0);

	for (unsigned int pa_idx = 0; pa_idx < partition_count; pa_idx++)
	{
		unpack_color_endpoints(decode_mode,
		                       scb.color_formats[pa_idx],
		                       scb.get_color_quant_mode(),
		                       scb.color_values[pa_idx],
		                       rgb_hdr, alpha_hdr,
		                       endpnt0[pa_idx],
		                       endpnt1[pa_idx]);
	}

	uint8_t uq_pl_weights[BLOCK_MAX_WEIGHTS];
	uint8_t* weight_set8 = weight_set8_plane1;
	bool adjustments = false;

	// For each plane and partition ...
	for (unsigned int pl_idx = 0; pl_idx <= max_plane; pl_idx++)
	{
		for (unsigned int pa_idx = 0; pa_idx < partition_count; pa_idx++)
		{
			// Compute the endpoint delta for all components in current plane
			vint4 epd = endpnt1[pa_idx] - endpnt0[pa_idx];
			epd = select(epd, vint4::zero(), plane_mask);

			endpnt0f[pa_idx] = int_to_float(endpnt0[pa_idx]);
			offset[pa_idx] = int_to_float(epd) * (1.0f / 64.0f);
		}

		// Create an unquantized weight grid for this decimation level
		for (unsigned int we_idx = 0; we_idx < weight_count; we_idx++)
		{
			uq_pl_weights[we_idx] = qat->unquantized_value[weight_set8[we_idx]];
		}

		// For each weight compute previous, current, and next errors
		for (unsigned int we_idx = 0; we_idx < weight_count; we_idx++)
		{
			unsigned int uqw = uq_pl_weights[we_idx];

			uint32_t prev_and_next = qat->prev_next_values[uqw];
			unsigned int prev_wt_uq = prev_and_next & 0xFF;
			unsigned int next_wt_uq = (prev_and_next >> 8) & 0xFF;

			int uqw_next_dif = next_wt_uq - uqw;
			int uqw_prev_dif = prev_wt_uq - uqw;

			float current_error = 0.0f;
			float up_error = 0.0f;
			float down_error = 0.0f;

			// Interpolate the colors to create the diffs
			unsigned int texels_to_evaluate = di.weight_texel_count[we_idx];
			promise(texels_to_evaluate > 0);
			for (unsigned int te_idx = 0; te_idx < texels_to_evaluate; te_idx++)
			{
				unsigned int texel = di.weight_texel[te_idx][we_idx];
				const uint8_t *texel_weights = di.texel_weights_texel[we_idx][te_idx];
				const float *texel_weights_float = di.texel_weights_float_texel[we_idx][te_idx];
				float twf0 = texel_weights_float[0];
				float weight_base =
				    ((static_cast<float>(uqw) * twf0
				    + static_cast<float>(uq_pl_weights[texel_weights[1]])  * texel_weights_float[1])
				    + (static_cast<float>(uq_pl_weights[texel_weights[2]]) * texel_weights_float[2]
				    + static_cast<float>(uq_pl_weights[texel_weights[3]]) * texel_weights_float[3]));

				unsigned int partition = pi.partition_of_texel[texel];

				weight_base = weight_base + 0.5f;
				float plane_weight = astc::flt_rd(weight_base);
				float plane_up_weight = astc::flt_rd(weight_base + static_cast<float>(uqw_next_dif) * twf0) - plane_weight;
				float plane_down_weight = astc::flt_rd(weight_base + static_cast<float>(uqw_prev_dif) * twf0) - plane_weight;

				vfloat4 color_offset = offset[partition];
				vfloat4 color_base   = endpnt0f[partition];

				vfloat4 color = color_base + color_offset * plane_weight;

				vfloat4 origcolor    = blk.texel(texel);
				vfloat4 error_weight = ewb.error_weights[texel];

				vfloat4 colordiff       = color - origcolor;
				vfloat4 color_up_diff   = colordiff + color_offset * plane_up_weight;
				vfloat4 color_down_diff = colordiff + color_offset * plane_down_weight;
				current_error += dot_s(colordiff       * colordiff,       error_weight);
				up_error      += dot_s(color_up_diff   * color_up_diff,   error_weight);
				down_error    += dot_s(color_down_diff * color_down_diff, error_weight);
			}

			// Check if the prev or next error is better, and if so use it
			if ((up_error < current_error) && (up_error < down_error))
			{
				uq_pl_weights[we_idx] = next_wt_uq;
				weight_set8[we_idx] = (uint8_t)((prev_and_next >> 24) & 0xFF);
				adjustments = true;
			}
			else if (down_error < current_error)
			{
				uq_pl_weights[we_idx] = prev_wt_uq;
				weight_set8[we_idx] = (uint8_t)((prev_and_next >> 16) & 0xFF);
				adjustments = true;
			}
		}

		// Prepare iteration for plane 2
		weight_set8 = weight_set8_plane2;
		plane_mask = ~plane_mask;
	}

	return adjustments;
}

/**
 * @brief Compress a block using a chosen partitioning and 1 plane of weights.
 *
 * @param      config                    The compressor configuration.
 * @param      bsd                       The block size information.
 * @param      blk                       The image block color data to compress.
 * @param      ewb                       The image block weighted error data.
 * @param      only_always               True if we only use "always" percentile block modes.
 * @param      tune_errorval_threshold   The error value threshold.
 * @param      partition_count           The partition count.
 * @param      partition_index           The partition index if @c partition_count is 2-4.
 * @param[out] scb                       The symbolic compressed block output.
 * @param[out] tmpbuf                    The quantized weights for plane 1.
 */
static float compress_symbolic_block_for_partition_1plane(
	const astcenc_config& config,
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	bool only_always,
	float tune_errorval_threshold,
	unsigned int partition_count,
	unsigned int partition_index,
	symbolic_compressed_block& scb,
	compression_working_buffers& tmpbuf
) {
	promise(partition_count > 0);
	promise(config.tune_candidate_limit > 0);
	promise(config.tune_refinement_limit > 0);
	promise(bsd.decimation_mode_count > 0);

	static const int free_bits_for_partition_count[5] = {
		0, 115 - 4, 111 - 4 - PARTITION_INDEX_BITS, 108 - 4 - PARTITION_INDEX_BITS, 105 - 4 - PARTITION_INDEX_BITS
	};

	const auto& pi = bsd.get_partition_info(partition_count, partition_index);

	// Compute ideal weights and endpoint colors, with no quantization or decimation
	endpoints_and_weights& ei = tmpbuf.ei1;
	endpoints_and_weights *eix = tmpbuf.eix1;
	compute_ideal_colors_and_weights_1plane(bsd, blk, ewb, pi, ei);

	// Compute ideal weights and endpoint colors for every decimation
	float *decimated_quantized_weights = tmpbuf.decimated_quantized_weights;
	float *decimated_weights = tmpbuf.decimated_weights;
	float *flt_quantized_decimated_quantized_weights = tmpbuf.flt_quantized_decimated_quantized_weights;
	uint8_t *u8_quantized_decimated_quantized_weights = tmpbuf.u8_quantized_decimated_quantized_weights;

	// For each decimation mode, compute an ideal set of weights with no quantization
	for (unsigned int i = 0; i < bsd.decimation_mode_count; i++)
	{
		const auto& dm = bsd.get_decimation_mode(i);
		if (dm.maxprec_1plane < 0 || (only_always && !dm.percentile_always) || !dm.percentile_hit)
		{
			continue;
		}

		const auto& di = bsd.get_decimation_info(i);

		compute_ideal_weights_for_decimation(
		    ei,
		    eix[i],
		    di,
		    decimated_quantized_weights + i * BLOCK_MAX_WEIGHTS,
		    decimated_weights + i * BLOCK_MAX_WEIGHTS);
	}

	// Compute maximum colors for the endpoints and ideal weights, then for each endpoint and ideal
	// weight pair, compute the smallest weight that will result in a color value greater than 1
	vfloat4 min_ep(10.0f);
	for (unsigned int i = 0; i < partition_count; i++)
	{
		vfloat4 ep = (vfloat4(1.0f) - ei.ep.endpt0[i]) / (ei.ep.endpt1[i] - ei.ep.endpt0[i]);

		vmask4 use_ep = (ep > vfloat4(0.5f)) & (ep < min_ep);
		min_ep = select(min_ep, ep, use_ep);
	}

	float min_wt_cutoff = hmin_s(min_ep);

	// For each mode, use the angular method to compute a shift
	float weight_low_value[WEIGHTS_MAX_BLOCK_MODES];
	float weight_high_value[WEIGHTS_MAX_BLOCK_MODES];

	compute_angular_endpoints_1plane(
	    only_always, bsd,
	    decimated_quantized_weights, decimated_weights,
	    weight_low_value, weight_high_value);

	// For each mode (which specifies a decimation and a quantization):
	//     * Compute number of bits needed for the quantized weights
	//     * Generate an optimized set of quantized weights
	//     * Compute quantization errors for the mode

	int qwt_bitcounts[WEIGHTS_MAX_BLOCK_MODES];
	float qwt_errors[WEIGHTS_MAX_BLOCK_MODES];

	for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
	{
		const block_mode& bm = bsd.block_modes[i];
		if (bm.is_dual_plane || (only_always && !bm.percentile_always) || !bm.percentile_hit)
		{
			qwt_errors[i] = 1e38f;
			continue;
		}

		if (weight_high_value[i] > 1.02f * min_wt_cutoff)
		{
			weight_high_value[i] = 1.0f;
		}

		int decimation_mode = bm.decimation_mode;
		const auto& di = bsd.get_decimation_info(decimation_mode);

		// Compute weight bitcount for the mode
		unsigned int bits_used_by_weights = get_ise_sequence_bitcount(
		    di.weight_count,
		    bm.get_weight_quant_mode());

		int bitcount = free_bits_for_partition_count[partition_count] - bits_used_by_weights;
		if (bitcount <= 0 || bits_used_by_weights < 24 || bits_used_by_weights > 96)
		{
			qwt_errors[i] = 1e38f;
			continue;
		}
		qwt_bitcounts[i] = bitcount;

		// Generate the optimized set of weights for the weight mode
		compute_quantized_weights_for_decimation(
		    di,
		    weight_low_value[i], weight_high_value[i],
		    decimated_quantized_weights + BLOCK_MAX_WEIGHTS * decimation_mode,
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * i,
		    u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * i,
		    bm.get_weight_quant_mode());

		// Compute weight quantization errors for the block mode
		qwt_errors[i] = compute_error_of_weight_set_1plane(
		    eix[decimation_mode],
		    di,
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * i);
	}

	// Decide the optimal combination of color endpoint encodings and weight encodings
	int partition_format_specifiers[TUNE_MAX_TRIAL_CANDIDATES][BLOCK_MAX_PARTITIONS];
	int block_mode_index[TUNE_MAX_TRIAL_CANDIDATES];

	quant_method color_quant_level[TUNE_MAX_TRIAL_CANDIDATES];
	quant_method color_quant_level_mod[TUNE_MAX_TRIAL_CANDIDATES];

	unsigned int candidate_count = compute_ideal_endpoint_formats(
	    bsd, pi, blk, ewb, ei.ep, qwt_bitcounts, qwt_errors,
	    config.tune_candidate_limit, partition_format_specifiers, block_mode_index,
	    color_quant_level, color_quant_level_mod);

	// Iterate over the N believed-to-be-best modes to find out which one is actually best
	float best_errorval_in_mode = 1e30f;
	float best_errorval_in_scb = scb.errorval;

	for (unsigned int i = 0; i < candidate_count; i++)
	{
		TRACE_NODE(node0, "candidate");

		const int bm_packed_index = block_mode_index[i];
		assert(bm_packed_index >= 0 && bm_packed_index < (int)bsd.block_mode_count);
		const block_mode& qw_bm = bsd.block_modes[bm_packed_index];

		int decimation_mode = qw_bm.decimation_mode;
		int weight_quant_mode = qw_bm.quant_mode;
		const auto& di = bsd.get_decimation_info(decimation_mode);
		promise(di.weight_count > 0);

		trace_add_data("weight_x", di.weight_x);
		trace_add_data("weight_y", di.weight_y);
		trace_add_data("weight_z", di.weight_z);
		trace_add_data("weight_quant", weight_quant_mode);

		// Recompute the ideal color endpoints before storing them
		vfloat4 rgbs_colors[BLOCK_MAX_PARTITIONS];
		vfloat4 rgbo_colors[BLOCK_MAX_PARTITIONS];

		symbolic_compressed_block workscb;

		for (unsigned int j = 0; j < BLOCK_MAX_COMPONENTS; j++)
		{
			workscb.constant_color[j] = 0;
		}

		uint8_t* u8_weight_src = u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * bm_packed_index;

		for (unsigned int j = 0; j < di.weight_count; j++)
		{
			workscb.weights[j] = u8_weight_src[j];
		}

		for (unsigned int l = 0; l < config.tune_refinement_limit; l++)
		{
			recompute_ideal_colors_1plane(
			    blk, ewb, pi, di,
			    weight_quant_mode, workscb.weights,
			    eix[decimation_mode].ep, rgbs_colors, rgbo_colors);

			// Quantize the chosen color
			for (unsigned int j = 0; j < partition_count; j++)
			{
				workscb.color_formats[j] = pack_color_endpoints(
				    eix[decimation_mode].ep.endpt0[j],
				    eix[decimation_mode].ep.endpt1[j],
				    rgbs_colors[j],
				    rgbo_colors[j],
				    partition_format_specifiers[i][j],
				    workscb.color_values[j],
				    (quant_method)color_quant_level[i]);
			}

			// If all the color endpoint modes are the same, we get a few more bits to store colors;
			// let's see if we can take advantage of this: requantize all the colors and see if the
			// endpoint modes remain the same.
			workscb.color_formats_matched = 0;

			if ((partition_count >= 2 && workscb.color_formats[0] == workscb.color_formats[1]
			    && color_quant_level[i] != color_quant_level_mod[i])
			    && (partition_count == 2 || (workscb.color_formats[0] == workscb.color_formats[2]
			    && (partition_count == 3 || (workscb.color_formats[0] == workscb.color_formats[3])))))
			{
				uint8_t colorvals[BLOCK_MAX_PARTITIONS][12];
				int color_formats_mod[BLOCK_MAX_PARTITIONS] { 0 };
				for (unsigned int j = 0; j < partition_count; j++)
				{
					color_formats_mod[j] = pack_color_endpoints(
					    eix[decimation_mode].ep.endpt0[j],
					    eix[decimation_mode].ep.endpt1[j],
					    rgbs_colors[j],
					    rgbo_colors[j],
					    partition_format_specifiers[i][j],
					    colorvals[j],
					    (quant_method)color_quant_level_mod[i]);
				}

				if (color_formats_mod[0] == color_formats_mod[1]
				    && (partition_count == 2 || (color_formats_mod[0] == color_formats_mod[2]
				    && (partition_count == 3 || (color_formats_mod[0] == color_formats_mod[3])))))
				{
					workscb.color_formats_matched = 1;
					for (unsigned int j = 0; j < BLOCK_MAX_PARTITIONS; j++)
					{
						for (unsigned int k = 0; k < 8; k++)
						{
							workscb.color_values[j][k] = colorvals[j][k];
						}

						workscb.color_formats[j] = color_formats_mod[j];
					}
				}
			}

			// Store header fields
			workscb.partition_count = partition_count;
			workscb.partition_index = partition_index;
			workscb.quant_mode = workscb.color_formats_matched ? color_quant_level_mod[i] : color_quant_level[i];
			workscb.block_mode = qw_bm.mode_index;
			workscb.block_type = SYM_BTYPE_NONCONST;

			if (workscb.quant_mode < QUANT_6)
			{
				workscb.block_type = SYM_BTYPE_ERROR;
			}

			// Pre-realign test
			if (l == 0)
			{
				float errorval = compute_symbolic_block_difference(config, bsd, workscb, blk, ewb);
				if (errorval == -1e30f)
				{
					errorval = -errorval;
					workscb.block_type = SYM_BTYPE_ERROR;
				}

				trace_add_data("error_prerealign", errorval);
				best_errorval_in_mode = astc::min(errorval, best_errorval_in_mode);

				// Average refinement improvement is 3.5% per iteration (allow 5%), but the first
				// iteration can help more so we give it a extra 10% leeway. Use this knowledge to
				// drive a heuristic to skip blocks that are unlikely to catch up with the best
				// block we have already.
				unsigned int iters_remaining = config.tune_refinement_limit - l;
				float threshold = (0.05f * static_cast<float>(iters_remaining)) + 1.1f;
				if (errorval > (threshold * best_errorval_in_scb))
				{
					break;
				}

				if (errorval < best_errorval_in_scb)
				{
					best_errorval_in_scb = errorval;
					workscb.errorval = errorval;
					scb = workscb;

					if (errorval < tune_errorval_threshold)
					{
						return errorval;
					}
				}
			}

			// Perform a final pass over the weights to try to improve them.
			bool adjustments = realign_weights(
			    config.profile, bsd, blk, ewb, workscb,
			    workscb.weights, nullptr);

			// Post-realign test
			float errorval = compute_symbolic_block_difference(config, bsd, workscb, blk, ewb);
			if (errorval == -1e30f)
			{
				errorval = -errorval;
				workscb.block_type = SYM_BTYPE_ERROR;
			}

			trace_add_data("error_postrealign", errorval);
			best_errorval_in_mode = astc::min(errorval, best_errorval_in_mode);

			// Average refinement improvement is 3.5% per iteration, so skip blocks that are
			// unlikely to catch up with the best block we have already. Assume a 5% per step to
			// give benefit of the doubt ...
			unsigned int iters_remaining = config.tune_refinement_limit - 1 - l;
			float threshold = (0.05f * static_cast<float>(iters_remaining)) + 1.0f;
			if (errorval > (threshold * best_errorval_in_scb))
			{
				break;
			}

			if (errorval < best_errorval_in_scb)
			{
				best_errorval_in_scb = errorval;
				workscb.errorval = errorval;
				scb = workscb;

				if (errorval < tune_errorval_threshold)
				{
					return errorval;
				}
			}

			if (!adjustments)
			{
				break;
			}
		}
	}

	return best_errorval_in_mode;
}

/**
 * @brief Compress a block using a chosen partitioning and 2 planes of weights.
 *
 * @param      config                    The compressor configuration.
 * @param      bsd                       The block size information.
 * @param      blk                       The image block color data to compress.
 * @param      ewb                       The image block weighted error data.
 * @param      tune_errorval_threshold   The error value threshold.
 * @param      partition_count           The partition count.
 * @param      partition_index           The partition index if @c partition_count is 2-4.
 * @param      plane2_component          The component index for the second plane of weights.
 * @param[out] scb                       The symbolic compressed block output.
 * @param[out] tmpbuf                    The quantized weights for plane 1.
 */
static float compress_symbolic_block_for_partition_2planes(
	const astcenc_config& config,
	const block_size_descriptor& bsd,
	const image_block& blk,
	const error_weight_block& ewb,
	float tune_errorval_threshold,
	unsigned int partition_count,
	unsigned int partition_index,
	unsigned int plane2_component,
	symbolic_compressed_block& scb,
	compression_working_buffers& tmpbuf
) {
	promise(partition_count > 0);
	promise(config.tune_candidate_limit > 0);
	promise(config.tune_refinement_limit > 0);
	promise(bsd.decimation_mode_count > 0);

	static const int free_bits_for_partition_count[5] = {
		0, 113 - 4, 109 - 4 - PARTITION_INDEX_BITS, 106 - 4 - PARTITION_INDEX_BITS, 103 - 4 - PARTITION_INDEX_BITS
	};

	const auto& pi = bsd.get_partition_info(partition_count, partition_index);

	// Compute ideal weights and endpoint colors, with no quantization or decimation
	endpoints_and_weights& ei1 = tmpbuf.ei1;
	endpoints_and_weights& ei2 = tmpbuf.ei2;
	endpoints_and_weights* eix1 = tmpbuf.eix1;
	endpoints_and_weights* eix2 = tmpbuf.eix2;
	compute_ideal_colors_and_weights_2planes(bsd, blk, ewb, pi, plane2_component, ei1, ei2);

	// Compute ideal weights and endpoint colors for every decimation
	float *decimated_quantized_weights = tmpbuf.decimated_quantized_weights;
	float *decimated_weights = tmpbuf.decimated_weights;
	float *flt_quantized_decimated_quantized_weights = tmpbuf.flt_quantized_decimated_quantized_weights;
	uint8_t *u8_quantized_decimated_quantized_weights = tmpbuf.u8_quantized_decimated_quantized_weights;

	// For each decimation mode, compute an ideal set of weights with no quantization
	for (unsigned int i = 0; i < bsd.decimation_mode_count; i++)
	{
		const auto& dm = bsd.get_decimation_mode(i);
		if (dm.maxprec_2planes < 0 || !dm.percentile_hit)
		{
			continue;
		}

		const auto& di = bsd.get_decimation_info(i);

		compute_ideal_weights_for_decimation(
		    ei1,
		    eix1[i],
		    di,
		    decimated_quantized_weights + (2 * i) * BLOCK_MAX_WEIGHTS,
		    decimated_weights + (2 * i) * BLOCK_MAX_WEIGHTS);

		compute_ideal_weights_for_decimation(
		    ei2,
		    eix2[i],
		    di,
		    decimated_quantized_weights + (2 * i + 1) * BLOCK_MAX_WEIGHTS,
		    decimated_weights + (2 * i + 1) * BLOCK_MAX_WEIGHTS);
	}

	// Compute maximum colors for the endpoints and ideal weights, then for each endpoint and ideal
	// weight pair, compute the smallest weight that will result in a color value greater than 1
	vfloat4 min_ep1(10.0f);
	vfloat4 min_ep2(10.0f);
	for (unsigned int i = 0; i < partition_count; i++)
	{
		vfloat4 ep1 = (vfloat4(1.0f) - ei1.ep.endpt0[i]) / (ei1.ep.endpt1[i] - ei1.ep.endpt0[i]);
		vmask4 use_ep1 = (ep1 > vfloat4(0.5f)) & (ep1 < min_ep1);
		min_ep1 = select(min_ep1, ep1, use_ep1);

		vfloat4 ep2 = (vfloat4(1.0f) - ei2.ep.endpt0[i]) / (ei2.ep.endpt1[i] - ei2.ep.endpt0[i]);
		vmask4 use_ep2 = (ep2 > vfloat4(0.5f)) & (ep2 < min_ep2);
		min_ep2 = select(min_ep2, ep2, use_ep2);
	}

	vfloat4 err_max(1e30f);
	vmask4 err_mask = vint4::lane_id() == vint4(plane2_component);

	// Set the plane2 component to max error in ep1
	min_ep1 = select(min_ep1, err_max, err_mask);

	float min_wt_cutoff1 = hmin_s(min_ep1);

	// Set the minwt2 to the plane2 component min in ep2
	float min_wt_cutoff2 = hmin_s(select(err_max, min_ep2, err_mask));

	float weight_low_value1[WEIGHTS_MAX_BLOCK_MODES];
	float weight_high_value1[WEIGHTS_MAX_BLOCK_MODES];
	float weight_low_value2[WEIGHTS_MAX_BLOCK_MODES];
	float weight_high_value2[WEIGHTS_MAX_BLOCK_MODES];

	compute_angular_endpoints_2planes(
	    bsd, decimated_quantized_weights, decimated_weights,
	    weight_low_value1, weight_high_value1,
	    weight_low_value2, weight_high_value2);

	// For each mode (which specifies a decimation and a quantization):
	//     * Compute number of bits needed for the quantized weights
	//     * Generate an optimized set of quantized weights
	//     * Compute quantization errors for the mode

	int qwt_bitcounts[WEIGHTS_MAX_BLOCK_MODES];
	float qwt_errors[WEIGHTS_MAX_BLOCK_MODES];
	for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
	{
		const block_mode& bm = bsd.block_modes[i];
		if (!bm.is_dual_plane || !bm.percentile_hit)
		{
			qwt_errors[i] = 1e38f;
			continue;
		}

		unsigned int decimation_mode = bm.decimation_mode;
		const auto& di = bsd.get_decimation_info(decimation_mode);

		if (weight_high_value1[i] > 1.02f * min_wt_cutoff1)
		{
			weight_high_value1[i] = 1.0f;
		}

		if (weight_high_value2[i] > 1.02f * min_wt_cutoff2)
		{
			weight_high_value2[i] = 1.0f;
		}

		// Compute weight bitcount for the mode
		unsigned int bits_used_by_weights = get_ise_sequence_bitcount(
		    2 * di.weight_count,
		    bm.get_weight_quant_mode());
		int bitcount = free_bits_for_partition_count[partition_count] - bits_used_by_weights;
		if (bitcount <= 0 || bits_used_by_weights < 24 || bits_used_by_weights > 96)
		{
			qwt_errors[i] = 1e38f;
			continue;
		}
		qwt_bitcounts[i] = bitcount;

		// Generate the optimized set of weights for the mode
		compute_quantized_weights_for_decimation(
		    di,
		    weight_low_value1[i],
		    weight_high_value1[i],
		    decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * decimation_mode),
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i),
		    u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i), bm.get_weight_quant_mode());

		compute_quantized_weights_for_decimation(
		    di,
		    weight_low_value2[i],
		    weight_high_value2[i],
		    decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * decimation_mode + 1),
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i + 1),
		    u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i + 1), bm.get_weight_quant_mode());

		// Compute weight quantization errors for the block mode
		qwt_errors[i] = compute_error_of_weight_set_2planes(
		    eix1[decimation_mode],
		    eix2[decimation_mode],
		    di,
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i),
		    flt_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * i + 1));
	}

	// Decide the optimal combination of color endpoint encodings and weight encodings
	int partition_format_specifiers[TUNE_MAX_TRIAL_CANDIDATES][BLOCK_MAX_PARTITIONS];
	int block_mode_index[TUNE_MAX_TRIAL_CANDIDATES];

	quant_method color_quant_level[TUNE_MAX_TRIAL_CANDIDATES];
	quant_method color_quant_level_mod[TUNE_MAX_TRIAL_CANDIDATES];

	endpoints epm;
	merge_endpoints(ei1.ep, ei2.ep, plane2_component, epm);

	unsigned int candidate_count = compute_ideal_endpoint_formats(
	    bsd, pi, blk, ewb, epm, qwt_bitcounts, qwt_errors,
	    config.tune_candidate_limit, partition_format_specifiers, block_mode_index,
	    color_quant_level, color_quant_level_mod);

	// Iterate over the N believed-to-be-best modes to find out which one is actually best
	float best_errorval_in_mode = 1e30f;
	float best_errorval_in_scb = scb.errorval;

	for (unsigned int i = 0; i < candidate_count; i++)
	{
		TRACE_NODE(node0, "candidate");

		const int bm_packed_index = block_mode_index[i];
		assert(bm_packed_index >= 0 && bm_packed_index < (int)bsd.block_mode_count);
		const block_mode& qw_bm = bsd.block_modes[bm_packed_index];

		int decimation_mode = qw_bm.decimation_mode;
		int weight_quant_mode = qw_bm.quant_mode;
		const auto& di = bsd.get_decimation_info(decimation_mode);
		promise(di.weight_count > 0);

		trace_add_data("weight_x", di.weight_x);
		trace_add_data("weight_y", di.weight_y);
		trace_add_data("weight_z", di.weight_z);
		trace_add_data("weight_quant", weight_quant_mode);

		// Recompute the ideal color endpoints before storing them.
		merge_endpoints(eix1[decimation_mode].ep, eix2[decimation_mode].ep, plane2_component, epm);

		vfloat4 rgbs_colors[BLOCK_MAX_PARTITIONS];
		vfloat4 rgbo_colors[BLOCK_MAX_PARTITIONS];

		symbolic_compressed_block workscb;

		for (unsigned int j = 0; j < BLOCK_MAX_COMPONENTS; j++)
		{
			workscb.constant_color[j] = 0;
		}

		uint8_t* u8_weight1_src = u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * bm_packed_index);
		uint8_t* u8_weight2_src = u8_quantized_decimated_quantized_weights + BLOCK_MAX_WEIGHTS * (2 * bm_packed_index + 1);

		for (int j = 0; j < di.weight_count; j++)
		{
			workscb.weights[j] = u8_weight1_src[j];
			workscb.weights[j + WEIGHTS_PLANE2_OFFSET] = u8_weight2_src[j];
		}

		for (unsigned int l = 0; l < config.tune_refinement_limit; l++)
		{
			recompute_ideal_colors_2planes(
			    blk, ewb, pi, di,
			    weight_quant_mode, workscb.weights, workscb.weights + WEIGHTS_PLANE2_OFFSET,
			    epm, rgbs_colors, rgbo_colors, plane2_component);

			// Quantize the chosen color
			for (unsigned int j = 0; j < partition_count; j++)
			{
				workscb.color_formats[j] = pack_color_endpoints(
				                            epm.endpt0[j],
				                            epm.endpt1[j],
				                            rgbs_colors[j], rgbo_colors[j],
				                            partition_format_specifiers[i][j],
				                            workscb.color_values[j],
				                            (quant_method)color_quant_level[i]);
			}

			// If all the color endpoint modes are the same, we get a few more bits to store colors;
			// let's see if we can take advantage of this: requantize all the colors and see if the
			// endpoint modes remain the same.
			workscb.color_formats_matched = 0;

			if ((partition_count >= 2 && workscb.color_formats[0] == workscb.color_formats[1]
			    && color_quant_level[i] != color_quant_level_mod[i])
			    && (partition_count == 2 || (workscb.color_formats[0] == workscb.color_formats[2]
			    && (partition_count == 3 || (workscb.color_formats[0] == workscb.color_formats[3])))))
			{
				uint8_t colorvals[BLOCK_MAX_PARTITIONS][12];
				int color_formats_mod[BLOCK_MAX_PARTITIONS] { 0 };
				for (unsigned int j = 0; j < partition_count; j++)
				{
					color_formats_mod[j] = pack_color_endpoints(
					    epm.endpt0[j],
					    epm.endpt1[j],
					    rgbs_colors[j],
					    rgbo_colors[j],
					    partition_format_specifiers[i][j],
					    colorvals[j],
					    (quant_method)color_quant_level_mod[i]);
				}

				if (color_formats_mod[0] == color_formats_mod[1]
				    && (partition_count == 2 || (color_formats_mod[0] == color_formats_mod[2]
				    && (partition_count == 3 || (color_formats_mod[0] == color_formats_mod[3])))))
				{
					workscb.color_formats_matched = 1;
					for (unsigned int j = 0; j < BLOCK_MAX_PARTITIONS; j++)
					{
						for (unsigned int k = 0; k < 8; k++)
						{
							workscb.color_values[j][k] = colorvals[j][k];
						}

						workscb.color_formats[j] = color_formats_mod[j];
					}
				}
			}

			// Store header fields
			workscb.partition_count = partition_count;
			workscb.partition_index = partition_index;
			workscb.quant_mode = workscb.color_formats_matched ? color_quant_level_mod[i] : color_quant_level[i];
			workscb.block_mode = qw_bm.mode_index;
			workscb.plane2_component = plane2_component;
			workscb.block_type = SYM_BTYPE_NONCONST;

			if (workscb.quant_mode < 4)
			{
				workscb.block_type = SYM_BTYPE_ERROR;
			}

			// Pre-realign test
			if (l == 0)
			{
				float errorval = compute_symbolic_block_difference(config, bsd, workscb, blk, ewb);
				if (errorval == -1e30f)
				{
					errorval = -errorval;
					workscb.block_type = SYM_BTYPE_ERROR;
				}

				trace_add_data("error_prerealign", errorval);
				best_errorval_in_mode = astc::min(errorval, best_errorval_in_mode);

				// Average refinement improvement is 3.5% per iteration (allow 5%), but the first
				// iteration can help more so we give it a extra 10% leeway. Use this knowledge to
				// drive a heuristic to skip blocks that are unlikely to catch up with the best
				// block we have already.
				unsigned int iters_remaining = config.tune_refinement_limit - l;
				float threshold = (0.05f * static_cast<float>(iters_remaining)) + 1.1f;
				if (errorval > (threshold * best_errorval_in_scb))
				{
					break;
				}

				if (errorval < best_errorval_in_scb)
				{
					best_errorval_in_scb = errorval;
					workscb.errorval = errorval;
					scb = workscb;

					if (errorval < tune_errorval_threshold)
					{
						return errorval;
					}
				}
			}

			// Perform a final pass over the weights to try to improve them
			bool adjustments = realign_weights(
			    config.profile, bsd, blk, ewb, workscb,
			    workscb.weights, workscb.weights + WEIGHTS_PLANE2_OFFSET);

			// Post-realign test
			float errorval = compute_symbolic_block_difference(config, bsd, workscb, blk, ewb);
			if (errorval == -1e30f)
			{
				errorval = -errorval;
				workscb.block_type = SYM_BTYPE_ERROR;
			}

			trace_add_data("error_postrealign", errorval);
			best_errorval_in_mode = astc::min(errorval, best_errorval_in_mode);

			// Average refinement improvement is 3.5% per iteration, so skip blocks that are
			// unlikely to catch up with the best block we have already. Assume a 5% per step to
			// give benefit of the doubt ...
			unsigned int iters_remaining = config.tune_refinement_limit - 1 - l;
			float threshold = (0.05f * static_cast<float>(iters_remaining)) + 1.0f;
			if (errorval > (threshold * best_errorval_in_scb))
			{
				break;
			}

			if (errorval < best_errorval_in_scb)
			{
				best_errorval_in_scb = errorval;
				workscb.errorval = errorval;
				scb = workscb;

				if (errorval < tune_errorval_threshold)
				{
					return errorval;
				}
			}

			if (!adjustments)
			{
				break;
			}
		}
	}

	return best_errorval_in_mode;
}

/**
 * @brief Create a per-texel expansion of the error weights for deblocking.
 *
 * Deblockign works by assigning a higher error weight to blocks the closer they are the edge of the
 * block. The encourages the compressor to keep the periphery colors more accurate, which can help
 * reduce block artifacts when compressing gradients.
 *
 * @param[in,out] ctx   The context containing both deblog memory and config.
 */
void expand_deblock_weights(
	astcenc_context& ctx
) {
	unsigned int xdim = ctx.config.block_x;
	unsigned int ydim = ctx.config.block_y;
	unsigned int zdim = ctx.config.block_z;

	float centerpos_x = static_cast<float>(xdim - 1) * 0.5f;
	float centerpos_y = static_cast<float>(ydim - 1) * 0.5f;
	float centerpos_z = static_cast<float>(zdim - 1) * 0.5f;
	float *bef = ctx.deblock_weights;

	for (unsigned int z = 0; z < zdim; z++)
	{
		for (unsigned int y = 0; y < ydim; y++)
		{
			for (unsigned int x = 0; x < xdim; x++)
			{
				float xdif = (static_cast<float>(x) - centerpos_x) / static_cast<float>(xdim);
				float ydif = (static_cast<float>(y) - centerpos_y) / static_cast<float>(ydim);
				float zdif = (static_cast<float>(z) - centerpos_z) / static_cast<float>(zdim);

				float wdif = 0.36f;
				float dist = astc::sqrt(xdif * xdif + ydif * ydif + zdif * zdif + wdif * wdif);
				*bef = astc::pow(dist, ctx.config.b_deblock_weight);
				bef++;
			}
		}
	}
}

/**
 * @brief Create a per-texel and per-channel expansion of the error weights.
 *
 * This approach creates relatively large error block tables, but it allows a very flexible level of
 * control over how specific texels and channels are prioritized by the compressor.
 *
 * TODO: Inline the expand_deblock_weights here? The computation is cheap, and it would remove some
 * memory allocation from the context, at the expense of recomputing per block.
 *
 * @param      ctx     The compressor context and configuration.
 * @param      image   The input image information.
 * @param      bsd     The block size information.
 * @param      blk     The image block color data to compress.
 * @param[out] ewb     The image block weighted error data.
 *
 * @return Return the total error weight sum for all texels and channels.
 */
static float prepare_error_weight_block(
	const astcenc_context& ctx,
	const astcenc_image& image,
	const block_size_descriptor& bsd,
	const image_block& blk,
	error_weight_block& ewb
) {
	unsigned int idx = 0;
	bool any_mean_stdev_weight =
	    ctx.config.v_rgb_mean != 0.0f || ctx.config.v_rgb_stdev != 0.0f || \
	    ctx.config.v_a_mean != 0.0f || ctx.config.v_a_stdev != 0.0f;

	vfloat4 color_weights(ctx.config.cw_r_weight,
	                      ctx.config.cw_g_weight,
	                      ctx.config.cw_b_weight,
	                      ctx.config.cw_a_weight);

	// This works because HDR is imposed globally at compression time
	unsigned int rgb_lns = blk.rgb_lns[0];
	unsigned int a_lns = blk.alpha_lns[0];
	vint4 use_lns(rgb_lns, rgb_lns, rgb_lns, a_lns);
	vmask4 lns_mask = use_lns != vint4::zero();

	promise(bsd.xdim > 0);
	promise(bsd.ydim > 0);
	promise(bsd.zdim > 0);

	for (unsigned int z = 0; z < bsd.zdim; z++)
	{
		for (unsigned int y = 0; y < bsd.ydim; y++)
		{
			for (unsigned int x = 0; x < bsd.xdim; x++)
			{
				unsigned int xpos = x + blk.xpos;
				unsigned int ypos = y + blk.ypos;
				unsigned int zpos = z + blk.zpos;

				if (xpos >= image.dim_x || ypos >= image.dim_y || zpos >= image.dim_z)
				{
					ewb.error_weights[idx] = vfloat4(1e-11f);
				}
				else
				{
					vfloat4 derv(65535.0f);

					// Compute derivative if we have any use of LNS
					if (any(lns_mask))
					{
						// TODO: Can we avoid some of the multi-type translation?
						vfloat4 data = blk.texel(idx);
						vint4 datai = lns_to_sf16(float_to_int(data));

						vfloat4 dataf = float16_to_float(datai);
						dataf = max(dataf, 6e-5f);

						vfloat4 data_lns1 = dataf * 1.05f;
						data_lns1 = float_to_lns(data_lns1);

						vfloat4 data_lns2 = dataf;
						data_lns2 = float_to_lns(data_lns2);

						vfloat4 divisor_lns = dataf * 0.05f;

						// Clamp derivatives between 1/32 and 2^25
						float lo = 1.0f / 32.0f;
						float hi = 33554432.0f;
						vfloat4 derv_lns = clamp(lo, hi, (data_lns1 - data_lns2) / divisor_lns);
						derv = select(derv, derv_lns, lns_mask);
					}

					// Compute error weight
					vfloat4 error_weight(ctx.config.v_rgb_base,
					                     ctx.config.v_rgb_base,
					                     ctx.config.v_rgb_base,
					                     ctx.config.v_a_base);

					unsigned int ydt = image.dim_x;
					unsigned int zdt = image.dim_x * image.dim_y;

					if (any_mean_stdev_weight)
					{
						vfloat4 avg = ctx.input_averages[zpos * zdt + ypos * ydt + xpos];
						avg = max(avg, 6e-5f);
						avg = avg * avg;

						vfloat4 variance = ctx.input_variances[zpos * zdt + ypos * ydt + xpos];
						variance = variance * variance;

						float favg = hadd_rgb_s(avg) * (1.0f / 3.0f);
						float fvar = hadd_rgb_s(variance) * (1.0f / 3.0f);

						float mixing = ctx.config.v_rgba_mean_stdev_mix;
						avg.set_lane<0>(favg * mixing + avg.lane<0>() * (1.0f - mixing));
						avg.set_lane<1>(favg * mixing + avg.lane<1>() * (1.0f - mixing));
						avg.set_lane<2>(favg * mixing + avg.lane<2>() * (1.0f - mixing));

						variance.set_lane<0>(fvar * mixing + variance.lane<0>() * (1.0f - mixing));
						variance.set_lane<1>(fvar * mixing + variance.lane<1>() * (1.0f - mixing));
						variance.set_lane<2>(fvar * mixing + variance.lane<2>() * (1.0f - mixing));

						vfloat4 stdev = sqrt(max(variance, 0.0f));

						vfloat4 scalea(ctx.config.v_rgb_mean, ctx.config.v_rgb_mean, ctx.config.v_rgb_mean, ctx.config.v_a_mean);
						avg = avg * scalea;

						vfloat4 scales(ctx.config.v_rgb_stdev, ctx.config.v_rgb_stdev, ctx.config.v_rgb_stdev, ctx.config.v_a_stdev);
						stdev = stdev * scales;

						error_weight = error_weight + avg + stdev;
						error_weight = 1.0f / error_weight;
					}

					if (ctx.config.flags & ASTCENC_FLG_MAP_NORMAL)
					{
						// Convert from 0 to 1 to -1 to +1 range.
						float xN = ((blk.data_r[idx] * (1.0f / 65535.0f)) - 0.5f) * 2.0f;
						float yN = ((blk.data_a[idx] * (1.0f / 65535.0f)) - 0.5f) * 2.0f;

						float denom = 1.0f - xN * xN - yN * yN;
						denom = astc::max(denom, 0.1f);
						denom = 1.0f / denom;
						error_weight.set_lane<0>(error_weight.lane<0>() * (1.0f + xN * xN * denom));
						error_weight.set_lane<3>(error_weight.lane<3>() * (1.0f + yN * yN * denom));
					}

					if (ctx.config.flags & ASTCENC_FLG_USE_ALPHA_WEIGHT)
					{
						float alpha_scale;
						if (ctx.config.a_scale_radius != 0)
						{
							alpha_scale = ctx.input_alpha_averages[zpos * zdt + ypos * ydt + xpos];
						}
						else
						{
							alpha_scale = blk.data_a[idx] * (1.0f / 65535.0f);
						}

						alpha_scale = astc::max(alpha_scale, 0.0001f);

						alpha_scale *= alpha_scale;
						error_weight.set_lane<0>(error_weight.lane<0>() * alpha_scale);
						error_weight.set_lane<1>(error_weight.lane<1>() * alpha_scale);
						error_weight.set_lane<2>(error_weight.lane<2>() * alpha_scale);
					}

					error_weight = error_weight * color_weights;
					error_weight = error_weight * ctx.deblock_weights[idx];

					// When we loaded the block to begin with, we applied a transfer function and
					// computed the derivative of the transfer function. However, the error-weight
					// computation so far is based on the original color values, not the
					// transfer-function values. As such, we must multiply the error weights by the
					// derivative of the inverse of the transfer function, which is equivalent to
					// dividing by the derivative of the transfer function.

					error_weight = error_weight / (derv * derv * 1e-10f);
					ewb.error_weights[idx] = error_weight;
				}
				idx++;
			}
		}
	}

	vfloat4 error_weight_sum = vfloat4::zero();
	int texels_per_block = bsd.texel_count;
	for (int i = 0; i < texels_per_block; i++)
	{
		error_weight_sum = error_weight_sum + ewb.error_weights[i];

		float wr = ewb.error_weights[i].lane<0>();
		float wg = ewb.error_weights[i].lane<1>();
		float wb = ewb.error_weights[i].lane<2>();
		float wa = ewb.error_weights[i].lane<3>();

		ewb.texel_weight_r[i] = wr;
		ewb.texel_weight_g[i] = wg;
		ewb.texel_weight_b[i] = wb;
		ewb.texel_weight_a[i] = wa;

		ewb.texel_weight_rg[i] = (wr + wg) * 0.5f;
		ewb.texel_weight_rb[i] = (wr + wb) * 0.5f;
		ewb.texel_weight_gb[i] = (wg + wb) * 0.5f;

		ewb.texel_weight_gba[i] = (wg + wb + wa) * 0.333333f;
		ewb.texel_weight_rba[i] = (wr + wb + wa) * 0.333333f;
		ewb.texel_weight_rga[i] = (wr + wg + wa) * 0.333333f;
		ewb.texel_weight_rgb[i] = (wr + wg + wb) * 0.333333f;

		ewb.texel_weight[i] = (wr + wg + wb + wa) * 0.25f;
	}

	return hadd_s(error_weight_sum);
}

/**
 * @brief Determine the lowest cross-channel correlation factor.
 *
 * @param texels_per_block   The number of texels in a block.
 * @param blk                The image block color data to compress.
 * @param ewb                The image block weighted error data.
 *
 * @return Return the lowest correlation factor.
 */
static float prepare_block_statistics(
	int texels_per_block,
	const image_block& blk,
	const error_weight_block& ewb
) {
	// Compute covariance matrix, as a collection of 10 scalars that form the upper-triangular row
	// of the matrix. The matrix is symmetric, so this is all we need for this use case.
	float rs = 0.0f;
	float gs = 0.0f;
	float bs = 0.0f;
	float as = 0.0f;
	float rr_var = 0.0f;
	float gg_var = 0.0f;
	float bb_var = 0.0f;
	float aa_var = 0.0f;
	float rg_cov = 0.0f;
	float rb_cov = 0.0f;
	float ra_cov = 0.0f;
	float gb_cov = 0.0f;
	float ga_cov = 0.0f;
	float ba_cov = 0.0f;

	float weight_sum = 0.0f;

	promise(texels_per_block > 0);
	for (int i = 0; i < texels_per_block; i++)
	{
		float weight = ewb.texel_weight[i];
		assert(weight >= 0.0f);
		weight_sum += weight;

		float r = blk.data_r[i];
		float g = blk.data_g[i];
		float b = blk.data_b[i];
		float a = blk.data_a[i];

		float rw = r * weight;
		rs += rw;
		rr_var += r * rw;
		rg_cov += g * rw;
		rb_cov += b * rw;
		ra_cov += a * rw;

		float gw = g * weight;
		gs += gw;
		gg_var += g * gw;
		gb_cov += b * gw;
		ga_cov += a * gw;

		float bw = b * weight;
		bs += bw;
		bb_var += b * bw;
		ba_cov += a * bw;

		float aw = a * weight;
		as += aw;
		aa_var += a * aw;
	}

	float rpt = 1.0f / astc::max(weight_sum, 1e-7f);

	rr_var -= rs * (rs * rpt);
	rg_cov -= gs * (rs * rpt);
	rb_cov -= bs * (rs * rpt);
	ra_cov -= as * (rs * rpt);

	gg_var -= gs * (gs * rpt);
	gb_cov -= bs * (gs * rpt);
	ga_cov -= as * (gs * rpt);

	bb_var -= bs * (bs * rpt);
	ba_cov -= as * (bs * rpt);

	aa_var -= as * (as * rpt);

	rg_cov *= astc::rsqrt(astc::max(rr_var * gg_var, 1e-30f));
	rb_cov *= astc::rsqrt(astc::max(rr_var * bb_var, 1e-30f));
	ra_cov *= astc::rsqrt(astc::max(rr_var * aa_var, 1e-30f));
	gb_cov *= astc::rsqrt(astc::max(gg_var * bb_var, 1e-30f));
	ga_cov *= astc::rsqrt(astc::max(gg_var * aa_var, 1e-30f));
	ba_cov *= astc::rsqrt(astc::max(bb_var * aa_var, 1e-30f));

	if (astc::isnan(rg_cov)) rg_cov = 1.0f;
	if (astc::isnan(rb_cov)) rb_cov = 1.0f;
	if (astc::isnan(ra_cov)) ra_cov = 1.0f;
	if (astc::isnan(gb_cov)) gb_cov = 1.0f;
	if (astc::isnan(ga_cov)) ga_cov = 1.0f;
	if (astc::isnan(ba_cov)) ba_cov = 1.0f;

	float lowest_correlation = astc::min(fabsf(rg_cov), fabsf(rb_cov));
	lowest_correlation       = astc::min(lowest_correlation, fabsf(ra_cov));
	lowest_correlation       = astc::min(lowest_correlation, fabsf(gb_cov));
	lowest_correlation       = astc::min(lowest_correlation, fabsf(ga_cov));
	lowest_correlation       = astc::min(lowest_correlation, fabsf(ba_cov));

	// Diagnostic trace points
	trace_add_data("min_r", blk->data_min.lane<0>());
	trace_add_data("max_r", blk->data_max.lane<0>());
	trace_add_data("min_g", blk->data_min.lane<1>());
	trace_add_data("max_g", blk->data_max.lane<1>());
	trace_add_data("min_b", blk->data_min.lane<2>());
	trace_add_data("max_b", blk->data_max.lane<2>());
	trace_add_data("min_a", blk->data_min.lane<3>());
	trace_add_data("max_a", blk->data_max.lane<3>());
	trace_add_data("cov_rg", fabsf(rg_cov));
	trace_add_data("cov_rb", fabsf(rb_cov));
	trace_add_data("cov_ra", fabsf(ra_cov));
	trace_add_data("cov_gb", fabsf(gb_cov));
	trace_add_data("cov_ga", fabsf(ga_cov));
	trace_add_data("cov_ba", fabsf(ba_cov));

	return lowest_correlation;
}

// Do not reorder; we compute array indices programatically
enum trial_modes {
	MODE_0_1_PARTITION_1_PLANE = 0,
	MODE_1_1_PARTITION_2_PLANE_R,
	MODE_2_1_PARTITION_2_PLANE_G,
	MODE_3_1_PARTITION_2_PLANE_B,
	MODE_4_1_PARTITION_2_PLANE_A,
	MODE_5_2_PARTITION_1_PLANE_UNCOR,
	MODE_6_2_PARTITION_1_PLANE_COR,
	MODE_7_2_PARTITION_2_PLANE,
	MODE_8_3_PARTITION_1_PLANE_UNCOR,
	MODE_9_3_PARTITION_1_PLANE_COR,
	MODE_10_3_PARTITION_2_PLANE,
	MODE_11_4_PARTITION_1_PLANE_UNCOR,
	MODE_12_4_PARTITION_1_PLANE_COR,
	MODE_COUNT
};

/* See header for documentation. */
void compress_block(
	const astcenc_context& ctx,
	const astcenc_image& input_image,
	const image_block& blk,
	physical_compressed_block& pcb,
	compression_working_buffers& tmpbuf)
{
	astcenc_profile decode_mode = ctx.config.profile;
	symbolic_compressed_block scb;
	error_weight_block& ewb = tmpbuf.ewb;
	const block_size_descriptor* bsd = ctx.bsd;
	float lowest_correl;

	TRACE_NODE(node0, "block");
	trace_add_data("pos_x", blk->xpos);
	trace_add_data("pos_y", blk->ypos);
	trace_add_data("pos_z", blk->zpos);

	// Set stricter block targets for luminance data as we have more bits to play with
	bool block_is_l = blk.is_luminance();
	float block_is_l_scale = block_is_l ? 1.0f / 1.5f : 1.0f;

	// Set slightly stricter block targets for lumalpha data as we have more bits to play with
	bool block_is_la = blk.is_luminancealpha();
	float block_is_la_scale = block_is_la ? 1.0f / 1.05f : 1.0f;

	bool block_skip_two_plane = false;

	// Default max partition, but +1 if only have 1 or 2 active components
	int max_partitions = ctx.config.tune_partition_count_limit;
	if (block_is_l || block_is_la)
	{
		max_partitions = astc::min(max_partitions + 1, 4);
	}


#if defined(ASTCENC_DIAGNOSTICS)
	// Do this early in diagnostic builds so we can dump uniform metrics
	// for every block. Do it later in release builds to avoid redundant work!
	float error_weight_sum = prepare_error_weight_block(ctx, input_image, *bsd, *blk, *ewb);
	float error_threshold = ctx.config.tune_db_limit
	                      * error_weight_sum
	                      * block_is_l_scale
	                      * block_is_la_scale;

	lowest_correl = prepare_block_statistics(bsd->texel_count, *blk, *ewb);

	trace_add_data("tune_error_threshold", error_threshold);
#endif

	// Detected a constant-color block
	if (all(blk.data_min == blk.data_max))
	{
		TRACE_NODE(node1, "pass");
		trace_add_data("partition_count", 0);
		trace_add_data("plane_count", 1);

		scb.partition_count = 0;

		// Encode as FP16 if using HDR
		if ((decode_mode == ASTCENC_PRF_HDR) ||
		    (decode_mode == ASTCENC_PRF_HDR_RGB_LDR_A))
		{
			scb.block_type = SYM_BTYPE_CONST_F16;
			vint4 color_f16 = float_to_float16(blk.origin_texel);
			store(color_f16, scb.constant_color);
		}
		// Encode as UNORM16 if NOT using HDR
		else
		{
			scb.block_type = SYM_BTYPE_CONST_U16;
			vfloat4 color_f32 = clamp(0.0f, 1.0f, blk.origin_texel) * 65535.0f;
			vint4 color_u16 = float_to_int_rtn(color_f32);
			store(color_u16, scb.constant_color);
		}

		trace_add_data("exit", "quality hit");

		symbolic_to_physical(*bsd, scb, pcb);
		return;
	}

#if !defined(ASTCENC_DIAGNOSTICS)
	float error_weight_sum = prepare_error_weight_block(ctx, input_image, *bsd, blk, ewb);
	float error_threshold = ctx.config.tune_db_limit
	                      * error_weight_sum
	                      * block_is_l_scale
	                      * block_is_la_scale;
#endif

	// Set SCB and mode errors to a very high error value
	scb.errorval = 1e30f;
	scb.block_type = SYM_BTYPE_ERROR;

	float best_errorvals_in_modes[MODE_COUNT];
	for (int i = 0; i < MODE_COUNT; i++)
	{
		best_errorvals_in_modes[i] = 1e30f;
	}

	bool uses_alpha = blk.is_using_alpha();

	// Trial using 1 plane of weights and 1 partition.

	// Most of the time we test it twice, first with a mode cutoff of 0 and then with the specified
	// mode cutoff. This causes an early-out that speeds up encoding of easy blocks. However, this
	// optimization is disabled for 4x4 and 5x4 blocks where it nearly always slows down the
	// compression and slightly reduces image quality.

	float errorval_mult[2] = {
		1.0f / ctx.config.tune_mode0_mse_overshoot,
		1.0f
	};

	static const float errorval_overshoot = 1.0f / ctx.config.tune_refinement_mse_overshoot;

	int start_trial = bsd->texel_count < (int)TUNE_MAX_TEXELS_MODE0_FASTPATH ? 1 : 0;
	for (int i = start_trial; i < 2; i++)
	{
		TRACE_NODE(node1, "pass");
		trace_add_data("partition_count", 1);
		trace_add_data("plane_count", 1);
		trace_add_data("search_mode", i);

		float errorval = compress_symbolic_block_for_partition_1plane(
		    ctx.config, *bsd, blk, ewb, i == 0,
		    error_threshold * errorval_mult[i] * errorval_overshoot,
		    1, 0,  scb, tmpbuf);

		// Mode 0
		best_errorvals_in_modes[MODE_0_1_PARTITION_1_PLANE] = errorval;
		if (errorval < (error_threshold * errorval_mult[i]))
		{
			trace_add_data("exit", "quality hit");
			goto END_OF_TESTS;
		}
	}

#if !defined(ASTCENC_DIAGNOSTICS)
	lowest_correl = prepare_block_statistics(bsd->texel_count, blk, ewb);
#endif

	block_skip_two_plane = lowest_correl > ctx.config.tune_2_plane_early_out_limit_correlation;

	// Test the four possible 1-partition, 2-planes modes
	for (unsigned int i = 0; i < BLOCK_MAX_COMPONENTS; i++)
	{
		TRACE_NODE(node1, "pass");
		trace_add_data("partition_count", 1);
		trace_add_data("plane_count", 2);
		trace_add_data("plane_component", i);

		if (block_skip_two_plane)
		{
			trace_add_data("skip", "tune_2_plane_early_out_limit_correlation");
			continue;
		}

		if (blk.grayscale && i != 3)
		{
			trace_add_data("skip", "grayscale block");
			continue;
		}

		if (!uses_alpha && i == 3)
		{
			trace_add_data("skip", "no alpha component");
			continue;
		}

		float errorval = compress_symbolic_block_for_partition_2planes(
		    ctx.config, *bsd, blk, ewb,
		    error_threshold * errorval_overshoot,
		    1,	// partition count
		    0,	// partition index
		    i,	// the color component to test a separate plane of weights for.
		    scb, tmpbuf);

		best_errorvals_in_modes[MODE_1_1_PARTITION_2_PLANE_R + i] = errorval;

		// Modes 7, 10 (13 is unreachable)
		if (errorval < error_threshold)
		{
			trace_add_data("exit", "quality hit");
			goto END_OF_TESTS;
		}
	}

	// Find best blocks for 2, 3 and 4 partitions
	for (int partition_count = 2; partition_count <= max_partitions; partition_count++)
	{
		unsigned int partition_indices_1plane[2] { 0, 0 };
		unsigned int partition_index_2planes { 0 };

		find_best_partition_candidates(*bsd, blk, ewb, partition_count,
		                        ctx.config.tune_partition_index_limit,
		                        partition_indices_1plane[0],
		                        partition_indices_1plane[1],
		                        block_skip_two_plane ? nullptr : &partition_index_2planes);

		for (int i = 0; i < 2; i++)
		{
			TRACE_NODE(node1, "pass");
			trace_add_data("partition_count", partition_count);
			trace_add_data("partition_index", partition_indices_1plane[i]);
			trace_add_data("plane_count", 1);
			trace_add_data("search_mode", i);

			float errorval = compress_symbolic_block_for_partition_1plane(
			    ctx.config, *bsd, blk, ewb, false,
			    error_threshold * errorval_overshoot,
			    partition_count, partition_indices_1plane[i],
			    scb, tmpbuf);

			// Modes 5, 6, 8, 9, 11, 12
			best_errorvals_in_modes[3 * (partition_count - 2) + 5 + i] = errorval;
			if (errorval < error_threshold)
			{
				trace_add_data("exit", "quality hit");
				goto END_OF_TESTS;
			}
		}

		float best_1_partition_1_plane_result = best_errorvals_in_modes[MODE_0_1_PARTITION_1_PLANE];

		float best_1_partition_2_plane_result = astc::min(best_errorvals_in_modes[MODE_1_1_PARTITION_2_PLANE_R],
		                                                  best_errorvals_in_modes[MODE_2_1_PARTITION_2_PLANE_G],
		                                                  best_errorvals_in_modes[MODE_3_1_PARTITION_2_PLANE_B],
		                                                  best_errorvals_in_modes[MODE_4_1_PARTITION_2_PLANE_A]);

		float best_2_partition_1_plane_result = astc::min(best_errorvals_in_modes[MODE_5_2_PARTITION_1_PLANE_UNCOR],
		                                                  best_errorvals_in_modes[MODE_6_2_PARTITION_1_PLANE_COR]);

		float best_3_partition_1_plane_result = astc::min(best_errorvals_in_modes[MODE_8_3_PARTITION_1_PLANE_UNCOR],
		                                                  best_errorvals_in_modes[MODE_9_3_PARTITION_1_PLANE_COR]);

		// If adding a second partition doesn't improve much over using one partition then skip more
		// thorough searches. This skips 2pt/2pl, 3pt/1pl, 3pt/2pl, 4pt/1pl.
		if (partition_count == 2 && (best_2_partition_1_plane_result > (best_1_partition_1_plane_result * ctx.config.tune_2_partition_early_out_limit_factor)))
		{
			trace_add_data("skip", "tune_2_partition_early_out_limit_factor");
			goto END_OF_TESTS;
		}

		// If adding a third partition doesn't improve much over using two partitions then skip more
		// thorough searches as it's not going to help. This skips 3pt/2pl, 4pt/1pl.
		if (partition_count == 3 && (best_3_partition_1_plane_result > (best_2_partition_1_plane_result * ctx.config.tune_3_partition_early_out_limit_factor)))
		{
			trace_add_data("skip", "tune_3_partition_early_out_limit_factor");
			goto END_OF_TESTS;
		}

		// Skip testing dual weight planes for:
		// * 4 partitions (can't be encoded by the format)
		if (partition_count == 4)
		{
			continue;
		}

		// * Luminance only blocks (never need for a second plane)
		if (blk.grayscale && !uses_alpha)
		{
			trace_add_data("skip", "grayscale no alpha block ");
			continue;
		}

		// * Blocks with higher component correlation than the tuning cutoff
		if (block_skip_two_plane)
		{
			trace_add_data("skip", "tune_2_plane_early_out_limit_correlation");
			continue;
		}

		// If adding a 2nd plane to 1 partition doesn't help by some margin then trying with more
		// partitions is even less likely to help, so skip those. This is NOT exposed as a heuristic that the user can
		// control as we've found no need to - this is a very reliable heuristic even on torture
		// test images.
		if (best_1_partition_2_plane_result > (best_1_partition_1_plane_result * ctx.config.tune_2_plane_early_out_limit_factor))
		{
			trace_add_data("skip", "tune_2_plane_early_out_limit_correlation_builtin");
			continue;
		}

		TRACE_NODE(node1, "pass");
		trace_add_data("partition_count", partition_count);
		trace_add_data("partition_index", partition_index_2planes & (BLOCK_MAX_PARTITIONINGS - 1));
		trace_add_data("plane_count", 2);
		trace_add_data("plane_component", partition_index_2planes >> PARTITION_INDEX_BITS);

		float errorval = compress_symbolic_block_for_partition_2planes(
			ctx.config, *bsd, blk, ewb,
			error_threshold * errorval_overshoot,
			partition_count,
			partition_index_2planes & (BLOCK_MAX_PARTITIONINGS - 1),
			partition_index_2planes >> PARTITION_INDEX_BITS,
			scb, tmpbuf);

		// Modes 7, 10 (13 is unreachable)
		if (errorval < error_threshold)
		{
			trace_add_data("exit", "quality hit");
			goto END_OF_TESTS;
		}
	}

	trace_add_data("exit", "quality not hit");

END_OF_TESTS:
	// Compress to a physical block
	symbolic_to_physical(*bsd, scb, pcb);
}

#endif
