//
//  NodeReaderProtocol.cpp
//  cocos2d_libs
//
//  Created by pipu on 14/11/18.
//
//

#include "WidgetReader/NodeReaderProtocol.h"
#include "CSParseBinary_generated.h"  // flatbuffers::ResourceData, it's ok for all revision of loaders
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CCArmature.h"
#include "ActionTimeline/CCSkeletonNode.h"

// x-studio spec, csb batch load support, assets hook functions.
static bool onLoadObjectAssetDummy(axis::Node*, axis::ResourceData& assets, int index)
{
    return false;
}

template <typename _T>
static _T* object_create_func()
{
    return _T::create();
}

static axis::Node* createArmatureNode()
{
    return cocostudio::Armature::create();
}

static axis::ParticleSystemQuad* createParticleSystemQuad(std::string_view path)
{
    return axis::ParticleSystemQuad::create(path);
}

static axis::Node* createNestingNode(std::string)
{
    return axis::Node::create();
}

static void onLoadSpriteFramesWithFileDummy(std::string&) {}

namespace cocostudio
{
NodeReaderProtocol::NodeReaderProtocol(){};
NodeReaderProtocol::~NodeReaderProtocol(){};

void NodeReaderProtocol::setCurrentCustomClassName(const char* className){};
}  // namespace cocostudio

NS_AX_BEGIN
namespace wext
{
bool (*onBeforeLoadObjectAsset)(axis::Node*,
                                axis::ResourceData& assets,
                                int index /*= 0*/)                     = &onLoadObjectAssetDummy;
bool (*onAfterLoadObjectAsset)(axis::Node*,
                               axis::ResourceData& assets,
                               int index /*= 0*/)                      = &onLoadObjectAssetDummy;
void (*onLoadSpriteFramesWithFile)(std::string& file)                  = nullptr;
void (*onNestingNodeLoading)(std::string_view filePath)                = nullptr;
void (*onNestingNodeLoaded)(axis::Node*, std::string_view filePath) = nullptr;
axis::Node* (*aNode)();
axis::ui::Widget* (*aWidget)();
axis::Sprite* (*aSprite)();
axis::ui::ImageView* (*aImageView)();
axis::ui::Button* (*aButton)();
axis::ui::CheckBox* (*aCheckBox)();
axis::ui::Slider* (*aSlider)();
axis::ui::LoadingBar* (*aLoadingBar)();
axis::ui::Text* (*aText)();
axis::ui::TextField* (*aTextField)();
axis::ui::TextAtlas* (*aTextAtlas)();
axis::ui::TextBMFont* (*aTextBMFont)();
axis::ui::Layout* (*aLayout)();
axis::ui::ScrollView* (*aScrollView)();
axis::ui::ListView* (*aListView)();
axis::ui::PageView* (*aPageView)();
axis::ParticleSystemQuad* (*aParticleSystemQuad)(std::string_view);
axis::Node* (*aArmatureNode)();
cocostudio::timeline::SkeletonNode* (*aSkeletonNode)();
cocostudio::timeline::BoneNode* (*aBoneNode)();
axis::Node* (*aNestingNode)(std::string);

// 3d stubs
axis::Node* (*aNode3D)();
axis::Node* (*aGameNode3D)();
axis::Node* (*aLight3D)();
axis::Camera* (*aCamera)();
axis::MeshRenderer* (*aSprite3D)();
axis::Node* (*aParticleSystem3D)();

void resetReaderAllHooks()
{
    onLoadSpriteFramesWithFile = onLoadSpriteFramesWithFileDummy;
    onBeforeLoadObjectAsset    = onLoadObjectAssetDummy;
    onAfterLoadObjectAsset     = onLoadObjectAssetDummy;
    onNestingNodeLoaded        = nullptr;
    onNestingNodeLoading       = nullptr;

    aNode               = object_create_func<Node>;
    aSprite             = object_create_func<Sprite>;
    aWidget             = object_create_func<ui::Widget>;
    aImageView          = object_create_func<ui::ImageView>;
    aButton             = object_create_func<ui::Button>;
    aCheckBox           = object_create_func<ui::CheckBox>;
    aSlider             = object_create_func<ui::Slider>;
    aLoadingBar         = object_create_func<ui::LoadingBar>;
    aText               = object_create_func<ui::Text>;
    aTextField          = object_create_func<ui::TextField>;
    aTextAtlas          = object_create_func<ui::TextAtlas>;
    aTextBMFont         = object_create_func<ui::TextBMFont>;
    aLayout             = object_create_func<ui::Layout>;
    aScrollView         = object_create_func<ui::ScrollView>;
    aListView           = object_create_func<ui::ListView>;
    aPageView           = object_create_func<ui::PageView>;
    aSkeletonNode       = object_create_func<cocostudio::timeline::SkeletonNode>;
    aBoneNode           = object_create_func<cocostudio::timeline::BoneNode>;
    aArmatureNode       = createArmatureNode;
    aParticleSystemQuad = &createParticleSystemQuad;
    aNestingNode        = createNestingNode;
}

static uint8_t _AUTO_INIT_VARS = (resetReaderAllHooks(), 0);
} // namespace wext

NS_AX_END // namespace axis

axis::ResourceData axis::wext::makeResourceData(const flatbuffers::ResourceData* orig)
{
    axis::ResourceData fileData;
    fileData.file  = orig->path()->c_str();
    fileData.plist = orig->plistFile()->c_str();
    fileData.type  = orig->resourceType();
    return fileData;
}

axis::ResourceData axis::wext::makeResourceData(std::string_view path, int type)
{
    axis::ResourceData fileData;
    fileData.file = path;
    fileData.type = type;
    return fileData;
}

axis::ResourceData axis::wext::makeResourceData(std::string&& path, int type)
{
    axis::ResourceData fileData;
    fileData.file = std::move(path);
    fileData.type = type;
    return fileData;
}
