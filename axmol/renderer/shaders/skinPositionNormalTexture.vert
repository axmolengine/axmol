#version 310 es

#include "base.glsl"

layout(location = POSITION) in vec3 a_position;

layout(location = BLENDWEIGHT) in vec4 a_blendWeight;
layout(location = BLENDINDICES) in vec4 a_blendIndex;

layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = NORMAL) in vec3 a_normal;
#ifdef USE_NORMAL_MAPPING
layout(location = TANGENT) in vec3 a_tangent;
layout(location = BINORMAL) in vec3 a_binormal;
#endif

#define SKINNING_JOINT_COUNT 60
// Uniforms


// Varyings
layout(location = TEXCOORD0) out vec2 v_texCoord;

#ifdef USE_NORMAL_MAPPING
layout(location = DIRLIGHT) out vec3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
layout(location = POINTLIGHT) out vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];

layout(location = SPOTLIGHT) out vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#ifdef USE_NORMAL_MAPPING
layout(location = SPOTLIGHT_NORM) out vec3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif

#ifndef USE_NORMAL_MAPPING
layout(location = NORMAL) out vec3 v_normal;
#endif

layout(std140) uniform vs_ub {
#ifdef USE_NORMAL_MAPPING
    vvec3_def(u_DirLightSourceDirection, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceDirection, MAX_SPOT_LIGHT_NUM);
#endif
    vvec3_def(u_PointLightSourcePosition, MAX_POINT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourcePosition, MAX_SPOT_LIGHT_NUM);
    vec4 u_matrixPalette[SKINNING_JOINT_COUNT * 3];
    mat4 u_MVMatrix;
    mat3 u_NormalMatrix;
    mat4 u_PMatrix;
};

void getPositionAndNormal(out vec4 position, out vec3 normal, out vec3 tangent, out vec3 binormal)
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

    vec4 p = vec4(a_position, 1.0);
    position.x = dot(p, matrixPalette1);
    position.y = dot(p, matrixPalette2);
    position.z = dot(p, matrixPalette3);
    position.w = p.w;

    vec4 n = vec4(a_normal, 0.0);
    normal.x = dot(n, matrixPalette1);
    normal.y = dot(n, matrixPalette2);
    normal.z = dot(n, matrixPalette3);
#ifdef USE_NORMAL_MAPPING
    vec4 t = vec4(a_tangent, 0.0);
    tangent.x = dot(t, matrixPalette1);
    tangent.y = dot(t, matrixPalette2);
    tangent.z = dot(t, matrixPalette3);
    vec4 b = vec4(a_binormal, 0.0);
    binormal.x = dot(b, matrixPalette1);
    binormal.y = dot(b, matrixPalette2);
    binormal.z = dot(b, matrixPalette3);
#endif
}

void main()
{
    vec4 position;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    getPositionAndNormal(position, normal, tangent, binormal);
    vec4 ePosition = u_MVMatrix * position;

#ifdef USE_NORMAL_MAPPING
    vec3 eTangent = normalize(u_NormalMatrix * tangent);
    vec3 eBinormal = normalize(u_NormalMatrix * binormal);
    vec3 eNormal = normalize(u_NormalMatrix * normal);

    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        vec3 pointD = vvec3_at(u_DirLightSourceDirection, i);
        v_dirLightDirection[i].x = dot(eTangent, pointD);
        v_dirLightDirection[i].y = dot(eBinormal, pointD);
        v_dirLightDirection[i].z = dot(eNormal, pointD);
    }

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 pointLightDir = vvec3_at(u_PointLightSourcePosition, i) - ePosition.xyz;
        v_vertexToPointLightDirection[i].x = dot(eTangent, pointLightDir);
        v_vertexToPointLightDirection[i].y = dot(eBinormal, pointLightDir);
        v_vertexToPointLightDirection[i].z = dot(eNormal, pointLightDir);
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        vec3 spotLightDir = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
        v_vertexToSpotLightDirection[i].x = dot(eTangent, spotLightDir);
        v_vertexToSpotLightDirection[i].y = dot(eBinormal, spotLightDir);
        v_vertexToSpotLightDirection[i].z = dot(eNormal, spotLightDir);

        vec3 pointP = vvec3_at(u_SpotLightSourceDirection, i);
        v_spotLightDirection[i].x = dot(eTangent, pointP);
        v_spotLightDirection[i].y = dot(eBinormal, pointP);
        v_spotLightDirection[i].z = dot(eNormal, pointP);
    }
#else
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        v_vertexToPointLightDirection[i] = vvec3_at(u_PointLightSourcePosition, i) - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        v_vertexToSpotLightDirection[i] = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
    }

    v_normal = u_NormalMatrix * normal;
#endif

    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
    gl_Position = u_PMatrix * ePosition;
}

