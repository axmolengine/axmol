/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __cocos2d_libs__NodeReaderProtocol__
#define __cocos2d_libs__NodeReaderProtocol__

#include "cocostudio/CocosStudioExport.h"
#include "cocostudio/CocosStudioExtension.h"
#include "ui/GUIDefine.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

#include "flatbuffers/flatbuffers.h"

#define CCS_DEF_FONT "Microsoft YaHei"

namespace flatbuffers
{
struct ResourceData;  // x-studio spec, csb batch load support, assets hook functions.
}  // namespace flatbuffers

namespace ax
{
class Node;
}

#if !defined(GL_ONE)
#    define GL_ZERO 0
#    define GL_ONE 1
#    define GL_SRC_COLOR 0x0300
#    define GL_ONE_MINUS_SRC_COLOR 0x0301
#    define GL_SRC_ALPHA 0x0302
#    define GL_ONE_MINUS_SRC_ALPHA 0x0303
#    define GL_DST_ALPHA 0x0304
#    define GL_ONE_MINUS_DST_ALPHA 0x0305
#    define GL_DST_COLOR 0x0306
#    define GL_ONE_MINUS_DST_COLOR 0x0307
#    define GL_SRC_ALPHA_SATURATE 0x0308
#    define GL_NONE 0
#endif

// V4 blendFunc not GL compatible only
struct GLBlendFunc
{
    int src = GL_ONE;
    int dst = GL_ONE_MINUS_SRC_ALPHA;
};

namespace cocostudio
{
class CCS_DLL NodeReaderProtocol
{
public:
    NodeReaderProtocol();
    virtual ~NodeReaderProtocol();

    virtual void setCurrentCustomClassName(const char* className);
    virtual flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder)                                                     = 0;
    virtual void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOptions) = 0;
    virtual ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)          = 0;
};
}  // namespace cocostudio

// x-studio spec, csb batch load support, assets hook functions.

namespace cocostudio
{
class Armature;
namespace timeline
{
class SkeletonNode;
class BoneNode;
}  // namespace timeline
};  // namespace cocostudio

namespace ax
{

// builtin ui predecl
class Sprite;
class Camera;
class MeshRenderer;
class PUParticleSystem3D;
class ParticleSystemQuad;
namespace ui
{
class ImageView;

class Button;
class CheckBox;
class Slider;
class LoadingBar;

class Text;
class TextField;
class TextAtlas;
class TextBMFont;

class Layout;
class ScrollView;
class ListView;
class PageView;

class Widget;
}  // namespace ui
typedef void (*APP_LOGERROR_FUNC)(const char* pszFormat, ...);
namespace wext
{
// engine extends APIs
// TODO-2020
AX_DLL extern APP_LOGERROR_FUNC getAppErrorLogFunc();

CCS_DLL extern void (*onLoadSpriteFramesWithFile)(std::string& file);

CCS_DLL extern ax::ResourceData makeResourceData(const flatbuffers::ResourceData* data);
CCS_DLL extern ax::ResourceData makeResourceData(std::string_view path, int type = 0);
CCS_DLL extern ax::ResourceData makeResourceData(std::string&& path, int type = 0);
CCS_DLL extern void resetReaderAllHooks();

/// Assets Hooks
CCS_DLL extern bool (*onBeforeLoadObjectAsset)(ax::Node*, ax::ResourceData& assets, int index /*= 0*/);

// Object creator Hooks
CCS_DLL extern ax::Node* (*aNode)();
CCS_DLL extern ax::ui::Widget* (*aWidget)();
CCS_DLL extern ax::Sprite* (*aSprite)();
CCS_DLL extern ax::ui::ImageView* (*aImageView)();
CCS_DLL extern ax::ui::Button* (*aButton)();
CCS_DLL extern ax::ui::CheckBox* (*aCheckBox)();
CCS_DLL extern ax::ui::Slider* (*aSlider)();
CCS_DLL extern ax::ui::LoadingBar* (*aLoadingBar)();
CCS_DLL extern ax::ui::Text* (*aText)();
CCS_DLL extern ax::ui::TextField* (*aTextField)();
CCS_DLL extern ax::ui::TextAtlas* (*aTextAtlas)();
CCS_DLL extern ax::ui::TextBMFont* (*aTextBMFont)();
CCS_DLL extern ax::ui::Layout* (*aLayout)();
CCS_DLL extern ax::ui::ScrollView* (*aScrollView)();
CCS_DLL extern ax::ui::ListView* (*aListView)();
CCS_DLL extern ax::ui::PageView* (*aPageView)();
CCS_DLL extern ax::Node* (*aArmatureNode)();
CCS_DLL extern cocostudio::timeline::SkeletonNode* (*aSkeletonNode)();
CCS_DLL extern cocostudio::timeline::BoneNode* (*aBoneNode)();
CCS_DLL extern ax::ParticleSystemQuad* (*aParticleSystemQuad)(std::string_view);
CCS_DLL extern ax::Node* (*aNestingNode)(std::string);

///// 3d objects /////
CCS_DLL extern ax::Node* (*aNode3D)();
CCS_DLL extern ax::Node* (*aGameNode3D)();
CCS_DLL extern ax::Node* (*aLight3D)();
CCS_DLL extern ax::Camera* (*aCamera)();
CCS_DLL extern ax::MeshRenderer* (*aSprite3D)();
CCS_DLL extern ax::Node* (*aParticleSystem3D)();
}  // namespace wext

}  // namespace ax

#endif /* defined(__cocos2d_libs__NodeReaderProtocol__) */
