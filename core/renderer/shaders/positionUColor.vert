#version 310 es
precision highp float;
precision highp int;
layout (location = 0) in vec4 a_position;
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_color;
    mat4 u_MVPMatrix;
};
#ifdef GL_ES
layout (location = 0) out lowp vec4 v_fragmentColor;
#else
layout (location = 1) out vec4 v_fragmentColor;
#endif
void main()
{
    gl_Position = u_MVPMatrix * a_position;
    v_fragmentColor = u_color;
}
