// SPDX-FileCopyrightText: 2024 Erin Catto
// SPDX-License-Identifier: MIT

#version 420

layout(location = 0) in vec2 a_localPosition;
layout(location = 1) in vec2 a_instancePosition;
layout(location = 2) in float a_instanceRadius;
layout(location = 3) in vec4 a_instanceColor;

layout(location = 0) out vec2 v_position;
layout(location = 1) out vec4 v_color;
layout(location = 2) out float v_thickness;

layout(std140) uniform vs_ub {
    float u_pixelScale;
    mat4 u_MVPMatrix;
};

void main()
{
    v_position = a_localPosition;
    v_color = a_instanceColor;
    float radius = a_instanceRadius;

    // resolution.y = pixelScale * radius
    v_thickness = 3.0f / (u_pixelScale * radius);

    vec2 p = vec2(radius * a_localPosition.x, radius * a_localPosition.y) + a_instancePosition;
    gl_Position = u_MVPMatrix * vec4(p, 0.0f, 1.0f);
}
