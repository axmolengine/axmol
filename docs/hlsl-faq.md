# Axmol HLSL FAQ

## Should I use HLSL or GLSL for new Axmol shaders?

Use HLSL. Axmol v3 treats HLSL as the primary shader authoring language and
uses `axslcc` to generate backend shader code and reflection.

## Do HLSL variable names matter?

For vertex inputs, no. Axmol matches vertex data by semantic base name and
semantic index, such as `POSITION` / `0` or `TEXCOORD` / `1`.

```hlsl
float3 a_position : POSITION;
float3 pos        : POSITION;
```

For uniforms and textures, names still matter because engine code often looks
them up by name, such as `u_MVPMatrix` or `u_tex0`.

## Is `POSITION` the same as `POSITION0`?

Yes. In HLSL, `POSITION` means semantic index 0. Axmol stores it as
`(POSITION, 0)`.

## Can I invent custom semantics?

Yes, for custom rendering code. Use stable semantic names and explicit indices:

```hlsl
float2 localPosition : LOCAL_POSITION0;
float4 instanceColor : INSTANCE_COLOR0;
```

The C++ side must query the same semantic base and index. Built-in mesh data
only maps the standard Axmol mesh semantics.

## Should I write `register(tN, spaceN)` in shader code?

No. Axmol v3 now lets `axslcc` own the resource layout. Declare resources
without manual registers:

```hlsl
cbuffer vs_ub
{
    float4x4 u_MVPMatrix;
};

Texture2D u_tex0;
SamplerState u_tex0Sampler;
```

`axslcc` keeps the engine's deterministic backend assignments internally:

- vertex uniform buffers start at `b0, space0`;
- fragment uniform buffers start at `b1, space0`;
- textures are assigned to `tN, space1`;
- sampler preset and TextureOwned sampler slots are assigned by `axslcc`.

Those bindings are reflection/backend details, not user-authored HLSL syntax.

## Which sampler style should I use?

Use the built-in sampler presets from `base.hlsli` for most shaders:

```hlsl
Texture2D u_tex0;

float4 c = u_tex0.Sample(LinearClamp, uv);
```

This is the most predictable cross-backend path.

## What does `presetIndex < 0` mean internally?

It means the shader did not request an Axmol shader preset sampler. It does not
mean "invalid" and it does not mean "runtime custom sampler".

The runtime treats that path as a TextureOwned sampler. Its sampler state comes
from exactly one associated texture recorded in reflection.

## Does Axmol support arbitrary runtime custom samplers?

No. Current Axmol RHI supports TextureOwned sampler state and built-in shader
presets. There is no public runtime API for binding arbitrary `SamplerState`
objects by shader name.

If that is needed later, it should be added as an explicit third sampler source,
not by overloading `presetIndex`.

## Can one texture be sampled with two different samplers?

Avoid it for cross-backend Axmol shaders:

```hlsl
float4 a = u_tex0.Sample(LinearClamp, uv);
float4 b = u_tex0.Sample(PointClamp, uv);
```

D3D, Metal, and Vulkan separate samplers can represent this natively, but
OpenGL and OpenGL ES use combined sampler uniforms. Axmol does not expose
runtime sampling pairs, so GL/GLES lowering intentionally rejects this pattern.

Use separate texture bindings or a single sampler state for that texture.

## Can two textures share one TextureOwned sampler?

No. This is invalid:

```hlsl
a.Sample(sharedSampler, uv);
b.Sample(sharedSampler, uv);
```

TextureOwned means "this sampler state comes from exactly one owner texture".
If multiple textures should share the same sampler state, use a ShaderPreset
such as `LinearClamp`, `LinearWrap`, or `PointClamp`.

## Why did old shaders fail after an axslcc update?

The `.sc` reflection layout can change while Axmol v3 is still in alpha. When
that happens, rebuild all shaders with the matching `axslcc`. Do not mix old
compiled shader packages with a newer RHI reflection reader.

## Does GL/GLES support separate samplers?

The OpenGL API has sampler objects, but GLSL 330 and ESSL 300 shader interfaces
use combined sampler uniforms such as `sampler2D`. `axslcc` lowers HLSL
texture/sampler usage into final combined GLSL/ESSL resources at compile time.

The runtime does not need sampling-pair reflection.

## Should I add manual Y flips?

Only when the coordinate data requires it.

For mesh UVs, decide from the asset/shader contract:

```hlsl
output.uv = input.uv;

// Use this only when the incoming asset UV convention requires it.
output.uv.y = 1.0 - output.uv.y;
```

For backend-dependent screen or render-target coordinates, prefer the helper:

```hlsl
float yUp = AX_Y_UP(screenCoord);
```

The short rule is:

- `1.0 - uv.y` means "this input data needs to be inverted".
- `AX_Y_UP(coord)` means "convert this target-dependent coordinate to Y-up".

Do not add or remove flips purely because the shader is HLSL. Verify what the
incoming coordinate represents.

## Is `[[vk::builtin("PointSize")]]` supported?

Yes. Use it for point-size output when targeting Vulkan:

```hlsl
[[vk::builtin("PointSize")]] float pointSize : PSIZE;
```

## Why is `SV_Position` special?

`SV_Position` is a system-value semantic. Vertex shaders write clip-space
position with it. Pixel shaders can read it as screen-space position. It is not
a normal user varying like `TEXCOORD0`.

## What should runtime reflection contain?

Only final backend binding data:

- vertex input semantics and locations;
- textures;
- samplers;
- uniform buffers;
- storage resources where applicable.

It should not contain sampling pairs. Those are compiler-internal data used
while lowering HLSL to backend resource models.
