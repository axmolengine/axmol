# Axmol HLSL Shader Specification

This document describes the HLSL authoring rules used by Axmol v3 shaders.
It is written for engine users and extension authors who want one HLSL source
to work across D3D11, D3D12, Vulkan, Metal, OpenGL 3.3, and OpenGL ES 3.x.

For common migration questions and backend caveats, see
[hlsl-faq.md](hlsl-faq.md).

## Goals

Axmol uses HLSL as the primary shader source language. The shader compiler
generates the backend shader code and runtime reflection used by the RHI.

The public shader contract is:

- vertex inputs are matched by semantic name and semantic index;
- textures, samplers, and uniform buffers use explicit registers;
- runtime reflection contains final backend resource bindings only;
- sampler pairs are compiler-internal lowering data and are not part of the
  runtime reflection ABI.

## File Names

Use these suffixes for Axmol shader stages:

| Stage | Suffix | Example |
| --- | --- | --- |
| Vertex | `_vs.hlsl` | `positionTexture_vs.hlsl` |
| Pixel / Fragment | `_fs.hlsl` or `_ps.hlsl` | `positionTexture_fs.hlsl` |
| Compute | `_cs.hlsl` | `particles_cs.hlsl` |

`.hlsl` and `.fx` files are treated as HLSL. GLSL source files are still
recognized by their GLSL extensions, but new Axmol shaders should use HLSL.

## Entry Points

Use `main` as the entry point.

```hlsl
VS_OUT main(VS_IN input)
{
    ...
}

float4 main(PS_IN input) : SV_Target0
{
    ...
}
```

Compute shaders use the normal HLSL thread-count attribute:

```hlsl
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    ...
}
```

## Include `base.hlsli`

Most Axmol shaders should include `base.hlsli`:

```hlsl
#include "base.hlsli"
```

It provides:

- built-in sampler declarations such as `LinearClamp` and `PointClamp`;
- target macros such as `AXSLC_TARGET_HLSL`, `AXSLC_TARGET_SPIRV`,
  `AXSLC_TARGET_GLSL`, and `AXSLC_TARGET_MSL`;
- compatibility helpers used by existing engine shaders.

## Vertex Inputs

Vertex input variable names are not the binding contract. Axmol matches vertex
data to shader inputs by semantic base name and semantic index.

```hlsl
struct VS_IN
{
    float3 position : POSITION;
    float2 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};
```

`POSITION` is treated as `POSITION0`. `TEXCOORD1`, `COLOR1`, and similar names
use the numeric suffix as the semantic index.

Common Axmol mesh semantics:

| Semantic | Typical Use |
| --- | --- |
| `POSITION` | position, semantic index 0 |
| `COLOR0`, `COLOR1` | vertex colors, including Spine two-color tint |
| `TEXCOORD0` - `TEXCOORD7` | mesh UV channels |
| `NORMAL` | normal |
| `TANGENT` | tangent |
| `BINORMAL` | binormal |
| `BLENDWEIGHT` | skin weights |
| `BLENDINDICES` | skin joint indices |

Custom semantics are allowed for custom render paths:

```hlsl
struct VS_IN
{
    float2 localPosition : LOCAL_POSITION0;
    float4 instanceColor : INSTANCE_COLOR0;
};
```

The C++ side must query the same semantic base and index. Do not rely on HLSL
member names such as `a_position`; those names may be changed freely.

## Varyings

Use matching semantics between vertex output and pixel input.

```hlsl
struct VS_OUT
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

struct PS_IN
{
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};
```

Rules:

- `SV_Position` is the clip-space position output from the vertex shader.
- Non-built-in varyings should use ordinary HLSL semantics such as `TEXCOORD0`,
  `COLOR0`, and `NORMAL`.
- Match semantic names and indices between stages.
- Variable names do not need to match.

For point-size output on Vulkan, annotate the `PSIZE` output:

```hlsl
struct VS_OUT
{
    float4 position : SV_Position;
    [[vk::builtin("PointSize")]] float pointSize : PSIZE;
};
```

## Uniform Buffers

Use explicit constant buffer registers.

```hlsl
cbuffer vs_ub : register(b0, space0)
{
    float4x4 u_MVPMatrix;
};

cbuffer fs_ub : register(b1, space0)
{
    float4 u_color;
};
```

Conventions:

| Register | Use |
| --- | --- |
| `b0, space0` | vertex-stage uniforms |
| `b1, space0` | pixel-stage uniforms |

Existing engine shaders may use packing helpers from `base.hlsli`, such as
`vfloat_def` and `vvec3_def`. New shaders should prefer ordinary HLSL types
unless they need to match an existing packed uniform layout.

## Textures

Declare textures in `space1` using `t#` registers.

```hlsl
Texture2D u_tex0 : register(t0, space1);
Texture2D u_tex1 : register(t1, space1);
TextureCube u_Env : register(t0, space1);
```

Texture arrays are supported when the C++ side binds an array to the same
texture binding:

```hlsl
Texture2D u_details[4] : register(t0, space1);
```

## Sampler Model

Axmol has two sampler sources.

| Source | Meaning | Shader Authoring |
| --- | --- | --- |
| TextureOwned | sampler state comes from exactly one associated texture/resource | declare one non-preset sampler for that texture |
| Shader preset | sampler state comes from one of Axmol's built-in presets | sample with `LinearClamp`, `PointClamp`, etc. |

There is no runtime API for arbitrary custom sampler objects in the current RHI.
Do not treat a non-preset sampler declaration as a user-bindable custom sampler.

### Shader Preset Samplers

Most Axmol HLSL should use the built-in sampler presets from `base.hlsli`:

```hlsl
#include "base.hlsli"

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    return u_tex0.Sample(LinearClamp, input.uv);
}
```

Built-in presets:

| Name | Register | Type |
| --- | --- | --- |
| `LinearClamp` | `s0, space1` | `SamplerState` |
| `LinearWrap` | `s1, space1` | `SamplerState` |
| `LinearMirror` | `s2, space1` | `SamplerState` |
| `LinearBorder` | `s3, space1` | `SamplerState` |
| `PointClamp` | `s4, space1` | `SamplerState` |
| `PointWrap` | `s5, space1` | `SamplerState` |
| `PointMirror` | `s6, space1` | `SamplerState` |
| `PointBorder` | `s7, space1` | `SamplerState` |
| `LinearMipClamp` | `s8, space1` | `SamplerState` |
| `LinearMipWrap` | `s9, space1` | `SamplerState` |
| `LinearMipMirror` | `s10, space1` | `SamplerState` |
| `LinearMipBorder` | `s11, space1` | `SamplerState` |
| `AnisoClamp` | `s12, space1` | `SamplerState` |
| `AnisoWrap` | `s13, space1` | `SamplerState` |
| `AnisoMirror` | `s14, space1` | `SamplerState` |
| `AnisoBorder` | `s15, space1` | `SamplerState` |
| `ShadowCmpClamp` | `s16, space1` | `SamplerComparisonState` |
| `ShadowCmpWrap` | `s17, space1` | `SamplerComparisonState` |
| `ShadowCmpMirror` | `s18, space1` | `SamplerComparisonState` |
| `ShadowCmpBorder` | `s19, space1` | `SamplerComparisonState` |
| `LinearNoMipClamp` | `s20, space1` | `SamplerState` |
| `PointNoMipClamp` | `s21, space1` | `SamplerState` |

Shader preset samplers are independent of texture registers. For example, this
is valid:

```hlsl
Texture2D albedo : register(t3, space1);

float4 c = albedo.Sample(LinearClamp, uv); // texture t3, sampler s0
```

### TextureOwned Samplers

TextureOwned sampling means the sampler state is taken from one associated
texture. A TextureOwned sampler may internally reuse an existing built-in
sampler state owned by that texture; the shader register still follows Axmol's
sampler ABI.

The texture register and sampler register do not need to have the same number,
but TextureOwned sampler registers must use `s22` or higher because `s0` - `s21`
are reserved binding slots for Axmol ShaderPreset samplers declared in
`base.hlsli`:

```hlsl
Texture2D u_albedo : register(t3, space1);
SamplerState u_albedoSampler : register(s22, space1);

float4 c = u_albedo.Sample(u_albedoSampler, uv);
```

Reflection records this as `u_albedoSampler.binding = 22` and
`u_albedoSampler.textureBinding = 3`.

A TextureOwned sampler must have exactly one owner texture. Do not share one
non-preset sampler across multiple textures:

```hlsl
// Not supported for TextureOwned samplers:
a.Sample(sharedSampler, uv);
b.Sample(sharedSampler, uv);
```

If several textures should use the same sampler state, use a ShaderPreset such
as `LinearClamp` instead.

### GL/GLES Combined Samplers

OpenGL 3.3 and OpenGL ES 3.x use combined sampler uniforms in GLSL/ESSL.
axslcc lowers HLSL texture + sampler usage into the final combined GLSL resource
at compile time. Runtime reflection for GL/GLES describes the final combined
uniforms, not the original HLSL separate image/sampler resources.

For cross-backend shaders, do not sample the same texture with multiple sampler
states in one shader:

```hlsl
// Not supported for Axmol GL/GLES lowering:
float4 a = u_tex0.Sample(LinearClamp, uv);
float4 b = u_tex0.Sample(PointClamp, uv);
```

Use separate texture bindings or a single sampler state per texture.

## Register Spaces

| Resource | Space | Registers |
| --- | --- | --- |
| Uniform buffers | `space0` | `b0`, `b1`, ... |
| Textures | `space1` | `t0`, `t1`, ... |
| ShaderPreset sampler slots | `space1` | `s0` - `s21`, reserved by `base.hlsli` |
| TextureOwned sampler slots | `space1` | `s22` and above |

Axmol currently flattens runtime bindings for its RHI backends. Use the spaces
above consistently.

## Complete Example

```hlsl
#include "base.hlsli"

struct VS_IN
{
    float3 position : POSITION;
    float2 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

cbuffer vs_ub : register(b0, space0)
{
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    output.position = mul(u_MVPMatrix, float4(input.position, 1.0));
    output.uv = input.uv;
    output.color = input.color;
    return output;
}
```

```hlsl
#include "base.hlsli"

struct PS_IN
{
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0
{
    return input.color * u_tex0.Sample(LinearClamp, input.uv);
}
```

## Target Macros

axslcc defines target macros while compiling each backend variant.

| Target | Defines |
| --- | --- |
| D3D/HLSL | `AXSLC_TARGET_HLSL=1` |
| Vulkan/SPIR-V | `AXSLC_TARGET_SPIRV=1` |
| Metal/MSL | `AXSLC_TARGET_MSL=1` |
| OpenGL/GLSL | `AXSLC_TARGET_GLSL=1` |
| OpenGL ES/ESSL | `AXSLC_TARGET_ESSL=1`, `AXSLC_TARGET_GLSL=1` |

Use these only when a backend really needs different source behavior.

## Coordinate Notes

Do not treat UV Y flipping as a backend rule. Whether a shader should flip
`uv.y` depends on the meaning of the coordinate being passed through the shader.

There are three common cases:

| Input meaning | Recommended code | Why |
| --- | --- | --- |
| Mesh/asset UVs are already in the sampling convention expected by the texture | pass the UV through unchanged | most new shaders should start here |
| The asset or old GLSL shader intentionally stored UVs in the opposite convention | explicitly convert the data, for example `uv.y = 1.0 - uv.y` | this preserves the asset/shader contract across all backends |
| The shader is converting a backend-dependent screen or render-target coordinate to a bottom-left/Y-up convention | use `AX_Y_UP(coord)` from `base.hlsli` | this documents that the conversion depends on target coordinate origin |

In other words, `1.0 - uv.y` is a data-space conversion, while `AX_Y_UP()` is a
target-space conversion helper. Do not replace one with the other mechanically.

When migrating old GLSL, compare the original shader behavior instead of
assuming all GLSL-era flips were backend workarounds. Some were part of the
shader's intended asset UV semantics.

## Compile Variants

CMake can build shader variants with extra defines.

```cmake
set_source_files_properties(
    positionNormalTexture_vs.hlsl
    PROPERTIES AXSLCC_VARIANT_DEFINES "USE_NORMAL_MAPPING=1"
)
```

Rules:

- `;` separates variants;
- `,` separates defines in one variant;
- `AXSLCC_DEFINES` applies to all variants;
- `AXSLCC_VARIANT_DEFINES` applies only to generated variant outputs.

## Runtime Reflection

Runtime reflection contains the resources the backend needs to bind:

- vertex input semantic, semantic index, and backend location;
- textures;
- samplers;
- uniform buffers;
- storage resources where applicable.

It does not contain texture/sampler sampling pairs. Texture/sampler pair
analysis is a compiler-internal step used for GL/GLES lowering and Vulkan
combined-mode preparation.

## Checklist

- Include `base.hlsli` when using preset samplers.
- Use `b0, space0` for vertex uniforms and `b1, space0` for pixel uniforms.
- Use `tN, space1` for textures.
- Use built-in sampler presets for explicit shader sampler state.
- Use one TextureOwned sampler per owner texture, or use a ShaderPreset for
  sampler state shared by multiple textures.
- Do not sample one texture with multiple sampler states if GL/GLES is a target.
- Match varyings by semantic name and index, not variable name.
- Recompile all shaders after changing the reflection layout or axslcc runtime
  ABI.
