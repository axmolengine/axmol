/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-2020 simdsoft.com, @HALX99.

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

#include "FlatBuffersSerialize.h"

#include "base/ObjectFactory.h"
#include "base/Constants.h"
#include "ui/CocosGUI.h"
#include "platform/FileUtils.h"
#include "CSLanguageDataBinary_generated.h"
#include "CSParseBinary_generated.h"

#include "WidgetReader/NodeReaderProtocol.h"
#include "WidgetReader/NodeReaderDefine.h"

#include "WidgetReader/NodeReader/NodeReader.h"
#include "WidgetReader/SingleNodeReader/SingleNodeReader.h"
#include "WidgetReader/SpriteReader/SpriteReader.h"
#include "WidgetReader/ParticleReader/ParticleReader.h"
#include "WidgetReader/GameMapReader/GameMapReader.h"
#include "WidgetReader/ComAudioReader/ComAudioReader.h"
#include "WidgetReader/ProjectNodeReader/ProjectNodeReader.h"

#include "WidgetReader/ButtonReader/ButtonReader.h"
#include "WidgetReader/CheckBoxReader/CheckBoxReader.h"
#include "WidgetReader/ImageViewReader/ImageViewReader.h"
#include "WidgetReader/TextBMFontReader/TextBMFontReader.h"
#include "WidgetReader/TextReader/TextReader.h"
#include "WidgetReader/TextFieldReader/TextFieldReader.h"
#include "WidgetReader/TextAtlasReader/TextAtlasReader.h"
#include "WidgetReader/LoadingBarReader/LoadingBarReader.h"
#include "WidgetReader/SliderReader/SliderReader.h"
#include "WidgetReader/LayoutReader/LayoutReader.h"
#include "WidgetReader/ScrollViewReader/ScrollViewReader.h"
#include "WidgetReader/PageViewReader/PageViewReader.h"
#include "WidgetReader/ListViewReader/ListViewReader.h"

#include "WidgetReader/TextFieldReader/TextFieldExReader.h"

#include "flatbuffers/flatbuffers.h"

USING_NS_AX;
using namespace ax::ui;
using namespace cocostudio;
using namespace cocostudio::timeline;
using namespace flatbuffers;

namespace cocostudio
{

static const char* Property_VisibleForFrame = "VisibleForFrame";
static const char* Property_Position        = "Position";
static const char* Property_Scale           = "Scale";
static const char* Property_RotationSkew    = "RotationSkew";
static const char* Property_CColor          = "CColor";
static const char* Property_FileData        = "FileData";
static const char* Property_FrameEvent      = "FrameEvent";
static const char* Property_Alpha           = "Alpha";
static const char* Property_AnchorPoint     = "AnchorPoint";
static const char* Property_ZOrder          = "ZOrder";
static const char* Property_ActionValue     = "ActionValue";
static const char* Property_BlendValue      = "BlendFunc";

static FlatBuffersSerialize* _instanceFlatBuffersSerialize = nullptr;

FlatBuffersSerialize::FlatBuffersSerialize() : _isSimulator(false), _builder(nullptr), _csparsebinary(nullptr)
{
    CREATE_CLASS_NODE_READER_INFO(NodeReader);
    CREATE_CLASS_NODE_READER_INFO(SingleNodeReader);
    CREATE_CLASS_NODE_READER_INFO(SpriteReader);
    CREATE_CLASS_NODE_READER_INFO(ParticleReader);
    CREATE_CLASS_NODE_READER_INFO(GameMapReader);

    CREATE_CLASS_NODE_READER_INFO(ButtonReader);
    CREATE_CLASS_NODE_READER_INFO(CheckBoxReader);
    CREATE_CLASS_NODE_READER_INFO(ImageViewReader);
    CREATE_CLASS_NODE_READER_INFO(TextBMFontReader);
    CREATE_CLASS_NODE_READER_INFO(TextReader);
    CREATE_CLASS_NODE_READER_INFO(TextFieldReader);
    CREATE_CLASS_NODE_READER_INFO(TextAtlasReader);
    CREATE_CLASS_NODE_READER_INFO(LoadingBarReader);
    CREATE_CLASS_NODE_READER_INFO(SliderReader);
    CREATE_CLASS_NODE_READER_INFO(LayoutReader);
    CREATE_CLASS_NODE_READER_INFO(ScrollViewReader);
    CREATE_CLASS_NODE_READER_INFO(PageViewReader);
    CREATE_CLASS_NODE_READER_INFO(ListViewReader);
    CREATE_CLASS_NODE_READER_INFO(TextFieldExReader);
}

FlatBuffersSerialize::~FlatBuffersSerialize() {}

FlatBuffersSerialize* FlatBuffersSerialize::getInstance()
{
    if (!_instanceFlatBuffersSerialize)
    {
        _instanceFlatBuffersSerialize = new FlatBuffersSerialize();
    }

    return _instanceFlatBuffersSerialize;
}

void FlatBuffersSerialize::purge()
{
    AX_SAFE_DELETE(_instanceFlatBuffersSerialize);
}

void FlatBuffersSerialize::destroyInstance()
{
    AX_SAFE_DELETE(_instanceFlatBuffersSerialize);
}

void FlatBuffersSerialize::deleteFlatBufferBuilder()
{
    if (_builder != nullptr)
    {
        _builder->Clear();
        AX_SAFE_DELETE(_builder);
    }
}

std::string FlatBuffersSerialize::serializeFlatBuffersWithXMLFile(std::string_view xmlFileName,
                                                                  std::string_view flatbuffersFileName)
{
    std::string inFullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName).c_str();

    // xml read
    if (!FileUtils::getInstance()->isFileExist(inFullpath))
    {
        return ".csd file doesn't exists!";
    }

    std::string xmlBuffer = FileUtils::getInstance()->getStringFromFile(inFullpath);
    return serializeFlatBuffersWithXMLBuffer(xmlBuffer, flatbuffersFileName);
}

std::string FlatBuffersSerialize::serializeFlatBuffersWithXMLBuffer(std::string& xmlBuffer,
                                                                    std::string_view flatbuffersFileName)
{
    // xml parse
    pugi::xml_document document;
    if (!xmlBuffer.empty())
        document.load_buffer_inplace(&xmlBuffer.front(), xmlBuffer.length());
    if (document)
        return serializeFlatBuffersWithOpaque(&document, flatbuffersFileName);
    return "";
}

std::string FlatBuffersSerialize::serializeFlatBuffersWithOpaque(void* opaque, std::string_view flatbuffersFileName)
{
    auto thiz = FlatBuffersSerialize::getInstance();

    // xml parse
    pugi::xml_document& document = *reinterpret_cast<pugi::xml_document*>(opaque);

    pugi::xml_node rootElement = document.document_element();  // Root
                                                               //    AXLOG("rootElement name = %s", rootelement.name());

    pugi::xml_node element = rootElement.first_child();

    bool serializeEnabled = false;
    std::string rootType;

    while (element)
    {
        //        AXLOG("entity name = %s", element.name());
        if ("PropertyGroup"sv == element.name())
        {
            auto attribute = element.first_attribute();
            while (attribute && "Version"sv != attribute.name())
                attribute = attribute.next_attribute();
            if (attribute)
                thiz->_csdVersion = attribute.value();

            thiz->_csdVersion = "10.0.3000.0";
        }

        if ("Content"sv == element.name())
        {
            auto attribute = element.first_attribute();

            //
            if (!attribute)
            {
                serializeEnabled = true;
                rootType         = "NodeObjectData";
            }
            //

            //
            //            while (attribute)
            //            {
            //                std::string name = attribute.name();
            //                std::string_view value = attribute.value();
            //                AXLOG("attribute name = %s, value = %s", name, value);
            //                if (name == "")
            //                {
            //                    serializeEnabled = true;
            //                    rootType = (strcmp("", value) == 0) ? "Node" : value;
            //                }
            //
            //                if (serializeEnabled)
            //                {
            //                    break;
            //                }
            //
            //                attribute = attribute.next_attribute();
            //            }
            //
        }

        if (serializeEnabled)
        {
            break;
        }

        auto child = element.first_child();
        if (child)
        {
            element = child;
        }
        else
        {
            element = element.next_sibling();
        }
    }

    if (serializeEnabled)
    {
        thiz->_builder = new FlatBufferBuilder();

        Offset<NodeTree> nodeTree;
        Offset<NodeAction> action;
        std::vector<Offset<flatbuffers::AnimationInfo>> animationInfos;

        auto child = element.first_child();

        while (child)
        {
            std::string_view name = child.name();

            if (name == "Animation")  // action
            {
                pugi::xml_node animation = child;
                action                   = thiz->createNodeAction(animation);
            }
            else if (name == "ObjectData")  // nodeTree
            {
                pugi::xml_node objectData = child;

                auto nameElem = objectData.first_attribute();
                while (nameElem)
                {
                    if ("ctype"sv == nameElem.name())
                    {
                        rootType = nameElem.value();
                        break;
                    }
                    else
                        nameElem = nameElem.next_attribute();
                }
                if (rootType == "GameNodeObjectData" || rootType == "GameLayerObjectData")  // for adaptate old version
                    rootType = "NodeObjectData";

                nodeTree = thiz->createNodeTree(objectData, rootType);
            }
            else if (name == "AnimationList")  // animation list
            {
                pugi::xml_node animationinfoElement = child.first_child();
                while (animationinfoElement)
                {
                    auto animationinfo = thiz->createAnimationInfo(animationinfoElement);
                    animationInfos.emplace_back(animationinfo);
                    animationinfoElement = animationinfoElement.next_sibling();
                }
            }
            child = child.next_sibling();
        }

        auto csparsebinary = CreateCSParseBinary(*thiz->_builder, thiz->_builder->CreateString(thiz->_csdVersion),
                                                 thiz->_builder->CreateVector(thiz->_textures),
                                                 thiz->_builder->CreateVector(thiz->_texturePngs), nodeTree, action,
                                                 thiz->_builder->CreateVector(animationInfos));
        thiz->_builder->Finish(csparsebinary);

        thiz->_textures.clear();
        thiz->_texturePngs.clear();

        std::string outFullPath = FileUtils::getInstance()->fullPathForFilename(flatbuffersFileName);
        size_t pos              = outFullPath.find_last_of('.');
        std::string convert     = outFullPath.substr(0, pos).append(".csb");
        auto save =
            FileUtils::writeBinaryToFile(thiz->_builder->GetBufferPointer(), thiz->_builder->GetSize(), convert);
        if (!save)
        {
            return "couldn't save files!";
        }

        thiz->deleteFlatBufferBuilder();
    }

    return "";
}

// NodeTree
Offset<NodeTree> FlatBuffersSerialize::createNodeTree(pugi::xml_node objectData, std::string_view classType)
{
    auto classname = classType.substr(0, classType.find("ObjectData"));
    //    AXLOG("classname = %s", classname.c_str());

    Offset<Options> options;
    std::vector<Offset<NodeTree>> children;

    if (classname == "ProjectNode")
    {
        auto reader      = ProjectNodeReader::getInstance();
        auto tempOptions = reader->createOptionsWithFlatBuffers(objectData, _builder);

        options = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&tempOptions));
    }
    else if (classname == "SimpleAudio")
    {
        auto reader      = ComAudioReader::getInstance();
        auto tempOptions = reader->createOptionsWithFlatBuffers(objectData, _builder);
        options          = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&tempOptions));
    }
    else
    {
        std::string readername = getGUIClassName(classname);
        readername.append("Reader");

        NodeReaderProtocol* reader =
            dynamic_cast<NodeReaderProtocol*>(ObjectFactory::getInstance()->createObject(readername));
        if (reader != nullptr)
        {
            auto tempOptions = reader->createOptionsWithFlatBuffers(objectData, _builder);
            options          = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&tempOptions));
        }
    }

    // children
    bool containChildrenElement = false;
    auto child                  = objectData.first_child();

    while (child)
    {
        //        AXLOG("child name = %s", child.name());

        if ("Children" == child.name())
        {
            containChildrenElement = true;
            break;
        }

        child = child.next_sibling();
    }

    if (containChildrenElement)
    {
        child = child.first_child();
        //        AXLOG("element name = %s", child.name());

        while (child)
        {
            auto attribute = child.first_attribute();
            bool bHasType  = false;
            while (attribute)
            {
                std::string_view attriname = attribute.name();
                std::string_view value     = attribute.value();

                if (attriname == "ctype")
                {
                    children.emplace_back(createNodeTree(child, value));

                    bHasType = true;
                    break;
                }

                attribute = attribute.next_attribute();
            }

            if (!bHasType)
            {
                children.emplace_back(createNodeTree(child, "NodeObjectData"));
            }

            child = child.next_sibling();
        }
    }
    //

    std::string customClassName;
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "CustomClassName")
        {
            customClassName = value;
            break;
        }

        attribute = attribute.next_attribute();
    }

    return CreateNodeTree(*_builder, _builder->CreateString(classname), _builder->CreateVector(children), options,
                          _builder->CreateString(customClassName));
}

int FlatBuffersSerialize::getResourceType(std::string_view key)
{
    if (key == "Normal" || key == "Default")
    {
        return 0;
    }

    if (_isSimulator)
    {
        if (key == "MarkedSubImage")
        {
            return 0;
        }
    }
    return 1;
}

std::string FlatBuffersSerialize::getGUIClassName(std::string_view name)
{
    std::string convertedClassName;
    if (name == "Panel")
    {
        convertedClassName = "Layout";
    }
    else if (name == "TextArea")
    {
        convertedClassName = "Text";
    }
    else if (name == "TextButton")
    {
        convertedClassName = "Button";
    }
    else if (name == "Label")
    {
        convertedClassName = "Text";
    }
    else if (name == "LabelAtlas")
    {
        convertedClassName = "TextAtlas";
    }
    else if (name == "LabelBMFont")
    {
        convertedClassName = "TextBMFont";
    }
    else
        convertedClassName = name;

    return convertedClassName;
}

std::string FlatBuffersSerialize::getWidgetReaderClassName(Widget* widget)
{
    std::string readerName;

    // 1st., custom widget parse properties of parent widget with parent widget reader
    if (dynamic_cast<Button*>(widget))
    {
        readerName = "ButtonReader";
    }
    else if (dynamic_cast<CheckBox*>(widget))
    {
        readerName = "CheckBoxReader";
    }
    else if (dynamic_cast<ImageView*>(widget))
    {
        readerName = "ImageViewReader";
    }
    else if (dynamic_cast<TextAtlas*>(widget))
    {
        readerName = "TextAtlasReader";
    }
    else if (dynamic_cast<TextBMFont*>(widget))
    {
        readerName = "TextBMFontReader";
    }
    else if (dynamic_cast<Text*>(widget))
    {
        readerName = "TextReader";
    }
    else if (dynamic_cast<LoadingBar*>(widget))
    {
        readerName = "LoadingBarReader";
    }
    else if (dynamic_cast<Slider*>(widget))
    {
        readerName = "SliderReader";
    }
    else if (dynamic_cast<TextField*>(widget))
    {
        readerName = "TextFieldReader";
    }
    else if (dynamic_cast<ListView*>(widget))
    {
        readerName = "ListViewReader";
    }
    else if (dynamic_cast<PageView*>(widget))
    {
        readerName = "PageViewReader";
    }
    else if (dynamic_cast<ScrollView*>(widget))
    {
        readerName = "ScrollViewReader";
    }

    else if (dynamic_cast<Layout*>(widget))
    {
        readerName = "LayoutReader";
    }
    else if (dynamic_cast<Widget*>(widget))
    {
        readerName = "WidgetReader";
    }

    return readerName;
}

// NodeAction
Offset<NodeAction> FlatBuffersSerialize::createNodeAction(pugi::xml_node objectData)
{
    int duration = 0;
    float speed  = 0.0f;
    std::string currentAnimationName;

    //    AXLOG("animation name = %s", objectData->Name());

    // ActionTimeline
    auto attribute = objectData.first_attribute();

    // attributes
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "Duration")
        {
            duration = atoi(value.data());
        }
        else if (name == "Speed")
        {
            speed = atof(value.data());
        }
        else if (name == "ActivedAnimationName")
        {
            currentAnimationName = value;
        }

        attribute = attribute.next_attribute();
    }

    // all Timeline
    std::vector<Offset<TimeLine>> timelines;
    pugi::xml_node timelineElement = objectData.first_child();
    while (timelineElement)
    {
        auto timeLine = createTimeLine(timelineElement);
        timelines.emplace_back(timeLine);

        timelineElement = timelineElement.next_sibling();
    }

    return CreateNodeAction(*_builder, duration, speed, _builder->CreateVector(timelines),
                            _builder->CreateString(currentAnimationName));
}

Offset<flatbuffers::AnimationInfo> FlatBuffersSerialize::createAnimationInfo(pugi::xml_node objectData)
{
    std::string infoName;
    int startIndex = 0;
    int endIndex   = 0;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto attriname  = attribute.name();
        auto attrivalue = attribute.value();
        if (attriname == "Name")
        {
            infoName = attrivalue;
        }
        else if (attriname == "StartIndex")
        {
            startIndex = atoi(attrivalue.data());
        }
        else if (attriname == "EndIndex")
        {
            endIndex = atoi(attrivalue.data());
        }
        attribute = attribute.next_attribute();
    }
    return CreateAnimationInfo(*_builder, _builder->CreateString(infoName), startIndex, endIndex);
}

Offset<TimeLine> FlatBuffersSerialize::createTimeLine(pugi::xml_node objectData)
{
    int actionTag = 0;
    std::string property;

    // TimelineData attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "ActionTag")
        {
            actionTag = atoi(value.data());
        }
        else if (name == "Property")
        {
            property = value;
        }

        attribute = attribute.next_attribute();
    }

    // all Frame
    std::vector<Offset<flatbuffers::Frame>> frames;

    pugi::xml_node frameElement = objectData.first_child();
    while (frameElement)
    {
        Offset<flatbuffers::Frame> frame;

        if (property == Property_VisibleForFrame)
        {
            auto boolFrame = createBoolFrame(frameElement);
            frame          = CreateFrame(*_builder,
                                         0,  // PointFrame
                                         0,  // ScaleFrame
                                         0,  // ColorFrame
                                         0,  // TextureFrame
                                         0,  // EventFrame
                                         0,  // IntFrame
                                         boolFrame);
        }
        else if (property == Property_Position)
        {
            auto pointFrame = createPointFrame(frameElement);
            frame           = CreateFrame(*_builder, pointFrame);
        }
        else if (property == Property_Scale)
        {
            auto scaleFrame = createScaleFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          scaleFrame);
        }
        else if (property == Property_RotationSkew)
        {
            auto scaleFrame = createScaleFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          scaleFrame);
        }
        else if (property == Property_CColor)
        {
            auto colorFrame = createColorFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          0,  // ScaleFrame
                                          colorFrame);
        }
        else if (property == Property_FileData)
        {
            auto textureFrame = createTextureFrame(frameElement);
            frame             = CreateFrame(*_builder,
                                            0,  // PointFrame
                                            0,  // ScaleFrame
                                            0,  // ColorFrame
                                            textureFrame);
        }
        else if (property == Property_FrameEvent)
        {
            auto eventFrame = createEventFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          0,  // ScaleFrame
                                          0,  // ColorFrame
                                          0,  // TextureFrame
                                          eventFrame);
        }
        else if (property == Property_Alpha)
        {
            auto intFrame = createIntFrame(frameElement);
            frame         = CreateFrame(*_builder,
                                        0,  // PointFrame
                                        0,  // ScaleFrame
                                        0,  // ColorFrame
                                        0,  // TextureFrame
                                        0,  // EventFrame
                                        intFrame);
        }
        else if (property == Property_AnchorPoint)
        {
            auto scaleFrame = createScaleFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          scaleFrame);
        }
        else if (property == Property_ZOrder)
        {
            auto intFrame = createIntFrame(frameElement);
            frame         = CreateFrame(*_builder,
                                        0,  // PointFrame
                                        0,  // ScaleFrame
                                        0,  // ColorFrame
                                        0,  // TextureFrame
                                        0,  // EventFrame
                                        intFrame);
        }
        else if (property == Property_ActionValue)
        {
            auto innerActionFrame = createInnerActionFrame(frameElement);
            frame                 = CreateFrame(*_builder,
                                                0,  // PointFrame
                                                0,  // ScaleFrame
                                                0,  // ColorFrame
                                                0,  // TextureFrame
                                                0,  // EventFrame
                                                0,  // IntFrame
                                                0,  // BoolFrame
                                                innerActionFrame);
        }
        else if (property == Property_BlendValue)
        {
            auto blendFrame = createBlendFrame(frameElement);
            frame           = CreateFrame(*_builder,
                                          0,  // PointFrame
                                          0,  // ScaleFrame
                                          0,  // ColorFrame
                                          0,  // TextureFrame
                                          0,  // EventFrame
                                          0,  // IntFrame
                                          0,  // BoolFrame
                                          0,  // InnerActionFrame
                                          blendFrame);
        }

        frames.emplace_back(frame);

        frameElement = frameElement.next_sibling();
    }

    return CreateTimeLine(*_builder, _builder->CreateString(property), actionTag, _builder->CreateVector(frames));
}

Offset<flatbuffers::PointFrame> FlatBuffersSerialize::createPointFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    Vec2 position;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "X")
        {
            position.x = atof(value.data());
        }
        else if (name == "Y")
        {
            position.y = atof(value.data());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(value.data());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    FVec2 f_position(position.x, position.y);

    return CreatePointFrame(*_builder, frameIndex, tween, &f_position, createEasingData(objectData.first_child()));
}

Offset<flatbuffers::ScaleFrame> FlatBuffersSerialize::createScaleFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    Vec2 scale;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "X")
        {
            scale.x = atof(value.data());
        }
        else if (name == "Y")
        {
            scale.y = atof(value.data());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(value.data());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    FVec2 f_scale(scale.x, scale.y);

    return CreateScaleFrame(*_builder, frameIndex, tween, &f_scale, createEasingData(objectData.first_child()));
}

Offset<flatbuffers::ColorFrame> FlatBuffersSerialize::createColorFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    Color3B color;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "FrameIndex")
        {
            frameIndex = atoi(value.data());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    // color
    auto child = objectData.first_child();
    while (child)
    {
        attribute = child.first_attribute();
        while (attribute)
        {
            std::string_view name  = attribute.name();
            std::string_view value = attribute.value();

            if (name == "R")
            {
                color.r = atoi(value.data());
            }
            else if (name == "G")
            {
                color.g = atoi(value.data());
            }
            else if (name == "B")
            {
                color.b = atoi(value.data());
            }

            attribute = attribute.next_attribute();
        }

        child = child.next_sibling();
    }

    Color f_color(255, color.r, color.g, color.b);

    return CreateColorFrame(*_builder, frameIndex, tween, &f_color, createEasingData(objectData.first_child()));
}

Offset<flatbuffers::TextureFrame> FlatBuffersSerialize::createTextureFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;

    std::string path;
    std::string plistFile;
    int resourceType = 0;

    std::string texture;
    std::string texturePng;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "FrameIndex")
        {
            frameIndex = atoi(value.data());
        }
        else if (attriname == "Tween")
        {
            tween = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        attribute = child.first_attribute();
        while (attribute)
        {
            std::string_view attriname = attribute.name();
            std::string_view value     = attribute.value();

            if (attriname == "Path")
            {
                path = value;
            }
            else if (attriname == "Type")
            {
                resourceType = getResourceType(value);
            }
            else if (attriname == "Plist")
            {
                plistFile = value;
                texture   = value;
            }

            attribute = attribute.next_attribute();
        }

        if (resourceType == 1)
        {
            _textures.emplace_back(_builder->CreateString(texture));
        }

        child = child.next_sibling();
    }

    return CreateTextureFrame(
        *_builder, frameIndex, tween,
        CreateResourceData(*_builder, _builder->CreateString(path), _builder->CreateString(plistFile), resourceType),
        createEasingData(objectData.first_child()));
}

Offset<flatbuffers::EventFrame> FlatBuffersSerialize::createEventFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    std::string value;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name       = attribute.name();
        auto attrivalue = attribute.value();

        if (name == "Value")  // to be gonna modify
        {
            value = attrivalue;
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.data());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    return CreateEventFrame(*_builder, frameIndex, tween, _builder->CreateString(value),
                            createEasingData(objectData.first_child()));
}

Offset<flatbuffers::IntFrame> FlatBuffersSerialize::createIntFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    int value      = 0;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name       = attribute.name();
        auto attrivalue = attribute.value();

        if (name == "Value")  // to be gonna modify
        {
            value = atoi(attrivalue.data());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.data());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    return CreateIntFrame(*_builder, frameIndex, tween, value, createEasingData(objectData.first_child()));
}

Offset<flatbuffers::BoolFrame> FlatBuffersSerialize::createBoolFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    bool value     = true;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name       = attribute.name();
        auto attrivalue = attribute.value();

        if (name == "Value")
        {
            value = (attrivalue == "True") ? true : false;
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.data());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    return CreateBoolFrame(*_builder, frameIndex, tween, value, createEasingData(objectData.first_child()));
}

Offset<flatbuffers::InnerActionFrame> FlatBuffersSerialize::createInnerActionFrame(pugi::xml_node objectData)
{
    int frameIndex      = 0;
    bool tween          = true;
    int innerActionType = 0;
    std::string currentAniamtionName;
    int singleFrameIndex = 0;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name       = attribute.name();
        auto attrivalue = attribute.value();

        if (name == "InnerActionType")
        {
            if (attrivalue == "LoopAction")
            {
                innerActionType = 0;
            }
            else if (attrivalue == "NoLoopAction")
            {
                innerActionType = 1;
            }
            else if (attrivalue == "SingleFrame")
            {
                innerActionType = 2;
            }
        }
        else if (name == "CurrentAniamtionName")
        {
            currentAniamtionName = attrivalue;
        }
        else if (name == "SingleFrameIndex")
        {
            singleFrameIndex = atoi(attrivalue.data());
        }
        else if (name == "FrameIndex")
        {
            frameIndex = atoi(attrivalue.data());
        }
        else if (name == "Tween")
        {
            tween = (attrivalue == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    return CreateInnerActionFrame(*_builder, frameIndex, tween, innerActionType,
                                  _builder->CreateString(currentAniamtionName), singleFrameIndex,
                                  createEasingData(objectData.first_child()));
}

flatbuffers::Offset<flatbuffers::BlendFrame> FlatBuffersSerialize::createBlendFrame(pugi::xml_node objectData)
{
    int frameIndex = 0;
    bool tween     = true;
    int32_t src = GLBlendConst::ONE, dst = GLBlendConst::ONE_MINUS_SRC_ALPHA;
    std::string name;
    std::string value;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        name  = attribute.name();
        value = attribute.value();

        if (name == "FrameIndex")
        {
            frameIndex = atoi(value.data());
        }
        else if (name == "Tween")
        {
            tween = (value == "True") ? true : false;
        }
        else if (name == "Src")
        {
            src = atoi(value.data());
        }
        else if (name == "Dst")
        {
            dst = atoi(value.data());
        }

        attribute = attribute.next_attribute();
    }

    flatbuffers::Offset<flatbuffers::EasingData> easingData;
    flatbuffers::BlendFunc blendFunc(src, dst);
    return CreateBlendFrame(*_builder, frameIndex, tween, &blendFunc, easingData);
}

flatbuffers::Offset<flatbuffers::EasingData> FlatBuffersSerialize::createEasingData(pugi::xml_node objectData)
{
    if (!objectData)
    {
        return 0;
    }

    int type = -1;
    std::vector<flatbuffers::FVec2> points;

    auto attribute = objectData.first_attribute();

    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "Type")
        {
            type = atoi(value.data());
            break;
        }

        attribute = attribute.next_attribute();
    }

    pugi::xml_node Points = objectData.first_child();
    if (Points)
    {
        pugi::xml_node PointF = Points.first_child();
        while (PointF)
        {
            Vec2 pointF;

            attribute = PointF.first_attribute();

            while (attribute)
            {
                std::string_view name  = attribute.name();
                std::string_view value = attribute.value();

                if (name == "X")
                {
                    pointF.x = atof(value.data());
                }
                else if (name == "Y")
                {
                    pointF.y = atof(value.data());
                }
                attribute = attribute.next_attribute();
            }
            flatbuffers::FVec2 f_PointF(pointF.x, pointF.y);
            points.emplace_back(f_PointF);

            PointF = PointF.next_sibling();
        }
    }

    return CreateEasingData(*_builder, type, _builder->CreateVectorOfStructs(points));
}

/* create flat buffers with XML */
FlatBufferBuilder* FlatBuffersSerialize::createFlatBuffersWithXMLFileForSimulator(std::string_view xmlFileName)
{
    std::string inFullpath = FileUtils::getInstance()->fullPathForFilename(xmlFileName);

    // xml read
    if (!FileUtils::getInstance()->isFileExist(inFullpath))
    {
        //        AXLOG(".csd file does not exist.");
    }

    std::string content = FileUtils::getInstance()->getStringFromFile(inFullpath);

    // xml parse
    pugi::xml_document document;
    if (!content.empty())
        document.load_buffer_inplace(&content.front(), content.length());

    pugi::xml_node rootElement = document.document_element();  // Root
                                                               //    AXLOG("rootElement name = %s", rootelement.name());

    pugi::xml_node element = rootElement.first_child();

    bool serializeEnabled = false;
    std::string rootType;

    while (element)
    {
        //        AXLOG("entity name = %s", element.name());
        if ("PropertyGroup"sv == element.name())
        {
            auto attribute = element.first_attribute();
            while (attribute && "Version"sv != attribute.name())
                attribute = attribute.next_attribute();
            if (attribute)
                _csdVersion = attribute.value();
        }

        if ("Content"sv == element.name())
        {
            auto attribute = element.first_attribute();

            //
            if (!attribute)
            {
                serializeEnabled = true;
                rootType         = "NodeObjectData";
            }
        }

        if (serializeEnabled)
        {
            break;
        }

        auto child = element.first_child();
        if (child)
        {
            element = child;
        }
        else
        {
            element = element.next_sibling();
        }
    }

    if (serializeEnabled)
    {
        _builder = new FlatBufferBuilder();

        Offset<NodeTree> nodeTree;
        Offset<NodeAction> action;
        std::vector<Offset<flatbuffers::AnimationInfo>> animationInfos;

        auto child = element.first_child();

        while (child)
        {
            std::string_view name = child.name();

            if (name == "Animation")  // action
            {
                pugi::xml_node animation = child;
                action                   = createNodeAction(animation);
            }
            else if (name == "ObjectData")  // nodeTree
            {
                pugi::xml_node objectData = child;
                auto nameElem             = objectData.first_attribute();
                while (nameElem)
                {
                    if ("ctype"sv == nameElem.name())
                    {
                        rootType = nameElem.value();
                        break;
                    }
                    else
                        nameElem = nameElem.next_attribute();
                }
                if (rootType == "GameNodeObjectData" || rootType == "GameLayerObjectData")  // for adaptate old version
                    rootType = "NodeObjectData";
                nodeTree = createNodeTreeForSimulator(objectData, rootType);
            }
            else if (name == "AnimationList")  // animation list
            {
                pugi::xml_node animationinfoElement = child.first_child();
                while (animationinfoElement)
                {
                    auto animationinfo = createAnimationInfo(animationinfoElement);
                    animationInfos.emplace_back(animationinfo);
                    animationinfoElement = animationinfoElement.next_sibling();
                }
            }

            child = child.next_sibling();
        }

        auto csparsebinary = CreateCSParseBinary(
            *_builder, _builder->CreateString(_csdVersion), _builder->CreateVector(_textures),
            _builder->CreateVector(_texturePngs), nodeTree, action, _builder->CreateVector(animationInfos));
        _builder->Finish(csparsebinary);

        _textures.clear();
        _texturePngs.clear();
    }
    return _builder;
}

Offset<NodeTree> FlatBuffersSerialize::createNodeTreeForSimulator(pugi::xml_node objectData, std::string_view classType)
{
    auto classname = classType.substr(0, classType.find("ObjectData"));
    //    AXLOG("classname = %s", classname.c_str());

    Offset<Options> options;
    std::vector<Offset<NodeTree>> children;

    if (classname == "ProjectNode")
    {
        auto projectNodeOptions = createProjectNodeOptionsForSimulator(objectData);
        options                 = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&projectNodeOptions));
    }
    else if (classname == "SimpleAudio")
    {
        auto reader      = ComAudioReader::getInstance();
        auto tempOptions = reader->createOptionsWithFlatBuffers(objectData, _builder);

        options = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&tempOptions));
    }
    else
    {
        std::string readername = getGUIClassName(classname);
        readername.append("Reader");

        NodeReaderProtocol* reader =
            dynamic_cast<NodeReaderProtocol*>(ObjectFactory::getInstance()->createObject(readername));
        if (reader != nullptr)
        {
            auto tempOptions = reader->createOptionsWithFlatBuffers(objectData, _builder);

            options = CreateOptions(*_builder, *(Offset<WidgetOptions>*)(&tempOptions));
        }
    }

    // children
    bool containChildrenElement = false;
    auto child                  = objectData.first_child();

    while (child)
    {
        //        AXLOG("child name = %s", child.name());

        if ("Children"sv == child.name())
        {
            containChildrenElement = true;
            break;
        }

        child = child.next_sibling();
    }

    if (containChildrenElement)
    {
        child = child.first_child();
        //        AXLOG("element name = %s", child.name());

        while (child)
        {
            auto attribute = child.first_attribute();
            bool bHasType  = false;
            while (attribute)
            {
                std::string_view attriname = attribute.name();
                std::string_view value     = attribute.value();

                if (attriname == "ctype")
                {
                    children.emplace_back(createNodeTreeForSimulator(child, value));

                    bHasType = true;
                    break;
                }

                attribute = attribute.next_attribute();
            }

            if (!bHasType)
            {
                children.emplace_back(createNodeTreeForSimulator(child, "NodeObjectData"sv));
            }

            child = child.next_sibling();
        }
    }
    //

    std::string customClassName;
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "CustomClassName")
        {
            customClassName = value;
            break;
        }

        attribute = attribute.next_attribute();
    }

    return CreateNodeTree(*_builder, _builder->CreateString(classname), _builder->CreateVector(children), options,
                          _builder->CreateString(customClassName));
}

Offset<ProjectNodeOptions> FlatBuffersSerialize::createProjectNodeOptionsForSimulator(pugi::xml_node objectData)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, _builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string filename;
    float innerspeed = 1.0f;

    pugi::xml_attribute objattri = objectData.first_attribute();
    // inneraction speed
    while (objattri)
    {
        std::string_view name  = objattri.name();
        std::string_view value = objattri.value();
        if (name == "InnerActionSpeed")
        {
            innerspeed = atof(value.data());
            break;
        }
        objattri = objattri.next_attribute();
    }

    // FileData
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "FileData")
        {
            auto attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    filename = value;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    return CreateProjectNodeOptions(*_builder, nodeOptions, _builder->CreateString(filename), innerspeed);
}

/* Serialize language XML file to Flat Buffers file. */
std::string FlatBuffersSerialize::serializeFlatBuffersWithXMLFileForLanguageData(std::string_view xmlFilePath,
                                                                                 std::string_view flatBuffersFilePath,
                                                                                 std::string_view languageName)
{
    // Read and parse XML data file.
    if (!FileUtils::getInstance()->isFileExist(xmlFilePath))
        return "Language XML file does not exist.";
    std::string content = FileUtils::getInstance()->getStringFromFile(xmlFilePath);
    pugi::xml_document document;
    if (!content.empty())
        document.load_buffer_inplace(&content.front(), content.length());
    pugi::xml_node element = document.document_element();
    element                = element.first_child();

    // Create FlatBuffers file using the language data in XML file.
    _builder = new FlatBufferBuilder();
    std::vector<Offset<LanguageItem>> langItemList;
    while (element)
    {
        if ("language"sv != element.name())
        {
            element = element.next_sibling();
            continue;
        }

        // Read all of the Key-Values in the XML file.
        std::string key;
        std::string text;
        bool hasKeyReaded           = false;
        bool hasTextReaded          = false;
        pugi::xml_node childElement = element.first_child();
        while (childElement)
        {
            // Record language key.
            if ("key" == childElement.name())
            {
                key          = childElement.text().as_string();
                hasKeyReaded = true;
            }
            // Record corresponding text.
            else if (languageName == childElement.name())
            {
                auto langText = childElement.text().as_string();
                if (!langText.empty())
                    text = langText;
                else
                    text = key;
                hasTextReaded = true;
            }

            if (hasKeyReaded && hasTextReaded)
                break;

            childElement = childElement.next_sibling();
        }

        Offset<flatbuffers::LanguageItem> langItem =
            CreateLanguageItem(*_builder, _builder->CreateString(key), _builder->CreateString(text));
        langItemList.emplace_back(langItem);

        element = element.next_sibling();
    }

    auto langSet = CreateLanguageSet(*_builder, _builder->CreateVector(langItemList));
    _builder->Finish(langSet);
    bool isSuccess =
        FileUtils::writeBinaryToFile(_builder->GetBufferPointer(), _builder->GetSize(), flatBuffersFilePath);

    if (isSuccess)
        return "";
    else
        return "Failed to save language .csb file.";
}
}  // namespace cocostudio
/**/
