# Axmol HLSL FAQ

## Should I write shaders in HLSL or GLSL?

HLSL. Axmol v3 uses HLSL as the primary shading language. `axslcc` compiles
your HLSL into the backend format needed by D3D11, D3D12, Vulkan, Metal, and
OpenGL/ES.

## Do variable names in my shader matter?

For **vertex inputs**, no. Axmol matches vertices by semantic name and index
(`POSITION`, `TEXCOORD0`), not by your variable name:

```hlsl
// Both work identically
float3 a_position : POSITION;
float3 pos        : POSITION;
```

For **uniforms and textures**, names do matter because C++ code looks them up
by name (e.g. `u_MVPMatrix`, `u_tex0`).

## Can I write `register(t0, space1)` in my shader?

No. Let `axslcc` handle resource binding. Just declare resources plainly:

```hlsl
cbuffer vs_ub { float4x4 u_MVPMatrix; };
Texture2D u_tex0;
```

`axslcc` automatically assigns:
- Vertex UBOs → `b0, space0`
- Fragment UBOs → `b1, space0`
- Textures → `t0, t1, ...` in `space1`

## How do I use samplers?

### Built-in presets (recommended)

Include `base.hlsli` and use one of the 22 preset names:

```hlsl
#include "base.hlsli"
Texture2D u_tex0;
float4 color = u_tex0.Sample(LinearClamp, uv);
```

Presets: `LinearClamp`, `LinearWrap`, `LinearMirror`, `PointClamp`, `PointWrap`,
`AnisoClamp`, `ShadowCmpClamp`, `LinearNoMipClamp`, and more. See `base.hlsli`
for the full list.

### Custom samplers

If none of the presets fit, declare your own `SamplerState`:

```hlsl
Texture2D u_tex0;
SamplerState mySampler;
float4 color = u_tex0.Sample(mySampler, uv);
```

Then in C++, **before** loading the Program, register it:

```cpp
rhi::SamplerDesc desc;
desc.minFilter    = rhi::SamplerFilter::MIN_LINEAR;
desc.magFilter    = rhi::SamplerFilter::MAG_LINEAR;
desc.sAddressMode = rhi::SamplerAddressMode::REPEAT;
desc.tAddressMode = rhi::SamplerAddressMode::REPEAT;
SamplerRegistry::getInstance()->registerSampler("mySampler", desc);
// Now load the Program - the sampler name will be resolved
```

For a sampler whose state changes at runtime, register its name once, then
override that custom sampler on each `ProgramState`:

```cpp
auto location = program->getSamplerLocation("mySampler");
programState->setSampler(location, desc);
```

Built-in sampler presets are immutable.

**Limits**: custom sampler arrays (`SamplerState arr[4]`) are not supported.
Each sampler must be a single non-array declaration. Multiple custom sampler
declarations are supported in one shader; register each sampler name before
creating the `Program`.

## Can I sample one texture two different ways?

Avoid it. D3D, Metal, and Vulkan can handle it, but OpenGL/ES combine texture
and sampler into a single `sampler2D` uniform. Sampling the same texture with
both `LinearClamp` and `PointClamp` in one shader will fail on GL/ES targets.

Use separate texture bindings or pick one sampler per texture.

Multiple custom samplers are still portable when each texture binding uses only
one sampler state:

```hlsl
Texture2D u_clampedTex;
Texture2D u_wrappedTex;
SamplerState clampSampler;
SamplerState wrapSampler;

float4 a = u_clampedTex.Sample(clampSampler, uv);
float4 b = u_wrappedTex.Sample(wrapSampler, uv);
```

## Why does OpenGL/ES work differently?

GLSL/ESSL uses `sampler2D` uniforms that bundle texture and sampler together.
`axslcc` automatically merges your HLSL texture + sampler into a combined
`sampler2D` for GL/ES targets. You don't need to do anything special, but the
one-sampler-per-texture rule above applies.

## Why does axslcc reject some legal HLSL resource types?

Axmol supports the subset that has a defined cross-RHI binding model today.
Some legal HLSL resource forms are intentionally rejected with a clear error
until the engine has portable runtime support for them. Examples include:

- `ConstantBuffer<T>`; use ordinary `cbuffer` declarations.
- `Texture2DMS` and `Texture2DMSArray`.
- `AppendStructuredBuffer<T>` and `ConsumeStructuredBuffer<T>`.
- rasterizer ordered resources such as `RasterizerOrderedTexture2D<T>`.

## Which compute shaders are portable?

Use `[numthreads(...)]`, ordinary `cbuffer` declarations,
`StructuredBuffer<T>` / `RWStructuredBuffer<T>`, sampled textures, and
registered custom samplers. The first RHI compute API does not support storage
images, indirect dispatch, or asynchronous compute queues.

Compile desktop OpenGL compute shaders as `gl-430` and OpenGL ES compute shaders
as `gles-310`. The runtime chooses the highest archive profile supported by the
actual context, so normal GL 330 / ESSL 300 graphics shaders continue to load on
newer contexts. Always check the RHI compute, storage-buffer, texture, and limit
capabilities before enabling a GPU-only feature.

## Do I need to add Y flips?

Not mechanically. Decide based on what your coordinate data means:

- `uv.y = 1.0 - uv.y` — invert asset data that uses the opposite convention.
- `AX_Y_UP(coord)` — convert a backend-dependent coordinate to Y-up.

Don't flip just because the shader is HLSL. Check what the incoming data
represents.

## How do I output point size?

Annotate the `PSIZE` output:

```hlsl
[[vk::builtin("PointSize")]] float pointSize : PSIZE;
```

This works on Vulkan and OpenGL/ES. D3D and Metal render points at a fixed
1-pixel size.

## Why did my shaders break after updating axslcc?

The `.sc` shader package format may change during Axmol v3 alpha. Rebuild all
`.hlsl` sources with the matching `axslcc` version. Don't mix old `.sc` files
with a new engine build.
