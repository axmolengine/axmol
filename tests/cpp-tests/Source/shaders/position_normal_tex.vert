#version 310 es

#define MAX_POINT_LIGHT_NUM 1
#define MAX_SPOT_LIGHT_NUM 1
#define MAX_DIRECTIONAL_LIGHT_NUM 1

#if (MAX_POINT_LIGHT_NUM > 0)
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
#endif

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;
layout(location = 0) out vec2 TextureCoordOut;

#if MAX_POINT_LIGHT_NUM
layout(location = 1) out vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#endif
#if MAX_SPOT_LIGHT_NUM
layout(location = 2) out vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout(location = 3) out vec3 v_normal;
#endif

layout(std140, binding = 0) uniform vs_ub {
#if (MAX_POINT_LIGHT_NUM > 0)
    vec3 u_PointLightSourcePosition[MAX_POINT_LIGHT_NUM];
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
    vec3 u_SpotLightSourcePosition[MAX_SPOT_LIGHT_NUM];
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
        v_vertexToPointLightDirection[i] = u_PointLightSourcePosition[i].xyz - ePosition.xyz;
    }
#endif

#if (MAX_SPOT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        v_vertexToSpotLightDirection[i] = u_SpotLightSourcePosition[i] - ePosition.xyz;
    }
#endif
        
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    v_normal = u_NormalMatrix * a_normal;
#endif

    TextureCoordOut = a_texCoord;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    gl_Position = u_PMatrix * ePosition;
}

