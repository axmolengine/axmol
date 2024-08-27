/****************************************************************************
 Copyright (c) 2015-2017 Chukong Technologies Inc.

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

#include "flatbuffers/flatbuffers.h"
#include "cocostudio/WidgetReader/WidgetReader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/TabControlReader/TabControlReader.h"
#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/ActionTimeline/CSLoader.h"
#include "ui/UITabControl.h"
#include "platform/FileUtils.h"
#include "2d/SpriteFrameCache.h"

using namespace ax;
using namespace cocostudio;
using namespace flatbuffers;
using namespace ax::ui;

IMPLEMENT_CLASS_NODE_READER_INFO(TabControlReader)

TabControlReader::TabControlReader() {}

TabControlReader* TabControlReader::_tabReaderInstance = nullptr;
TabControlReader* TabControlReader::getInstance()
{
    if (_tabReaderInstance == nullptr)
    {
        _tabReaderInstance = new TabControlReader();
    }
    return _tabReaderInstance;
}

void TabControlReader::destroyInstance()
{
    AX_SAFE_DELETE(_tabReaderInstance);
}

flatbuffers::Offset<flatbuffers::Table> TabControlReader::createOptionsWithFlatBuffers(
    pugi::xml_node objectData,
    flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    int headerPlace        = 0;
    int headerWidth        = 50;
    int headerHeight       = 20;
    float selectedTabZoom  = 0.0f;
    int selectedIndex      = 0;
    bool ignoretexturesize = true;

    std::vector<Offset<TabItemOption>> tabItems;
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "HeaderPlace")
        {
            auto placeStr = value.data();
            if (strcmp(placeStr, "TOP") == 0)
                headerPlace = 0;
            else if (strcmp(placeStr, "LEFT") == 0)
                headerPlace = 1;
            else if (strcmp(placeStr, "BOTTOM") == 0)
                headerPlace = 2;
            else if (strcmp(placeStr, "RIGHT") == 0)
                headerPlace = 3;
        }
        else if (attriname == "HeaderWidth")
        {
            headerWidth = atoi(value.data());
        }
        else if (attriname == "HeaderHeight")
        {
            headerHeight = atoi(value.data());
        }
        else if (attriname == "SelectedTabZoom")
        {
            selectedTabZoom = atof(value.data());
        }
        else if (attriname == "SelectedTabIndex")
        {
            selectedIndex = atoi(value.data());
        }
        else if (attriname == "IgnoreHeaderTextureSize")
        {
            ignoretexturesize = FLATSTR_TO_BOOL(value);
        }
        attribute = attribute.next_attribute();
    }

    bool containChildrenElement = false;
    auto child                  = objectData.first_child();
    while (child)
    {
        if ("Children"sv == child.name())
        {
            containChildrenElement = true;
            break;
        }

        child = child.next_sibling();
    }

    if (containChildrenElement)
    {
        child        = child.first_child();  // first child
        bool hasItem = true;
        while (child && hasItem)
        {
            pugi::xml_attribute childattribute = child.first_attribute();
            while (childattribute)
            {
                auto attriname = childattribute.name();
                auto value     = childattribute.value();

                if (attriname == "ctype")
                {
                    if (value.compare("TabItemObjectData") == 0)
                    {
                        auto itemOption =
                            TabItemReader::getInstance()->createTabItemOptionWithFlatBuffers(child, builder);
                        tabItems.emplace_back(itemOption);
                        break;
                    }
                    else
                        hasItem = false;

                    break;
                }
                childattribute = childattribute.next_attribute();
            }
            child = child.next_sibling();
        }
    }

    auto options =
        CreateTabControlOption(*builder, nodeOptions, headerPlace, headerWidth, headerHeight, selectedTabZoom,
                               selectedIndex, ignoretexturesize, builder->CreateVector(tabItems));

    return *(Offset<Table>*)(&options);
}

void TabControlReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOption)
{
    auto tabControl = static_cast<ax::ui::TabControl*>(node);
    auto options    = (flatbuffers::TabControlOption*)nodeOption;

    int headerPlace = options->headerPlace();
    tabControl->ignoreHeadersTextureSize(options->ignoreHeaderTextureSize() != 0);
    tabControl->setHeaderDockPlace((ax::ui::TabControl::Dock)headerPlace);
    tabControl->setHeaderWidth(options->headerWidth());
    tabControl->setHeaderHeight(options->headerHeight());
    tabControl->setHeaderSelectedZoom(options->selectedTabZoom());

    int tabItemCount = options->tabItems()->size();
    for (int i = 0; i < tabItemCount; i++)
    {
        auto item      = options->tabItems()->Get(i);
        auto header    = TabHeaderReader::getInstance()->createNodeWithFlatBuffers((Table*)item->header());
        auto container = CSLoader::getInstance()->nodeWithFlatBuffers(item->container());
        tabControl->insertTab(i, (TabHeader*)header, (Layout*)container);
    }
    tabControl->setSelectTab(options->selectedTabIndex());
}

ax::Node* TabControlReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    auto node = ax::ui::TabControl::create();

    auto taboptions = (flatbuffers::TabControlOption*)nodeOptions;
    setPropsWithFlatBuffers(node, nodeOptions);

    auto nodereader = WidgetReader::getInstance();
    nodereader->setPropsWithFlatBuffers(node, (Table*)taboptions->nodeOptions());
    return node;
}

TabControlReader::~TabControlReader() {}

TabHeaderReader::TabHeaderReader() {}

TabHeaderReader::~TabHeaderReader() {}

TabHeaderReader* TabHeaderReader::_tabheaderReaderInstance = nullptr;
TabHeaderReader* TabHeaderReader::getInstance()
{
    if (_tabheaderReaderInstance == nullptr)
    {
        _tabheaderReaderInstance = new TabHeaderReader();
    }
    return _tabheaderReaderInstance;
}

void TabHeaderReader::destroyInstance()
{
    AX_SAFE_DELETE(_tabheaderReaderInstance);
}

flatbuffers::Offset<flatbuffers::Table> TabHeaderReader::createOptionsWithFlatBuffers(
    pugi::xml_node objectData,
    flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    int fontsize = 12;
    std::string text;
    ax::Color4B textColor(255, 255, 255, 255);
    std::string fontName;

    int backgroundboxResourceType = 0;
    std::string backgroundboxPath;
    std::string backgroundboxPlistFile;

    int backGroundBoxSelectedResourceType = 0;
    std::string backGroundBoxSelectedPath;
    std::string backGroundBoxSelectedPlistFile;

    int frontCrossResourceType = 0;
    std::string frontCrossPath;
    std::string frontCrossPlistFile;

    int backGroundBoxDisabledResourceType = 0;
    std::string backGroundBoxDisabledPath;
    std::string backGroundBoxDisabledPlistFile;

    int frontCrossDisabledResourceType = 0;
    std::string frontCrossDisabledPath;
    std::string frontCrossDisabledPlistFile;

    std::string fontResourcePath;
    std::string fontResourcePlistFile;
    int fontResourceResourceType = 0;

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname.compare("FontSize") == 0)
        {
            fontsize = atoi(value.data());
        }
        else if (attriname.compare("TitleText") == 0)
        {
            text = value;
        }
        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "TextColor")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "R")
                {
                    textColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    textColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    textColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "NormalBackFileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    backgroundboxPath = value;
                }
                else if (name == "Type")
                {
                    backgroundboxResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backgroundboxPlistFile = value;
                    texture                = value;
                }

                attribute = attribute.next_attribute();
            }

            if (backgroundboxResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "PressedBackFileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    backGroundBoxSelectedPath = value;
                }
                else if (name == "Type")
                {
                    backGroundBoxSelectedResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backGroundBoxSelectedPlistFile = value;
                    texture                        = value;
                }

                attribute = attribute.next_attribute();
            }

            if (backGroundBoxSelectedResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "NodeNormalFileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    frontCrossPath = value;
                }
                else if (name == "Type")
                {
                    frontCrossResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    frontCrossPlistFile = value;
                    texture             = value;
                }

                attribute = attribute.next_attribute();
            }

            if (frontCrossResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "DisableBackFileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    backGroundBoxDisabledPath = value;
                }
                else if (name == "Type")
                {
                    backGroundBoxDisabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    backGroundBoxDisabledPlistFile = value;
                    texture                        = value;
                }

                attribute = attribute.next_attribute();
            }

            if (backGroundBoxDisabledResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "NodeDisableFileData")
        {
            std::string texture;
            std::string texturePng;

            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    frontCrossDisabledPath = value;
                }
                else if (name == "Type")
                {
                    frontCrossDisabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    frontCrossDisabledPlistFile = value;
                    texture                     = value;
                }

                attribute = attribute.next_attribute();
            }

            if (frontCrossDisabledResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "FontResource")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    fontResourcePath = value;
                }
                else if (name == "Type")
                {
                    fontResourceResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    fontResourcePlistFile = value;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }
    Color f_textColor(255, textColor.r, textColor.g, textColor.b);
    auto option = CreateTabHeaderOption(
        *builder, nodeOptions,
        CreateResourceData(*builder, builder->CreateString(fontResourcePath),
                           builder->CreateString(fontResourcePlistFile), fontResourceResourceType),
        fontsize, builder->CreateString(text), &f_textColor,
        CreateResourceData(*builder, builder->CreateString(backgroundboxPath),
                           builder->CreateString(backgroundboxPlistFile), backgroundboxResourceType),
        CreateResourceData(*builder, builder->CreateString(backGroundBoxSelectedPath),
                           builder->CreateString(backGroundBoxSelectedPlistFile), backGroundBoxSelectedResourceType),
        CreateResourceData(*builder, builder->CreateString(backGroundBoxDisabledPath),
                           builder->CreateString(backGroundBoxDisabledPlistFile), backGroundBoxDisabledResourceType),
        CreateResourceData(*builder, builder->CreateString(frontCrossPath), builder->CreateString(frontCrossPlistFile),
                           frontCrossResourceType),
        CreateResourceData(*builder, builder->CreateString(frontCrossDisabledPath),
                           builder->CreateString(frontCrossDisabledPlistFile), frontCrossDisabledResourceType));

    return *(Offset<Table>*)(&option);
}

void TabHeaderReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOption)
{
    auto header  = static_cast<ax::ui::TabHeader*>(node);
    auto options = (flatbuffers::TabHeaderOption*)nodeOption;

    header->setTitleFontSize(options->fontSize());
    header->setTitleText(options->titleText()->c_str());
    auto textColor = options->textColor();
    Color4B titleColor(textColor->r(), textColor->g(), textColor->b(), textColor->a());
    header->setTitleColor(titleColor);

    auto resourceData = options->fontRes();
    bool fileExist    = false;
    std::string errorFilePath;
    std::string path = resourceData->path()->c_str();
    if (path != "")
    {
        if (FileUtils::getInstance()->isFileExist(path))
        {
            fileExist = true;
        }
        else
        {
            errorFilePath = path;
            fileExist     = false;
        }
        if (fileExist)
        {
            header->setTitleFontName(path);
        }
    }

    bool backGroundFileExist = false;
    std::string backGroundErrorFilePath;
    auto backGroundDic                = options->normalBackFile();
    int backGroundType                = backGroundDic->resourceType();
    std::string backGroundTexturePath = backGroundDic->path()->c_str();
    switch (backGroundType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(backGroundTexturePath))
        {
            backGroundFileExist = true;
        }
        else
        {
            backGroundErrorFilePath = backGroundTexturePath;
            backGroundFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = backGroundDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(backGroundTexturePath);
        if (spriteFrame)
        {
            backGroundFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    backGroundErrorFilePath = textureFileName;
                }
            }
            else
            {
                backGroundErrorFilePath = plist;
            }
            backGroundFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (backGroundFileExist)
    {
        header->loadTextureBackGround(backGroundTexturePath, (Widget::TextureResType)backGroundType);
    }

    // load background selected image
    bool backGroundSelectedfileExist = false;
    std::string backGroundSelectedErrorFilePath;
    auto backGroundSelectedDic                = options->pressBackFile();
    int backGroundSelectedType                = backGroundSelectedDic->resourceType();
    std::string backGroundSelectedTexturePath = backGroundSelectedDic->path()->c_str();
    switch (backGroundSelectedType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(backGroundSelectedTexturePath))
        {
            backGroundSelectedfileExist = true;
        }
        else
        {
            backGroundSelectedErrorFilePath = backGroundSelectedTexturePath;
            backGroundSelectedfileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = backGroundSelectedDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(backGroundSelectedTexturePath);
        if (spriteFrame)
        {
            backGroundSelectedfileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    backGroundSelectedErrorFilePath = textureFileName;
                }
            }
            else
            {
                backGroundSelectedErrorFilePath = plist;
            }
            backGroundSelectedfileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (backGroundSelectedfileExist)
    {
        header->loadTextureBackGroundSelected(backGroundSelectedTexturePath,
                                              (ax::ui::Widget::TextureResType)backGroundSelectedType);
    }

    // load frontCross image
    bool frontCrossFileExist = false;
    std::string frontCrossErrorFilePath;
    auto frontCrossDic             = options->crossNormalFile();
    int frontCrossType             = frontCrossDic->resourceType();
    std::string frontCrossFileName = frontCrossDic->path()->c_str();
    switch (frontCrossType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(frontCrossFileName))
        {
            frontCrossFileExist = true;
        }
        else
        {
            frontCrossErrorFilePath = frontCrossFileName;
            frontCrossFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = frontCrossDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(frontCrossFileName);
        if (spriteFrame)
        {
            frontCrossFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    frontCrossErrorFilePath = textureFileName;
                }
            }
            else
            {
                frontCrossErrorFilePath = plist;
            }
            frontCrossFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (frontCrossFileExist)
    {
        header->loadTextureFrontCross(frontCrossFileName, (Widget::TextureResType)frontCrossType);
    }

    // load backGroundBoxDisabledData
    bool backGroundBoxDisabledFileExist = false;
    std::string backGroundBoxDisabledErrorFilePath;
    auto backGroundDisabledDic             = options->disableBackFile();
    int backGroundDisabledType             = backGroundDisabledDic->resourceType();
    std::string backGroundDisabledFileName = backGroundDisabledDic->path()->c_str();
    switch (backGroundDisabledType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(backGroundDisabledFileName))
        {
            backGroundBoxDisabledFileExist = true;
        }
        else
        {
            backGroundBoxDisabledErrorFilePath = backGroundDisabledFileName;
            backGroundBoxDisabledFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = backGroundDisabledDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(backGroundDisabledFileName);
        if (spriteFrame)
        {
            backGroundBoxDisabledFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    backGroundBoxDisabledErrorFilePath = textureFileName;
                }
            }
            else
            {
                backGroundBoxDisabledErrorFilePath = plist;
            }
            backGroundBoxDisabledFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (backGroundBoxDisabledFileExist)
    {
        header->loadTextureBackGroundDisabled(backGroundDisabledFileName,
                                              (Widget::TextureResType)backGroundDisabledType);
    }

    /// load frontCrossDisabledData
    bool frontCrossDisabledFileExist = false;
    std::string frontCrossDisabledErrorFilePath;
    auto frontCrossDisabledDic             = options->crossDisableFile();
    int frontCrossDisabledType             = frontCrossDisabledDic->resourceType();
    std::string frontCrossDisabledFileName = frontCrossDisabledDic->path()->c_str();
    switch (frontCrossDisabledType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(frontCrossDisabledFileName))
        {
            frontCrossDisabledFileExist = true;
        }
        else
        {
            frontCrossDisabledErrorFilePath = frontCrossDisabledFileName;
            frontCrossDisabledFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = frontCrossDisabledDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(frontCrossDisabledFileName);
        if (spriteFrame)
        {
            frontCrossDisabledFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value              = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata           = value["metadata"].asValueMap();
                std::string textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    frontCrossDisabledErrorFilePath = textureFileName;
                }
            }
            else
            {
                frontCrossDisabledErrorFilePath = plist;
            }
            frontCrossDisabledFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (frontCrossDisabledFileExist)
    {
        header->loadTextureFrontCrossDisabled(frontCrossDisabledFileName,
                                              (Widget::TextureResType)frontCrossDisabledType);
    }
}

ax::Node* TabHeaderReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    auto node = ax::ui::TabHeader::create();

    auto taboptions = (flatbuffers::TabHeaderOption*)nodeOptions;
    setPropsWithFlatBuffers(node, nodeOptions);

    auto nodereader = WidgetReader::getInstance();
    nodereader->setPropsWithFlatBuffers(node, (Table*)taboptions->nodeOptions());
    return node;
}

int TabHeaderReader::getResourceType(std::string_view key)
{
    if (key == "Normal" || key == "Default")
    {
        return 0;
    }

    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
    if (fbs->_isSimulator)
    {
        if (key == "MarkedSubImage")
        {
            return 0;
        }
    }
    return 1;
}

TabItemReader::TabItemReader() {}

TabItemReader::~TabItemReader() {}
TabItemReader* TabItemReader::_tabItemReaderInstance = nullptr;
TabItemReader* TabItemReader::getInstance()
{
    if (_tabItemReaderInstance == nullptr)
    {
        _tabItemReaderInstance = new TabItemReader();
    }
    return _tabItemReaderInstance;
}

void TabItemReader::destroyInstance()
{
    AX_SAFE_DELETE(_tabItemReaderInstance);
}

flatbuffers::Offset<flatbuffers::TabItemOption> TabItemReader::createTabItemOptionWithFlatBuffers(
    pugi::xml_node objectData,
    flatbuffers::FlatBufferBuilder* builder)
{

    flatbuffers::Offset<Table> header;
    flatbuffers::Offset<NodeTree> container;
    pugi::xml_node containerData;
    pugi::xml_node containerChildrenData;

    auto child = objectData.first_child();
    while (child)
    {
        std::string_view attriname = child.name();
        if (attriname.compare("Children"sv) == 0)
        {
            containerChildrenData = child;
        }
        if (attriname.compare("Header"sv) == 0)
        {
            header = TabHeaderReader::getInstance()->createOptionsWithFlatBuffers(child, builder);
        }
        else if (attriname.compare("Container"sv) == 0)
        {
            containerData = child;
        }
        child = child.next_sibling();
    }

    if (containerChildrenData != nullptr)
    {
        containerData.append_copy(containerChildrenData);
    }

    container = FlatBuffersSerialize::getInstance()->createNodeTree(containerData, "PanelObjectData");

    auto options = CreateTabItemOption(*builder, *(Offset<flatbuffers::TabHeaderOption>*)(&header), container);
    return *(&options);
}

void TabItemReader::setPropsWithFlatBuffers(ax::Node* /*node*/, const flatbuffers::Table* /*nodeOption*/)
{
    // do nothing
}

ax::Node* TabItemReader::createNodeWithFlatBuffers(const flatbuffers::Table* /*nodeOptions*/)
{
    // do nothing
    return nullptr;
}

flatbuffers::Offset<flatbuffers::Table> TabItemReader::createOptionsWithFlatBuffers(
    pugi::xml_node /*objectData*/,
    flatbuffers::FlatBufferBuilder* /*builder*/)
{

    // nothing
    return flatbuffers::Offset<flatbuffers::Table>();
}
