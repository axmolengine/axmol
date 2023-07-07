#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
precision mediump float;
#endif
layout (location = 0) in vec4 v_fragmentColor;
layout (location = 1) in vec2 v_texCoord;
layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout (location = 0) out vec4 FragColor;
void main(void)
{
    vec4 c = texture(u_tex0, v_texCoord);
     c = v_fragmentColor * c;
    FragColor.xyz = vec3(0.2126*c.r + 0.7152*c.g + 0.0722*c.b);
    FragColor.w = c.w;
}
