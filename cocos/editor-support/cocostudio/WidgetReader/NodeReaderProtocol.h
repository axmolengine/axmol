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

#include "editor-support/cocostudio/CocosStudioExport.h"
#include "editor-support/cocostudio/CocosStudioExtension.h"

#include "pugixml/pugixml_imp.hpp"
#include "pugixml/pugiext.hpp"

namespace flatbuffers
{
    class FlatBufferBuilder;
    template<typename T> struct Offset;
    
    class Table;

    struct ResourceData; // x-studio365 spec, csb batch load support, assets hook functions.
}

namespace cocos2d
{
    class Node;
}

namespace cocostudio
{
    class CC_STUDIOP_DLL NodeReaderProtocol
    {
    public:
        NodeReaderProtocol() {};
        virtual ~NodeReaderProtocol() {};
        
        virtual void setCurrentCustomClassName(const char* className) {};
        virtual flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                                     flatbuffers::FlatBufferBuilder* builder) = 0;
        virtual void setPropsWithFlatBuffers(cocos2d::Node* node, const flatbuffers::Table* nodeOptions) = 0;
        virtual cocos2d::Node* createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions) = 0;
    };
}

// x-studio365 spec, csb batch load support, assets hook functions.

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
        CC_DLL extern APP_LOGERROR_FUNC getAppErrorLogFunc();

		CC_STUDIOP_DLL extern void(*onLoadSpriteFramesWithFile)(std::string& file);

		CC_STUDIOP_DLL extern cocos2d::ResourceData makeResourceData(const flatbuffers::ResourceData* data);
		CC_STUDIOP_DLL extern cocos2d::ResourceData makeResourceData(const std::string& path, int type = 0);
		CC_STUDIOP_DLL extern cocos2d::ResourceData makeResourceData(std::string&& path, int type = 0);
		CC_STUDIOP_DLL extern void resetReaderAllHooks();

        /// Assets Hooks
		CC_STUDIOP_DLL extern bool(*onBeforeLoadObjectAsset)(cocos2d::Node*, cocos2d::ResourceData& assets, int index/*= 0*/);
		CC_STUDIOP_DLL extern bool(*onAfterLoadObjectAsset)(cocos2d::Node*, cocos2d::ResourceData& assets, int index/*= 0*/);

        // Object creator Hooks
		CC_STUDIOP_DLL extern cocos2d::Node*(*aNode)();
        CC_STUDIOP_DLL extern cocos2d::ui::Widget*(*aWidget)();
		CC_STUDIOP_DLL extern cocos2d::Sprite*(*aSprite)();
		CC_STUDIOP_DLL extern cocos2d::ui::ImageView*(*aImageView)();
        CC_STUDIOP_DLL extern cocos2d::ui::Button*(*aButton)();
        CC_STUDIOP_DLL extern cocos2d::ui::CheckBox*(*aCheckBox)();
        CC_STUDIOP_DLL extern cocos2d::ui::Slider*(*aSlider)();
        CC_STUDIOP_DLL extern cocos2d::ui::LoadingBar*(*aLoadingBar)();
        CC_STUDIOP_DLL extern cocos2d::ui::Text*(*aText)();
        CC_STUDIOP_DLL extern cocos2d::ui::TextField*(*aTextField)();
        CC_STUDIOP_DLL extern cocos2d::ui::TextAtlas*(*aTextAtlas)();
        CC_STUDIOP_DLL extern cocos2d::ui::TextBMFont*(*aTextBMFont)();
        CC_STUDIOP_DLL extern cocos2d::ui::Layout*(*aLayout)();
        CC_STUDIOP_DLL extern cocos2d::ui::ScrollView*(*aScrollView)();
        CC_STUDIOP_DLL extern cocos2d::ui::ListView*(*aListView)();
        CC_STUDIOP_DLL extern cocos2d::ui::PageView*(*aPageView)();
        CC_STUDIOP_DLL extern cocos2d::Node*(*aArmatureNode)();
        CC_STUDIOP_DLL extern cocostudio::timeline::SkeletonNode*(*aSkeletonNode)();
        CC_STUDIOP_DLL extern cocostudio::timeline::BoneNode*(*aBoneNode)();
        CC_STUDIOP_DLL extern cocos2d::ParticleSystemQuad*(*aParticleSystemQuad)(const std::string&);
        CC_STUDIOP_DLL extern cocos2d::Node*(*aNestingNode)(std::string);
    };
};

#endif /* defined(__cocos2d_libs__NodeReaderProtocol__) */
