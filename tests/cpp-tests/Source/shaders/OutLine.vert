#version 310 es

#include "base.glsl"

layout(location = POSITION) in vec4 a_position;
layout(location = NORMAL) in vec3 a_normal;

layout(std140, set = 0, binding = 0) uniform vs_ub {
    float OutlineWidth;
    mat4 u_MVPMatrix;
};

void main(void)
{
    vec4 pos = u_MVPMatrix * a_position;
    vec4 normalproj = u_MVPMatrix * vec4(a_normal, 0);
    normalproj = normalize(normalproj);
    pos.xy += normalproj.xy * (OutlineWidth * (pos.z * 0.5 + 0.5));

    gl_Position = pos;
}
