
#include "glslutils.hpp"

#include "renderer/backend/ProgramCache.h"
#include "renderer/backend/Device.h"

static const char* s_wintell_hsv_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif 

varying vec2 v_texCoord;  
varying vec4 v_fragmentColor;

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
    vec4 pixColor = texture2D(CC_Texture0, v_texCoord);  // * v_fragmentColor; 
    vec3 rgbColor = u_mix_hsv * pixColor.rgb;

    float sum = pixColor.r + pixColor.g + pixColor.b;
    float rv = pixColor.r / sum;
    float gv = pixColor.g / sum;
    float bv = pixColor.b / sum;
    if(  (rv < u_filter_rgb.r && gv < u_filter_rgb.g && bv < u_filter_rgb.b) || pixColor.a < 0.1)
    { // color filters, resume to original color
         rgbColor = pixColor.rgb;

         rgbColor.r *= v_fragmentColor.a;
         rgbColor.g *= v_fragmentColor.a;
         rgbColor.b *= v_fragmentColor.a;
    } 
    else { // shading color
        if(pixColor.a > 0.0) {
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
    gl_FragColor = vec4(rgbColor, pixColor.a * v_fragmentColor.a); 
} 
)";

USING_NS_CC;

bool glslutils::enableNodeIntelliShading(cocos2d::Node* node,
    bool noMVP,
    const cocos2d::Vec3& hsv,
    const cocos2d::Vec3& filter,
    bool forceShading,
    const cocos2d::Vec3& hsvShading)
{
    if (node == nullptr)
        return false;

    // TODO: cache it?
    auto program = newHSVProgram();
    if (program == nullptr) {
        return false;
    }

    auto programState = new backend::ProgramState(program);
    program->release();

    node->setProgramState(programState);
    programState->release();

    updateNodeHsv(node, hsv, filter, forceShading, hsvShading);

    return true;
}

void glslutils::updateNodeHsv(cocos2d::Node* node,
    const cocos2d::Vec3& hsv,
    const cocos2d::Vec3& filter,
    bool forceShading,
    const cocos2d::Vec3& hsvShading)
{
    Matrix3f hsvMatrix;
    setMatrixHueOptimized(&hsvMatrix, hsv.x);
    setMatrixSat(&hsvMatrix, hsv.y);
    setMatrixVal(&hsvMatrix, hsv.z);

    printmat3(hsvMatrix.m);

    auto programState = node->getProgramState();
    programState->setCallbackUniform(programState->getUniformLocation("u_mix_hsv"), [hsvMatrix](backend::ProgramState* programState, const backend::UniformLocation& location) {
        programState->setUniform(location, &hsvMatrix.m[0], sizeof(hsvMatrix));
    });

    programState->setUniform(programState->getUniformLocation("u_filter_rgb"), &filter, sizeof(filter));
    int32_t value = forceShading ? 1 : 0;
    programState->setUniform(programState->getUniformLocation("u_force_shading"), &value, sizeof(value));
}

backend::Program* glslutils::newHSVProgram()
{
    auto fragSource = s_wintell_hsv_frag;
    return backend::Device::getInstance()->newProgram(positionTextureColor_vert, fragSource);
}
