// ----------------------------------------------------------------------------
//  This confidential and proprietary software may be used only as authorised
//  by a licensing agreement from Arm Limited.
//      (C) COPYRIGHT 2011-2019 Arm Limited, ALL RIGHTS RESERVED
//  The entire notice above must be reproduced on all authorised copies and
//  copies may only be made to the extent permitted by a licensing agreement
//  from Arm Limited.
// ----------------------------------------------------------------------------

/**
 * @brief Library of math functions.
 */

#ifndef MATHLIB_H_INCLUDED
#define MATHLIB_H_INCLUDED

#define _USE_MATH_DEFINES
#include <cmath>

#include "vectypes.h"

float nan(int p);

#if (!_MSC_VER) && (__cplusplus < 201103L)
float fmax(float p, float q);
float fmin(float p, float q);
#endif  // C++11

float2 fmin(float2 p, float2 q);
float3 fmin(float3 p, float3 q);
float4 fmin(float4 p, float4 q);

float2 fmax(float2 p, float2 q);
float3 fmax(float3 p, float3 q);
float4 fmax(float4 p, float4 q);

static inline float dot(float2 p, float2 q)
{
	return p.x * q.x + p.y * q.y;
}

static inline float dot(float3 p, float3 q)
{
	return p.x * q.x + p.y * q.y + p.z * q.z;
}

static inline float dot(float4 p, float4 q)
{
	return p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
}

float3 cross(float3 p, float3 q);
float4 cross(float4 p, float4 q);

float length(float2 p);
float length(float3 p);
float length(float4 p);

float length_sqr(float2 p);
float length_sqr(float3 p);
float length_sqr(float4 p);

float distance(float2 p, float2 q);
float distance(float3 p, float3 q);
float distance(float4 p, float4 q);

float distance_sqr(float2 p, float2 q);
float distance_sqr(float3 p, float3 q);
float distance_sqr(float4 p, float4 q);

float2 normalize(float2 p);
float3 normalize(float3 p);
float4 normalize(float4 p);

// functions other than just basic OpenCL functions
struct mat2
{
	float2 v[2];
};

struct mat3
{
	float3 v[3];
};

struct mat4
{
	float4 v[4];
};

float determinant(mat2 p);
float determinant(mat3 p);
float determinant(mat4 p);

float2 transform(mat2 p, float2 q);
float3 transform(mat3 p, float3 q);
float4 transform(mat4 p, float4 q);

mat2 invert(mat2 p);
mat3 invert(mat3 p);
mat4 invert(mat4 p);

mat2 operator *(mat2 a, mat2 b);
mat3 operator *(mat3 a, mat3 b);
mat4 operator *(mat4 a, mat4 b);

// parametric line, 2D: The line is given by line = a + b*t.
struct line2
{
	float2 a;
	float2 b;
};

// parametric line, 3D
struct line3
{
	float3 a;
	float3 b;
};

struct line4
{
	float4 a;
	float4 b;
};

#endif
