#include "3DProgramInfo.h"

namespace ax
{

namespace shaderinfos
{
/* TODO these consts should not be used */
namespace shader
{
const char* SHADER_NAME_ETC1AS_POSITION_TEXTURE_COLOR        = "#ShaderETC1ASPositionTextureColor";
const char* SHADER_NAME_ETC1AS_POSITION_TEXTURE_COLOR_NO_MVP = "#ShaderETC1ASPositionTextureColor_noMVP";

const char* SHADER_NAME_ETC1AS_POSITION_TEXTURE_GRAY        = "#ShaderETC1ASPositionTextureGray";
const char* SHADER_NAME_ETC1AS_POSITION_TEXTURE_GRAY_NO_MVP = "#ShaderETC1ASPositionTextureGray_noMVP";

const char* SHADER_NAME_POSITION_TEXTURE_COLOR            = "ShaderPositionTextureColor";
const char* SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP     = "ShaderPositionTextureColor_noMVP";
const char* SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST       = "ShaderPositionTextureColorAlphaTest";
const char* SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV = "ShaderPositionTextureColorAlphaTest_NoMV";
const char* SHADER_NAME_POSITION_COLOR                    = "ShaderPositionColor";
const char* SHADER_NAME_POSITION_COLOR_TEXASPOINTSIZE     = "ShaderPositionColorTexAsPointsize";
const char* SHADER_NAME_POSITION_COLOR_NO_MVP             = "ShaderPositionColor_noMVP";

const char* SHADER_NAME_POSITION_TEXTURE              = "ShaderPositionTexture";
const char* SHADER_NAME_POSITION_TEXTURE_U_COLOR      = "ShaderPositionTexture_uColor";
const char* SHADER_NAME_POSITION_TEXTURE_A8_COLOR     = "ShaderPositionTextureA8Color";
const char* SHADER_NAME_POSITION_U_COLOR              = "ShaderPosition_uColor";
const char* SHADER_NAME_POSITION_LENGTH_TEXTURE_COLOR = "ShaderPositionLengthTextureColor";
const char* SHADER_NAME_POSITION_GRAYSCALE            = "ShaderUIGrayScale";
const char* SHADER_NAME_LABEL_DISTANCEFIELD_NORMAL    = "ShaderLabelDFNormal";
const char* SHADER_NAME_LABEL_DISTANCEFIELD_GLOW      = "ShaderLabelDFGlow";
const char* SHADER_NAME_LABEL_NORMAL                  = "ShaderLabelNormal";
const char* SHADER_NAME_LABEL_OUTLINE                 = "ShaderLabelOutline";

const char* SHADER_3D_POSITION                          = "Shader3DPosition";
const char* SHADER_3D_POSITION_TEXTURE                  = "Shader3DPositionTexture";
const char* SHADER_3D_SKINPOSITION_TEXTURE              = "Shader3DSkinPositionTexture";
const char* SHADER_3D_POSITION_NORMAL                   = "Shader3DPositionNormal";
const char* SHADER_3D_POSITION_NORMAL_TEXTURE           = "Shader3DPositionNormalTexture";
const char* SHADER_3D_SKINPOSITION_NORMAL_TEXTURE       = "Shader3DSkinPositionNormalTexture";
const char* SHADER_3D_POSITION_BUMPEDNORMAL_TEXTURE     = "Shader3DPositionBumpedNormalTexture";
const char* SHADER_3D_SKINPOSITION_BUMPEDNORMAL_TEXTURE = "Shader3DSkinPositionBumpedNormalTexture";
const char* SHADER_3D_PARTICLE_COLOR                    = "Shader3DParticleColor";
const char* SHADER_3D_PARTICLE_TEXTURE                  = "Shader3DParticleTexture";
const char* SHADER_3D_SKYBOX                            = "Shader3DSkybox";
const char* SHADER_3D_TERRAIN                           = "Shader3DTerrain";
const char* SHADER_CAMERA_CLEAR                         = "ShaderCameraClear";
const char* SHADER_LAYER_RADIAL_GRADIENT                = "ShaderLayerRadialGradient";

}  // namespace shader

namespace uniform
{
// uniform names
const char* UNIFORM_NAME_AMBIENT_COLOR        = "AX_AmbientColor";
const char* UNIFORM_NAME_P_MATRIX             = "AX_PMatrix";
const char* UNIFORM_NAME_MULTIVIEW_P_MATRIX   = "AX_MultiViewPMatrix";
const char* UNIFORM_NAME_MV_MATRIX            = "AX_MVMatrix";
const char* UNIFORM_NAME_MVP_MATRIX           = "AX_MVPMatrix";
const char* UNIFORM_NAME_MULTIVIEW_MVP_MATRIX = "AX_MultiViewMVPMatrix";
const char* UNIFORM_NAME_NORMAL_MATRIX        = "AX_NormalMatrix";
const char* UNIFORM_NAME_TIME                 = "AX_Time";
const char* UNIFORM_NAME_SIN_TIME             = "AX_SinTime";
const char* UNIFORM_NAME_COS_TIME             = "AX_CosTime";
const char* UNIFORM_NAME_RANDOM01             = "AX_Random01";
const char* UNIFORM_NAME_SAMPLER0             = "AX_Texture0";
const char* UNIFORM_NAME_SAMPLER1             = "AX_Texture1";
const char* UNIFORM_NAME_SAMPLER2             = "AX_Texture2";
const char* UNIFORM_NAME_SAMPLER3             = "AX_Texture3";
const char* UNIFORM_NAME_ALPHA_TEST_VALUE     = "AX_alpha_value";
}  // namespace uniform

namespace attribute
{
// Attribute names
const char* ATTRIBUTE_NAME_COLOR        = "a_color";
const char* ATTRIBUTE_NAME_POSITION     = "a_position";
const char* ATTRIBUTE_NAME_TEX_COORD    = "a_texCoord";
const char* ATTRIBUTE_NAME_TEX_COORD1   = "a_texCoord1";
const char* ATTRIBUTE_NAME_TEX_COORD2   = "a_texCoord2";
const char* ATTRIBUTE_NAME_TEX_COORD3   = "a_texCoord3";
const char* ATTRIBUTE_NAME_NORMAL       = "a_normal";
const char* ATTRIBUTE_NAME_BLEND_WEIGHT = "a_blendWeight";
const char* ATTRIBUTE_NAME_BLEND_INDEX  = "a_blendIndex";
const char* ATTRIBUTE_NAME_TANGENT      = "a_tangent";
const char* ATTRIBUTE_NAME_BINORMAL     = "a_binormal";
}  // namespace attribute

static std::vector<std::pair<const char*, VertexKey>> locations = {
    {attribute::ATTRIBUTE_NAME_POSITION, VertexKey::VERTEX_ATTRIB_POSITION},
    {attribute::ATTRIBUTE_NAME_COLOR, VertexKey::VERTEX_ATTRIB_COLOR},
    {attribute::ATTRIBUTE_NAME_TEX_COORD, VertexKey::VERTEX_ATTRIB_TEX_COORD},
    {attribute::ATTRIBUTE_NAME_TEX_COORD1, VertexKey::VERTEX_ATTRIB_TEX_COORD1},
    {attribute::ATTRIBUTE_NAME_TEX_COORD2, VertexKey::VERTEX_ATTRIB_TEX_COORD2},
    {attribute::ATTRIBUTE_NAME_TEX_COORD3, VertexKey::VERTEX_ATTRIB_TEX_COORD3},
    {attribute::ATTRIBUTE_NAME_NORMAL, VertexKey::VERTEX_ATTRIB_NORMAL},
};

const std::vector<std::pair<const char*, VertexKey>>& getPredefinedAttributes()
{
    return locations;
}

const std::string getAttributeName(const VertexKey& key)
{
    static std::string s_attributeNames[] = {
        shaderinfos::attribute::ATTRIBUTE_NAME_POSITION,    shaderinfos::attribute::ATTRIBUTE_NAME_COLOR,
        shaderinfos::attribute::ATTRIBUTE_NAME_TEX_COORD,   shaderinfos::attribute::ATTRIBUTE_NAME_TEX_COORD1,
        shaderinfos::attribute::ATTRIBUTE_NAME_TEX_COORD2,  shaderinfos::attribute::ATTRIBUTE_NAME_TEX_COORD3,
        shaderinfos::attribute::ATTRIBUTE_NAME_NORMAL,      shaderinfos::attribute::ATTRIBUTE_NAME_BLEND_WEIGHT,
        shaderinfos::attribute::ATTRIBUTE_NAME_BLEND_INDEX, shaderinfos::attribute::ATTRIBUTE_NAME_TANGENT,
        shaderinfos::attribute::ATTRIBUTE_NAME_BINORMAL};

    static int max = sizeof(s_attributeNames) / sizeof(s_attributeNames[0]);
    auto idx       = static_cast<int>(key);
    AXASSERT(idx >= 0 && idx < max, "invalid key ");
    return s_attributeNames[idx];
}
};  // namespace shaderinfos

}