/*
 * cocos2d for iPhone: http://www.cocos2d-iphone.org
 *
 * Copyright (c) 2011 Ricardo Quesada
 * Copyright (c) 2012 Zynga Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

const char* dualSampler_gray_frag = R"(#version 310 es
precision highp float;
precision highp int;

layout (location = 0) in vec4 v_fragmentColor;
layout (location = 1) in vec2 v_texCoord;

layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout(location = 3, binding = 0) uniform sampler2D u_tex1;

layout (location = 0) out vec4 FragColor;
void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    texColor.a = texture(u_tex1, v_texCoord).r;
    texColor.rgb *= texColor.a; // premultiply alpha channel
    texColor = v_fragmentColor * texColor;
    FragColor.rgb = vec3(0.2126*texColor.r + 0.7152*texColor.g + 0.0722*texColor.b);
    FragColor.a = texColor.a;
}
)";
