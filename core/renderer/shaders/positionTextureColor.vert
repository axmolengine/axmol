#version 310 es
precision highp float;
precision highp int;
layout (location = 0) in vec4 a_position;
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec4 a_color;
layout(std140, binding = 0) uniform Block_0 {
    mat4 u_MVPMatrix;
};
#ifdef GL_ES
layout (location = 0) out lowp vec4 v_fragmentColor;
layout (location = 1) out mediump vec2 v_texCoord;
#else
layout (location = 2) out vec4 v_fragmentColor;
layout (location = 3) out vec2 v_texCoord;
#endif
void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    v_texCoord = a_texCoord;
}
