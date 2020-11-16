// ----------------------------------------------------------------------------
//  This confidential and proprietary software may be used only as authorised
//  by a licensing agreement from Arm Limited.
//      (C) COPYRIGHT 2011-2019 Arm Limited, ALL RIGHTS RESERVED
//  The entire notice above must be reproduced on all authorised copies and
//  copies may only be made to the extent permitted by a licensing agreement
//  from Arm Limited.
// ----------------------------------------------------------------------------

/**
 * @brief Library of maths functions.
 */


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "mathlib.h"

float nan(int p)
{
	union
	{
		int p;
		float q;
	} v;
	v.p = p | 0x7FC00000U;
	return v.q;
}

#if (!_MSC_VER) && (__cplusplus < 201103L)
float fmax(float p, float q)
{
	if (p != p)
		return q;
	if (q != q)
		return p;
	if (p > q)
		return p;
	return q;
}

float fmin(float p, float q)
{
	if (p != p)
		return q;
	if (q != q)
		return p;
	if (p < q)
		return p;
	return q;
}
#endif  // C++11

float2 fmax(float2 p, float2 q)
{
	return float2(fmax(p.x, q.x), fmax(p.y, q.y));
}

float3 fmax(float3 p, float3 q)
{
	return float3(fmax(p.x, q.x), fmax(p.y, q.y), fmax(p.z, q.z));
}

float4 fmax(float4 p, float4 q)
{
	return float4(fmax(p.x, q.x), fmax(p.y, q.y), fmax(p.z, q.z), fmax(p.w, q.w));
}

float2 fmin(float2 p, float2 q)
{
	return float2(fmin(p.x, q.x), fmin(p.y, q.y));
}

float3 fmin(float3 p, float3 q)
{
	return float3(fmin(p.x, q.x), fmin(p.y, q.y), fmin(p.z, q.z));
}

float4 fmin(float4 p, float4 q)
{
	return float4(fmin(p.x, q.x), fmin(p.y, q.y), fmin(p.z, q.z), fmin(p.w, q.w));
}

float3 cross(float3 p, float3 q)
{
	return p.yzx * q.zxy - p.zxy * q.yzx;
}

float4 cross(float4 p, float4 q)
{
	return float4(p.yzx * q.zxy - p.zxy * q.yzx, 0.0f);
}

float length(float2 p)
{
	return sqrt(dot(p, p));
}

float length(float3 p)
{
	return sqrt(dot(p, p));
}

float length(float4 p)
{
	return sqrt(dot(p, p));
}

float length_sqr(float2 p)
{
	return dot(p, p);
}

float length_sqr(float3 p)
{
	return dot(p, p);
}

float length_sqr(float4 p)
{
	return dot(p, p);
}

float distance(float2 p, float2 q)
{
	return length(q - p);
}

float distance(float3 p, float3 q)
{
	return length(q - p);
}

float distance(float4 p, float4 q)
{
	return length(q - p);
}

float distance_sqr(float2 p, float2 q)
{
	return length_sqr(q - p);
}

float distance_sqr(float3 p, float3 q)
{
	return length_sqr(q - p);
}

float distance_sqr(float4 p, float4 q)
{
	return length_sqr(q - p);
}

float2 normalize(float2 p)
{
	return p / length(p);
}

float3 normalize(float3 p)
{
	return p / length(p);
}

float4 normalize(float4 p)
{
	return p / length(p);
}

/**************************************************
  matrix functions, for 2x2, 3x3 and 4x4 matrices:

   * determinant
   * transform
   * inverse
*************************************************/
float determinant(mat2 p)
{
	float2 v = p.v[0].xy * p.v[1].yx;
	return v.x - v.y;
}

float determinant(mat3 p)
{
	return dot(p.v[0], cross(p.v[1], p.v[2]));
}

float determinant(mat4 p)
{
	return dot(p.v[0],
			   float4(dot(p.v[1].yzw, cross(p.v[2].yzw, p.v[3].yzw)),
					  -dot(p.v[1].xzw, cross(p.v[2].xzw, p.v[3].xzw)), dot(p.v[1].xyw, cross(p.v[2].xyw, p.v[3].xyw)), -dot(p.v[1].xyz, cross(p.v[2].xyz, p.v[3].xyz))));
}

float2 transform(mat2 p, float2 q)
{
	return float2(dot(p.v[0], q), dot(p.v[1], q));
}


float3 transform(mat3 p, float3 q)
{
	return float3(dot(p.v[0], q), dot(p.v[1], q), dot(p.v[2], q));
}


float4 transform(mat4 p, float4 q)
{
	return float4(dot(p.v[0], q), dot(p.v[1], q), dot(p.v[2], q), dot(p.v[3], q));
}

mat2 invert(mat2 p)
{
	float rdet = 1.0f / determinant(p);
	mat2 res;
	res.v[0] = float2(p.v[1].y, -p.v[0].y) * rdet;
	res.v[1] = float2(-p.v[1].x, p.v[0].x) * rdet;
	return res;
}

mat3 invert(mat3 p)
{
	float3 cross0 = cross(p.v[1], p.v[2]);
	float det = dot(cross0, p.v[0]);
	float rdet = 1.0f / det;
	mat3 res;
	float3 prd0 = cross0 * rdet;
	float3 prd1 = cross(p.v[2], p.v[0]) * rdet;
	float3 prd2 = cross(p.v[0], p.v[1]) * rdet;
	res.v[0] = float3(prd0.x, prd1.x, prd2.x);
	res.v[1] = float3(prd0.y, prd1.y, prd2.y);
	res.v[2] = float3(prd0.z, prd1.z, prd2.z);
	return res;
}

mat4 invert(mat4 p)
{
	// cross products between the bottom two rows
	float3 bpc0 = cross(p.v[2].yzw, p.v[3].yzw);
	float3 bpc1 = cross(p.v[2].xzw, p.v[3].xzw);
	float3 bpc2 = cross(p.v[2].xyw, p.v[3].xyw);
	float3 bpc3 = cross(p.v[2].xyz, p.v[3].xyz);

	// dot-products for the top rows
	float4 row1 = float4(dot(bpc0, p.v[1].yzw),
						 -dot(bpc1, p.v[1].xzw),
						 dot(bpc2, p.v[1].xyw),
						 -dot(bpc3, p.v[1].xyz));

	float det = dot(p.v[0], row1);
	float rdet = 1.0f / det;

	mat4 res;

	float3 tpc0 = cross(p.v[0].yzw, p.v[1].yzw);
	res.v[0] = float4(row1.x, -dot(bpc0, p.v[0].yzw), dot(tpc0, p.v[3].yzw), -dot(tpc0, p.v[2].yzw)) * rdet;

	float3 tpc1 = cross(p.v[0].xzw, p.v[1].xzw);
	res.v[1] = float4(row1.y, dot(bpc1, p.v[0].xzw), -dot(tpc1, p.v[3].xzw), dot(tpc1, p.v[2].xzw)) * rdet;
	float3 tpc2 = cross(p.v[0].xyw, p.v[1].xyw);

	res.v[2] = float4(row1.z, -dot(bpc2, p.v[0].xyw), dot(tpc2, p.v[3].xyw), -dot(tpc2, p.v[2].xyw)) * rdet;

	float3 tpc3 = cross(p.v[0].xyz, p.v[1].xyz);
	res.v[3] = float4(row1.w, dot(bpc3, p.v[0].xyz), -dot(tpc3, p.v[3].xyz), dot(tpc3, p.v[2].xyz)) * rdet;

	return res;
}

// matrix multiply
mat2 operator *(mat2 a, mat2 b)
{
	mat2 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1];
	return res;
}

mat3 operator *(mat3 a, mat3 b)
{
	mat3 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1] + a.v[0].z * b.v[2];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1] + a.v[1].z * b.v[2];
	res.v[2] = a.v[2].x * b.v[0] + a.v[2].y * b.v[1] + a.v[2].z * b.v[2];
	return res;
}

mat4 operator *(mat4 a, mat4 b)
{
	mat4 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1] + a.v[0].z * b.v[2] + a.v[0].w * b.v[3];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1] + a.v[1].z * b.v[2] + a.v[1].w * b.v[3];
	res.v[2] = a.v[2].x * b.v[0] + a.v[2].y * b.v[1] + a.v[2].z * b.v[2] + a.v[2].w * b.v[3];
	res.v[3] = a.v[3].x * b.v[0] + a.v[3].y * b.v[1] + a.v[3].z * b.v[2] + a.v[3].w * b.v[3];
	return res;
}
