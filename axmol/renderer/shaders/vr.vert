#version 310 es

#include "base.glsl"

layout(location = POSITION) in vec2 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;
layout(location = TEXCOORD1) in float a_vignette;

layout (location = TEXCOORD0) out vec2 v_texCoord;
layout (location = TEXCOORD1) out float v_vignette;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texCoord = vec2(a_texCoord.x, TEXCOORD_Y(a_texCoord));
    v_vignette = a_vignette;
}
