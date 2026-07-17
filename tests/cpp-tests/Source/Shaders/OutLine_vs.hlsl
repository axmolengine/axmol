struct VS_IN
{
    float4 a_position : POSITION;
    float3 a_normal : NORMAL;
};

cbuffer vs_ub
{
    float OutlineWidth;
    float4x4 u_MVPMatrix;
};

float4 main(VS_IN input) : SV_Position
{
    float4 pos = mul(u_MVPMatrix, input.a_position);
    float4 normalproj = mul(u_MVPMatrix, float4(input.a_normal, 0.0));
    normalproj = normalize(normalproj);
    pos.xy += normalproj.xy * (OutlineWidth * (pos.z * 0.5 + 0.5));
    return pos;
}
