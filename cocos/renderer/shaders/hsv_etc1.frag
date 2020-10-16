
const char* hsv_etc1_frag = R"(#ifdef GL_ES
precision mediump float;
#endif 

varying vec2 v_texCoord;  
varying vec4 v_fragmentColor;

uniform sampler2D u_texture;
uniform sampler2D u_texture1;

// HSV matrix
uniform mat3 u_mix_hsv;

// filter color RGB values
uniform vec3 u_filter_rgb;

// force shading HSV values
uniform vec3 u_shading_hsv;

// whether use force shading
uniform int u_force_shading;

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

void main() 
{ 
    vec4 texColor = vec4(texture2D(u_texture, v_texCoord).rgb, texture2D(u_texture1, v_texCoord).r);
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
    else { // shading color
        if(texColor.a > 0.0) {
            if( u_force_shading != 0 ) {  // force shading color
              if(gv > 0.115 && rgbColor.g > 0.115 && !(abs(rv - gv) < 0.10589 && abs(rv - bv) < 0.10589 && abs(gv - bv) < 0.0909) || ( rgbColor.b > 0.6280 && (bv - rv)  > 0.0599 ) ) { // currently is yellow, TODO: use hh,ss,vv
                  rgbColor.r = min(1.0, rgbColor.g * 1.2525);
                  rgbColor.b *= rgbColor.b;
                  rgbColor.g *= 0.9025;

                  rgbColor.r *= v_fragmentColor.a;
                  rgbColor.g *= v_fragmentColor.a;
                  rgbColor.b *= v_fragmentColor.a;
              }
           }
       }
    }
    rgbColor.rgb = rgbColor.rgb * v_fragmentColor.rgb;
    gl_FragColor = vec4(rgbColor, texColor.a * v_fragmentColor.a); 
} 
)";
