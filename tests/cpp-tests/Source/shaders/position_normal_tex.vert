#version 310 es

#include "base.glsl"

#define MAX_POINT_LIGHT_NUM 1
#define MAX_SPOT_LIGHT_NUM 1
#define MAX_DIRECTIONAL_LIGHT_NUM 1

#if (MAX_POINT_LIGHT_NUM > 0)
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
#endif

layout(location = POSITION) in vec4 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = NORMAL) in vec3 a_normal;
layout(location = TEXCOORD0) out vec2 v_texCoord;

#if MAX_POINT_LIGHT_NUM
layout(location = POINTLIGHT) out vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#endif
#if MAX_SPOT_LIGHT_NUM
layout(location = SPOTLIGHT) out vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout(location = NORMAL) out vec3 v_normal;
#endif

layout(std140, set = 0, binding = 0) uniform vs_ub {
#if (MAX_POINT_LIGHT_NUM > 0)
    vvec3_def(u_PointLightSourcePosition, MAX_POINT_LIGHT_NUM);
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
    vvec3_def(u_SpotLightSourcePosition, MAX_SPOT_LIGHT_NUM);
#endif
    mat4 u_MVMatrix;
    mat4 u_PMatrix;
    mat3 u_NormalMatrix;
};

void main(void)
{
    vec4 ePosition = u_MVMatrix * a_position;
#if (MAX_POINT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        v_vertexToPointLightDirection[i] = vvec3_at(u_PointLightSourcePosition, i) - ePosition.xyz;
    }
#endif

#if (MAX_SPOT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        v_vertexToSpotLightDirection[i] = vvec3_at(u_SpotLightSourcePosition, i) - ePosition.xyz;
    }
#endif
        
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    v_normal = u_NormalMatrix * a_normal;
#endif

    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
    gl_Position = u_PMatrix * ePosition;
}

