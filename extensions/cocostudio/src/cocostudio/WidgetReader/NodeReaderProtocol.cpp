//
//  NodeReaderProtocol.cpp
//  cocos2d_libs
//
//  Created by pipu on 14/11/18.
//
//

#include "NodeReaderProtocol.h"
#include "cocostudio/CSParseBinary_generated.h"  // flatbuffers::ResourceData, it's ok for all revision of loaders
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/Armature.h"
#include "cocostudio/ActionTimeline/SkeletonNode.h"

// x-studio spec, csb batch load support, assets hook functions.
static bool onLoadObjectAssetDummy(ax::Node*, ax::ResourceData& assets, int index)
{
    return false;
}

template <typename _T>
static _T* object_create_func()
{
    return _T::create();
}

static ax::Node* createArmatureNode()
{
    return cocostudio::Armature::create();
}

static ax::ParticleSystemQuad* createParticleSystemQuad(std::string_view path)
{
    return ax::ParticleSystemQuad::create(path);
}

static ax::Node* createNestingNode(std::string)
{
    return ax::Node::create();
}

static void onLoadSpriteFramesWithFileDummy(std::string&) {}

namespace cocostudio
{
NodeReaderProtocol::NodeReaderProtocol(){};
NodeReaderProtocol::~NodeReaderProtocol(){};

void NodeReaderProtocol::setCurrentCustomClassName(const char* className){};
}  // namespace cocostudio

namespace ax
{
namespace wext
{
bool (*onBeforeLoadObjectAsset)(ax::Node*,
                                ax::ResourceData& assets,
                                int index /*= 0*/)                     = &onLoadObjectAssetDummy;
bool (*onAfterLoadObjectAsset)(ax::Node*,
                               ax::ResourceData& assets,
                               int index /*= 0*/)                      = &onLoadObjectAssetDummy;
void (*onLoadSpriteFramesWithFile)(std::string& file)                  = nullptr;
void (*onNestingNodeLoading)(std::string_view filePath)                = nullptr;
void (*onNestingNodeLoaded)(ax::Node*, std::string_view filePath) = nullptr;
ax::Node* (*aNode)();
ax::ui::Widget* (*aWidget)();
ax::Sprite* (*aSprite)();
ax::ui::ImageView* (*aImageView)();
ax::ui::Button* (*aButton)();
ax::ui::CheckBox* (*aCheckBox)();
ax::ui::Slider* (*aSlider)();
ax::ui::LoadingBar* (*aLoadingBar)();
ax::ui::Text* (*aText)();
ax::ui::TextField* (*aTextField)();
ax::ui::TextAtlas* (*aTextAtlas)();
ax::ui::TextBMFont* (*aTextBMFont)();
ax::ui::Layout* (*aLayout)();
ax::ui::ScrollView* (*aScrollView)();
ax::ui::ListView* (*aListView)();
ax::ui::PageView* (*aPageView)();
ax::ParticleSystemQuad* (*aParticleSystemQuad)(std::string_view);
ax::Node* (*aArmatureNode)();
cocostudio::timeline::SkeletonNode* (*aSkeletonNode)();
cocostudio::timeline::BoneNode* (*aBoneNode)();
ax::Node* (*aNestingNode)(std::string);

// 3d stubs
ax::Node* (*aNode3D)();
ax::Node* (*aGameNode3D)();
ax::Node* (*aLight3D)();
ax::Camera* (*aCamera)();
ax::MeshRenderer* (*aSprite3D)();
ax::Node* (*aParticleSystem3D)();

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

} // namespace ax

ax::ResourceData ax::wext::makeResourceData(const flatbuffers::ResourceData* orig)
{
    ax::ResourceData fileData;
    fileData.file  = orig->path()->c_str();
    fileData.plist = orig->plistFile()->c_str();
    fileData.type  = orig->resourceType();
    return fileData;
}

ax::ResourceData ax::wext::makeResourceData(std::string_view path, int type)
{
    ax::ResourceData fileData;
    fileData.file = path;
    fileData.type = type;
    return fileData;
}

ax::ResourceData ax::wext::makeResourceData(std::string&& path, int type)
{
    ax::ResourceData fileData;
    fileData.file = std::move(path);
    fileData.type = type;
    return fileData;
}
