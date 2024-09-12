

#include "ButtonReader.h"

#include "ui/UIButton.h"
#include "2d/SpriteFrameCache.h"
#include "2d/Label.h"
#include "platform/FileUtils.h"
#include "cocostudio/CocoLoader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/LocalizationManager.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax;
using namespace ui;
using namespace flatbuffers;

namespace cocostudio
{

static const char* P_Scale9Enable    = "scale9Enable";
static const char* P_NormalData      = "normalData";
static const char* P_PressedData     = "pressedData";
static const char* P_DisabledData    = "disabledData";
static const char* P_Text            = "text";
static const char* P_CapInsetsX      = "capInsetsX";
static const char* P_CapInsetsY      = "capInsetsY";
static const char* P_CapInsetsWidth  = "capInsetsWidth";
static const char* P_CapInsetsHeight = "capInsetsHeight";
static const char* P_Scale9Width     = "scale9Width";
static const char* P_Scale9Height    = "scale9Height";
static const char* P_TextColorR      = "textColorR";
static const char* P_TextColorG      = "textColorG";
static const char* P_TextColorB      = "textColorB";
static const char* P_FontSize        = "fontSize";
static const char* P_FontName        = "fontName";

static ButtonReader* instanceButtonReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(ButtonReader)

ButtonReader::ButtonReader() {}

ButtonReader::~ButtonReader() {}

ButtonReader* ButtonReader::getInstance()
{
    if (!instanceButtonReader)
    {
        instanceButtonReader = new ButtonReader();
    }
    return instanceButtonReader;
}

void ButtonReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceButtonReader);
}

void ButtonReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    WidgetReader::setPropsFromBinary(widget, cocoLoader, cocoNode);

    Button* button = static_cast<Button*>(widget);

    stExpCocoNode* stChildArray = cocoNode->GetChildArray(cocoLoader);

    this->beginSetBasicProperties(widget);

    float capsx = 0.0f, capsy = 0.0, capsWidth = 0.0, capsHeight = 0.0f;
    int cri = 255, cgi = 255, cbi = 255;
    float scale9Width = 0.0f, scale9Height = 0.0f;
    for (int i = 0; i < cocoNode->GetChildNum(); ++i)
    {
        std::string key   = stChildArray[i].GetName(cocoLoader);
        std::string value = stChildArray[i].GetValue(cocoLoader);

        // read all basic properties of widget
        AX_BASIC_PROPERTY_BINARY_READER
        // read all color related properties of widget
        AX_COLOR_PROPERTY_BINARY_READER

        else if (key == P_Scale9Enable) { button->setScale9Enabled(valueToBool(value)); }
        else if (key == P_NormalData)
        {

            stExpCocoNode* backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
            std::string resType               = backGroundChildren[2].GetValue(cocoLoader);

            Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);

            std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);

            // assets[0] = backgroundValue;
            button->loadTextureNormal(backgroundValue, imageFileNameType);
        }
        else if (key == P_PressedData)
        {

            stExpCocoNode* backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
            std::string resType               = backGroundChildren[2].GetValue(cocoLoader);

            Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);

            std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);

            button->loadTexturePressed(backgroundValue, imageFileNameType);
        }
        else if (key == P_DisabledData)
        {

            stExpCocoNode* backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
            std::string resType               = backGroundChildren[2].GetValue(cocoLoader);

            Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);

            std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);

            button->loadTextureDisabled(backgroundValue, imageFileNameType);
        }
        else if (key == P_Text) { button->setTitleText(value); }
        else if (key == P_CapInsetsX) { capsx = valueToFloat(value); }
        else if (key == P_CapInsetsY) { capsy = valueToFloat(value); }
        else if (key == P_CapInsetsWidth) { capsWidth = valueToFloat(value); }
        else if (key == P_CapInsetsHeight) { capsHeight = valueToFloat(value); }
        else if (key == P_Scale9Width) { scale9Width = valueToFloat(value); }
        else if (key == P_Scale9Height) { scale9Height = valueToFloat(value); }
        else if (key == P_TextColorR) { cri = valueToInt(value); }
        else if (key == P_TextColorG) { cgi = valueToInt(value); }
        else if (key == P_TextColorB) { cbi = valueToInt(value); }
        else if (key == P_FontSize) { button->setTitleFontSize(valueToFloat(value)); }
        else if (key == P_FontName)
        {
            button->setTitleFontName(value);  // fonts
        }

    }  // end of for loop

    this->endSetBasicProperties(widget);

    if (button->isScale9Enabled())
    {
        button->setCapInsets(Rect(capsx, capsy, capsWidth, capsHeight));
        button->setContentSize(Size(scale9Width, scale9Height));
    }

    button->setTitleColor(Color3B(cri, cgi, cbi));
}

void ButtonReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    WidgetReader::setPropsFromJsonDictionary(widget, options);

    Button* button = static_cast<Button*>(widget);

    auto pHackAssets = new std::vector<std::string>();
    button->setUserData(pHackAssets);
    auto& hackAssets = *pHackAssets;
    hackAssets.resize(4);

    bool scale9Enable = DICTOOL->getBooleanValue_json(options, P_Scale9Enable);
    button->setScale9Enabled(scale9Enable);

    const rapidjson::Value& normalDic = DICTOOL->getSubDictionary_json(options, P_NormalData);
    int normalType                    = DICTOOL->getIntValue_json(normalDic, P_ResourceType);
    std::string normalTexturePath     = this->getResourcePath(normalDic, P_Path, (Widget::TextureResType)normalType);
    hackAssets[0]                     = normalTexturePath;
    button->loadTextureNormal(normalTexturePath, (Widget::TextureResType)normalType);

    const rapidjson::Value& pressedDic = DICTOOL->getSubDictionary_json(options, P_PressedData);
    int pressedType                    = DICTOOL->getIntValue_json(pressedDic, P_ResourceType);

    std::string pressedTexturePath = this->getResourcePath(pressedDic, P_Path, (Widget::TextureResType)pressedType);
    hackAssets[1]                  = pressedTexturePath;
    button->loadTexturePressed(pressedTexturePath, (Widget::TextureResType)pressedType);

    const rapidjson::Value& disabledDic = DICTOOL->getSubDictionary_json(options, P_DisabledData);
    int disabledType                    = DICTOOL->getIntValue_json(disabledDic, P_ResourceType);

    std::string disabledTexturePath = this->getResourcePath(disabledDic, P_Path, (Widget::TextureResType)disabledType);
    hackAssets[2]                   = disabledTexturePath;
    button->loadTextureDisabled(disabledTexturePath, (Widget::TextureResType)disabledType);

    if (scale9Enable)
    {
        float cx = DICTOOL->getFloatValue_json(options, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(options, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(options, P_CapInsetsWidth);
        float ch = DICTOOL->getFloatValue_json(options, P_CapInsetsHeight);

        button->setCapInsets(Rect(cx, cy, cw, ch));
        bool sw = DICTOOL->checkObjectExist_json(options, P_Scale9Width);
        bool sh = DICTOOL->checkObjectExist_json(options, P_Scale9Height);
        if (sw && sh)
        {
            float swf = DICTOOL->getFloatValue_json(options, P_Scale9Width);
            float shf = DICTOOL->getFloatValue_json(options, P_Scale9Height);
            button->setContentSize(Size(swf, shf));
        }
    }
    bool tt = DICTOOL->checkObjectExist_json(options, P_Text);
    if (tt)
    {
        const char* text = DICTOOL->getStringValue_json(options, P_Text);
        if (text)
        {
            button->setTitleText(text);
        }
    }

    int cri = DICTOOL->getIntValue_json(options, P_TextColorR, 255);
    int cgi = DICTOOL->getIntValue_json(options, P_TextColorG, 255);
    int cbi = DICTOOL->getIntValue_json(options, P_TextColorB, 255);
    button->setTitleColor(Color3B(cri, cgi, cbi));

    button->setTitleFontSize(DICTOOL->getIntValue_json(options, P_FontSize, 14));

    button->setTitleFontName(DICTOOL->getStringValue_json(options, P_FontName, ""));
    hackAssets[3] = button->getTitleFontName();

    WidgetReader::setColorPropsFromJsonDictionary(widget, options);
}

Offset<Table> ButtonReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                         flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    bool displaystate  = true;
    bool scale9Enabled = false;
    Rect capInsets;
    std::string text;
    bool isLocalized = false;
    int fontSize     = 14;
    std::string fontName;
    ax::Size scale9Size;
    Color4B textColor(255, 255, 255, 255);

    std::string normalPath;
    std::string normalPlistFile;
    int normalResourceType = 0;

    std::string pressedPath;
    std::string pressedPlistFile;
    int pressedResourceType = 0;

    std::string disabledPath;
    std::string disabledPlistFile;
    int disabledResourceType = 0;

    std::string fontResourcePath;
    std::string fontResourcePlistFile;
    int fontResourceResourceType = 0;

    bool outlineEnabled  = false;
    Color4B outlineColor = Color4B::BLACK;
    int outlineSize      = 1;
    bool shadowEnabled   = false;
    Color4B shadowColor  = Color4B::BLACK;
    Size shadowOffset    = Size(2, -2);
    int shadowBlurRadius = 0;

    bool glowEnabled  = false;
    Color4B glowColor = Color4B::BLACK;

    bool boldEnabled = false, underlineEnabled = false, italicsEnabled = false, strikethroughEnabled = false;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "Scale9Enable")
        {
            if (value == "True")
            {
                scale9Enabled = true;
            }
        }
        else if (name == "Scale9OriginX")
        {
            capInsets.origin.x = atof(value.data());
        }
        else if (name == "Scale9OriginY")
        {
            capInsets.origin.y = atof(value.data());
        }
        else if (name == "Scale9Width")
        {
            capInsets.size.width = atof(value.data());
        }
        else if (name == "Scale9Height")
        {
            capInsets.size.height = atof(value.data());
        }
        else if (name == "ButtonText")
        {
            text = value;
        }
        else if (name == "IsLocalized")
        {
            isLocalized = (value == "True") ? true : false;
        }
        else if (name == "FontSize")
        {
            fontSize = atoi(value.data());
        }
        else if (name == "FontName")
        {
            fontName = value;
        }
        else if (name == "DisplayState")
        {
            displaystate = (value == "True") ? true : false;
        }
        else if (name == "OutlineEnabled")
        {
            outlineEnabled = (value == "True") ? true : false;
        }
        else if (name == "OutlineSize")
        {
            outlineSize = atoi(value.data());
        }
        else if (name == "ShadowEnabled")
        {
            shadowEnabled = (value == "True") ? true : false;
        }
        else if (name == "ShadowOffsetX")
        {
            shadowOffset.width = atof(value.data());
        }
        else if (name == "ShadowOffsetY")
        {
            shadowOffset.height = atof(value.data());
        }
        else if (name == "ShadowBlurRadius")
        {
            shadowBlurRadius = atoi(value.data());
        }
        else if (name == "GlowEnabled")
        {
            glowEnabled = (value == "True") ? true : false;
        }
        else if (name == "BoldEnabled")
        {
            boldEnabled = (value == "True") ? true : false;
        }
        else if (name == "UnderlineEnabled")
        {
            underlineEnabled = (value == "True") ? true : false;
        }
        else if (name == "ItalicsEnabled")
        {
            italicsEnabled = (value == "True") ? true : false;
        }
        else if (name == "StrikethroughEnabled")
        {
            strikethroughEnabled = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    // child elements
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "Size" && scale9Enabled)
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "X")
                {
                    scale9Size.width = atof(value.data());
                }
                else if (name == "Y")
                {
                    scale9Size.height = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "TextColor")
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
        else if (name == "DisabledFileData")
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
                    disabledPath = value;
                }
                else if (name == "Type")
                {
                    disabledResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    disabledPlistFile = value;
                    texture           = value;
                }

                attribute = attribute.next_attribute();
            }

            if (disabledResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "PressedFileData")
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
                    pressedPath = value;
                }
                else if (name == "Type")
                {
                    pressedResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    pressedPlistFile = value;
                    texture          = value;
                }

                attribute = attribute.next_attribute();
            }

            if (pressedResourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }
        else if (name == "NormalFileData")
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
                    normalPath = value;
                }
                else if (name == "Type")
                {
                    normalResourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    normalPlistFile = value;
                    texture         = value;
                }

                attribute = attribute.next_attribute();
            }

            if (normalResourceType == 1)
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
        else if (name == "OutlineColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "A")
                {
                    outlineColor.a = atoi(value.data());
                }
                else if (name == "R")
                {
                    outlineColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    outlineColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    outlineColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "ShadowColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "A")
                {
                    shadowColor.a = atoi(value.data());
                }
                else if (name == "R")
                {
                    shadowColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    shadowColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    shadowColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "GlowColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "A")
                {
                    glowColor.a = atoi(value.data());
                }
                else if (name == "R")
                {
                    glowColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    glowColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    glowColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    Color f_textColor(255, textColor.r, textColor.g, textColor.b);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    flatbuffers::Color f_outlineColor(outlineColor.a, outlineColor.r, outlineColor.g, outlineColor.b);
    flatbuffers::Color f_shadowColor(shadowColor.a, shadowColor.r, shadowColor.g, shadowColor.b);
    flatbuffers::Color f_glowColor(glowColor.a, glowColor.r, glowColor.g, glowColor.b);

    auto options = CreateButtonOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(normalPath), builder->CreateString(normalPlistFile),
                           normalResourceType),
        CreateResourceData(*builder, builder->CreateString(pressedPath), builder->CreateString(pressedPlistFile),
                           pressedResourceType),
        CreateResourceData(*builder, builder->CreateString(disabledPath), builder->CreateString(disabledPlistFile),
                           disabledResourceType),
        CreateResourceData(*builder, builder->CreateString(fontResourcePath),
                           builder->CreateString(fontResourcePlistFile), fontResourceResourceType),
        builder->CreateString(text), builder->CreateString(fontName), fontSize, &f_textColor, &f_capInsets,
        &f_scale9Size, scale9Enabled, displaystate, outlineEnabled, &f_outlineColor, outlineSize, shadowEnabled,
        &f_shadowColor, shadowOffset.width, shadowOffset.height, shadowBlurRadius, glowEnabled, isLocalized,
        &f_glowColor, boldEnabled, underlineEnabled, italicsEnabled, strikethroughEnabled);

    return *(Offset<Table>*)(&options);
}

void ButtonReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* buttonOptions)
{
    Button* button = static_cast<Button*>(node);

    auto options = (ButtonOptions*)buttonOptions;

    bool scale9Enabled = options->scale9Enabled() != 0;
    button->setScale9Enabled(scale9Enabled);

    bool normalFileExist = false;
    std::string normalErrorFilePath;
    auto normalDic                = options->normalData();
    int normalType                = normalDic->resourceType();
    std::string normalTexturePath = normalDic->path()->c_str();
    switch (normalType)
    {
    case 0:
        if (FileUtils::getInstance()->isFileExist(normalTexturePath))
        {
            normalFileExist = true;
        }
        else if (SpriteFrameCache::getInstance()->findFrame(normalTexturePath))
        {
            normalFileExist = true;
            normalType      = 1;
        }
        else
        {
            normalErrorFilePath = std::move(normalTexturePath);
            normalFileExist     = false;
        }
        break;

    case 1:
    {
        std::string plist        = normalDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(normalTexturePath);
        if (spriteFrame)
        {
            normalFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value       = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata    = value["metadata"].asValueMap();
                auto textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    normalErrorFilePath = textureFileName;
                }
            }
            else
            {
                normalErrorFilePath = std::move(plist);
            }
            normalFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (normalFileExist)
    {
        button->loadTextureNormal(normalTexturePath, (Widget::TextureResType)normalType);
    }

    bool pressedFileExist = false;
    std::string pressedErrorFilePath;
    auto pressedDic                = options->pressedData();
    int pressedType                = pressedDic->resourceType();
    std::string pressedTexturePath = pressedDic->path()->c_str();
    switch (pressedType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(pressedTexturePath))
        {
            pressedFileExist = true;
        }
        else
        {
            pressedErrorFilePath = pressedTexturePath;
            pressedFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = pressedDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(pressedTexturePath);
        if (spriteFrame)
        {
            pressedFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value       = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata    = value["metadata"].asValueMap();
                auto textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    pressedErrorFilePath = textureFileName;
                }
            }
            else
            {
                pressedErrorFilePath = std::move(plist);
            }
            pressedFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (pressedFileExist)
    {
        button->loadTexturePressed(pressedTexturePath, (Widget::TextureResType)pressedType);
    }

    bool disabledFileExist = false;
    std::string disabledErrorFilePath;
    auto disabledDic                = options->disabledData();
    int disabledType                = disabledDic->resourceType();
    std::string disabledTexturePath = disabledDic->path()->c_str();
    switch (disabledType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(disabledTexturePath))
        {
            disabledFileExist = true;
        }
        else
        {
            disabledErrorFilePath = disabledTexturePath;
            disabledFileExist     = false;
        }
        break;
    }

    case 1:
    {
        std::string plist        = disabledDic->plistFile()->c_str();
        SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(disabledTexturePath);
        if (spriteFrame)
        {
            disabledFileExist = true;
        }
        else
        {
            if (FileUtils::getInstance()->isFileExist(plist))
            {
                ValueMap value       = FileUtils::getInstance()->getValueMapFromFile(plist);
                ValueMap metadata    = value["metadata"].asValueMap();
                auto textureFileName = metadata["textureFileName"].asString();
                if (!FileUtils::getInstance()->isFileExist(textureFileName))
                {
                    disabledErrorFilePath = textureFileName;
                }
            }
            else
            {
                disabledErrorFilePath = std::move(plist);
            }
            disabledFileExist = false;
        }
        break;
    }

    default:
        break;
    }
    if (disabledFileExist)
    {
        button->loadTextureDisabled(disabledTexturePath, (Widget::TextureResType)disabledType);
    }

    std::string titleText = options->text()->c_str();
    bool isLocalized      = options->isLocalized() != 0;
    if (isLocalized)
    {
        ILocalizationManager* lm = LocalizationHelper::getCurrentManager();
        button->setTitleText(lm->getLocalizationString(titleText));
    }
    else
    {
        button->setTitleText(titleText);
    }

    auto textColor = options->textColor();
    Color3B titleColor(textColor->r(), textColor->g(), textColor->b());
    button->setTitleColor(titleColor);

    std::string titleFontName = options->fontName()->c_str();
    button->setTitleFontName(titleFontName);

    auto resourceData = options->fontResource();
    std::string errorFilePath;
    std::string path = resourceData->path()->c_str();
    if (!path.empty())
    {
        if (FileUtils::getInstance()->isFileExist(path))
            button->setTitleFontName(path);
        else
            errorFilePath = path;
    }

    int titleFontSize = options->fontSize();
    button->setTitleFontSize(titleFontSize);

    bool displaystate = options->displaystate() != 0;
    button->setBright(displaystate);
    button->setEnabled(displaystate);

    bool outlineEnabled = options->outlineEnabled() != 0;
    if (outlineEnabled)
    {
        auto f_outlineColor = options->outlineColor();
        if (f_outlineColor)
        {
            Color4B outlineColor(f_outlineColor->r(), f_outlineColor->g(), f_outlineColor->b(), f_outlineColor->a());
            auto label = button->getTitleRenderer();
            label->enableOutline(outlineColor, options->outlineSize());
        }
    }

    bool shadowEnabled = options->shadowEnabled() != 0;
    if (shadowEnabled)
    {
        auto f_shadowColor = options->shadowColor();
        if (f_shadowColor)
        {
            Color4B shadowColor(f_shadowColor->r(), f_shadowColor->g(), f_shadowColor->b(), f_shadowColor->a());
            auto label = button->getTitleRenderer();
            label->enableShadow(shadowColor, Size(options->shadowOffsetX(), options->shadowOffsetY()),
                                options->shadowBlurRadius());
        }
    }

    if (options->glowEnabled() != 0)
    {
        auto f_glowColor = options->glowColor();
        if (f_glowColor)
        {
            Color4B glowColor(f_glowColor->r(), f_glowColor->g(), f_glowColor->b(), f_glowColor->a());
            auto label = button->getTitleRenderer();
            label->enableGlow(glowColor);
        }
    }

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    if (scale9Enabled)
    {
        button->setUnifySizeEnabled(false);
        button->ignoreContentAdaptWithSize(false);

        auto f_capInsets = options->capInsets();
        Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
        button->setCapInsets(capInsets);

        Size scale9Size(options->scale9Size()->width(), options->scale9Size()->height());
        button->setContentSize(scale9Size);
    }
    else
    {
        Size contentSize(options->widgetOptions()->size()->width(), options->widgetOptions()->size()->height());
        button->setContentSize(contentSize);
    }

    button->setBright(displaystate);

    auto labelRenderer = dynamic_cast<ax::Label*>(button->getTitleRenderer());
    if (labelRenderer != nullptr)
    {
        if (options->boldEnabled())
            labelRenderer->enableBold();
        if (options->underlineEnabled())
            labelRenderer->enableUnderline();
        if (options->italicsEnabled())
            labelRenderer->enableItalics();
        if (options->strikethroughEnabled())
            labelRenderer->enableStrikethrough();
    }
}

Node* ButtonReader::createNodeWithFlatBuffers(const flatbuffers::Table* buttonOptions)
{
    Button* button = Button::create();

    setPropsWithFlatBuffers(button, (Table*)buttonOptions);

    return button;
}

int ButtonReader::getResourceType(std::string_view key)
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

}  // namespace cocostudio
