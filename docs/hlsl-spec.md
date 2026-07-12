# Axmol HLSL 5.1 Shader Specification

> **axmol PR #3223** — HLSL shader authoring conventions for the axmol engine

## 1. File Naming

| Stage | Suffix | Extension | Description |
|-------|--------|-----------|-------------|
| Vertex Shader | `_vs` | `.hlsl` | Vertex shader |
| Fragment/Pixel Shader | `_ps` | `.hlsl` | Fragment shader |
| Compute Shader | `_cs` | `.hlsl` | Compute shader |

Examples: `positionTexture_vs.hlsl`, `positionTextureColor_ps.hlsl`, `computeParticles_cs.hlsl`

Input language is detected from the file extension:
- `.hlsl`, `.fx` → HLSL (default for axmol shaders)
- `.vert`, `.frag`, `.comp`, `.glsl` → GLSL

Use `-x hlsl` to override auto-detection and force HLSL interpretation.

## 2. Entry Point

```hlsl
// Vertex shader
VS_OUT main(VS_IN input) { ... }

// Fragment shader
float4 main(PS_IN input) : SV_Target0 { ... }
```

Compute shaders use `[numthreads(X, Y, Z)]` decorator.

## 3. Vertex Input Structure

```hlsl
#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;       // required
    float2 a_texCoord : TEXCOORD0;      // optional
    float4 a_color : COLOR0;            // optional
    float3 a_normal : NORMAL;           // optional
    float3 a_tangent : TANGENT;         // optional
    float3 a_binormal : BINORMAL;       // optional
    float4 a_blendIndices : BLENDINDICES; // optional
    float4 a_blendWeights : BLENDWEIGHT;  // optional
};
```

### Semantics

| Semantic | Location | Type | Index Convention |
|----------|----------|------|-----------------|
| `POSITION` | 0 | `float3`/`float4` | No index suffix |
| `NORMAL` | varies | `float3` | No index suffix |
| `TEXCOORD0`–`TEXCOORD7` | varies | `float2`/`float3`/`float4` | Index suffix required |
| `COLOR0`–`COLOR3` | varies | `float4`/`ubyte4` | Index suffix required |
| `TANGENT` | varies | `float3` | No index suffix |
| `BINORMAL` | varies | `float3` | No index suffix |
| `BLENDINDICES` | varies | `float4`/`int4` | No index suffix |
| `BLENDWEIGHT` | varies | `float4` | No index suffix |

### VS_IN Declaration Order

The declaration order of members in `VS_IN` **must match** the order defined in the
corresponding `VertexLayoutKind` layout (see `VertexLayoutManager.cpp`). The axslcc
pipeline assigns D3D input registers based on VS_IN declaration order. While D3D11
matches IA elements to VS inputs by semantic name+index, certain D3D11 debug layers
and GPU drivers validate that the register indices are consistent between IA output
slots and VS input registers. If the orders differ, the following error may appear:

```
D3D11 ERROR: ID3D11DeviceContext::Draw: Input Assembler - Vertex Shader linkage
error: Signatures between stages are incompatible.
Semantic 'COLOR' is defined for mismatched hardware registers between the output
stage and input stage.
Semantic 'TEXCOORD' is defined for mismatched hardware registers between the output
stage and input stage.
```

**Example — incorrect** (order mismatches `VertexLayoutKind::DrawNode` layout):

```hlsl
// VertexLayoutKind::DrawNode order:  POSITION → TEXCOORD → COLOR
struct VS_IN {
    float4 a_position : POSITION;
    float4 a_color : COLOR0;        // COLOR before TEXCOORD — wrong!
    float2 a_texCoord : TEXCOORD0;
};
```

**Fix**: match the layout order:

```hlsl
struct VS_IN {
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;  // TEXCOORD before COLOR
    float4 a_color : COLOR0;
};
```

## 4. Vertex Output / Fragment Input (Varyings)

```hlsl
struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 v_color : COLOR0;
    float3 v_normal : NORMAL;
    float4 position : SV_Position;      // required
};
```

### PointSize (Dynamic Point Size)

For vertex shaders that need dynamic point size, declare a `PSIZE` output with the
`[[vk::builtin("PointSize")]]` annotation:

```hlsl
struct VS_OUT {
    float4 v_color : COLOR0;
    float4 position : SV_Position;
    [[vk::builtin("PointSize")]] float pointSize : PSIZE;
};
```

The `[[vk::builtin("PointSize")]]` annotation is **required** — glslang recognizes
this Vulkan-specific attribute and emits the `BuiltIn PointSize` decoration in
SPIR-V. Without it, the SPIR-V output lacks the PointSize built-in decoration.

This annotation is consumed during glslang's HLSL→SPIR-V compilation and does
**not** appear in cross-compiled output (SPIRV-Cross ignores it). All `--dxbc`
binary targets (d3d11/d3d12) now route through SPIRV-Cross first, so DXC/FXC
only see the clean HLSL output without `[[vk::builtin(...)]]`.

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;      // must match VS_OUT
    float4 v_color : COLOR0;            // must match VS_OUT
    float3 v_normal : NORMAL;           // must match VS_OUT
};
```

### Rules
- VS_OUT and PS_IN varying names and semantics **must match**. The compiler validates this.
- `SV_Position` is the only varying that can change type (VS outputs `float4`, PS receives it as pixel coordinate).
- Only declare PS_IN members that are actually used by the pixel shader.
- **The declaration order of non-builtin members in VS_OUT and PS_IN must be identical.**
  glslang assigns SPIR-V `Location` decorations in struct declaration order (not by semantic name).
  SPIRV-Cross then generates D3D semantics (`TEXCOORDn`) from `Location` values during cross-compilation.
  If order differs, D3D12 will see mismatched register component masks and reject the PSO:
  ```
  D3D12 ERROR: ID3D12Device::CreateGraphicsPipelineState: Vertex Shader - Pixel Shader
  linkage error: Signatures between stages are incompatible.
  Semantic 'TEXCOORD' ... has a hardware register component mask that
  is not a subset of the output of the previous stage.
  ```
  This is independent of the semantic names — the names can be `COLOR0`/`TEXCOORD0` in source,
  but SPIRV-Cross will reassign them to sequential `TEXCOORDn` in cross-compiled output
  based on the SPIR-V location assignment. Only the *declaration order* matters.

## 5. Constant Buffers (Uniform Blocks)

```hlsl
// VS UBO: set=0, binding=0 → register(b0, space0)
cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
    float4x4 u_MVMatrix;
    float3x3 u_NormalMatrix;
    // ... other VS uniforms
};

// PS UBO: set=0, binding=1 → register(b1, space0)
cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
    float2 resolution;
    // ... other PS uniforms
};
```

### Layout Rules
- Use `space0` for all uniform buffers
- `b0` = VS UBO, `b1` = PS UBO
- For legacy packed UBOs, use `vfloat_def`/`vvec3_def` macros from `base.hlsli`

## 6. Textures and Samplers

```hlsl
// Textures: set=1 → space1
Texture2D u_tex0 : register(t0, space1);
Texture2D u_tex1 : register(t1, space1);
TextureCube u_cubeTex : register(t0, space1);

// Builtin samplers are declared in base.hlsli (s0–s21, space1)
// — include "base.hlsli" and use them directly:
float4 c = u_tex0.Sample(LinearClamp, uv);
float4 c = u_tex0.Sample(PointClamp, uv);
float3 c = u_cubeTex.Sample(LinearClamp, reflectDir);
```

### Builtin Samplers (from `base.hlsli`)

| Name | Register | Type |
|------|----------|------|
| `LinearClamp` | s0 | `SamplerState` |
| `LinearWrap` | s1 | `SamplerState` |
| `LinearMirror` | s2 | `SamplerState` |
| `LinearBorder` | s3 | `SamplerState` |
| `PointClamp` | s4 | `SamplerState` |
| `PointWrap` | s5 | `SamplerState` |
| `PointMirror` | s6 | `SamplerState` |
| `PointBorder` | s7 | `SamplerState` |
| `LinearMipClamp` | s8 | `SamplerState` |
| `LinearMipWrap` | s9 | `SamplerState` |
| `LinearMipMirror` | s10 | `SamplerState` |
| `LinearMipBorder` | s11 | `SamplerState` |
| `AnisoClamp` | s12 | `SamplerState` |
| `AnisoWrap` | s13 | `SamplerState` |
| `AnisoMirror` | s14 | `SamplerState` |
| `AnisoBorder` | s15 | `SamplerState` |
| `ShadowCmpClamp` | s16 | `SamplerComparisonState` |
| `ShadowCmpWrap` | s17 | `SamplerComparisonState` |
| `ShadowCmpMirror` | s18 | `SamplerComparisonState` |
| `ShadowCmpBorder` | s19 | `SamplerComparisonState` |
| `LinearNoMipClamp` | s20 | `SamplerState` |
| `PointNoMipClamp` | s21 | `SamplerState` |

## 7. Shader Entry Point Pattern

```hlsl
// ===== positionTexture_vs.hlsl =====
#include "base.hlsli"

struct VS_IN {
    float3 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT {
    float2 v_texCoord : TEXCOORD0;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0) {
    float4x4 u_MVPMatrix;
};

VS_OUT main(VS_IN input) {
    VS_OUT output;
    output.position = mul(u_MVPMatrix, float4(input.a_position, 1.0));
    output.v_texCoord = input.a_texCoord;
    return output;
}

// ===== positionTexture_ps.hlsl =====
#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    return u_tex0.Sample(LinearClamp, input.v_texCoord);
}
```

## 8. Fullscreen / Shadertoy Fragment Shaders

```hlsl
#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;  // pixel position
};

cbuffer fs_ub : register(b1, space0) {
    float2 center;
    float2 resolution;
    float4 u_Time;
};

float4 main(PS_IN input) : SV_Target0 {
    float2 fragCoord = input.gl_FragCoord.xy;
    // ... shadertoy body ...
}
```

## 9. Constants and Preprocessor

### Const Members in cbuffer

```hlsl
// cbuffer members that are compile-time constant
static const float intensity = 0.05;  // use 'static' for global constants

// Local constants (no 'static' needed)
float4 main(PS_IN input) : SV_Target0 {
    const float blurSize = 1.0 / 512.0;
    // ...
}
```

### Macros

```hlsl
#define FILTER_SIZE 3
#define COLOR_LEVELS 7.0
```

## 10. Y-Flip (no longer needed)

In the old GLSL path, `v_texCoord.y = 1.0 - v_texCoord.y` was used for OpenGL/D3D coordinate system differences. With HLSL 5.1 and SPIRV-Cross handling backend differences:

- **D3D11/D3D12**: No Y-flip. Texture origin is top-left.
- **GLES**: No Y-flip. SPIRV-Cross handles the flip in ESSL output.
- **Vulkan**: No Y-flip.
- **Metal**: Handled by `AXSLC_TARGET_MSL` macro (axslcc internal).

```hlsl
// In HLSL/D3D, texture origin is top-left (no Y flip needed).
// spirv-cross handles GL-specific Y flip when producing GLSL output.
#define TEXCOORD_Y(v) ((v).y)
```

## 11. Packed UBO Layout (Legacy)

For existing UBO layouts that need compact float packing (used by older engine code):

```hlsl
// From base.hlsli
#define vfloat_def(x, y) float4 x[(y + 3) / 4]
#define vfloat_at(x, y) x[y / 4][y % 4]
#define vvec3_def(x, y) float4 x[(y * 3 + 3) / 4]

// Usage
cbuffer fs_ub : register(b1, space0) {
    vvec3_def(u_DirLightSourceColor, MAX_DIRECTIONAL_LIGHT_NUM);
    vfloat_def(u_PointLightSourceRangeInverse, MAX_POINT_LIGHT_NUM);
};
```

## 12. Include Path

```hlsl
// include "base.hlsli" for builtin samplers and common macros
// include other .hlsli files as needed
#include "base.hlsli"
#include "colorUtils.hlsli"
```

## 13. Register Space Mapping

| Descriptor Type | Space | Registers |
|----------------|-------|-----------|
| Uniform Buffer (UBO) | `space0` | `b0` (VS), `b1` (PS) |
| Texture SRV | `space1` | `t0`, `t1`, `t2`, ... |
| Sampler | `space1` | `s0`–`s21` (builtin) |

## 14. Compilation Pipeline

axslcc performs per-target compilation to inject `AXSLC_TARGET_*` preprocessor defines
at source level:

```
for each target (HLSL, GLSL, ESSL, MSL, SPIRV):
  .hlsl source + target defines (-DAXSLC_TARGET_<LANG>=1)
    → glslang → SPIR-V
      → SPIRV-Cross + attribute remap → target output
    → packed into .sc file (axslcc -a)
```

### 14.1 Shader Compile Variants

A single `.hlsl` source can produce multiple compiled `.sc` outputs with different
preprocessor defines using the `AXSLCC_VARIANT_DEFINES` source property:

```cmake
# Single extra variant
set_source_files_properties(
    positionNormalTexture_vs.hlsl
    PROPERTIES AXSLCC_VARIANT_DEFINES "USE_NORMAL_MAPPING=1"
)
# → outputs: positionNormalTexture_vs (base), positionNormalTexture_vs_1 (-DUSE_NORMAL_MAPPING=1)

# Multiple variants
PROPERTIES AXSLCC_VARIANT_DEFINES "A=1,B=2;C=3"
# → outputs: file, file_1 (-DA=1 -DB=2), file_2 (-DC=3)
```

Rules:
- **`;`** (semicolon) separates variants
- **`,`** (comma) separates defines within one variant
- Variant defines are **only** applied to `_1`, `_2`, ... outputs — never the base output
- `AXSLCC_DEFINES` (comma-separated) applies to **all** outputs including the base

## 15. Target Identification Macros

axslcc automatically defines `AXSLC_TARGET_*` macros as preprocessor defines during
source compilation (per-target via glslang `-D` flags). These are available in source
and all `#include`d headers:

| Target | Defines |
|--------|---------|
| ESSL (GLES) | `AXSLC_TARGET_ESSL=1` + `AXSLC_TARGET_GLSL=1` |
| HLSL (D3D)  | `AXSLC_TARGET_HLSL=1` |
| MSL (Metal)  | `AXSLC_TARGET_MSL=1` |
| GLSL         | `AXSLC_TARGET_GLSL=1` |
| SPIR-V (Vulkan) | `AXSLC_TARGET_SPIRV=1` |

### Helper: `AX_Y_UP(v)` + `AXSLC_UV_TOP` (`base.hlsli`)

```hlsl
// AXSLC_UV_TOP: 0 = bottom-left origin (GLSL/ESSL), 1 = top-left origin (HLSL/SPIRV/MSL)
#if defined(AXSLC_TARGET_GLSL)
    #define AXSLC_UV_TOP 0
#else
    #define AXSLC_UV_TOP 1
#endif

// AX_Y_UP(v): Convert from platform origin to bottom-left (Y-up) coordinate
#define AX_Y_UP(v) (AXSLC_UV_TOP ? (1.0 - (v).y) : ((v).y))
```

Use for vertex shader texcoords that need Y-up convention:

```hlsl
output.v_texCoord = float2(input.a_texCoord.x, AX_Y_UP(input.a_texCoord));
```

### Fullscreen Fragment Shaders

For fullscreen PS shaders using `SV_Position` with Shadertoy-style coordinate math,
use `AXSLC_UV_TOP` to conditionally flip `gl_FragCoord.y`:

```hlsl
float4 main(PS_IN input) : SV_Target0 {
#if AXSLC_UV_TOP
    float2 fragCoord = float2(input.gl_FragCoord.x, u_screenSize.y - input.gl_FragCoord.y);
#else
    float2 fragCoord = input.gl_FragCoord.xy;
#endif
    // ... rest using fragCoord
}
```

## 16. Testing Checklist

- [ ] Vertex inputs declared with correct D3D semantics
- [ ] PS_IN varyings match VS_OUT varyings
- [ ] cbuffer bindings: `b0` for VS, `b1` for PS
- [ ] Texture bindings: `register(tN, space1)`
- [ ] No Y-flip in HLSL source (SPIRV-Cross handles it)
- [ ] `#include "base.hlsli"` when using any sampler
- [ ] No `vfloat_def`/`vvec3_def` in new shaders (use native `float4` arrays)
- [ ] `float4x4` multiply uses `mul(matrix, vector)` order
