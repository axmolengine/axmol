/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/SceneExtMacros.h"
#include "axmol/ui/GUIDefine.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

#include "flatbuffers/flatbuffer_builder.h"

#define CCS_DEF_FONT "Microsoft YaHei"

namespace flatbuffers
{
template <typename T>
struct Offset;

class Table;

struct ResourceData;  // x-studio spec, csb batch load support, assets hook functions.
}  // namespace flatbuffers

namespace ax
{
class Node;
}

#if !defined(GL_ONE)
#    define GL_ZERO                0
#    define GL_ONE                 1
#    define GL_SRC_COLOR           0x0300
#    define GL_ONE_MINUS_SRC_COLOR 0x0301
#    define GL_SRC_ALPHA           0x0302
#    define GL_ONE_MINUS_SRC_ALPHA 0x0303
#    define GL_DST_ALPHA           0x0304
#    define GL_ONE_MINUS_DST_ALPHA 0x0305
#    define GL_DST_COLOR           0x0306
#    define GL_ONE_MINUS_DST_COLOR 0x0307
#    define GL_SRC_ALPHA_SATURATE  0x0308
#    define GL_NONE                0
#endif

// V4 blendFunc not GL compatible only
struct GLBlendFunc
{
    int src = GL_ONE;
    int dst = GL_ONE_MINUS_SRC_ALPHA;
};

namespace ax::ext
{
class SCNEXT_API NodeReaderProtocol
{
public:
    NodeReaderProtocol();
    virtual ~NodeReaderProtocol();

    virtual void setCurrentCustomClassName(const char* className);
    virtual flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(
        pugi::xml_node objectData,
        flatbuffers::FlatBufferBuilder* builder)                                                = 0;
    virtual void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOptions) = 0;
    virtual ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)          = 0;
};
}  // namespace ax::ext

// x-studio spec, csb batch load support, assets hook functions.

namespace ax::ext
{
class Armature;
namespace timeline
{
class SkeletonNode;
class BoneNode;
}  // namespace timeline
};  // namespace ax::ext

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
class TextAtlas;
class TextBMFont;

class LayoutGroup;
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

SCNEXT_API extern void (*onLoadSpriteFramesWithFile)(std::string& file);

SCNEXT_API extern ax::ResourceData makeResourceData(const flatbuffers::ResourceData* data);
SCNEXT_API extern ax::ResourceData makeResourceData(std::string_view path, int type = 0);
SCNEXT_API extern void resetReaderAllHooks();

/// Assets Hooks
SCNEXT_API extern bool (*onBeforeLoadObjectAsset)(ax::Node*, ax::ResourceData& assets, int index /*= 0*/);
SCNEXT_API extern void (*onSetPropsEnded)(ax::Node*);  // add 2021

// Object creator Hooks
SCNEXT_API extern ax::Node* (*aNode)();
SCNEXT_API extern ax::ui::Widget* (*aWidget)();
SCNEXT_API extern ax::Sprite* (*aSprite)();
SCNEXT_API extern ax::ui::ImageView* (*aImageView)();
SCNEXT_API extern ax::ui::Button* (*aButton)();
SCNEXT_API extern ax::ui::CheckBox* (*aCheckBox)();
SCNEXT_API extern ax::ui::Slider* (*aSlider)();
SCNEXT_API extern ax::ui::LoadingBar* (*aLoadingBar)();
SCNEXT_API extern ax::ui::Text* (*aText)();
SCNEXT_API extern ax::ui::TextAtlas* (*aTextAtlas)();
SCNEXT_API extern ax::ui::TextBMFont* (*aTextBMFont)();
SCNEXT_API extern ax::ui::LayoutGroup* (*aLayout)();
SCNEXT_API extern ax::ui::ScrollView* (*aScrollView)();
SCNEXT_API extern ax::ui::ListView* (*aListView)();
SCNEXT_API extern ax::ui::PageView* (*aPageView)();
SCNEXT_API extern ax::Node* (*aArmatureNode)();
SCNEXT_API extern ax::ext::timeline::SkeletonNode* (*aSkeletonNode)();
SCNEXT_API extern ax::ext::timeline::BoneNode* (*aBoneNode)();
SCNEXT_API extern ax::ParticleSystemQuad* (*aParticleSystemQuad)(std::string_view);
SCNEXT_API extern ax::Node* (*aNestingNode)(std::string);

///// 3d objects /////
SCNEXT_API extern ax::Node* (*aNode3D)();
SCNEXT_API extern ax::Node* (*aGameNode3D)();
SCNEXT_API extern ax::Node* (*aLight3D)();
SCNEXT_API extern ax::Camera* (*aCamera)();
SCNEXT_API extern ax::MeshRenderer* (*aSprite3D)();
SCNEXT_API extern ax::Node* (*aParticleSystem3D)();
}  // namespace wext
}  // namespace ax
