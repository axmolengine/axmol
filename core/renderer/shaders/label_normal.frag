#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision lowp float;
#endif
layout (location = 0) in vec4 v_fragmentColor;
layout (location = 1) in vec2 v_texCoord;
layout(std140, binding = 0) uniform Block_0 {
    vec4 u_textColor;
};
layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout (location = 0) out vec4 FragColor;
void main()
{
    FragColor =  v_fragmentColor * vec4(u_textColor.rgb,// RGB from uniform
        u_textColor.a * texture(u_tex0, v_texCoord).a// A from texture & uniform
    );
}
