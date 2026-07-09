static const int SKINNING_JOINT_COUNT = 60;

struct VS_IN
{
    float3 a_position : POSITION;
    float4 a_blendWeight : BLENDWEIGHT;
    float4 a_blendIndex : BLENDINDICES;
    float2 a_texCoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 v_texCoord : TEXCOORD0;
};

cbuffer vs_ub : register(b0, space0)
{
    float4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    float4x4 u_MVPMatrix;
};

float4 getPosition(float4 blendWeight, float4 blendIndex, float3 pos)
{
    float bw = blendWeight[0];
    int matrixIndex = int(blendIndex[0]) * 3;
    float4 mp1 = u_matrixPalette[matrixIndex] * bw;
    float4 mp2 = u_matrixPalette[matrixIndex + 1] * bw;
    float4 mp3 = u_matrixPalette[matrixIndex + 2] * bw;

    bw = blendWeight[1];
    if (bw > 0.0)
    {
        matrixIndex = int(blendIndex[1]) * 3;
        mp1 += u_matrixPalette[matrixIndex] * bw;
        mp2 += u_matrixPalette[matrixIndex + 1] * bw;
        mp3 += u_matrixPalette[matrixIndex + 2] * bw;

        bw = blendWeight[2];
        if (bw > 0.0)
        {
            matrixIndex = int(blendIndex[2]) * 3;
            mp1 += u_matrixPalette[matrixIndex] * bw;
            mp2 += u_matrixPalette[matrixIndex + 1] * bw;
            mp3 += u_matrixPalette[matrixIndex + 2] * bw;

            bw = blendWeight[3];
            if (bw > 0.0)
            {
                matrixIndex = int(blendIndex[3]) * 3;
                mp1 += u_matrixPalette[matrixIndex] * bw;
                mp2 += u_matrixPalette[matrixIndex + 1] * bw;
                mp3 += u_matrixPalette[matrixIndex + 2] * bw;
            }
        }
    }

    float4 postion = float4(pos, 1.0);
    float4 result;
    result.x = dot(postion, mp1);
    result.y = dot(postion, mp2);
    result.z = dot(postion, mp3);
    result.w = postion.w;
    return result;
}

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    float4 position = getPosition(input.a_blendWeight, input.a_blendIndex, input.a_position);
    output.position = mul(u_MVPMatrix, position);
    output.v_texCoord = input.a_texCoord;
    output.v_texCoord.y = 1.0 - output.v_texCoord.y;
    return output;
}
