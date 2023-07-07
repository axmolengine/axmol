#version 310 es
precision highp float;
precision highp int;
#ifdef GL_ES
    precision mediump float;
#endif
layout (location = 0) in vec4 v_fragmentColor;
layout (location = 1) in vec2 v_texCoord;
layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout(location = 3, binding = 0) uniform sampler2D u_tex1;
layout (location = 0) out vec4 FragColor;
void main()
{
    vec4 texColor = texture(u_tex0, v_texCoord);
    texColor.a = texture(u_tex1, v_texCoord).r;
    texColor.rgb *= texColor.a; // premultiply alpha channel
    texColor = v_fragmentColor * texColor;
    FragColor.rgb = vec3(0.2126*texColor.r + 0.7152*texColor.g + 0.0722*texColor.b);
    FragColor.a = texColor.a;
}
