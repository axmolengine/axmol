#version 310 es

#include "base.glsl"

layout(location = POSITION) in vec3 a_position;

layout(location = BLENDWEIGHT) in vec4 a_blendWeight;
layout(location = BLENDINDICES) in vec4 a_blendIndex;

layout(location = TEXCOORD0) in vec2 a_texCoord;

#define SKINNING_JOINT_COUNT 60
// Uniforms

// Varyings
layout(location = TEXCOORD0) out vec2 v_texCoord;

layout(std140) uniform vs_ub {
    vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    mat4 u_MVPMatrix;
};

vec4 getPosition()
{
    float blendWeight = a_blendWeight[0];

    int matrixIndex = int (a_blendIndex[0]) * 3;
    vec4 matrixPalette1 = u_matrixPalette[matrixIndex] * blendWeight;
    vec4 matrixPalette2 = u_matrixPalette[matrixIndex + 1] * blendWeight;
    vec4 matrixPalette3 = u_matrixPalette[matrixIndex + 2] * blendWeight;


    blendWeight = a_blendWeight[1];
    if (blendWeight > 0.0)
    {
        matrixIndex = int(a_blendIndex[1]) * 3;
        matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
        matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
        matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;

        blendWeight = a_blendWeight[2];
        if (blendWeight > 0.0)
        {
            matrixIndex = int(a_blendIndex[2]) * 3;
            matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
            matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
            matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;

            blendWeight = a_blendWeight[3];
            if (blendWeight > 0.0)
            {
                matrixIndex = int(a_blendIndex[3]) * 3;
                matrixPalette1 += u_matrixPalette[matrixIndex] * blendWeight;
                matrixPalette2 += u_matrixPalette[matrixIndex + 1] * blendWeight;
                matrixPalette3 += u_matrixPalette[matrixIndex + 2] * blendWeight;
            }
        }
    }

    vec4 _skinnedPosition;
    vec4 position = vec4(a_position, 1.0);
    _skinnedPosition.x = dot(position, matrixPalette1);
    _skinnedPosition.y = dot(position, matrixPalette2);
    _skinnedPosition.z = dot(position, matrixPalette3);
    _skinnedPosition.w = position.w;

    return _skinnedPosition;
}

void main()
{
    vec4 position = getPosition();
    gl_Position = u_MVPMatrix * position;

    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}

