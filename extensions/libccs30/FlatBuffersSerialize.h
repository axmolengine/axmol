/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
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

#ifndef __cocos2d_libs__FlatBuffersSerialize__
#define __cocos2d_libs__FlatBuffersSerialize__

#include <string>
#include <vector>

#include "ExtensionMacros.h"
#include "libeditor/CocosStudioExport.h"
#include "platform/CCPlatformMacros.h"
#include "ui/UIWidget.h"

#include "pugixml/pugixml_imp.hpp"

namespace flatbuffers
{
    class FlatBufferBuilder;
    template<typename T> struct Offset;
    struct String;
    
    struct CSParseBinary;
    struct NodeTree;
    
    struct WidgetOptions;
    
    struct SingleNodeOptions;
    struct SpriteOptions;
    struct ParticleSystemOptions;
    struct TMXTiledMapOptions;
    
    struct ButtonOptions;
    struct CheckBoxOptions;
    struct ImageViewOptions;
    struct TextBMFontOptions;
    struct TextOptions;
    struct TextFieldOptions;
    struct LoadingBarOptions;
    struct SliderOptions;
    struct PanelOptions;
    struct ScrollViewOptions;
    struct PageViewOptions;
    struct ListViewOptions;
    
    struct ProjectNodeOptions;
    
    struct ComponentOptions;
    struct ComAudioOptions;
    
    struct TextAtlasOptions;
    
    struct NodeAction;
    struct AnimationInfo;
    struct TimeLine;
    struct Frame;
    struct PointFrame;
    struct ScaleFrame;
    struct ColorFrame;
    struct TextureFrame;
    struct EventFrame;
    struct IntFrame;
    struct BoolFrame;
    struct InnerActionFrame;
    struct EasingData;
    struct BlendFrame;
}

namespace cocostudio {
    
class CCS_LDR_DLL FlatBuffersSerialize
{
    
public:
    static FlatBuffersSerialize* getInstance();
    /** @deprecated Use method destroyInstance() instead */
    CC_DEPRECATED_ATTRIBUTE static void purge();
    static void destroyInstance();
    
    FlatBuffersSerialize();
    ~FlatBuffersSerialize();
    
    void deleteFlatBufferBuilder();
    
    std::string test(const std::string& xmlFileName,
                     const std::string& flatbuffersFileName);
    
    /* serialize flat buffers with XML */
    static std::string serializeFlatBuffersWithXMLFile(const std::string& xmlFileName,
                                                const std::string& flatbuffersFileName);
    /* x-studio spec: serialize flat buffers with XML buffer */
    static std::string serializeFlatBuffersWithXMLBuffer(std::string& xmlBuffer,
        const std::string& flatbuffersFileName);
    static std::string serializeFlatBuffersWithOpaque(void* opaque,
        const std::string& flatbuffersFileName);
    
    // NodeTree
    flatbuffers::Offset<flatbuffers::NodeTree> createNodeTree(pugi::xml_node objectData,
                                                              std::string classType);
    
    // NodeAction
    flatbuffers::Offset<flatbuffers::NodeAction> createNodeAction(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::TimeLine> createTimeLine(pugi::xml_node objectData);    
    flatbuffers::Offset<flatbuffers::PointFrame> createPointFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::ScaleFrame> createScaleFrame(pugi::xml_node objectData);    
    flatbuffers::Offset<flatbuffers::ColorFrame> createColorFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::TextureFrame> createTextureFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::EventFrame> createEventFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::IntFrame> createIntFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::BoolFrame> createBoolFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::InnerActionFrame> createInnerActionFrame(pugi::xml_node objectData);
    flatbuffers::Offset<flatbuffers::BlendFrame> createBlendFrame(pugi::xml_node objectData);
    
    flatbuffers::Offset<flatbuffers::EasingData> createEasingData(pugi::xml_node objectData);

    //Animation Info
    flatbuffers::Offset<flatbuffers::AnimationInfo> createAnimationInfo(pugi::xml_node objectData);
    /**/
    
    int getResourceType(std::string key);
    std::string getGUIClassName(const std::string &name);
    std::string getWidgetReaderClassName(cocos2d::ui::Widget *widget);
    
    /* create flat buffers with XML */
    flatbuffers::FlatBufferBuilder* createFlatBuffersWithXMLFileForSimulator(const std::string& xmlFileName);
    flatbuffers::Offset<flatbuffers::NodeTree> createNodeTreeForSimulator(pugi::xml_node objectData,
                                                                          std::string classType);
    flatbuffers::Offset<flatbuffers::ProjectNodeOptions> createProjectNodeOptionsForSimulator(pugi::xml_node objectData);
	/**/
    std::string getCsdVersion() { return _csdVersion; }

    /* Serialize language XML file to Flat Buffers file. */
    std::string serializeFlatBuffersWithXMLFileForLanguageData(const std::string& xmlFilePath,
                                                               const std::string& flatBuffersFilePath,
                                                               const std::string& languageName);
    
public:
    std::vector<flatbuffers::Offset<flatbuffers::String>> _textures;
    std::vector<flatbuffers::Offset<flatbuffers::String>> _texturePngs;
    bool _isSimulator;
    
private:
    flatbuffers::FlatBufferBuilder* _builder;
    flatbuffers::Offset<flatbuffers::CSParseBinary>* _csparsebinary;
    std::string _csdVersion;
};
}

#endif /* defined(__cocos2d_libs__FlatBuffersSerialize__) */
