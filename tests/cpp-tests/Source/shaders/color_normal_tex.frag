#version 310 es
precision highp float;
precision highp int;

#include "base.glsl"

#define MAX_DIRECTIONAL_LIGHT_NUM 1

#define MAX_POINT_LIGHT_NUM 1

#define MAX_SPOT_LIGHT_NUM 1

#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
#endif
#if (MAX_POINT_LIGHT_NUM > 0)
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
#endif


layout(location = TEXCOORD0) in vec2 v_texCoord;
#if MAX_POINT_LIGHT_NUM
layout(location = POINTLIGHT) in vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#if MAX_SPOT_LIGHT_NUM
layout(location = SPOTLIGHT) in vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout(location = NORMAL) in vec3 v_normal;
#endif

#endif

layout(binding = 0) uniform sampler2D u_sampler0;

layout(std140) uniform fs_ub {
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
    vvec3_def(u_DirLightSourceColor, MAX_DIRECTIONAL_LIGHT_NUM);
    vvec3_def(u_DirLightSourceDirection, MAX_DIRECTIONAL_LIGHT_NUM);
#endif
#if (MAX_POINT_LIGHT_NUM > 0)
    vvec3_def(u_PointLightSourceColor, MAX_POINT_LIGHT_NUM);
    vfloat_def(u_PointLightSourceRangeInverse, MAX_POINT_LIGHT_NUM);
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
    vvec3_def(u_SpotLightSourceColor, MAX_SPOT_LIGHT_NUM);
    vvec3_def(u_SpotLightSourceDirection, MAX_SPOT_LIGHT_NUM); 
    vfloat_def(u_SpotLightSourceInnerAngleCos, MAX_SPOT_LIGHT_NUM);
    vfloat_def(u_SpotLightSourceOuterAngleCos, MAX_SPOT_LIGHT_NUM);
    vfloat_def(u_SpotLightSourceRangeInverse, MAX_SPOT_LIGHT_NUM);
#endif
    vec3 u_AmbientLightSourceColor;
    vec4 u_color;
};

vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;
    
    return diffuseColor;
}

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    vec3 normal  = normalize(v_normal);
#endif

    vec4 combinedColor = vec4(u_AmbientLightSourceColor, 1.0);

    // Directional light contribution
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        vec3 lightDirection = normalize(vvec3_at(u_DirLightSourceDirection, i) * 2.0);
        combinedColor.xyz += computeLighting(normal, -lightDirection, vvec3_at(u_DirLightSourceColor, i), 1.0);
    }
#endif

    // Point light contribution
#if (MAX_POINT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 ldir = v_vertexToPointLightDirection[i] * vfloat_at(u_PointLightSourceRangeInverse, i);
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(v_vertexToPointLightDirection[i]), vvec3_at(u_PointLightSourceColor, i), attenuation);
    }
#endif

    // Spot light contribution
#if (MAX_SPOT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        // Compute range attenuation
        vec3 ldir = v_vertexToSpotLightDirection[i] * vfloat_at(u_SpotLightSourceRangeInverse, i);
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        vec3 vertexToSpotLightDirection = normalize(v_vertexToSpotLightDirection[i]);

        vec3 spotLightDirection = normalize(vvec3_at(u_SpotLightSourceDirection, i) * 2.0);

        // "-lightDirection" is used because light direction points in opposite direction to spot direction.
        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);

        // Apply spot attenuation
        attenuation *= smoothstep(vfloat_at(u_SpotLightSourceOuterAngleCos, i), vfloat_at(u_SpotLightSourceInnerAngleCos, i), spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, vvec3_at(u_SpotLightSourceColor, i), attenuation);
    }
#endif

#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    FragColor = texture(u_sampler0, v_texCoord) * u_color * combinedColor;
#else
    FragColor = texture(u_sampler0, v_texCoord) * u_color;
#endif

}
