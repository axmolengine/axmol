// Custom sampler test fragment shader.
// Uses a SamplerState NOT from base.hlsli to test Program-local custom sampler bindings.
// Multiply UVs to make WRAP vs CLAMP visually obvious when using a repeating address mode.
//
// Register the sampler by name before creating the Program:
//   SamplerRegistry::getInstance()->registerSampler("myCustomSampler", desc);

Texture2D u_tex0;
SamplerState myCustomSampler;

struct PS_INPUT {
    float4 v_position : SV_POSITION;
    float2 v_texCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target {
    // Sample with tiled UVs: Wrap mode shows repeating tiles, Clamp mode shows smeared edges.
    // Without custom samplers working, the shader fails to compile/load.
    return u_tex0.Sample(myCustomSampler, input.v_texCoord * 4.0);
}
