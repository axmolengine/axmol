#version 310 es
precision highp float;
precision highp int;
// Shader from here: http://www.iquilezles.org/


layout(set = 1, binding = 0) uniform sampler2D tex0;
layout(set = 1, binding = 1) uniform sampler2D tex1;

layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec2 resolution;
    vec2 u_screenSize;
    vec4 u_Time;
    vec4 u_CosTime;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
#ifdef AXSLC_TARGET_MSL
	vec2 fragCoord = vec2(gl_FragCoord.x, u_screenSize.y - gl_FragCoord.y);
#else
	vec2 fragCoord = gl_FragCoord.xy;
#endif
    float time = u_Time[1];
    vec2 p = -1.0 + 2.0 * fragCoord / resolution.xy;
    vec2 uv;

    float a = atan(p.y,p.x);
    float r = sqrt(dot(p,p));

    uv.x = r - u_Time[2];
    uv.y = sin(a*10.0 + 2.0)*u_CosTime[0];

    vec3 col =  (.5+.5*uv.y)*texture(tex0,uv).xyz;

    FragColor = vec4(col,1.0);
}
