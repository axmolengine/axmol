#version 310 es

#include "base.glsl"

#ifdef USE_NORMAL_MAPPING
#endif
#ifdef USE_NORMAL_MAPPING
#endif

layout(location = POSITION) in vec4 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = NORMAL) in vec3 a_normal;
#ifdef USE_NORMAL_MAPPING
layout(location = TANGENT) in vec3 a_tangent;
layout(location = BINORMAL) in vec3 a_binormal;
#endif
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
    mat4 u_MVPMatrix;
    mat4 u_MVMatrix;
    mat4 u_PMatrix;
    mat3 u_NormalMatrix;
};

void main(void)
{
    vec4 ePosition = u_MVMatrix * a_position;
#ifdef USE_NORMAL_MAPPING
    vec3 eTangent = normalize(u_NormalMatrix * a_tangent);
    vec3 eBinormal = normalize(u_NormalMatrix * a_binormal);
    vec3 eNormal = normalize(u_NormalMatrix * a_normal);
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        v_dirLightDirection[i].x = dot(eTangent, vvec3_at(u_DirLightSourceDirection, i));
        v_dirLightDirection[i].y = dot(eBinormal, vvec3_at(u_DirLightSourceDirection, i));
        v_dirLightDirection[i].z = dot(eNormal, vvec3_at(u_DirLightSourceDirection, i));
    }

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 pointLightDir = vvec3_at(u_PointLightSourcePosition, i).xyz - ePosition.xyz;
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

        v_spotLightDirection[i].x = dot(eTangent, vvec3_at(u_SpotLightSourceDirection, i));
        v_spotLightDirection[i].y = dot(eBinormal, vvec3_at(u_SpotLightSourceDirection, i));
        v_spotLightDirection[i].z = dot(eNormal, vvec3_at(u_SpotLightSourceDirection, i));
    }
#else
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        v_vertexToPointLightDirection[i] = vvec3_at(u_PointLightSourcePosition, i).xyz - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        v_vertexToSpotLightDirection[i] = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
    }

    v_normal = u_NormalMatrix * a_normal;
#endif

    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
    gl_Position = u_PMatrix * ePosition;
}
