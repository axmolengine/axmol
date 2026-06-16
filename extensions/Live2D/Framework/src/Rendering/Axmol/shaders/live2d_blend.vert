#version 310 es

#include "base.glsl"

layout(location = POSITION) in vec4 a_position;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = TEXCOORD0) out vec2 v_texCoord;
layout(location = TEXCOORD1) out vec2 v_blendCoord;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    mat4 u_matrix;
};

void main()
{
    vec4 pos = vec4(a_position.x, a_position.y, 0.0, 1.0);
    gl_Position = u_matrix * pos;
    // a_texCoord is Live2D model UVs in top-left origin (V=0=top).
    // Must unconditionally flip for all backends.
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;

    // v_blendCoord is derived from NDC (bottom-left origin, V=0=bottom).
    // Only flip on non-GLSL backends via TEXCOORD_Y.
    vec2 ndcPos = gl_Position.xy / gl_Position.w;
    v_blendCoord = ndcPos * 0.5 + 0.5;

    v_blendCoord.y = TEXCOORD_Y(v_blendCoord);
}
