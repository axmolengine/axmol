# Shader Migration Guide: GLSL → HLSL 5.1

> **axmol PR #3233** — Migration of all engine and test shaders from GLSL to HLSL 5.1

## Overview

This document describes the complete shader migration from GLSL (OpenGL ES 3.0) to HLSL 5.1 (D3D11/D3D12/Vulkan/GLES) for the axmol engine. The migration covers:

- **57 builtin engine shaders** (`axmol/renderer/shaders/`)
- **44 cpp-tests shaders** (`tests/cpp-tests/Source/shaders/`)
- **2 spine shaders** (`extensions/spine/shaders/`)
- **19 Live2D shaders** (`extensions/Live2D/.../shaders/`)

## Code Structure Changes

### New vs Old File Extensions

| Old | New |
|-----|-----|
| `.vert` | `_vs.hlsl` |
| `.frag` | `_ps.hlsl` |
| `.fsh` | `_ps.hlsl` |
| `.vsh` | `_vs.hlsl` |
| `.vs` | `_vs.hlsl` |
| `.fs` | `_ps.hlsl` |

**Stage detection**: axslcc now detects shader stage **only** from file stem suffix (`_vs`, `_ps`, `_cs`). Legacy extension-based detection removed.

### AXSLCC.cmake

```cmake
# Before: supported GLSL + HLSL extensions
axslcc_option(AXSLCC_SOURCE_FILE_EXTENSIONS ".hlsl;.vert;.vsh;.vs;.frag;.fsh;.fs")

# After: only .hlsl
axslcc_option(AXSLCC_SOURCE_FILE_EXTENSIONS ".hlsl")
```

### Key Files Involved

| File | Role |
|------|------|
| `axmol/renderer/shaders/base.hlsli` | Common macros, builtin sampler declarations |
| `axmol/cmake/Modules/AXSLCC.cmake` | Shader build rules |
| `axslcc/src/` | Offline shader compiler |

## Shader Conversion Patterns

### Vertex Shader: GLSL → HLSL

```hlsl
// BEFORE (GLSL)
#version 310 es
layout(location = POSITION) in vec3 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = TEXCOORD0) out vec2 v_texCoord;
layout(std140, set = 0, binding = 0) uniform vs_ub {
    mat4 u_MVPMatrix;
};

void main() {
    v_texCoord = a_texCoord;
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
}

// AFTER (HLSL)
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
```

### Fragment Shader: GLSL → HLSL

```hlsl
// BEFORE (GLSL)
#version 310 es
precision highp float;
layout(location = TEXCOORD0) in vec2 v_texCoord;
layout(set = 1, binding = 0) uniform sampler2D u_tex0;
layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec4 u_color;
};
layout(location = SV_Target0) out vec4 FragColor;

void main() {
    FragColor = texture(u_tex0, v_texCoord) * u_color;
}

// AFTER (HLSL)
#include "base.hlsli"

struct PS_IN {
    float2 v_texCoord : TEXCOORD0;
};

cbuffer fs_ub : register(b1, space0) {
    float4 u_color;
};

Texture2D u_tex0 : register(t0, space1);

float4 main(PS_IN input) : SV_Target0 {
    return u_tex0.Sample(LinearClamp, input.v_texCoord) * u_color;
}
```

### Shadertoy / Fullscreen PS (no varying inputs)

```hlsl
#include "base.hlsli"

struct PS_IN {
    float4 gl_FragCoord : SV_Position;
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

## Type Translation Table

| GLSL | HLSL |
|------|------|
| `vec2` | `float2` |
| `vec3` | `float3` |
| `vec4` | `float4` |
| `mat4` | `float4x4` |
| `mat3` | `float3x3` |
| `mix(a,b,t)` | `lerp(a,b,t)` |
| `fract(x)` | `frac(x)` |
| `mod(x,y)` | `fmod(x,y)` |
| `texture(sampler, uv)` | `sampler.Sample(LinearClamp, uv)` |
| `texture(samplerCube, dir)` | `samplerCube.Sample(LinearClamp, dir)` |
| `gl_Position` | `output.position` (SV_Position) |
| `gl_FragCoord.xy` | `input.gl_FragCoord.xy` (SV_Position) |
| `FragColor = val;` | `return val;` |
| `layout(location=SV_Target0) out vec4 FragColor;` | `float4 main(...) : SV_Target0` |

## Semantic Mapping

| GLSL Location | HLSL Semantic |
|--------------|---------------|
| `POSITION` (0) | `POSITION` |
| `TEXCOORD0` | `TEXCOORD0` |
| `TEXCOORD1` | `TEXCOORD1` |
| `NORMAL` | `NORMAL` |
| `COLOR0` | `COLOR0` |
| `BINORMAL` | `BINORMAL` |
| `BLENDINDICES` | `BLENDINDICES` |
| `BLENDWEIGHT` | `BLENDWEIGHT` |

## Uniform Buffer Layout

```hlsl
// Vertex shader UBO — set 0 → space 0, binding 0
cbuffer vs_ub : register(b0, space0) { ... }

// Fragment shader UBO — set 0 → space 0, binding 1
cbuffer fs_ub : register(b1, space0) { ... }
```

## Texture and Sampler Bindings

```hlsl
// Texture at t0 — set 1 → space 1
Texture2D u_tex0 : register(t0, space1);
Texture2D u_tex1 : register(t1, space1);
TextureCube u_cubeTex : register(t0, space1);

// Builtin samplers are declared in base.hlsli
// Sampling: use LinearClamp, LinearWrap, PointClamp, etc.
u_tex0.Sample(LinearClamp, uv)
```

## Legacy Cleanup

### base.hlsli — Removed Macros

```hlsl
// REMOVED — these were GLES2 legacy
#undef TANGENT
#undef BINORMAL
#undef BLENDINDICES
#undef BLENDWEIGHT
#define TANGENT TEXCOORD6
#define BINORMAL TEXCOORD7
#define BLENDINDICES COLOR1
#define BLENDWEIGHT COLOR2
```

### base.hlsli — Semantic Fix

```hlsl
// BEFORE: SPOTLIGHT overlapped with POINTLIGHT array (TEXCOORD2–5)
#ifndef SPOTLIGHT
#define SPOTLIGHT TEXCOORD4
#endif

// AFTER: moved to TEXCOORD6 (after POINTLIGHT's 4-element range)
#ifndef SPOTLIGHT
#define SPOTLIGHT TEXCOORD6
#endif
```

### Sampler12.h — Deleted

The entire `axmol/rhi/d3d12/Sampler12.h` file was removed. Its `BuiltinSamplers` string contained sampler declarations that are now handled by `base.hlsli`.

### colorUtils.hlsli — Matrix Multiply Fix

```hlsl
// BEFORE: DXC rejected implicit float3x3 constructor
return float3x3(colorTransform[0].xyz, colorTransform[1].xyz, colorTransform[2].xyz) * YUV;

// AFTER: explicit matrix construction
float3x3 m = { colorTransform[0].xyz, colorTransform[1].xyz, colorTransform[2].xyz };
return mul(m, YUV);
```

## Tool Changes

### axslcc — New Options

| Option | Description |
|--------|-------------|
| `--dxil` | Windows only: compile SPIRV-Cross HLSL output to DXIL bytecode via DXC |
| `--dxc-reflect` | Windows only: use DXC reflection for REFL chunk (stub, validation only) |

### axslcc — Compilation Pipeline

```
HLSL source
  → glslang → SPIR-V
    → SPIRV-Cross + attribute remap → HLSL (for D3D targets)
    → SPIRV-Cross → ESSL/GLSL/MSL (for other targets)
    → SPIR-V (for Vulkan, direct copy)
  → (+ --dxil) DXC → DXIL bytecode (for D3D targets, optional)
  → (+ --reflect) REFL chunk from SPIR-V reflection
```

### axslcc — Key Fixes

| Fix | File |
|-----|------|
| Attribute remap for HLSL output | `cross_compiler.cpp` |
| Varying name normalize for GLES | `cross_compiler.cpp` |
| HLSL source semantic parse for REFL | `utils.cpp` + `reflection.cpp` |
| `separate_images` for HLSL Texture2D | `reflection.cpp` |
| `input.` prefix strip from variable names | `utils.cpp` |
| SEH crash handler + `g_inDxcCompile` flag | `main.cpp` + `compiler.cpp` |
| `/EHa` for cross-DLL C++ exception catch | `CMakeLists.txt` |

## Backend-Specific Notes

### D3D11/D3D12

- Engine RHI receives **HLSL source code** from SC files
- D3D12 driver compiles HLSL → DXIL at runtime using DXC
- Use `--dxil` option for offline DXIL pre-compilation

### GLES (ANGLE / OpenGL ES 3.0)

- SPIRV-Cross generates ESSL 300 from SPIR-V
- Varying names normalized to strip `input.` prefix
- GLES linker matches varyings by name

### Vulkan

- SPIR-V binary directly embedded in SC file
- No cross-compilation needed

### Metal (MSL)

- Not tested yet
- Metal Y-flip handled by `AXSLC_TARGET_MSL` macro

## Migration Checklist

- [x] Builtin engine shaders (57 files)
- [x] cpp-tests shaders (44 GLSL → 50 HLSL)
- [x] spine shaders (2 files)
- [x] Live2D shaders (19 files)
- [x] AXSLCC.cmake: only `.hlsl`
- [x] axslcc: only `_vs`/`_ps`/`_cs` stage detection
- [x] D3D11/D3D12 basic rendering
- [x] Vulkan basic rendering
- [x] GLES (ANGLE) basic rendering
- [ ] Metal testing
- [ ] `--dxc-reflect` DXC reflection extraction
- [ ] `solid_circle`/`solid_capsule` SDF shaders (orphan, no test references)
