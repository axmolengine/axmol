#version 310 es

layout(location = POSITION) in vec4 a_position;
layout(location = COLOR0) in vec4 a_color;
layout(location = TEXCOORD0) in vec2 a_texCoord;

layout(location = TEXCOORD0) out vec2 v_texCoord;

void main()
{
#if defined(METAL)
    gl_Position = vec4(a_position.x, -a_position.y, a_position.z, 1.0);
#else
    gl_Position = vec4(a_position.xyz, 1.0);
#endif
    v_texCoord = a_texCoord;
}

