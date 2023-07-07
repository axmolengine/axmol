#version 310 es
precision highp float;
precision highp int;
#if !defined(MAX_DIRECTIONAL_LIGHT_NUM)
#define MAX_DIRECTIONAL_LIGHT_NUM 0
#endif
#if !defined(MAX_POINT_LIGHT_NUM)
#define MAX_POINT_LIGHT_NUM 0
#endif
#if !defined(MAX_SPOT_LIGHT_NUM)
#define MAX_SPOT_LIGHT_NUM 0
#endif
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
layout(std140, binding = 0) uniform Block_0 {
    vec3 u_DirLightSourceColor[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
};
#endif
#if (MAX_POINT_LIGHT_NUM > 0)
layout(std140, binding = 1) uniform Block_1 {
    vec3 u_PointLightSourceColor[MAX_POINT_LIGHT_NUM];
    float u_PointLightSourceRangeInverse[MAX_POINT_LIGHT_NUM];
};
#endif
#if (MAX_SPOT_LIGHT_NUM > 0)
layout(std140, binding = 2) uniform Block_2 {
    vec3 u_SpotLightSourceColor[MAX_SPOT_LIGHT_NUM];
    vec3 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceInnerAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceOuterAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceRangeInverse[MAX_SPOT_LIGHT_NUM];
};
#endif
layout(std140, binding = 3) uniform Block_3 {
    vec3 u_AmbientLightSourceColor;
};
#ifdef GL_ES
layout (location = 0) in mediump vec2 TextureCoordOut;
#ifdef USE_NORMAL_MAPPING
#if MAX_DIRECTIONAL_LIGHT_NUM
layout (location = 1) in mediump vec3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
#endif
#if MAX_POINT_LIGHT_NUM
layout (location = 2) in mediump vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#endif
#if MAX_SPOT_LIGHT_NUM
layout (location = 3) in mediump vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#ifdef USE_NORMAL_MAPPING
layout (location = 4) in mediump vec3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#endif
#ifndef USE_NORMAL_MAPPING
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout (location = 5) in mediump vec3 v_normal;
#endif
#endif
#else
layout (location = 6) in vec2 TextureCoordOut;
#ifdef USE_NORMAL_MAPPING
#if MAX_DIRECTIONAL_LIGHT_NUM
layout (location = 7) in vec3 v_dirLightDirection[MAX_DIRECTIONAL_LIGHT_NUM];
#endif
#endif
#if MAX_POINT_LIGHT_NUM
layout (location = 8) in vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#endif
#if MAX_SPOT_LIGHT_NUM
layout (location = 9) in vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#ifdef USE_NORMAL_MAPPING
layout (location = 10) in vec3 v_spotLightDirection[MAX_SPOT_LIGHT_NUM];
#endif
#endif
#ifndef USE_NORMAL_MAPPING
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout (location = 11) in vec3 v_normal;
#endif
#endif
#endif
layout(std140, binding = 4) uniform Block_4 {
    vec4 u_color;
};
#ifdef USE_NORMAL_MAPPING
layout(location = 12, binding = 0) uniform sampler2D u_normalTex;
#endif
layout(location = 13, binding = 0) uniform sampler2D u_tex0;
vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;
    return diffuseColor;
}
layout (location = 0) out vec4 FragColor;
void main(void)
{
#ifdef USE_NORMAL_MAPPING
    #if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
        vec3 normal  = normalize(2.0 * texture(u_normalTex, TextureCoordOut).xyz - 1.0);
    #endif
#else
    #if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
        vec3 normal  = normalize(v_normal);
    #endif
#endif
    vec4 combinedColor = vec4(u_AmbientLightSourceColor, 1.0);
    // Directional light contribution
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
#ifdef USE_NORMAL_MAPPING
        vec3 lightDirection = normalize(v_dirLightDirection[i] * 2.0);
#else
        vec3 lightDirection = normalize(u_DirLightSourceDirection[i] * 2.0);
#endif
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
#ifdef USE_NORMAL_MAPPING
        vec3 spotLightDirection = normalize(v_spotLightDirection[i] * 2.0);
#else
        vec3 spotLightDirection = normalize(u_SpotLightSourceDirection[i] * 2.0);
#endif
        // \-lightDirection\ is used because light direction points in opposite direction to spot direction.
        float spotCurrentAngleCos = dot(spotLightDirection, -vertexToSpotLightDirection);
        // Apply spot attenuation
        attenuation *= smoothstep(u_SpotLightSourceOuterAngleCos[i], u_SpotLightSourceInnerAngleCos[i], spotCurrentAngleCos);
        attenuation = clamp(attenuation, 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, vertexToSpotLightDirection, u_SpotLightSourceColor[i], attenuation);
    }
#endif
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    FragColor = texture(u_tex0, TextureCoordOut) * u_color * combinedColor;
#else
    FragColor = texture(u_tex0, TextureCoordOut) * u_color;
#endif
}
