#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision lowp float;
#endif
#ifdef GL_ES
layout (location = 0) in mediump vec4 a_position;
layout (location = 1) in mediump vec2 a_texCoord;
layout (location = 2) in mediump vec4 a_color;
layout (location = 0) out mediump vec4 v_color;
layout (location = 1) out mediump vec2 v_texCoord;
#else
layout (location = 3) in vec4 a_position;
layout (location = 4) in vec2 a_texCoord;
layout (location = 5) in vec4 a_color;
layout (location = 2) out vec4 v_color;
layout (location = 3) out vec2 v_texCoord;
#endif
layout(std140, binding = 0) uniform Block_0 {
    float u_alpha;
    mat4 u_MVPMatrix;
};
void main()
{
    v_color = vec4(a_color.rgb * a_color.a * u_alpha, a_color.a * u_alpha);
    v_texCoord = a_texCoord;
    gl_Position = u_MVPMatrix * a_position;
}
