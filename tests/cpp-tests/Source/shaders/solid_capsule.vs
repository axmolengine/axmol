// SPDX-FileCopyrightText: 2024 Erin Catto
// SPDX-License-Identifier: MIT

#version 420

layout(location=0) in vec2 a_localPosition;
layout(location=1) in vec4 a_instanceTransform;
layout(location=2) in float a_instanceRadius;
layout(location=3) in float a_instanceLength;
layout(location=4) in vec4 a_instanceColor;

layout(location=0) out vec2 v_position;
layout(location=1) out vec4 v_color;
layout(location=2) out float v_length;
layout(location=3) out float v_thickness;

layout(std140) uniform vs_ub {
    float u_pixelScale;
    mat4 u_MVPMatrix;
};

void main()
{
    v_position = a_localPosition;
    v_color = a_instanceColor;
    
    float radius = a_instanceRadius;
    float length = a_instanceLength;
    
    // scale quad large enough to hold capsule
    float scale = radius + 0.5 * length;
    
    // quad range of [-1, 1] implies normalize radius and length
    v_length = length / scale;
    
    // resolution.y = pixelScale * scale
    v_thickness = 3.0f / (u_pixelScale * scale);
    
    float x = a_instanceTransform.x;
    float y = a_instanceTransform.y;
    float c = a_instanceTransform.z;
    float s = a_instanceTransform.w;
    vec2 p = vec2(scale * a_localPosition.x, scale * a_localPosition.y);
    p = vec2((c * p.x - s * p.y) + x, (s * p.x + c * p.y) + y);
    gl_Position = u_MVPMatrix * vec4(p, 0.0, 1.0);
}
