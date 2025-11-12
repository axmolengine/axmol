#version 310 es
precision highp float;
precision highp int;
// Shader taken from: http://webglsamples.googlecode.com/hg/electricflower/electricflower.html


layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(set = 1, binding = 0) uniform sampler2D u_tex0;

const float intensity = 0.05;
layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec2 resolution;
    vec4 u_Time;
    vec2 u_screenSize;
};

vec3 noise(vec2 uv)
{
	vec2 p = abs(sin(uv * 13.0 + uv.x * u_Time[1] * sin(uv.y)));

	return vec3(sin (0.2 * u_Time[1] + sin(p * 0.5) * u_Time[1] / cos(50.0)) * 10.0,0.3+0.5 * abs(sin(u_Time[1] * tan(5.0))));

}

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
#ifdef AXSLC_TARGET_MSL
	vec2 fragCoord = vec2(gl_FragCoord.x, u_screenSize.y - gl_FragCoord.y);
#else
	vec2 fragCoord = gl_FragCoord.xy;
#endif
	FragColor.xyz = intensity * noise(fragCoord / sin(resolution.xy * u_Time[1] * 0.01)) + (1. - intensity) *
			texture(u_tex0,v_texCoord.xy).xyz;
	FragColor.w = 1.;
}
