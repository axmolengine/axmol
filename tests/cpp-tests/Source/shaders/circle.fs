// SPDX-FileCopyrightText: 2024 Erin Catto
// SPDX-License-Identifier: MIT

#version 420

layout(location = 0) in vec2 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_thickness;

layout(location = SV_Target0) out vec4 fragColor;

void main()
{
    // radius in unit quad
    float radius = 1.0;

    // distance to circle
    vec2 w = v_position;
    float dw = length(w);
    float d = abs(dw - radius);

    fragColor = vec4(v_color.rgb, smoothstep(v_thickness, 0.0, d));
}
