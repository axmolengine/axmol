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
 
const char* CC3D_terrain_frag = R"(#version 310 es
precision highp float;
precision highp int;

layout(std140, binding = 0) uniform Block_0 {
    int u_has_alpha;
    int u_has_light_map;
};

layout(location = 0, binding = 0) uniform sampler2D u_alphaMap;
layout(location = 1, binding = 0) uniform sampler2D u_tex0;
layout(location = 2, binding = 0) uniform sampler2D u_tex1;
layout(location = 3, binding = 0) uniform sampler2D u_tex2;
layout(location = 4, binding = 0) uniform sampler2D u_tex3;
layout(location = 5, binding = 0) uniform sampler2D u_lightMap;

layout(std140, binding = 1) uniform Block_1 {
    float u_detailSize[4];
    vec3 u_lightDir;
};

layout (location = 0) out vec4 FragColor;
void main()
{
vec4 lightColor;
if(u_has_light_map<=0)
{
    lightColor = vec4(1.0,1.0,1.0,1.0);
}else
{
    lightColor = texture(u_lightMap,v_texCoord);
}
float lightFactor = dot(-u_lightDir,v_normal);
if(u_has_alpha<=0)
{
    FragColor = texture(u_tex0, v_texCoord)*lightColor*lightFactor;
}else
{
    vec4 blendFactor =texture(u_alphaMap,v_texCoord);
    vec4 color = vec4(0.0,0.0,0.0,0.0);
    color = texture(u_tex0, v_texCoord*u_detailSize[0])*blendFactor.r +
    texture(u_tex1, v_texCoord*u_detailSize[1])*blendFactor.g + texture(u_tex2, v_texCoord*u_detailSize[2])*blendFactor.b
        + texture(u_tex3, v_texCoord*u_detailSize[3])*(1.0 - blendFactor.a);
    FragColor = vec4(color.rgb*lightColor.rgb*lightFactor, 1.0);
}
}
)";
