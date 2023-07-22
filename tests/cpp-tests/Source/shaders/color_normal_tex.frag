#version 310 es
precision highp float;
precision highp int;

#define MAX_DIRECTIONAL_LIGHT_NUM 1

#define MAX_POINT_LIGHT_NUM 1

#define MAX_SPOT_LIGHT_NUM 1

#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
#endif
#if (MAX_POINT_LIGHT_NUM > 0)
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
#endif


layout(location = 0) in vec2 TextureCoordOut;
#if MAX_POINT_LIGHT_NUM
layout(location = 1) in vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#if MAX_SPOT_LIGHT_NUM
layout(location = 2) in vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout(location = 3) in vec3 v_normal;
#endif

#endif

layout(binding = 0) uniform sampler2D u_sampler0;

layout(std140, binding = 0) uniform fs_ub {
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
    vec3 u_DirLightSourceColor[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
#if (MAX_POINT_LIGHT_NUM > 0)
    vec3 u_PointLightSourceColor[MAX_POINT_LIGHT_NUM];
    float u_PointLightSourceRangeInverse[MAX_POINT_LIGHT_NUM];
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
    vec3 u_SpotLightSourceColor[MAX_SPOT_LIGHT_NUM];
    vec3 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM]; 
    float u_SpotLightSourceInnerAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceOuterAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceRangeInverse[MAX_SPOT_LIGHT_NUM];
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

layout(location = 0) out vec4 FragColor;

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
        vec3 lightDirection = normalize(u_DirLightSourceDirection[i] * 2.0);
        combinedColor.xyz += computeLighting(normal, -lightDirection, u_DirLightSourceColor[i], 1.0);
    }
#endif

    // Point light contribution
#if (MAX_POINT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 ldir = v_vertexToPointLightDirection[i] * u_PointLightSourceRangeInverse[i];
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(v_vertexToPointLightDirection[i]), u_PointLightSourceColor[i], attenuation);
    }
#endif

    // Spot light contribution
#if (MAX_SPOT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_SPOT_LIGHT_NUM; ++i)
    {
        // Compute range attenuation
        vec3 ldir = v_vertexToSpotLightDirection[i] * u_SpotLightSourceRangeInverse[i];
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        vec3 vertexToSpotLightDirection = normalize(v_vertexToSpotLightDirection[i]);

        vec3 spotLightDirection = normalize(u_SpotLightSourceDirection[i] * 2.0);

        // "-lightDirection" is used because light direction points in opposite direction to spot direction.
        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);

        // Apply spot attenuation
        attenuation *= smoothstep(u_SpotLightSourceOuterAngleCos[i], u_SpotLightSourceInnerAngleCos[i], spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, u_SpotLightSourceColor[i], attenuation);
    }
#endif

#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    FragColor = texture(u_sampler0, TextureCoordOut) * u_color * combinedColor;
#else
    FragColor = texture(u_sampler0, TextureCoordOut) * u_color;
#endif

}
