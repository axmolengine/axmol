#version 310 es


#ifdef USE_NORMAL_MAPPING
#endif
#ifdef USE_NORMAL_MAPPING
#endif

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
#ifdef USE_NORMAL_MAPPING
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_binormal;
#endif
layout(location = 0) out vec2 TextureCoordOut;

#ifdef USE_NORMAL_MAPPING
layout(location = 1) out vec3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
layout(location = 2) out vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
layout(location = 3) out vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#ifdef USE_NORMAL_MAPPING
layout(location = 4) out vec3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif

#ifndef USE_NORMAL_MAPPING
layout(location = 5) out vec3 v_normal;
#endif


layout(std140, binding = 0) uniform vs_ub {
#ifdef USE_NORMAL_MAPPING
    vec3 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM];
#endif
    vec3 u_PointLightSourcePosition[MAX_POINT_LIGHT_NUM];
    vec3 u_SpotLightSourcePosition[MAX_SPOT_LIGHT_NUM];
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
        v_dirLightDirection[i].x = dot(eTangent, u_DirLightSourceDirection[i]);
        v_dirLightDirection[i].y = dot(eBinormal, u_DirLightSourceDirection[i]);
        v_dirLightDirection[i].z = dot(eNormal, u_DirLightSourceDirection[i]);
    }

    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 pointLightDir = u_PointLightSourcePosition[i].xyz - ePosition.xyz;
        v_vertexToPointLightDirection[i].x = dot(eTangent, pointLightDir);
        v_vertexToPointLightDirection[i].y = dot(eBinormal, pointLightDir);
        v_vertexToPointLightDirection[i].z = dot(eNormal, pointLightDir);
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        vec3 spotLightDir = u_SpotLightSourcePosition[i] - ePosition.xyz;
        v_vertexToSpotLightDirection[i].x = dot(eTangent, spotLightDir);
        v_vertexToSpotLightDirection[i].y = dot(eBinormal, spotLightDir);
        v_vertexToSpotLightDirection[i].z = dot(eNormal, spotLightDir);

        v_spotLightDirection[i].x = dot(eTangent, u_SpotLightSourceDirection[i]);
        v_spotLightDirection[i].y = dot(eBinormal, u_SpotLightSourceDirection[i]);
        v_spotLightDirection[i].z = dot(eNormal, u_SpotLightSourceDirection[i]);
    }
#else
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        v_vertexToPointLightDirection[i] = u_PointLightSourcePosition[i].xyz - ePosition.xyz;
    }

    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        v_vertexToSpotLightDirection[i] = u_SpotLightSourcePosition[i] - ePosition.xyz;
    }

    v_normal = u_NormalMatrix * a_normal;
#endif

    TextureCoordOut = a_texCoord;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    gl_Position = u_PMatrix * ePosition;
}
