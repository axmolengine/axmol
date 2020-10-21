/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
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

#include "CocosStudioExport.h"
#include "CocosStudioExtension.h"
#include "ui/GUIDefine.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

#define CCS_DEF_FONT "Microsoft YaHei"

namespace flatbuffers
{
    class FlatBufferBuilder;
    template<typename T> struct Offset;
    
    class Table;

    struct ResourceData; // x-studio spec, csb batch load support, assets hook functions.
}

namespace cocos2d
{
    class Node;
}

#if !defined(GL_ONE)
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_COLOR 0x0300
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_DST_ALPHA 0x0304
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_DST_COLOR 0x0306
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_NONE 0
#endif

// V4 blendFunc not GL compatible only
struct GLBlendFunc {
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
        virtual flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                                     flatbuffers::FlatBufferBuilder* builder) = 0;
        virtual void setPropsWithFlatBuffers(cocos2d::Node* node, const flatbuffers::Table* nodeOptions) = 0;
        virtual cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) = 0;
    };
}

// x-studio spec, csb batch load support, assets hook functions.

namespace cocostudio
{
    class Armature;
    namespace timeline {
        class SkeletonNode;
        class BoneNode;
    }
};

namespace cocos2d {

    // builtin ui predecl
    class Sprite;
    class Camera;
    class Sprite3D;
    class PUParticleSystem3D;
    class ParticleSystemQuad;
    namespace ui {
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
    }
    typedef void(*APP_LOGERROR_FUNC)(const char* pszFormat, ...);
    namespace wext {
        // engine extends APIs
         // TODO-2020
        CC_DLL extern APP_LOGERROR_FUNC getAppErrorLogFunc();

		CCS_DLL extern void(*onLoadSpriteFramesWithFile)(std::string& file);

        CCS_DLL extern cocos2d::ResourceData makeResourceData(const flatbuffers::ResourceData* data);
		CCS_DLL extern cocos2d::ResourceData makeResourceData(const std::string& path, int type = 0);
		CCS_DLL extern cocos2d::ResourceData makeResourceData(std::string&& path, int type = 0);
		CCS_DLL extern void resetReaderAllHooks();

        /// Assets Hooks
		CCS_DLL extern bool(*onBeforeLoadObjectAsset)(cocos2d::Node*, cocos2d::ResourceData& assets, int index/*= 0*/);

        // Object creator Hooks
		CCS_DLL extern cocos2d::Node*(*aNode)();
        CCS_DLL extern cocos2d::ui::Widget*(*aWidget)();
		CCS_DLL extern cocos2d::Sprite*(*aSprite)();
		CCS_DLL extern cocos2d::ui::ImageView*(*aImageView)();
        CCS_DLL extern cocos2d::ui::Button*(*aButton)();
        CCS_DLL extern cocos2d::ui::CheckBox*(*aCheckBox)();
        CCS_DLL extern cocos2d::ui::Slider*(*aSlider)();
        CCS_DLL extern cocos2d::ui::LoadingBar*(*aLoadingBar)();
        CCS_DLL extern cocos2d::ui::Text*(*aText)();
        CCS_DLL extern cocos2d::ui::TextField*(*aTextField)();
        CCS_DLL extern cocos2d::ui::TextAtlas*(*aTextAtlas)();
        CCS_DLL extern cocos2d::ui::TextBMFont*(*aTextBMFont)();
        CCS_DLL extern cocos2d::ui::Layout*(*aLayout)();
        CCS_DLL extern cocos2d::ui::ScrollView*(*aScrollView)();
        CCS_DLL extern cocos2d::ui::ListView*(*aListView)();
        CCS_DLL extern cocos2d::ui::PageView*(*aPageView)();
        CCS_DLL extern cocos2d::Node*(*aArmatureNode)();
        CCS_DLL extern cocostudio::timeline::SkeletonNode*(*aSkeletonNode)();
        CCS_DLL extern cocostudio::timeline::BoneNode*(*aBoneNode)();
        CCS_DLL extern cocos2d::ParticleSystemQuad*(*aParticleSystemQuad)(const std::string&);
        CCS_DLL extern cocos2d::Node*(*aNestingNode)(std::string);

        ///// 3d objects /////
        CCS_DLL extern cocos2d::Node* (*aNode3D)();
        CCS_DLL extern cocos2d::Node* (*aGameNode3D)();
        CCS_DLL extern cocos2d::Node* (*aLight3D)();
        CCS_DLL extern cocos2d::Camera* (*aCamera)();
        CCS_DLL extern cocos2d::Sprite3D* (*aSprite3D)();
        CCS_DLL extern cocos2d::Node* (*aParticleSystem3D)();
    };
};

#endif /* defined(__cocos2d_libs__NodeReaderProtocol__) */
