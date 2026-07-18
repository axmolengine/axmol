Texture2D u_tex0;
Texture2D u_tex1;
SamplerState myClampSampler;
SamplerState myWrapSampler;

struct PS_INPUT {
    float4 v_position : SV_POSITION;
    float2 v_texCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target {
    if (input.v_texCoord.x < 0.5) {
        float2 uv = float2(input.v_texCoord.x * 2.0, input.v_texCoord.y) * 4.0;
        return u_tex0.Sample(myClampSampler, uv);
    }

    float2 uv = float2((input.v_texCoord.x - 0.5) * 2.0, input.v_texCoord.y) * 4.0;
    return u_tex1.Sample(myWrapSampler, uv);
}
