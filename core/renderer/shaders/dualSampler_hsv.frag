
const char* dualSampler_hsv_frag = R"(#version 310 es
precision highp float;
precision highp int;

layout (location = 0) in vec2 v_texCoord;  
layout (location = 1) in vec4 v_fragmentColor;

layout(location = 2, binding = 0) uniform sampler2D u_tex0;
layout(location = 3, binding = 0) uniform sampler2D u_tex1;

// HSV matrix
layout(std140, binding = 0) uniform Block_0 {
    mat3 u_mix_hsv;
};

// filter color RGB values
layout(std140, binding = 1) uniform Block_1 {
    vec3 u_filter_rgb;
};

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

layout (location = 0) out vec4 FragColor;
void main() 
{ 
    vec4 texColor = vec4(texture(u_tex0, v_texCoord).rgb, texture(u_tex1, v_texCoord).r);
    texColor.rgb *= texColor.a; // Premultiply with Alpha channel
    
    vec3 rgbColor = u_mix_hsv * texColor.rgb;
    float sum = texColor.r + texColor.g + texColor.b;
    float rv = texColor.r / sum;
    float gv = texColor.g / sum;
    float bv = texColor.b / sum;
    if(  (rv < u_filter_rgb.r && gv < u_filter_rgb.g && bv < u_filter_rgb.b) || texColor.a < 0.1)
    { // color filters, resume to original color
         rgbColor = texColor.rgb;
         rgbColor.r *= v_fragmentColor.a;
         rgbColor.g *= v_fragmentColor.a;
         rgbColor.b *= v_fragmentColor.a;
    } 
    rgbColor.rgb = rgbColor.rgb * v_fragmentColor.rgb;
    FragColor = vec4(rgbColor, texColor.a * v_fragmentColor.a); 
}
)";
