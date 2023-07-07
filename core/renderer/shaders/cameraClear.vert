#version 310 es
precision highp float;
precision highp int;
layout(std140, binding = 0) uniform Block_0 {
    float depth;
};
layout (location = 0) in vec4 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoord;
#ifdef GL_ES
layout (location = 0) out mediump vec2 v_texCoord;
layout (location = 1) out mediump vec4 v_color;
#else
layout (location = 2) out vec2 v_texCoord;
layout (location = 3) out vec4 v_color;
#endif
void main()
{
    gl_Position = a_position;
    gl_Position.z = depth;
    gl_Position.w = 1.0;
    v_texCoord = a_texCoord;
    v_color = a_color;
}
