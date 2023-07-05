/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
const char* CC3D_colorNormal_frag = R"(#version 310 es
precision highp float;
precision highp int;

#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
layout(std140, binding = 0) uniform Block_0 {
    vec3 u_DirLightSourceColor[MAX_DIRECTIONAL_LIGHT_NUM];
    vec3 u_DirLightSourceDirection[MAX_DIRECTIONAL_LIGHT_NUM];
};
#if (MAX_POINT_LIGHT_NUM > 0)
layout(std140, binding = 1) uniform Block_1 {
    vec3 u_PointLightSourceColor[MAX_POINT_LIGHT_NUM];
    float u_PointLightSourceRangeInverse[MAX_POINT_LIGHT_NUM];
};
#if (MAX_SPOT_LIGHT_NUM > 0)
layout(std140, binding = 2) uniform Block_2 {
    vec3 u_SpotLightSourceColor[MAX_SPOT_LIGHT_NUM];
    vec3 u_SpotLightSourceDirection[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceInnerAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceOuterAngleCos[MAX_SPOT_LIGHT_NUM];
    float u_SpotLightSourceRangeInverse[MAX_SPOT_LIGHT_NUM];
    vec3 u_AmbientLightSourceColor;
};
layout (location = 0) in vec2 TextureCoordOut;
#if MAX_POINT_LIGHT_NUM
layout (location = 1) in vec3 v_vertexToPointLightDirection[MAX_POINT_LIGHT_NUM];
#if MAX_SPOT_LIGHT_NUM
layout (location = 2) in vec3 v_vertexToSpotLightDirection[MAX_SPOT_LIGHT_NUM];
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
layout (location = 3) in vec3 v_normal;
layout(std140, binding = 3) uniform Block_3 {
    vec4 u_color;
};
vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuseColor = lightColor  * diffuse * attenuation;
    return diffuseColor;
}
layout (location = 0) out vec4 FragColor;
void main(void)
{
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    vec3 normal  = normalize(v_normal);
    vec4 combinedColor = vec4(u_AmbientLightSourceColor, 1.0);
    // Directional light contribution
#if (MAX_DIRECTIONAL_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHT_NUM; ++i)
    {
        vec3 lightDirection = normalize(u_DirLightSourceDirection[i] * 2.0);
        combinedColor.xyz += computeLighting(normal, -lightDirection, u_DirLightSourceColor[i], 1.0);
    }
    // Point light contribution
#if (MAX_POINT_LIGHT_NUM > 0)
    for (int i = 0; i < MAX_POINT_LIGHT_NUM; ++i)
    {
        vec3 ldir = v_vertexToPointLightDirection[i] * u_PointLightSourceRangeInverse[i];
        float attenuation = clamp(1.0 - dot(ldir, ldir), 0.0, 1.0);
        combinedColor.xyz += computeLighting(normal, normalize(v_vertexToPointLightDirection[i]), u_PointLightSourceColor[i], attenuation);
    }
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
#if ((MAX_DIRECTIONAL_LIGHT_NUM > 0) || (MAX_POINT_LIGHT_NUM > 0) || (MAX_SPOT_LIGHT_NUM > 0))
    FragColor = u_color * combinedColor;
#else
    FragColor = u_color;
}

)";
