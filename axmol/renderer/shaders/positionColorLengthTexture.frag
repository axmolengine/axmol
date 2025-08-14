#version 310 es
precision highp float;
precision highp int;


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

void main()
{
// #if defined GL_OES_standard_derivatives
// FragColor = v_color*smoothstep(0.0, length(fwidth(v_texCoord)), 1.0 - length(v_texCoord));
// #else
    FragColor = v_color*step(0.0, 1.0 - length(v_texCoord));
// #endif
}
