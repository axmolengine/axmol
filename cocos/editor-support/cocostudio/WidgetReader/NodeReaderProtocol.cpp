//
//  NodeReaderProtocol.cpp
//  cocos2d_libs
// 
//  Created by pipu on 14/11/18.
//
//

#include "editor-support/cocostudio/WidgetReader/NodeReaderProtocol.h"
#include "editor-support/cocostudio/CSParseBinary_generated.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "editor-support/cocostudio/CCArmature.h"
#include "editor-support/cocostudio/ActionTimeline/CCSkeletonNode.h"

// x-studio365 spec, csb batch load support, assets hook functions.
static bool onLoadObjectAssetDummy(cocos2d::Node*, cocos2d::ResourceData& assets, int index)
{
    return false;
}

//static cocos2d::Node* aNodeDef() {
//}
//static cocos2d::Sprite* aSpriteDef() {
//}
//static cocos2d::ui::ImageView* aImageViewDef() {
//}
//static cocos2d::ui::Button* aButtonDef() {
//}
//static cocos2d::ui::CheckBox* CheckBoxDef() {
//}
//static cocos2d::ui::Slider* aSliderDef() {
//}
//static cocos2d::ui::LoadingBar* aLoadingBarDef() {
//}
//static cocos2d::ui::Text* aTextDef() {
//}
//static cocos2d::ui::TextField* aTextFieldDef() {
//}
//static cocos2d::ui::TextAtlas* aTextAtlasDef() {
//}
//static cocos2d::ui::TextBMFont* aTextBMFontDef() {
//}
//static cocos2d::ui::Layout* aLayoutDef() {
//}
//static cocos2d::ui::ScrollView* aScrollViewDef() {
//}
//static cocos2d::ui::ListView* aListViewDef() {
//}
//static cocos2d::ui::PageView* aPageViewDef() {
//}
//static cocos2d::ParticleSystemQuad* aParticleSystemQuadDef() {
//}

template<typename _T>
static _T* object_create_func()
{
    return _T::create();
}

static cocos2d::Node* createArmatureNode()
{
    return cocostudio::Armature::create();
}

static cocos2d::ParticleSystemQuad* createParticleSystemQuad(const std::string& path) {
    return cocos2d::ParticleSystemQuad::create(path);
}

static cocos2d::Node* createNestingNode(std::string) {
    return cocos2d::Node::create();
}

static void onLoadSpriteFramesWithFileDummy(std::string&)
{
}

namespace cocos2d {
    namespace wext {
        bool(*onBeforeLoadObjectAsset)(cocos2d::Node*, cocos2d::ResourceData& assets, int index/*= 0*/) = &onLoadObjectAssetDummy;
        bool(*onAfterLoadObjectAsset)(cocos2d::Node*, cocos2d::ResourceData& assets, int index/*= 0*/) = &onLoadObjectAssetDummy;
        void(*onLoadSpriteFramesWithFile)(std::string& file) = nullptr;
        void(*onNestingNodeLoading)(const std::string& filePath) = nullptr;
        void(*onNestingNodeLoaded)(cocos2d::Node*, const std::string& filePath) = nullptr;
        cocos2d::Node*(*aNode)();
        cocos2d::ui::Widget*(*aWidget)();
        cocos2d::Sprite*(*aSprite)();
        cocos2d::ui::ImageView*(*aImageView)();
        cocos2d::ui::Button*(*aButton)();
        cocos2d::ui::CheckBox*(*aCheckBox)();
        cocos2d::ui::Slider*(*aSlider)();
        cocos2d::ui::LoadingBar*(*aLoadingBar)();
        cocos2d::ui::Text*(*aText)();
        cocos2d::ui::TextField*(*aTextField)();
        cocos2d::ui::TextAtlas*(*aTextAtlas)();
        cocos2d::ui::TextBMFont*(*aTextBMFont)();
        cocos2d::ui::Layout*(*aLayout)();
        cocos2d::ui::ScrollView*(*aScrollView)();
        cocos2d::ui::ListView*(*aListView)();
        cocos2d::ui::PageView*(*aPageView)();
        cocos2d::ParticleSystemQuad*(*aParticleSystemQuad)(const std::string&);
        cocos2d::Node*(*aArmatureNode)();
        cocostudio::timeline::SkeletonNode*(*aSkeletonNode)();
        cocostudio::timeline::BoneNode*(*aBoneNode)();
        cocos2d::Node*(*aNestingNode)(std::string);

        void resetReaderAllHooks()
        {
            onLoadSpriteFramesWithFile = onLoadSpriteFramesWithFileDummy;
            onBeforeLoadObjectAsset = onLoadObjectAssetDummy;
            onAfterLoadObjectAsset = onLoadObjectAssetDummy;
            onNestingNodeLoaded = nullptr;
            onNestingNodeLoading = nullptr;

            aNode = object_create_func<Node>;
            aSprite = object_create_func<Sprite>;
            aWidget = object_create_func<ui::Widget>;
            aImageView = object_create_func<ui::ImageView>;
            aButton = object_create_func<ui::Button>;
            aCheckBox = object_create_func<ui::CheckBox>;
            aSlider = object_create_func<ui::Slider>;
            aLoadingBar = object_create_func<ui::LoadingBar>;
            aText = object_create_func<ui::Text>;
            aTextField = object_create_func<ui::TextField>;
            aTextAtlas = object_create_func<ui::TextAtlas>;
            aTextBMFont = object_create_func<ui::TextBMFont>;
            aLayout = object_create_func<ui::Layout>;
            aScrollView = object_create_func<ui::ScrollView>;
            aListView = object_create_func<ui::ListView>;
            aPageView = object_create_func<ui::PageView>;
            aSkeletonNode = object_create_func<cocostudio::timeline::SkeletonNode>;
            aBoneNode = object_create_func<cocostudio::timeline::BoneNode>;
            aArmatureNode = createArmatureNode;
            aParticleSystemQuad = &createParticleSystemQuad;
            aNestingNode = createNestingNode;
        }

        static uint8_t _AUTO_INIT_VARS = (resetReaderAllHooks(), 0);
    };
};

cocos2d::ResourceData cocos2d::wext::makeResourceData(const flatbuffers::ResourceData* orig)
{
    cocos2d::ResourceData fileData;
    fileData.file = orig->path()->c_str();
    fileData.plist = orig->plistFile()->c_str();
    fileData.type = orig->resourceType();
    return fileData;
}

cocos2d::ResourceData cocos2d::wext::makeResourceData(const std::string& path, int type)
{
    cocos2d::ResourceData fileData;
    fileData.file = path;
    fileData.type = type;
    return fileData;
}

cocos2d::ResourceData cocos2d::wext::makeResourceData(std::string&& path, int type)
{
    cocos2d::ResourceData fileData;
    fileData.file = std::move(path);
    fileData.type = type;
    return fileData;
}
