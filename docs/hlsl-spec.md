# Axmol HLSL Shader Specification

This document describes the HLSL authoring rules used by Axmol v3 shaders. It
is written for engine users and extension authors who want one HLSL source to
work across D3D11, D3D12, Vulkan, Metal, OpenGL 3.3, and OpenGL ES 3.x.

For common migration questions and backend caveats, see
[hlsl-faq.md](hlsl-faq.md).

## Public Contract

Axmol uses HLSL as the primary shader source language. `axslcc` generates the
backend shader code and runtime reflection used by the RHI.

Shader sources should use HLSL 2021 syntax within Axmol's Shader Model
5.1-compatible portable subset. Some legal HLSL 2021 features are intentionally
rejected until Axmol has a defined cross-RHI runtime model for them.

The public shader contract is:

- vertex inputs are matched by semantic name and semantic index;
- shader resources are declared by name and type, without manual
  `: register(...)` annotations;
- `axslcc` assigns deterministic backend bindings for uniform buffers,
  textures, samplers, and storage resources;
- runtime reflection contains final backend resource bindings only;
- texture/sampler sampling pairs are compiler-internal lowering data and are
  not part of the runtime reflection ABI.

Manual resource registers are intentionally rejected:

```hlsl
// Do not write this in Axmol v3 shaders:
Texture2D u_tex0 : register(t0, space1);
```

Write declarations without registers:

```hlsl
Texture2D u_tex0;
// SamplerState presets are declared in base.hlsli; sample with a built-in name.
// Custom SamplerState declarations are allowed when registered from C++.
```

## File Names

Use these suffixes for Axmol shader stages:

| Stage | Suffix | Example |
| --- | --- | --- |
| Vertex | `_vs.hlsl` | `positionTexture_vs.hlsl` |
| Pixel / Fragment | `_fs.hlsl` or `_ps.hlsl` | `positionTexture_fs.hlsl` |
| Compute | `_cs.hlsl` | `particles_cs.hlsl` |

Use `main` as the entry point unless a specific build rule says otherwise.

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

Declare constant buffers without registers:

```hlsl
cbuffer vs_ub
{
    float4x4 u_MVPMatrix;
};

cbuffer fs_ub
{
    float4 u_color;
};
```

`axslcc` preserves Axmol's deterministic uniform-buffer convention:

| Buffer | Internal assignment |
| --- | --- |
| `vs_ub` and the first vertex-stage cbuffer | `b0, space0` |
| `fs_ub` and the first fragment-stage cbuffer | `b1, space0` |

Additional cbuffers in the same stage are assigned following slots in source
order. These assignments are backend/reflection details; user shaders should
not spell them manually.

Existing engine shaders may use packing helpers from `base.hlsli`, such as
`vfloat_def` and `vvec3_def`. New shaders should prefer ordinary HLSL types
unless they need to match an existing packed uniform layout.

## Textures and Storage Resources

Declare textures and storage resources without registers:

```hlsl
Texture2D u_tex0;
Texture2D u_tex1;
TextureCube u_Env;
Texture2D u_details[4];
```

`axslcc` assigns textures to `tN, space1` in source order. Storage resources
are assigned to the corresponding UAV/resource binding range in source order.
The runtime uses reflection to bind the final backend resource locations.

Legal HLSL resource forms are not automatically part of Axmol's portable shader
ABI. The compiler intentionally rejects unsupported resource forms with a clear
diagnostic instead of silently lowering them incorrectly. Currently rejected
examples include:

- `ConstantBuffer<T>` template-style constant buffers; use ordinary `cbuffer`
  declarations instead.
- multisampled textures such as `Texture2DMS` and `Texture2DMSArray`;
- append/consume buffers such as `AppendStructuredBuffer<T>` and
  `ConsumeStructuredBuffer<T>`;
- rasterizer ordered resources such as `RasterizerOrderedTexture2D<T>`.

### Storage Buffers

The portable storage-buffer forms are `StructuredBuffer<T>` for read-only
access and `RWStructuredBuffer<T>` for read-write access:

```hlsl
StructuredBuffer<Particle> particles;
RWStructuredBuffer<Trail> trails;
```

Sampled textures and storage buffers share one logical resource-slot sequence
in declaration order. Reflection provides the descriptor set, logical and
backend binding, element stride, and read-only/read-write access. Storage
buffers may be consumed by vertex or compute stages. Do not write explicit
registers to make bindings line up across stages.

Storage images are reserved in the archive reflection ABI but are not exposed
by the first RHI compute API. A Program containing an active storage image is
unsupported.

## Compute Shaders

Compute entry points use the `_cs.hlsl` suffix and declare a fixed local size:

```hlsl
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Read or update reflected storage resources.
}
```

`axslcc` records the local size in reflection. The first RHI compute pipeline
supports uniform buffers, sampled 2D/3D textures, custom samplers, and storage
buffers. It does not expose storage images, indirect dispatch, or asynchronous
compute queues.

OpenGL compute shaders must be compiled for `gl-430`; OpenGL ES compute shaders
must be compiled for `gles-310`. Ordinary graphics shaders remain `gl-330` or
`gles-300`. Runtime code must check `COMPUTE_SHADER`, `STORAGE_BUFFER`, and any
required texture features and limits before creating a compute workload.

## Sampler Model

Axmol supports two sampler categories:

| Source | Binding | Shader Authoring |
| --- | --- | --- |
| Built-In | logical preset `0..21`, backend binding assigned by `axslcc` | sample with `LinearClamp`, `PointClamp`, etc. |
| Custom | Program-local logical binding, backend binding assigned by `axslcc` | declare own `SamplerState` without `#include "base.hlsli"` |

### Built-In Samplers

The 22 built-in sampler presets from `base.hlsli` use stable logical preset
IDs. Each name maps to a `SamplerPreset` enum value. This logical preset is
independent of shader declaration order — `PointClamp` always maps to the same
`SamplerPreset` value regardless of where it appears.

Physical sampler registers are backend ABI details emitted by `axslcc` and
reported through runtime reflection. Do not assume the logical preset index is
the final hardware register. For example, D3D11 exposes only 16 sampler slots
per shader stage (`s0..s15`), so `axslcc` compacts the samplers actually used by
each D3D11 shader into that range and fails compilation if more than 16 active
samplers are required.

Most Axmol HLSL should use the built-in sampler presets from `base.hlsli`:

```hlsl
#include "base.hlsli"

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    return u_tex0.Sample(LinearClamp, input.uv);
}
```

Built-in presets:

| Name | Type |
| --- | --- |
| `LinearClamp`, `LinearWrap`, `LinearMirror`, `LinearBorder` | `SamplerState` |
| `PointClamp`, `PointWrap`, `PointMirror`, `PointBorder` | `SamplerState` |
| `LinearMipClamp`, `LinearMipWrap`, `LinearMipMirror`, `LinearMipBorder` | `SamplerState` |
| `AnisoClamp`, `AnisoWrap`, `AnisoMirror`, `AnisoBorder` | `SamplerState` |
| `ShadowCmpClamp`, `ShadowCmpWrap`, `ShadowCmpMirror`, `ShadowCmpBorder` | `SamplerComparisonState` |
| `LinearNoMipClamp`, `PointNoMipClamp` | `SamplerState` |

### Custom Samplers (Program-Local)

Shaders may declare their own `SamplerState` variables (without `#include
"base.hlsli"` or in addition to it). These custom samplers are assigned
**Program-local** bindings in `space2` by `axslcc`:

```hlsl
Texture2D u_tex0;
SamplerState mySampler;

float4 main(PS_IN input) : SV_Target0
{
    return u_tex0.Sample(mySampler, input.uv);
}
```

Rules:

- Custom sampler names must be unique within the shader.
- Custom sampler arrays are **not supported** (`SamplerState arr[4]` is rejected).
- Multiple custom samplers are supported in one shader. They may use different
  sampler states as long as each sampler name is registered before `Program`
  creation.
- Each custom sampler must be registered with `SamplerRegistry` **before** the
  `Program` is created:
  ```cpp
  SamplerRegistry::getInstance()->registerSampler("mySampler", desc);
  ```
- A `ProgramState` may override a custom sampler after Program creation:
  ```cpp
  auto location = program->getSamplerLocation("mySampler");
  programState->setSampler(location, desc);
  ```
  Preset samplers are immutable and cannot be overridden this way.
- SamplerRegistry registration order does not affect shader logical bindings.

Custom sampler ABI:

| Property | Value |
| --- | --- |
| Logical space | `2` |
| Logical binding | `0, 1, ...` (declaration order, per Program) |
| Physical binding | backend-specific, reported by reflection |
| Reflection `preset_index` | `-1` |

### GL/GLES Combined Samplers

OpenGL 3.3 and OpenGL ES 3.x use combined sampler uniforms in GLSL/ESSL.
`axslcc` lowers HLSL texture + sampler usage into the final combined GLSL
resource at compile time. Runtime reflection for GL/GLES describes the final
combined uniforms, not the original HLSL separate image/sampler resources.

For cross-backend shaders, do not sample the same texture with multiple sampler
states in one shader:

```hlsl
// Not supported for Axmol GL/GLES lowering:
float4 a = u_tex0.Sample(LinearClamp, uv);
float4 b = u_tex0.Sample(PointClamp, uv);
```

Use separate texture bindings or a single sampler state per texture.

This restriction does not forbid multiple custom samplers. For GL/GLES portability,
bind the texture through separate HLSL texture declarations when different
sampler states are needed:

```hlsl
Texture2D u_clampedTex;
Texture2D u_wrappedTex;
SamplerState clampSampler;
SamplerState wrapSampler;

float4 a = u_clampedTex.Sample(clampSampler, uv);
float4 b = u_wrappedTex.Sample(wrapSampler, uv);
```

## Complete Example

Vertex shader:

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

cbuffer vs_ub
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

Fragment shader:

```hlsl
#include "base.hlsli"

struct PS_IN
{
    float2 uv    : TEXCOORD0;
    float4 color : COLOR0;
};

Texture2D u_tex0;

float4 main(PS_IN input) : SV_Target0
{
    return input.color * u_tex0.Sample(LinearClamp, input.uv);
}
```

## Target Macros

`axslcc` defines target macros while compiling each backend variant.

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

Per-shader target profiles can be selected with `AXSLCC_TARGETS`. This is used
for compute shaders without raising the profile of ordinary shaders:

```cmake
set_source_files_properties(
    particles_cs.hlsl
    PROPERTIES AXSLCC_TARGETS "gl-430;gles-310;d3d11;d3d12;vk;mtl"
)
```

When an archive contains multiple entries for one source language, the runtime
selects the highest profile not newer than the active device profile. Thus a GL
4.3 context can load both GLSL 430 compute archives and GLSL 330 graphics
archives, while a GL 3.3 context cannot load the compute archive.

## Runtime Reflection

Runtime reflection contains the resources the backend needs to bind:

- vertex input semantic, semantic index, and backend location;
- textures;
- samplers;
- uniform buffers;
- storage-buffer logical/backend bindings, stride, access, and stage usage;
- compute local workgroup size.

It does not contain texture/sampler sampling pairs. Texture/sampler pair
analysis is a compiler-internal step used for GL/GLES lowering and Vulkan
combined-mode preparation.

## Checklist

- Include `base.hlsli` when using preset samplers.
- Do not write manual `: register(...)` annotations.
- Use `vs_ub` for vertex uniforms and `fs_ub` for fragment uniforms; `axslcc`
  assigns their backend bindings.
- Declare textures and storage resources without binding syntax.
- Avoid unsupported HLSL resource forms such as `ConstantBuffer<T>`,
  `Texture2DMS`, `AppendStructuredBuffer<T>`, and `ConsumeStructuredBuffer<T>`.
- For basic needs, use built-in sampler names such as `LinearClamp`.
- For custom sampler state, declare your own `SamplerState`; register it with
  `SamplerRegistry` before creating the Program. Custom sampler arrays are not
  supported.
- Do not sample one texture with multiple sampler states if GL/GLES is a target.
- Match varyings by semantic name and index, not variable name.
- Recompile all shaders after changing the reflection layout or axslcc runtime
  ABI.
