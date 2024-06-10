/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org

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

#ifndef __cocos2d_libs__FlatBuffersSerialize__
#define __cocos2d_libs__FlatBuffersSerialize__

#include <string>
#include <vector>

#include "extensions/ExtensionMacros.h"
#include "CocosStudioExport.h"
#include "platform/PlatformMacros.h"
#include "ui/UIWidget.h"

#include "pugixml/pugixml.hpp"

#include "flatbuffers/flatbuffers.h"

namespace flatbuffers
{
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
}  // namespace flatbuffers

namespace cocostudio
{

class CCS_DLL FlatBuffersSerialize
{

public:
    static FlatBuffersSerialize* getInstance();
    /** @deprecated Use method destroyInstance() instead */
    AX_DEPRECATED_ATTRIBUTE static void purge();
    static void destroyInstance();

    FlatBuffersSerialize();
    ~FlatBuffersSerialize();

    void deleteFlatBufferBuilder();

    std::string test(std::string_view xmlFileName, std::string_view flatbuffersFileName);

    /* serialize flat buffers with XML */
    static std::string serializeFlatBuffersWithXMLFile(std::string_view xmlFileName,
                                                       std::string_view flatbuffersFileName);
    /* x-studio spec: serialize flat buffers with XML buffer */
    static std::string serializeFlatBuffersWithXMLBuffer(std::string& xmlBuffer, std::string_view flatbuffersFileName);
    static std::string serializeFlatBuffersWithOpaque(void* opaque, std::string_view flatbuffersFileName);

    // NodeTree
    flatbuffers::Offset<flatbuffers::NodeTree> createNodeTree(pugi::xml_node objectData, std::string_view classType);

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

    // Animation Info
    flatbuffers::Offset<flatbuffers::AnimationInfo> createAnimationInfo(pugi::xml_node objectData);
    /**/

    int getResourceType(std::string_view key);
    std::string getGUIClassName(std::string_view name);
    std::string getWidgetReaderClassName(ax::ui::Widget* widget);

    /* create flat buffers with XML */
    flatbuffers::FlatBufferBuilder* createFlatBuffersWithXMLFileForSimulator(std::string_view xmlFileName);
    flatbuffers::Offset<flatbuffers::NodeTree> createNodeTreeForSimulator(pugi::xml_node objectData,
                                                                          std::string_view classType);
    flatbuffers::Offset<flatbuffers::ProjectNodeOptions> createProjectNodeOptionsForSimulator(
        pugi::xml_node objectData);
    /**/
    std::string getCsdVersion() { return _csdVersion; }

    /* Serialize language XML file to Flat Buffers file. */
    std::string serializeFlatBuffersWithXMLFileForLanguageData(std::string_view xmlFilePath,
                                                               std::string_view flatBuffersFilePath,
                                                               std::string_view languageName);

public:
    std::vector<flatbuffers::Offset<flatbuffers::String>> _textures;
    std::vector<flatbuffers::Offset<flatbuffers::String>> _texturePngs;
    bool _isSimulator;

private:
    flatbuffers::FlatBufferBuilder* _builder;
    flatbuffers::Offset<flatbuffers::CSParseBinary>* _csparsebinary;
    std::string _csdVersion;
};
}  // namespace cocostudio

#endif /* defined(__cocos2d_libs__FlatBuffersSerialize__) */
