#define MAX_POINT_LIGHT_NUM 1
#define MAX_SPOT_LIGHT_NUM 1
#define MAX_DIRECTIONAL_LIGHT_NUM 1

struct VS_IN
{
    float4 a_position : POSITION;
    float2 a_texCoord : TEXCOORD0;
    float3 a_normal : NORMAL;
};

struct VS_OUT
{
    float2 v_texCoord : TEXCOORD0;
    float3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM] : TEXCOORD1;
    float3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM] : TEXCOORD2;
    float3 v_normal : NORMAL;
    float4 position : SV_Position;
};

cbuffer vs_ub : register(b0, space0)
{
    float4 u_PointLightSourcePosition[MAX_POINT_LIGHT_NUM];
    float4 u_SpotLightSourcePosition[MAX_SPOT_LIGHT_NUM];
    float4x4 u_MVMatrix;
    float4x4 u_PMatrix;
    float3x3 u_NormalMatrix;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 ePosition = mul(u_MVMatrix, input.a_position);

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        output.v_vertexToPointLightDirection[i] = u_PointLightSourcePosition[i].rgb - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        output.v_vertexToSpotLightDirection[i] = u_SpotLightSourcePosition[i].rgb - ePosition.xyz;
    }

    output.v_normal = mul(u_NormalMatrix, input.a_normal);
    output.v_texCoord = input.a_texCoord;
    // check: does it required, original GLSL has the unconditional flip logic
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    output.position = mul(u_PMatrix, ePosition);
    return output;
}
