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
 * @brief Functions for finding best endpoint format.
 *
 * We assume there are two independent sources of error in any given partition:
 *
 *   - Encoding choice errors
 *   - Quantization errors
 *
 * Encoding choice errors are caused by encoder decisions. For example:
 *
 *   - Using luminance instead of separate RGB components.
 *   - Using a constant 1.0 alpha instead of storing an alpha component.
 *   - Using RGB+scale instead of storing two full RGB endpoints.
 *
 * Quantization errors occur due to the limited precision we use for storage. These errors generally
 * scale with quantization level, but are not actually independent of color encoding. In particular:
 *
 *   - If we can use offset encoding then quantization error is halved.
 *   - If we can use blue-contraction then quantization error for RG is halved.
 *   - If we use HDR endpoints the quantization error is higher.
 *
 * Apart from these effects, we assume the error is proportional to the quantization step size.
 */


#include "astcenc_internal.h"
#include "astcenc_vecmathlib.h"

#include <assert.h>

/**
 * @brief Compute cumulative error weight of each partition.
 *
 * The cumulative error weight is used to determine the relative importance of each partiton when
 * deciding how to quantize colors, as not all partitions are equal. For example, some partitions
 * will have far fewer texels than others in the same block.
 *
 * @param      ewb   The block error weights.
 * @param      pi    The partiion info.
 * @param[out] pm    The output metrics; only writes to @c error_weight field.
 */
static void compute_partition_error_color_weightings(
	const error_weight_block& ewb,
	const partition_info& pi,
	partition_metrics pm[BLOCK_MAX_PARTITIONS]
) {
	int partition_count = pi.partition_count;
	promise(partition_count > 0);

	for (int i = 0; i < partition_count; i++)
	{
		vfloat4 error_weight(1e-12f);

		int texel_count = pi.partition_texel_count[i];
		promise(texel_count > 0);

		for (int j = 0; j < texel_count; j++)
		{
			int tidx = pi.texels_of_partition[i][j];
			error_weight = error_weight + ewb.error_weights[tidx];
		}

		error_weight = error_weight / pi.partition_texel_count[i];
		pm[i].error_weight = error_weight;
	}
}

/**
 * @brief Compute the errors of the endpoint line options for one partition.
 *
 * Uncorrelated data assumes storing completely independent RGBA channels for each endpoint. Same
 * chroma data assumes storing RGBA endpoints which pass though the origin (LDR only). RGBL data
 * assumes storing RGB + lumashift (HDR only). Luminance error assumes storing RGB channels as a
 * single value.
 *
 *
 * @param      pi                The partition info data.
 * @param      partition_index   The partition index to compule the error for.
 * @param      blk               The image block.
 * @param      ewb               The error weight block.
 * @param      uncor_pline       The endpoint line assuming uncorrelated endpoints.
 * @param[out] uncor_err         The computed error for the uncorrelated endpoint line.
 * @param      samec_pline       The endpoint line assuming the same chroma for both endpoints.
 * @param[out] samec_err         The computed error for the uncorrelated endpoint line.
 * @param      rgbl_pline        The endpoint line assuming RGB + lumashift data.
 * @param[out] rgbl_err          The computed error for the RGB + lumashift endpoint line.
 * @param      l_pline           The endpoint line assuming luminance data.
 * @param[out] l_err             The computed error for the luminance endpoint line.
 * @param[out] a_drop_err        The computed error for dropping the alpha component.
 */
static void compute_error_squared_rgb_single_partition(
	const partition_info& pi,
	int partition_index,
	const image_block& blk,
	const error_weight_block& ewb,
	const processed_line3& uncor_pline,
	float& uncor_err,
	const processed_line3& samec_pline,
	float& samec_err,
	const processed_line3& rgbl_pline,
	float& rgbl_err,
	const processed_line3& l_pline,
	float& l_err,
	float& a_drop_err
) {
	uncor_err = 0.0f;
	samec_err = 0.0f;
	rgbl_err = 0.0f;
	l_err = 0.0f;
	a_drop_err = 0.0f;

	int texels_in_partition = pi.partition_texel_count[partition_index];
	promise(texels_in_partition > 0);

	for (int i = 0; i < texels_in_partition; i++)
	{
		int tix = pi.texels_of_partition[partition_index][i];
		float texel_weight = ewb.texel_weight_rgb[tix];
		if (texel_weight < 1e-20f)
		{
			continue;
		}

		vfloat4 point = blk.texel(tix);
		vfloat4 ews = ewb.error_weights[tix];

		// Compute the error that arises from just ditching alpha
		float default_alpha = blk.get_default_alpha();
		float omalpha = point.lane<3>() - default_alpha;
		a_drop_err += omalpha * omalpha * ews.lane<3>();

		float param1 = dot3_s(point, uncor_pline.bs);
		vfloat4 rp1 = uncor_pline.amod + param1 * uncor_pline.bis;
		vfloat4 dist1 = rp1 - point;
		uncor_err += dot3_s(ews, dist1 * dist1);

		float param2 = dot3_s(point, samec_pline.bs);
		// No samec amod - we know it's always zero
		vfloat4 rp2 = /* samec_pline.amod + */ param2 * samec_pline.bis;
		vfloat4 dist2 = rp2 - point;
		samec_err += dot3_s(ews, dist2 * dist2);

		// TODO - this is only used for HDR textures. Can we skip?
		float param3 = dot3_s(point,  rgbl_pline.bs);
		vfloat4 rp3 = rgbl_pline.amod + param3 * rgbl_pline.bis;
		vfloat4 dist3 = rp3 - point;
		rgbl_err += dot3_s(ews, dist3 * dist3);

		float param4 = dot3_s(point, l_pline.bs);
		// No luma amod - we know it's always zero
		vfloat4 rp4 = /* l_pline.amod + */ param4 * l_pline.bis;
		vfloat4 dist4 = rp4 - point;
		l_err += dot3_s(ews, dist4 * dist4);
	}
}

/**
 * @brief For a given set of input colors and partitioning determine endpoint encode errors.
 *
 * This function determines the color error that results from RGB-scale encoding (LDR only),
 * RGB-lumashift encoding (HDR only), luminance-encoding, and alpha drop. Also determines whether
 * the endpoints are eligible for offset encoding or blue-contraction
 *
 * @param      bsd   The block size information.
 * @param      blk   The image block.
 * @param      pi    The partition info data.
 * @param      ewb   The error weight block.
 * @param      ep    The idealized endpoints.
 * @param[out] eci   The resulting encoding choice error metrics.
  */
static void compute_encoding_choice_errors(
	const block_size_descriptor& bsd,
	const image_block& blk,
	const partition_info& pi,
	const error_weight_block& ewb,
	const endpoints& ep,
	encoding_choice_errors eci[BLOCK_MAX_PARTITIONS])
{
	int partition_count = pi.partition_count;
	int texels_per_block = bsd.texel_count;

	promise(partition_count > 0);
	promise(texels_per_block > 0);

	partition_metrics pms[BLOCK_MAX_PARTITIONS];

	compute_avgs_and_dirs_3_comp(pi, blk, ewb, 3, pms);

	for (int i = 0; i < partition_count; i++)
	{
		partition_metrics& pm = pms[i];

		// TODO: Can we skip rgb_luma_lines for LDR images?
		line3 uncor_rgb_lines;
		line3 samec_rgb_lines;	// for LDR-RGB-scale
		line3 rgb_luma_lines;	// for HDR-RGB-scale

		processed_line3 uncor_rgb_plines;
		processed_line3 samec_rgb_plines;	// for LDR-RGB-scale
		processed_line3 rgb_luma_plines;	// for HDR-RGB-scale
		processed_line3 luminance_plines;

		float uncorr_rgb_error;
		float samechroma_rgb_error;
		float rgb_luma_error;
		float luminance_rgb_error;
		float alpha_drop_error;

		vfloat4 csf = pm.color_scale;
		vfloat4 csfn = normalize(csf);

		vfloat4 icsf = pm.icolor_scale;
		icsf.set_lane<3>(0.0f);

		uncor_rgb_lines.a = pm.avg;
		uncor_rgb_lines.b = normalize_safe(pm.dir, csfn);

		samec_rgb_lines.a = vfloat4::zero();
		samec_rgb_lines.b = normalize_safe(pm.avg, csfn);

		rgb_luma_lines.a = pm.avg;
		rgb_luma_lines.b = csfn;

		uncor_rgb_plines.amod = (uncor_rgb_lines.a - uncor_rgb_lines.b * dot3(uncor_rgb_lines.a, uncor_rgb_lines.b)) * icsf;
		uncor_rgb_plines.bs   = uncor_rgb_lines.b * csf;
		uncor_rgb_plines.bis  = uncor_rgb_lines.b * icsf;

		// Same chroma always goes though zero, so this is simpler than the others
		samec_rgb_plines.amod = vfloat4::zero();
		samec_rgb_plines.bs   = samec_rgb_lines.b * csf;
		samec_rgb_plines.bis  = samec_rgb_lines.b * icsf;

		// TODO - this is only used for HDR textures. Can we skip?
		rgb_luma_plines.amod = (rgb_luma_lines.a - rgb_luma_lines.b * dot3(rgb_luma_lines.a, rgb_luma_lines.b)) * icsf;
		rgb_luma_plines.bs   = rgb_luma_lines.b * csf;
		rgb_luma_plines.bis  = rgb_luma_lines.b * icsf;

		// Luminance always goes though zero, so this is simpler than the others
		luminance_plines.amod = vfloat4::zero();
		luminance_plines.bs   = csfn * csf;
		luminance_plines.bis  = csfn * icsf;

		compute_error_squared_rgb_single_partition(
		    pi, i, blk, ewb,
		    uncor_rgb_plines, uncorr_rgb_error,
		    samec_rgb_plines, samechroma_rgb_error,
		    rgb_luma_plines,  rgb_luma_error,
		    luminance_plines, luminance_rgb_error,
		                      alpha_drop_error);

		// Determine if we can offset encode RGB lanes
		vfloat4 endpt0 = ep.endpt0[i];
		vfloat4 endpt1 = ep.endpt1[i];
		vfloat4 endpt_diff = abs(endpt1 - endpt0);
		vmask4 endpt_can_offset = endpt_diff < vfloat4(0.12f * 65535.0f);
		bool can_offset_encode = (mask(endpt_can_offset) & 0x7) == 0x7;

		// Determine if we can blue contract encode RGB lanes
		vfloat4 endpt_diff_bc(
			endpt0.lane<0>() + (endpt0.lane<0>() - endpt0.lane<2>()),
			endpt1.lane<0>() + (endpt1.lane<0>() - endpt1.lane<2>()),
			endpt0.lane<1>() + (endpt0.lane<1>() - endpt0.lane<2>()),
			endpt1.lane<1>() + (endpt1.lane<1>() - endpt1.lane<2>())
		);

		vmask4 endpt_can_bc_lo = endpt_diff_bc > vfloat4(0.01f * 65535.0f);
		vmask4 endpt_can_bc_hi = endpt_diff_bc < vfloat4(0.99f * 65535.0f);
		bool can_blue_contract = (mask(endpt_can_bc_lo & endpt_can_bc_hi) & 0x7) == 0x7;

		// Store out the settings
		eci[i].rgb_scale_error = (samechroma_rgb_error - uncorr_rgb_error) * 0.7f;	// empirical
		eci[i].rgb_luma_error  = (rgb_luma_error - uncorr_rgb_error) * 1.5f;	// wild guess
		eci[i].luminance_error = (luminance_rgb_error - uncorr_rgb_error) * 3.0f;	// empirical
		eci[i].alpha_drop_error = alpha_drop_error * 3.0f;
		eci[i].can_offset_encode = can_offset_encode;
		eci[i].can_blue_contract = can_blue_contract;
	}
}

/**
 * @brief For a given partition compute the error for every endpoint integer count and quant level.
 *
 * @param      encode_hdr_rgb     @c true if using HDR for RGB, @c false for LDR.
 * @param      encode_hdr_alpha   @c true if using HDR for alpha, @c false for LDR.
 * @param      partition_index    The partition index.
 * @param      pi                 The partition info.
 * @param      eci                The encoding choice error metrics.
 * @param      ep                 The idealized endpoints.
 * @param      error_weight       The resulting encoding choice error metrics.
 * @param[out] best_error         The best error for each integer count and quant level.
 * @param[out] format_of_choice   The preferred endpoint format for each integer count and quant level.
 */
static void compute_color_error_for_every_integer_count_and_quant_level(
	bool encode_hdr_rgb,
	bool encode_hdr_alpha,
	int partition_index,
	const partition_info& pi,
	const encoding_choice_errors& eci,
	const endpoints& ep,
	vfloat4 error_weight,
	float best_error[21][4],
	int format_of_choice[21][4]
) {
	int partition_size = pi.partition_texel_count[partition_index];

	static const float baseline_quant_error[21] = {
		(65536.0f * 65536.0f / 18.0f),				// 2 values, 1 step
		(65536.0f * 65536.0f / 18.0f) / (2 * 2),	// 3 values, 2 steps
		(65536.0f * 65536.0f / 18.0f) / (3 * 3),	// 4 values, 3 steps
		(65536.0f * 65536.0f / 18.0f) / (4 * 4),	// 5 values
		(65536.0f * 65536.0f / 18.0f) / (5 * 5),
		(65536.0f * 65536.0f / 18.0f) / (7 * 7),
		(65536.0f * 65536.0f / 18.0f) / (9 * 9),
		(65536.0f * 65536.0f / 18.0f) / (11 * 11),
		(65536.0f * 65536.0f / 18.0f) / (15 * 15),
		(65536.0f * 65536.0f / 18.0f) / (19 * 19),
		(65536.0f * 65536.0f / 18.0f) / (23 * 23),
		(65536.0f * 65536.0f / 18.0f) / (31 * 31),
		(65536.0f * 65536.0f / 18.0f) / (39 * 39),
		(65536.0f * 65536.0f / 18.0f) / (47 * 47),
		(65536.0f * 65536.0f / 18.0f) / (63 * 63),
		(65536.0f * 65536.0f / 18.0f) / (79 * 79),
		(65536.0f * 65536.0f / 18.0f) / (95 * 95),
		(65536.0f * 65536.0f / 18.0f) / (127 * 127),
		(65536.0f * 65536.0f / 18.0f) / (159 * 159),
		(65536.0f * 65536.0f / 18.0f) / (191 * 191),
		(65536.0f * 65536.0f / 18.0f) / (255 * 255)
	};

	vfloat4 ep0 = ep.endpt0[partition_index];
	vfloat4 ep1 = ep.endpt1[partition_index];

	float ep1_min = hmin_rgb_s(ep1);
	ep1_min = astc::max(ep1_min, 0.0f);

	float error_weight_rgbsum = hadd_rgb_s(error_weight);

	float range_upper_limit_rgb = encode_hdr_rgb ? 61440.0f : 65535.0f;
	float range_upper_limit_alpha = encode_hdr_alpha ? 61440.0f : 65535.0f;

	// It is possible to get endpoint colors significantly outside [0,upper-limit] even if the
	// input data are safely contained in [0,upper-limit]; we need to add an error term for this
	vfloat4 ep0_range_error_high;
	vfloat4 ep1_range_error_high;
	vfloat4 ep0_range_error_low;
	vfloat4 ep1_range_error_low;

	vfloat4 offset(range_upper_limit_rgb, range_upper_limit_rgb, range_upper_limit_rgb, range_upper_limit_alpha);
	ep0_range_error_high = max(ep0 - offset, 0.0f);
	ep1_range_error_high = max(ep1 - offset, 0.0f);

	ep0_range_error_low = min(ep0, 0.0f);
	ep1_range_error_low = min(ep1, 0.0f);

	vfloat4 sum_range_error =
		(ep0_range_error_low * ep0_range_error_low) +
		(ep1_range_error_low * ep1_range_error_low) +
		(ep0_range_error_high * ep0_range_error_high) +
		(ep1_range_error_high * ep1_range_error_high);

	float rgb_range_error = dot3_s(sum_range_error, error_weight)
	                      * 0.5f * static_cast<float>(partition_size);
	float alpha_range_error = sum_range_error.lane<3>() * error_weight.lane<3>()
	                        * 0.5f * static_cast<float>(partition_size);

	if (encode_hdr_rgb)
	{

		// Collect some statistics
		float af, cf;
		if (ep1.lane<0>() > ep1.lane<1>() && ep1.lane<0>() > ep1.lane<2>())
		{
			af = ep1.lane<0>();
			cf = ep1.lane<0>() - ep0.lane<0>();
		}
		else if (ep1.lane<1>() > ep1.lane<2>())
		{
			af = ep1.lane<1>();
			cf = ep1.lane<1>() - ep0.lane<1>();
		}
		else
		{
			af = ep1.lane<2>();
			cf = ep1.lane<2>() - ep0.lane<2>();
		}

		// Estimate of color-component spread in high endpoint color
		float bf = af - ep1_min;
		vfloat4 prd = (ep1 - vfloat4(cf)).swz<0, 1, 2>();
		vfloat4 pdif = prd - ep0.swz<0, 1, 2>();
		// Estimate of color-component spread in low endpoint color
		float df = hmax_s(abs(pdif));

		int b = (int)bf;
		int c = (int)cf;
		int d = (int)df;

		// Determine which one of the 6 submodes is likely to be used in case of an RGBO-mode
		int rgbo_mode = 5;		// 7 bits per component
		// mode 4: 8 7 6
		if (b < 32768 && c < 16384)
		{
			rgbo_mode = 4;
		}

		// mode 3: 9 6 7
		if (b < 8192 && c < 16384)
		{
			rgbo_mode = 3;
		}

		// mode 2: 10 5 8
		if (b < 2048 && c < 16384)
		{
			rgbo_mode = 2;
		}

		// mode 1: 11 6 5
		if (b < 2048 && c < 1024)
		{
			rgbo_mode = 1;
		}

		// mode 0: 11 5 7
		if (b < 1024 && c < 4096)
		{
			rgbo_mode = 0;
		}

		// Determine which one of the 9 submodes is likely to be used in case of an RGB-mode.
		int rgb_mode = 8;		// 8 bits per component, except 7 bits for blue

		// mode 0: 9 7 6 7
		if (b < 16384 && c < 8192 && d < 8192)
		{
			rgb_mode = 0;
		}

		// mode 1: 9 8 6 6
		if (b < 32768 && c < 8192 && d < 4096)
		{
			rgb_mode = 1;
		}

		// mode 2: 10 6 7 7
		if (b < 4096 && c < 8192 && d < 4096)
		{
			rgb_mode = 2;
		}

		// mode 3: 10 7 7 6
		if (b < 8192 && c < 8192 && d < 2048)
		{
			rgb_mode = 3;
		}

		// mode 4: 11 8 6 5
		if (b < 8192 && c < 2048 && d < 512)
		{
			rgb_mode = 4;
		}

		// mode 5: 11 6 8 6
		if (b < 2048 && c < 8192 && d < 1024)
		{
			rgb_mode = 5;
		}

		// mode 6: 12 7 7 5
		if (b < 2048 && c < 2048 && d < 256)
		{
			rgb_mode = 6;
		}

		// mode 7: 12 6 7 6
		if (b < 1024 && c < 2048 && d < 512)
		{
			rgb_mode = 7;
		}

		static const float rgbo_error_scales[6] { 4.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f };
		static const float rgb_error_scales[9] { 64.0f, 64.0f, 16.0f, 16.0f, 4.0f, 4.0f, 1.0f, 1.0f, 384.0f };

		float mode7mult = rgbo_error_scales[rgbo_mode] * 0.0015f;  // Empirically determined ....
		float mode11mult = rgb_error_scales[rgb_mode] * 0.010f;    // Empirically determined ....


		float lum_high = hadd_rgb_s(ep1) * (1.0f / 3.0f);
		float lum_low = hadd_rgb_s(ep0) * (1.0f / 3.0f);
		float lumdif = lum_high - lum_low;
		float mode23mult = lumdif < 960 ? 4.0f : lumdif < 3968 ? 16.0f : 128.0f;

		mode23mult *= 0.0005f;  // Empirically determined ....

		// Pick among the available HDR endpoint modes
		for (int i = 0; i < 8; i++)
		{
			best_error[i][3] = 1e30f;
			format_of_choice[i][3] = encode_hdr_alpha ? FMT_HDR_RGBA : FMT_HDR_RGB_LDR_ALPHA;
			best_error[i][2] = 1e30f;
			format_of_choice[i][2] = FMT_HDR_RGB;
			best_error[i][1] = 1e30f;
			format_of_choice[i][1] = FMT_HDR_RGB_SCALE;
			best_error[i][0] = 1e30f;
			format_of_choice[i][0] = FMT_HDR_LUMINANCE_LARGE_RANGE;
		}

		for (int i = 8; i < 21; i++)
		{
			// The base_quant_error should depend on the scale-factor that would be used during
			// actual encode of the color value

			float base_quant_error = baseline_quant_error[i] * static_cast<float>(partition_size);
			float rgb_quantization_error = error_weight_rgbsum * base_quant_error * 2.0f;
			float alpha_quantization_error = error_weight.lane<3>() * base_quant_error * 2.0f;
			float rgba_quantization_error = rgb_quantization_error + alpha_quantization_error;

			// For 8 integers, we have two encodings: one with HDR A and another one with LDR A

			float full_hdr_rgba_error = rgba_quantization_error + rgb_range_error + alpha_range_error;
			best_error[i][3] = full_hdr_rgba_error;
			format_of_choice[i][3] = encode_hdr_alpha ? FMT_HDR_RGBA : FMT_HDR_RGB_LDR_ALPHA;

			// For 6 integers, we have one HDR-RGB encoding
			float full_hdr_rgb_error = (rgb_quantization_error * mode11mult) + rgb_range_error + eci.alpha_drop_error;
			best_error[i][2] = full_hdr_rgb_error;
			format_of_choice[i][2] = FMT_HDR_RGB;

			// For 4 integers, we have one HDR-RGB-Scale encoding
			float hdr_rgb_scale_error = (rgb_quantization_error * mode7mult) + rgb_range_error + eci.alpha_drop_error + eci.rgb_luma_error;

			best_error[i][1] = hdr_rgb_scale_error;
			format_of_choice[i][1] = FMT_HDR_RGB_SCALE;

			// For 2 integers, we assume luminance-with-large-range
			float hdr_luminance_error = (rgb_quantization_error * mode23mult) + rgb_range_error + eci.alpha_drop_error + eci.luminance_error;
			best_error[i][0] = hdr_luminance_error;
			format_of_choice[i][0] = FMT_HDR_LUMINANCE_LARGE_RANGE;
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			best_error[i][3] = 1e30f;
			best_error[i][2] = 1e30f;
			best_error[i][1] = 1e30f;
			best_error[i][0] = 1e30f;

			format_of_choice[i][3] = FMT_RGBA;
			format_of_choice[i][2] = FMT_RGB;
			format_of_choice[i][1] = FMT_RGB_SCALE;
			format_of_choice[i][0] = FMT_LUMINANCE;
		}

		float base_quant_error_rgb = error_weight_rgbsum * static_cast<float>(partition_size);
		float base_quant_error_a = error_weight.lane<3>() * static_cast<float>(partition_size);
		float base_quant_error_rgba = base_quant_error_rgb + base_quant_error_a;

		float error_scale_bc_rgba = eci.can_blue_contract ? 0.625f : 1.0f;
		float error_scale_oe_rgba = eci.can_offset_encode ? 0.5f : 1.0f;

		float error_scale_bc_rgb = eci.can_blue_contract ? 0.5f : 1.0f;
		float error_scale_oe_rgb = eci.can_offset_encode ? 0.25f : 1.0f;

		// Pick among the available LDR endpoint modes
		for (int i = 4; i < 21; i++)
		{
			// Offset encoding not possible at higher quant levels
			if (i == 19)
			{
				error_scale_oe_rgba = 1.0f;
				error_scale_oe_rgb = 1.0f;
			}

			float base_quant_error = baseline_quant_error[i];
			float quant_error_rgb  = base_quant_error_rgb * base_quant_error;
			float quant_error_rgba = base_quant_error_rgba * base_quant_error;

			// 8 integers can encode as RGBA+RGBA
			float full_ldr_rgba_error = quant_error_rgba
			                          * error_scale_bc_rgba
			                          * error_scale_oe_rgba
			                          + rgb_range_error
			                          + alpha_range_error;

			best_error[i][3] = full_ldr_rgba_error;
			format_of_choice[i][3] = FMT_RGBA;

			// 6 integers can encode as RGB+RGB or RGBS+AA
			float full_ldr_rgb_error = quant_error_rgb
			                         * error_scale_bc_rgb
			                         * error_scale_oe_rgb
			                         + rgb_range_error
			                         + eci.alpha_drop_error;

			float rgbs_alpha_error = quant_error_rgba
			                       + eci.rgb_scale_error
			                       + rgb_range_error
			                       + alpha_range_error;

			if (rgbs_alpha_error < full_ldr_rgb_error)
			{
				best_error[i][2] = rgbs_alpha_error;
				format_of_choice[i][2] = FMT_RGB_SCALE_ALPHA;
			}
			else
			{
				best_error[i][2] = full_ldr_rgb_error;
				format_of_choice[i][2] = FMT_RGB;
			}

			// 4 integers can encode as RGBS or LA+LA
			float ldr_rgbs_error = quant_error_rgb
			                     + rgb_range_error
			                     + eci.alpha_drop_error
			                     + eci.rgb_scale_error;

			float lum_alpha_error = quant_error_rgba
			                      + rgb_range_error
			                      + alpha_range_error
			                      + eci.luminance_error;

			if (ldr_rgbs_error < lum_alpha_error)
			{
				best_error[i][1] = ldr_rgbs_error;
				format_of_choice[i][1] = FMT_RGB_SCALE;
			}
			else
			{
				best_error[i][1] = lum_alpha_error;
				format_of_choice[i][1] = FMT_LUMINANCE_ALPHA;
			}

			// 2 integers can encode as L+L
			float luminance_error = quant_error_rgb
			                      + rgb_range_error
			                      + eci.alpha_drop_error
			                      + eci.luminance_error;

			best_error[i][0] = luminance_error;
			format_of_choice[i][0] = FMT_LUMINANCE;
		}
	}
}

/**
 * @brief For one partition compute the best format and quantization for a given bit count.
 *
 * @param      best_combined_error    The best error for each quant level and integer count.
 * @param      best_combined_format   The best format for each quant level and integer count.
 * @param      bits_available         The number of bits available for encoding.
 * @param[out] best_quant_level       The output best color quant level.
 * @param[out] best_format            The output best color format.
 * @param[out] best_error             The output error for the best pairing.
 */
static void one_partition_find_best_combination_for_bitcount(
	const float best_combined_error[21][4],
	const int best_combined_format[21][4],
	int bits_available,
	quant_method& best_quant_level,
	int& best_format,
	float& best_error
) {
	int best_integer_count = 0;
	float best_integer_count_error = 1e20f;

	for (int integer_count = 1; integer_count <= 4;  integer_count++)
	{
		// Compute the quantization level for a given number of integers and a given number of bits
		int quant_level = quant_mode_table[integer_count][bits_available];

		// Don't have enough bits to represent a given endpoint format at all!
		if (quant_level == -1)
		{
			continue;
		}

		float integer_count_error = best_combined_error[quant_level][integer_count - 1];
		if (integer_count_error < best_integer_count_error)
		{
			best_integer_count_error = integer_count_error;
			best_integer_count = integer_count - 1;
		}
	}

	int ql = quant_mode_table[best_integer_count + 1][bits_available];

	best_quant_level = (quant_method)ql;
	best_format = FMT_LUMINANCE;
	best_error = best_integer_count_error;

	if (ql >= 0)
	{
		best_format = best_combined_format[ql][best_integer_count];
	}
}

/**
 * @brief For 2 partitions compute the best format combinations for every pair of quant mode and integer count.
 *
 * @param      best_error             The best error for a single endpoint quant level and integer count.
 * @param      best_format            The best format for a single endpoint quant level and integer count.
 * @param[out] best_combined_error    The best combined error pairings for the 2 partitions.
 * @param[out] best_combined_format   The best combined format pairings for the 2 partitions.
 */
static void two_partitions_find_best_combination_for_every_quantization_and_integer_count(
	const float best_error[2][21][4],	// indexed by (partition, quant-level, integer-pair-count-minus-1)
	const int best_format[2][21][4],
	float best_combined_error[21][7],	// indexed by (quant-level, integer-pair-count-minus-2)
	int best_combined_format[21][7][2]
) {
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			best_combined_error[i][j] = 1e30f;
		}
	}

	for (int quant = 5; quant < 21; quant++)
	{
		for (int i = 0; i < 4; i++)	// integer-count for first endpoint-pair
		{
			for (int j = 0; j < 4; j++)	// integer-count for second endpoint-pair
			{
				int low2 = astc::min(i, j);
				int high2 = astc::max(i, j);
				if ((high2 - low2) > 1)
				{
					continue;
				}

				int intcnt = i + j;
				float errorterm = astc::min(best_error[0][quant][i] + best_error[1][quant][j], 1e10f);
				if (errorterm <= best_combined_error[quant][intcnt])
				{
					best_combined_error[quant][intcnt] = errorterm;
					best_combined_format[quant][intcnt][0] = best_format[0][quant][i];
					best_combined_format[quant][intcnt][1] = best_format[1][quant][j];
				}
			}
		}
	}
}

/**
 * @brief For 2 partitions compute the best format and quantization for a given bit count.
 *
 * @param      best_combined_error    The best error for each quant level and integer count.
 * @param      best_combined_format   The best format for each quant level and integer count.
 * @param      bits_available         The number of bits available for encoding.
 * @param[out] best_quant_level       The output best color quant level.
 * @param[out] best_quant_level_mod   The output best color quant level assuming two more bits are available.
 * @param[out] best_formats           The output best color formats.
 * @param[out] best_error             The output error for the best pairing.
 */
static void two_partitions_find_best_combination_for_bitcount(
	float best_combined_error[21][7],
	int best_combined_format[21][7][2],
	int bits_available,
	quant_method& best_quant_level,
	quant_method& best_quant_level_mod,
	int* best_formats,
	float& best_error
) {
	int best_integer_count = 0;
	float best_integer_count_error = 1e20f;

	for (int integer_count = 2; integer_count <= 8; integer_count++)
	{
		// Compute the quantization level for a given number of integers and a given number of bits
		int quant_level = quant_mode_table[integer_count][bits_available];

		// Don't have enough bits to represent a given endpoint format at all!
		if (quant_level == -1)
		{
			break;
		}

		float integer_count_error = best_combined_error[quant_level][integer_count - 2];
		if (integer_count_error < best_integer_count_error)
		{
			best_integer_count_error = integer_count_error;
			best_integer_count = integer_count;
		}
	}

	int ql = quant_mode_table[best_integer_count][bits_available];
	int ql_mod = quant_mode_table[best_integer_count][bits_available + 2];

	best_quant_level = (quant_method)ql;
	best_quant_level_mod = (quant_method)ql_mod;
	best_error = best_integer_count_error;
	if (ql >= 0)
	{
		for (int i = 0; i < 2; i++)
		{
			best_formats[i] = best_combined_format[ql][best_integer_count - 2][i];
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			best_formats[i] = FMT_LUMINANCE;
		}
	}
}

/**
 * @brief For 3 partitions compute the best format combinations for every pair of quant mode and integer count.
 *
 * @param      best_error             The best error for a single endpoint quant level and integer count.
 * @param      best_format            The best format for a single endpoint quant level and integer count.
 * @param[out] best_combined_error    The best combined error pairings for the 3 partitions.
 * @param[out] best_combined_format   The best combined format pairings for the 3 partitions.
 */
static void three_partitions_find_best_combination_for_every_quantization_and_integer_count(
	const float best_error[3][21][4],	// indexed by (partition, quant-level, integer-count)
	const int best_format[3][21][4],
	float best_combined_error[21][10],
	int best_combined_format[21][10][3]
) {
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			best_combined_error[i][j] = 1e30f;
		}
	}

	for (int quant = 5; quant < 21; quant++)
	{
		for (int i = 0; i < 4; i++)	// integer-count for first endpoint-pair
		{
			for (int j = 0; j < 4; j++)	// integer-count for second endpoint-pair
			{
				int low2 = astc::min(i, j);
				int high2 = astc::max(i, j);
				if ((high2 - low2) > 1)
				{
					continue;
				}

				for (int k = 0; k < 4; k++)	// integer-count for third endpoint-pair
				{
					int low3 = astc::min(k, low2);
					int high3 = astc::max(k, high2);
					if ((high3 - low3) > 1)
					{
						continue;
					}

					int intcnt = i + j + k;
					float errorterm = astc::min(best_error[0][quant][i] + best_error[1][quant][j] + best_error[2][quant][k], 1e10f);
					if (errorterm <= best_combined_error[quant][intcnt])
					{
						best_combined_error[quant][intcnt] = errorterm;
						best_combined_format[quant][intcnt][0] = best_format[0][quant][i];
						best_combined_format[quant][intcnt][1] = best_format[1][quant][j];
						best_combined_format[quant][intcnt][2] = best_format[2][quant][k];
					}
				}
			}
		}
	}
}

/**
 * @brief For 3 partitions compute the best format and quantization for a given bit count.
 *
 * @param      best_combined_error    The best error for each quant level and integer count.
 * @param      best_combined_format   The best format for each quant level and integer count.
 * @param      bits_available         The number of bits available for encoding.
 * @param[out] best_quant_level       The output best color quant level.
 * @param[out] best_quant_level_mod   The output best color quant level assuming two more bits are available.
 * @param[out] best_formats           The output best color formats.
 * @param[out] best_error             The output error for the best pairing.
 */
static void three_partitions_find_best_combination_for_bitcount(
	const float best_combined_error[21][10],
	const int best_combined_format[21][10][3],
	int bits_available,
	quant_method& best_quant_level,
	quant_method& best_quant_level_mod,
	int* best_formats,
	float& best_error
) {
	int best_integer_count = 0;
	float best_integer_count_error = 1e20f;

	for (int integer_count = 3; integer_count <= 9; integer_count++)
	{
		// Compute the quantization level for a given number of integers and a given number of bits
		int quant_level = quant_mode_table[integer_count][bits_available];

		// Don't have enough bits to represent a given endpoint format at all!
		if (quant_level == -1)
		{
			break;
		}

		float integer_count_error = best_combined_error[quant_level][integer_count - 3];
		if (integer_count_error < best_integer_count_error)
		{
			best_integer_count_error = integer_count_error;
			best_integer_count = integer_count;
		}
	}

	int ql = quant_mode_table[best_integer_count][bits_available];
	int ql_mod = quant_mode_table[best_integer_count][bits_available + 5];

	best_quant_level = (quant_method)ql;
	best_quant_level_mod = (quant_method)ql_mod;
	best_error = best_integer_count_error;
	if (ql >= 0)
	{
		for (int i = 0; i < 3; i++)
		{
			best_formats[i] = best_combined_format[ql][best_integer_count - 3][i];
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			best_formats[i] = FMT_LUMINANCE;
		}
	}
}

/**
 * @brief For 4 partitions compute the best format combinations for every pair of quant mode and integer count.
 *
 * @param      best_error             The best error for a single endpoint quant level and integer count.
 * @param      best_format            The best format for a single endpoint quant level and integer count.
 * @param[out] best_combined_error    The best combined error pairings for the 4 partitions.
 * @param[out] best_combined_format   The best combined format pairings for the 4 partitions.
 */
static void four_partitions_find_best_combination_for_every_quantization_and_integer_count(
	const float best_error[4][21][4],	// indexed by (partition, quant-level, integer-count)
	const int best_format[4][21][4],
	float best_combined_error[21][13],
	int best_combined_format[21][13][4]
) {
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			best_combined_error[i][j] = 1e30f;
		}
	}

	for (int quant = 5; quant < 21; quant++)
	{
		for (int i = 0; i < 4; i++)	// integer-count for first endpoint-pair
		{
			for (int j = 0; j < 4; j++)	// integer-count for second endpoint-pair
			{
				int low2 = astc::min(i, j);
				int high2 = astc::max(i, j);
				if ((high2 - low2) > 1)
				{
					continue;
				}

				for (int k = 0; k < 4; k++)	// integer-count for third endpoint-pair
				{
					int low3 = astc::min(k, low2);
					int high3 = astc::max(k, high2);
					if ((high3 - low3) > 1)
					{
						continue;
					}

					for (int l = 0; l < 4; l++)	// integer-count for fourth endpoint-pair
					{
						int low4 = astc::min(l, low3);
						int high4 = astc::max(l, high3);
						if ((high4 - low4) > 1)
						{
							continue;
						}

						int intcnt = i + j + k + l;
						float errorterm = astc::min(best_error[0][quant][i] + best_error[1][quant][j] + best_error[2][quant][k] + best_error[3][quant][l], 1e10f);
						if (errorterm <= best_combined_error[quant][intcnt])
						{
							best_combined_error[quant][intcnt] = errorterm;
							best_combined_format[quant][intcnt][0] = best_format[0][quant][i];
							best_combined_format[quant][intcnt][1] = best_format[1][quant][j];
							best_combined_format[quant][intcnt][2] = best_format[2][quant][k];
							best_combined_format[quant][intcnt][3] = best_format[3][quant][l];
						}
					}
				}
			}
		}
	}
}

/**
 * @brief For 4 partitions compute the best format and quantization for a given bit count.
 *
 * @param      best_combined_error    The best error for each quant level and integer count.
 * @param      best_combined_format   The best format for each quant level and integer count.
 * @param      bits_available         The number of bits available for encoding.
 * @param[out] best_quant_level       The output best color quant level.
 * @param[out] best_quant_level_mod   The output best color quant level assuming two more bits are available.
 * @param[out] best_formats           The output best color formats.
 * @param[out] best_error             The output error for the best pairing.
 */
static void four_partitions_find_best_combination_for_bitcount(
	const float best_combined_error[21][13],
	const int best_combined_format[21][13][4],
	int bits_available,
	quant_method& best_quant_level,
	quant_method& best_quant_level_mod,
	int* best_formats,
	float& best_error
) {
	int best_integer_count = 0;
	float best_integer_count_error = 1e20f;

	for (int integer_count = 4; integer_count <= 9; integer_count++)
	{
		// Compute the quantization level for a given number of integers and a given number of bits
		int quant_level = quant_mode_table[integer_count][bits_available];

		// Don't have enough bits to represent a given endpoint format at all!
		if (quant_level == -1)
		{
			break;
		}

		float integer_count_error = best_combined_error[quant_level][integer_count - 4];
		if (integer_count_error < best_integer_count_error)
		{
			best_integer_count_error = integer_count_error;
			best_integer_count = integer_count;
		}
	}

	int ql = quant_mode_table[best_integer_count][bits_available];
	int ql_mod = quant_mode_table[best_integer_count][bits_available + 8];

	best_quant_level = (quant_method)ql;
	best_quant_level_mod = (quant_method)ql_mod;
	best_error = best_integer_count_error;
	if (ql >= 0)
	{
		for (int i = 0; i < 4; i++)
		{
			best_formats[i] = best_combined_format[ql][best_integer_count - 4][i];
		}
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			best_formats[i] = FMT_LUMINANCE;
		}
	}
}

/* See header for documentation. */
unsigned int compute_ideal_endpoint_formats(
	const block_size_descriptor& bsd,
	const partition_info& pi,
	const image_block& blk,
	const error_weight_block& ewb,
	const endpoints& ep,
	 // bitcounts and errors computed for the various quantization methods
	const int* qwt_bitcounts,
	const float* qwt_errors,
	unsigned int tune_candidate_limit,
	// output data
	int partition_format_specifiers[TUNE_MAX_TRIAL_CANDIDATES][BLOCK_MAX_PARTITIONS],
	int block_mode[TUNE_MAX_TRIAL_CANDIDATES],
	quant_method quant_level[TUNE_MAX_TRIAL_CANDIDATES],
	quant_method quant_level_mod[TUNE_MAX_TRIAL_CANDIDATES]
) {
	int partition_count = pi.partition_count;

	promise(partition_count > 0);
	promise(bsd.block_mode_count > 0);

	int encode_hdr_rgb = blk.rgb_lns[0];
	int encode_hdr_alpha = blk.alpha_lns[0];

	// Compute the errors that result from various encoding choices (such as using luminance instead
	// of RGB, discarding Alpha, using RGB-scale in place of two separate RGB endpoints and so on)
	encoding_choice_errors eci[BLOCK_MAX_PARTITIONS];
	compute_encoding_choice_errors(bsd, blk, pi, ewb, ep, eci);

	// For each partition, compute the error weights to apply for that partition
	partition_metrics pms[BLOCK_MAX_PARTITIONS];

	compute_partition_error_color_weightings(ewb, pi, pms);

	float best_error[BLOCK_MAX_PARTITIONS][21][4];
	int format_of_choice[BLOCK_MAX_PARTITIONS][21][4];
	for (int i = 0; i < partition_count; i++)
	{
		compute_color_error_for_every_integer_count_and_quant_level(
		    encode_hdr_rgb, encode_hdr_alpha, i,
		    pi, eci[i], ep, pms[i].error_weight, best_error[i],
		    format_of_choice[i]);
	}

	alignas(ASTCENC_VECALIGN) float errors_of_best_combination[WEIGHTS_MAX_BLOCK_MODES];
	alignas(ASTCENC_VECALIGN) quant_method best_quant_levels[WEIGHTS_MAX_BLOCK_MODES];
	quant_method best_quant_levels_mod[WEIGHTS_MAX_BLOCK_MODES];
	int best_ep_formats[WEIGHTS_MAX_BLOCK_MODES][4];

	// Ensure that the "overstep" of the last iteration in the vectorized loop will contain data
	// that will never be picked as best candidate
	const int packed_mode_count = bsd.block_mode_count;
	const int packed_mode_count_simd_up = round_up_to_simd_multiple_vla(packed_mode_count);
	for (int i = packed_mode_count; i < packed_mode_count_simd_up; ++i)
	{
		errors_of_best_combination[i] = 1e30f;
		best_quant_levels[i] = QUANT_2;
		best_quant_levels_mod[i] = QUANT_2;
	}

	// The block contains 1 partition
	if (partition_count == 1)
	{
		float error_of_best_combination;
		for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
		{
			if (qwt_errors[i] >= 1e29f)
			{
				errors_of_best_combination[i] = 1e30f;
				continue;
			}

			one_partition_find_best_combination_for_bitcount(
			    best_error[0], format_of_choice[0], qwt_bitcounts[i],
			    best_quant_levels[i], best_ep_formats[i][0], error_of_best_combination);
			error_of_best_combination += qwt_errors[i];

			errors_of_best_combination[i] = error_of_best_combination;
			best_quant_levels_mod[i] = best_quant_levels[i];
		}
	}
	// The block contains 2 partitions
	else if (partition_count == 2)
	{
		float combined_best_error[21][7];
		int formats_of_choice[21][7][2];

		two_partitions_find_best_combination_for_every_quantization_and_integer_count(
		    best_error, format_of_choice, combined_best_error, formats_of_choice);


		for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
		{
			if (qwt_errors[i] >= 1e29f)
			{
				errors_of_best_combination[i] = 1e30f;
				continue;
			}

			float error_of_best_combination;
			two_partitions_find_best_combination_for_bitcount(
			    combined_best_error, formats_of_choice, qwt_bitcounts[i],
			    best_quant_levels[i], best_quant_levels_mod[i],
			    best_ep_formats[i], error_of_best_combination);

			errors_of_best_combination[i] = error_of_best_combination + qwt_errors[i];
		}
	}
	// The block contains 3 partitions
	else if (partition_count == 3)
	{
		float combined_best_error[21][10];
		int formats_of_choice[21][10][3];

		three_partitions_find_best_combination_for_every_quantization_and_integer_count(
		    best_error, format_of_choice, combined_best_error, formats_of_choice);

		for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
		{
			if (qwt_errors[i] >= 1e29f)
			{
				errors_of_best_combination[i] = 1e30f;
				continue;
			}

			float error_of_best_combination;
			three_partitions_find_best_combination_for_bitcount(
			    combined_best_error, formats_of_choice, qwt_bitcounts[i],
			    best_quant_levels[i], best_quant_levels_mod[i],
			    best_ep_formats[i], error_of_best_combination);

			errors_of_best_combination[i] = error_of_best_combination + qwt_errors[i];
		}
	}
	// The block contains 4 partitions
	else if (partition_count == 4)
	{
		float combined_best_error[21][13];
		int formats_of_choice[21][13][4];

		four_partitions_find_best_combination_for_every_quantization_and_integer_count(
		    best_error, format_of_choice, combined_best_error, formats_of_choice);

		for (unsigned int i = 0; i < bsd.block_mode_count; ++i)
		{
			if (qwt_errors[i] >= 1e29f)
			{
				errors_of_best_combination[i] = 1e30f;
				continue;
			}

			float error_of_best_combination;
			four_partitions_find_best_combination_for_bitcount(
			    combined_best_error, formats_of_choice, qwt_bitcounts[i],
			    best_quant_levels[i], best_quant_levels_mod[i],
			    best_ep_formats[i], error_of_best_combination);

			errors_of_best_combination[i] = error_of_best_combination + qwt_errors[i];
		}
	}

	// Go through the results and pick the best candidate modes
	int best_error_weights[TUNE_MAX_TRIAL_CANDIDATES];
	for (unsigned int i = 0; i < tune_candidate_limit; i++)
	{
		vint vbest_error_index(-1);
		vfloat vbest_ep_error(1e30f);
		vint lane_ids = vint::lane_id();
		for (unsigned int j = 0; j < bsd.block_mode_count; j += ASTCENC_SIMD_WIDTH)
		{
			vfloat err = vfloat(&errors_of_best_combination[j]);
			vmask mask1 = err < vbest_ep_error;
			vmask mask2 = vint((int*)(&best_quant_levels[j])) > vint(4);
			vmask mask = mask1 & mask2;
			vbest_ep_error = select(vbest_ep_error, err, mask);
			vbest_error_index = select(vbest_error_index, lane_ids, mask);
			lane_ids = lane_ids + vint(ASTCENC_SIMD_WIDTH);
		}

		// Pick best mode from the SIMD result, using lowest matching index to ensure invariance
		vmask lanes_min_error = vbest_ep_error == hmin(vbest_ep_error);
		vbest_error_index = select(vint(0x7FFFFFFF), vbest_error_index, lanes_min_error);
		vbest_error_index = hmin(vbest_error_index);
		int best_error_index = vbest_error_index.lane<0>();

		best_error_weights[i] = best_error_index;

		// Max the error for this candidate so we don't pick it again
		if (best_error_index >= 0)
		{
			errors_of_best_combination[best_error_index] = 1e30f;
		}
		// Early-out if no more candidates are valid
		else
		{
			break;
		}
	}

	for (unsigned int i = 0; i < tune_candidate_limit; i++)
	{
		if (best_error_weights[i] < 0)
		{
			return i;
		}

		block_mode[i] = best_error_weights[i];
		quant_level[i] = best_quant_levels[best_error_weights[i]];
		assert(quant_level[i] >= 0 && quant_level[i] < 21);
		quant_level_mod[i] = best_quant_levels_mod[best_error_weights[i]];
		for (int j = 0; j < partition_count; j++)
		{
			partition_format_specifiers[i][j] = best_ep_formats[best_error_weights[i]][j];
		}
	}

	return tune_candidate_limit;
}

#endif
