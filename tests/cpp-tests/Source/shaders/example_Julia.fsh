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
#ifdef AXSLC_TARGET_MSL
	vec2 fragCoord = vec2(gl_FragCoord.x, u_screenSize.y - gl_FragCoord.y);
#else
	vec2 fragCoord = gl_FragCoord.xy;
#endif
    vec2 p = 2.0 * (fragCoord - center.xy) / resolution.xy;
    vec2 cc = vec2( cos(.25*time), sin(.25*time*1.423) );

    float dmin = 1000.0;
    vec2 z  = p*vec2(1.33,1.0);
    for( int i=0; i<64; i++ )
    {
        z = cc + vec2( z.x*z.x - z.y*z.y, 2.0*z.x*z.y );
        float m2 = dot(z,z);
        if( m2>100.0 ) break;
        dmin=min(dmin,m2);
        }

    float color = sqrt(sqrt(dmin))*0.7;
    FragColor = vec4(color,color,color,1.0);
}

