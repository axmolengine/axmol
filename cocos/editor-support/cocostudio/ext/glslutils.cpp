
#include "glslutils.hpp"

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

    auto glProgram = addPositionTextureColorGLProgram("glsl.builtin.wintelli_hsv.frag", noMVP);
    if (glProgram == nullptr) {
        return false;
    }

    node->setGLProgramState(GLProgramState::create(glProgram));

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
    node->getGLProgramState()->setUniformCallback("u_mix_hsv", [hsvMatrix](cocos2d::GLProgram *p, cocos2d::Uniform *u) {
        glUniformMatrix3fv(u->location, 1, GL_FALSE, (GLfloat*)&hsvMatrix.m[0]);
    });

    node->getGLProgramState()->setUniformVec3("u_filter_rgb", filter);
    node->getGLProgramState()->setUniformInt("u_force_shading", forceShading ? 1 : 0);
    // node->getGLProgramState()->setUniformVec3("u_shading_hsv", hsvShading);
}

GLProgram* glslutils::addPositionTextureColorGLProgram(const std::string& fragFile, bool noMVP, bool refresh)
{
    auto glProgram = (GLProgramCache::getInstance())->getGLProgram(fragFile);
    if (!refresh  && glProgram != nullptr)
        return glProgram;

    auto fragSource = s_wintell_hsv_frag;// FileUtils::getInstance()->getStringFromFile(fragFile);
    glProgram = new(std::nothrow) GLProgram();
    const GLchar* vertSource = nullptr;
    if (noMVP) {
        vertSource = ccPositionTextureColor_noMVP_vert;
    }
    else {
        vertSource = ccPositionTextureColor_vert;
    }
    if (glProgram != nullptr && glProgram->initWithByteArrays(vertSource, fragSource)) {
        glProgram->link();
        glProgram->updateUniforms();
        GLProgramCache::getInstance()->addGLProgram(glProgram, fragFile);
        glProgram->release();
        return glProgram;
    }

    assert(false);
    CC_SAFE_DELETE(glProgram);
    return glProgram;
}
