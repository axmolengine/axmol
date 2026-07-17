//------------------------------------------------------------------------------
// Basic definitions for HLSL shaders
//------------------------------------------------------------------------------

// Vertex input semantic alias
#ifndef DIRLIGHT
#define DIRLIGHT TEXCOORD1
#endif
#ifndef POINTLIGHT
#define POINTLIGHT TEXCOORD2
#endif
#ifndef SPOTLIGHT
#define SPOTLIGHT TEXCOORD6
#endif
#ifndef SPOTLIGHT_NORM
#define SPOTLIGHT_NORM TEXCOORD7
#endif

#define vfloat_def(x, y) float4 x[(y + 3) / 4]
#define vfloat_at(x, y) x[y / 4][y % 4]

#define vvec2_def(x, y) float4 x[(y * 2 + 3) / 4]
#define vvec2_at1(x, y, z) x[(y / 2)][y % 2 * 2 + z]
#define vvec2_at(x, y) float2(vvec2_at1(x, y, 0), vvec2_at1(x, y, 1))

#define vvec3_def(x, y) float4 x[(y * 3 + 3) / 4]
#define vvec3_at1(x, y, z) x[(y * 3 + z) / 4][((y * 3 + z) % 4)]
#define vvec3_at(x, y) float3(vvec3_at1(x, y, 0), vvec3_at1(x, y, 1), vvec3_at1(x, y, 2))

#define RG8_CHANNEL xy

#define FWIDTH(dist) 0.4

// AXSLC_UV_TOP: built-in define by axslcc.
//   0 = GLSL/ESSL (origin bottom-left, Y up)
//   1 = HLSL/SPIRV/MSL (origin top-left, Y down)
//
// AX_Y_UP(v): Convert to bottom-left (Y-up) coordinate convention.
//   HLSL/SPIRV/MSL → flip: (1.0 - (v).y)
//   GLSL/ESSL       → no flip: ((v).y)
#define AX_Y_UP(v) (AXSLC_UV_TOP ? (1.0 - (v).y) : ((v).y))

//------------------------------------------------------------------------------
// Builtin Samplers
//------------------------------------------------------------------------------

// --- Linear sampling ---
SamplerState LinearClamp;
SamplerState LinearWrap;
SamplerState LinearMirror;
SamplerState LinearBorder;

// --- Point sampling ---
SamplerState PointClamp;
SamplerState PointWrap;
SamplerState PointMirror;
SamplerState PointBorder;

// --- Linear + Mipmap ---
SamplerState LinearMipClamp;
SamplerState LinearMipWrap;
SamplerState LinearMipMirror;
SamplerState LinearMipBorder;

// --- Anisotropic filtering ---
SamplerState AnisoClamp;
SamplerState AnisoWrap;
SamplerState AnisoMirror;
SamplerState AnisoBorder;

// --- Depth comparison samplers ---
SamplerComparisonState ShadowCmpClamp;
SamplerComparisonState ShadowCmpWrap;
SamplerComparisonState ShadowCmpMirror;
SamplerComparisonState ShadowCmpBorder;

// --- Special cases ---
SamplerState LinearNoMipClamp;
SamplerState PointNoMipClamp;

//------------------------------------------------------------------------------
// GPU Instancing
//------------------------------------------------------------------------------

// Declares the per-instance transform input.
//
// Example:
//   AX_INSTANCE_INPUT(TEXCOORD1)
#define AX_INSTANCE_INPUT(semantic) \
    AX_INSTANCE_INPUT_IMPL(semantic)

#define AX_INSTANCE_INPUT_IMPL(semantic) \
    AX_INSTANCE_INPUT_##semantic

#define AX_INSTANCE_INPUT_TEXCOORD0 \
    float4x4 __ax_instance : TEXCOORD0

#define AX_INSTANCE_INPUT_TEXCOORD1 \
    float4x4 __ax_instance : TEXCOORD1

#define AX_INSTANCE_INPUT_TEXCOORD2 \
    float4x4 __ax_instance : TEXCOORD2

#define AX_INSTANCE_INPUT_TEXCOORD3 \
    float4x4 __ax_instance : TEXCOORD3

#define AX_INSTANCE_INPUT_TEXCOORD4 \
    float4x4 __ax_instance : TEXCOORD4

// Transforms an object-space position to clip space using the current
// per-instance transform.
//
// NOTE:
// Axmol stores instance transforms as four column vectors. HLSL assembles
// vertex input matrices from semantic rows, so the matrix is transposed
// to recover the logical object-to-world transform.
#define AX_INSTANCE_TRANSFORM(input, position, mvp) \
    mul(mul((mvp), transpose((input).__ax_instance)), (position))
