
#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;

uniform float u_interpolate;

void main() {
    vec4 color1 = texture2D(u_tex0, v_texCoord);
    vec4 color2 = texture2D(u_tex1, v_texCoord);
    gl_FragColor = v_fragmentColor * mix( color1, color2, u_interpolate);
}

