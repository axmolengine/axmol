#version 310 es
precision highp float;
precision highp int;
// Shader from http://www.iquilezles.org/apps/shadertoy/



layout(std140, set = 0, binding = 1) uniform fs_ub {
    vec2 center;
    vec2 resolution;
    vec2 u_screenSize;
    vec4 u_Time;
};

layout(location = SV_Target0) out vec4 FragColor;

void main(void)
{
    float time = u_Time[1];
#if !defined(AXSLC_TARGET_GLSL)
    vec2 fragCoord = vec2(gl_FragCoord.x, u_screenSize.y - gl_FragCoord.y);
#else
    vec2 fragCoord = gl_FragCoord.xy;
#endif
    vec2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;

    // animate
    float tt = mod(time,2.0)/2.0;
    float ss = pow(tt,.2)*0.5 + 0.5;
    ss -= ss*0.2*sin(tt*6.2831*5.0)*exp(-tt*6.0);
    p *= vec2(0.5,1.5) + ss*vec2(0.5,-0.5);


    float a = atan(p.x,p.y)/3.141593;
    float r = length(p);

    // shape
    float h = abs(a);
    float d = (13.0*h - 22.0*h*h + 10.0*h*h*h)/(6.0-5.0*h);

    // color
    float f = step(r,d) * pow(1.0-r/d,0.25);

    FragColor = vec4(f,0.0,0.0,1.0);
}
