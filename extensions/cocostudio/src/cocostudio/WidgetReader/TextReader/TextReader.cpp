

#include "cocostudio/WidgetReader/TextReader/TextReader.h"

#include "ui/UIText.h"
#include "2d/Label.h"
#include "platform/FileUtils.h"

#include "cocostudio/CocoLoader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/LocalizationManager.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax;
using namespace ui;
using namespace flatbuffers;

namespace cocostudio
{
static const char* P_TouchScaleEnable = "touchScaleEnable";
static const char* P_Text             = "text";
static const char* P_FontSize         = "fontSize";
static const char* P_FontName         = "fontName";
static const char* P_AreaWidth        = "areaWidth";
static const char* P_AreaHeight       = "areaHeight";
static const char* P_HAlignment       = "hAlignment";
static const char* P_VAlignment       = "vAlignment";

static TextReader* instanceTextReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(TextReader)

TextReader::TextReader() {}

TextReader::~TextReader() {}

TextReader* TextReader::getInstance()
{
    if (!instanceTextReader)
    {
        instanceTextReader = new TextReader();
    }
    return instanceTextReader;
}

void TextReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceTextReader);
}

void TextReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    this->beginSetBasicProperties(widget);

    stExpCocoNode* stChildArray = cocoNode->GetChildArray(cocoLoader);

    Text* label = static_cast<Text*>(widget);

    std::string binaryFilePath{GUIReader::getInstance()->getFilePath()};

    for (int i = 0; i < cocoNode->GetChildNum(); ++i)
    {
        std::string key   = stChildArray[i].GetName(cocoLoader);
        std::string value = stChildArray[i].GetValue(cocoLoader);
        // read all basic properties of widget
        AX_BASIC_PROPERTY_BINARY_READER
        // read all color related properties of widget
        AX_COLOR_PROPERTY_BINARY_READER

        else if (key == P_TouchScaleEnable) { label->setTouchScaleChangeEnabled(valueToBool(value)); }

        else if (key == P_Text) { label->setString(value); }
        else if (key == P_FontSize) { label->setFontSize(valueToInt(value)); }
        else if (key == P_FontName)
        {
            auto& fontFilePath = binaryFilePath.append(value);
            label->setFontName(fontFilePath);
        }
        else if (key == P_AreaWidth)
        {
            label->setTextAreaSize(Size(valueToFloat(value), label->getTextAreaSize().height));
        }
        else if (key == P_AreaHeight)
        {
            label->setTextAreaSize(Size(label->getTextAreaSize().width, valueToFloat(value)));
        }
        else if (key == P_HAlignment) { label->setTextHorizontalAlignment((TextHAlignment)valueToInt(value)); }
        else if (key == P_VAlignment) { label->setTextVerticalAlignment((TextVAlignment)valueToInt(value)); }

    }  // end of for loop
    this->endSetBasicProperties(widget);
}

void TextReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    WidgetReader::setPropsFromJsonDictionary(widget, options);

    std::string fontFilePath{GUIReader::getInstance()->getFilePath()};

    Text* label               = static_cast<Text*>(widget);
    bool touchScaleChangeAble = DICTOOL->getBooleanValue_json(options, P_TouchScaleEnable);
    label->setTouchScaleChangeEnabled(touchScaleChangeAble);
    const char* text = DICTOOL->getStringValue_json(options, P_Text, "Text Label");
    label->setString(text);

    label->setFontSize(DICTOOL->getIntValue_json(options, P_FontSize, 20));

    auto fontName = DICTOOL->getStringValue_json(options, P_FontName, "");

    fontFilePath.append(fontName);
    if (FileUtils::getInstance()->isFileExist(fontFilePath))
    {
        label->setFontName(fontFilePath);
    }
    else
    {
        label->setFontName(fontName);
    }

    bool aw = DICTOOL->checkObjectExist_json(options, P_AreaWidth);
    bool ah = DICTOOL->checkObjectExist_json(options, P_AreaHeight);
    if (aw && ah)
    {
        Size size =
            Size(DICTOOL->getFloatValue_json(options, P_AreaWidth), DICTOOL->getFloatValue_json(options, P_AreaHeight));
        label->setTextAreaSize(size);
    }
    bool ha = DICTOOL->checkObjectExist_json(options, P_HAlignment);
    if (ha)
    {
        label->setTextHorizontalAlignment((TextHAlignment)DICTOOL->getIntValue_json(options, P_HAlignment));
    }
    bool va = DICTOOL->checkObjectExist_json(options, P_VAlignment);
    if (va)
    {
        label->setTextVerticalAlignment((TextVAlignment)DICTOOL->getIntValue_json(options, P_VAlignment));
    }

    WidgetReader::setColorPropsFromJsonDictionary(widget, options);
}

Offset<Table> TextReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                       flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    bool touchScaleEnabled = false;
    bool isCustomSize      = false;
    std::string fontName;
    int fontSize         = 20;
    std::string text     = "Text Label";
    bool isLocalized     = false;
    int areaWidth        = 0;
    int areaHeight       = 0;
    int h_alignment      = 0;
    int v_alignment      = 0;
    bool outlineEnabled  = false;
    Color4B outlineColor = Color4B::BLACK;
    int outlineSize      = 1;
    bool shadowEnabled   = false;
    Color4B shadowColor  = Color4B::BLACK;
    Size shadowOffset    = Size(2, -2);
    int shadowBlurRadius = 0;

    // since x-studio reader 10.0.593.0
    bool glowEnabled  = false;
    Color4B glowColor = Color4B::BLACK;

    bool boldEnabled = false, underlineEnabled = false, italicsEnabled = false, strikethroughEnabled = false;

    std::string path;
    std::string plistFile;
    int resourceType = 0;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "TouchScaleChangeAble")
        {
            touchScaleEnabled = (value == "True") ? true : false;
        }
        else if (name == "LabelText")
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
        else if (name == "AreaWidth")
        {
            areaWidth = atoi(value.data());
        }
        else if (name == "AreaHeight")
        {
            areaHeight = atoi(value.data());
        }
        else if (name == "HorizontalAlignmentType")
        {
            if (value == "HT_Left")
            {
                h_alignment = 0;
            }
            else if (value == "HT_Center")
            {
                h_alignment = 1;
            }
            else if (value == "HT_Right")
            {
                h_alignment = 2;
            }
        }
        else if (name == "VerticalAlignmentType")
        {
            if (value == "VT_Top")
            {
                v_alignment = 0;
            }
            else if (value == "VT_Center")
            {
                v_alignment = 1;
            }
            else if (value == "VT_Bottom")
            {
                v_alignment = 2;
            }
        }
        else if (name == "IsCustomSize")
        {
            isCustomSize = (value == "True") ? true : false;
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

        if (name == "FontResource")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = 0;
                }
                else if (name == "Plist")
                {
                    plistFile = value;
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

    FColor f_outlineColor(outlineColor.a, outlineColor.r, outlineColor.g, outlineColor.b);
    FColor f_shadowColor(shadowColor.a, shadowColor.r, shadowColor.g, shadowColor.b);
    FColor f_glowColor(glowColor.a, glowColor.r, glowColor.g, glowColor.b);

    auto options = CreateTextOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        builder->CreateString(fontName), fontSize, builder->CreateString(text), areaWidth, areaHeight, h_alignment,
        v_alignment, touchScaleEnabled, isCustomSize, outlineEnabled, &f_outlineColor, outlineSize, shadowEnabled,
        &f_shadowColor, shadowOffset.width, shadowOffset.height, shadowBlurRadius, isLocalized, glowEnabled,
        &f_glowColor, boldEnabled, underlineEnabled, italicsEnabled, strikethroughEnabled);

    return *(Offset<Table>*)(&options);
}

void TextReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textOptions)
{
    Text* label  = static_cast<Text*>(node);
    auto options = (TextOptions*)textOptions;

    bool touchScaleEnabled = options->touchScaleEnable() != 0;
    label->setTouchScaleChangeEnabled(touchScaleEnabled);

    int fontSize = options->fontSize();
    label->setFontSize(fontSize);

    Size areaSize = Size(options->areaWidth(), options->areaHeight());
    if (!areaSize.equals(Size::ZERO))
    {
        label->setTextAreaSize(areaSize);
    }

    auto resourceDataDic = (options->fontResource());
    std::string path     = resourceDataDic->path()->c_str();
    if (!path.empty() && FileUtils::getInstance()->isFileExist(path))
    {
        label->setFontName(path);
    }
    else
    {
        path = options->fontName()->c_str();
        label->setFontName(path);
    }

    TextHAlignment h_alignment = (TextHAlignment)options->hAlignment();
    label->setTextHorizontalAlignment(h_alignment);

    TextVAlignment v_alignment = (TextVAlignment)options->vAlignment();
    label->setTextVerticalAlignment((TextVAlignment)v_alignment);

    bool outlineEnabled = options->outlineEnabled() != 0;
    if (outlineEnabled)
    {
        auto f_outlineColor = options->outlineColor();
        if (f_outlineColor)
        {
            Color4B outlineColor(f_outlineColor->r(), f_outlineColor->g(), f_outlineColor->b(), f_outlineColor->a());
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
            label->enableGlow(glowColor);
        }
    }

    std::string text = options->text()->c_str();
    bool isLocalized = options->isLocalized() != 0;
    if (isLocalized)
    {
        ILocalizationManager* lm = LocalizationHelper::getCurrentManager();
        label->setString(lm->getLocalizationString(text));
    }
    else
    {
        label->setString(text);
    }

    // Save node color before set widget properties
    auto oldColor = node->getColor();

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    // restore node color and set color to text to fix shadow & outline color won't show correct bug
    node->setColor(oldColor);
    auto optionsWidget = (WidgetOptions*)options->widgetOptions();
    auto f_color       = optionsWidget->color();
    Color4B color(f_color->r(), f_color->g(), f_color->b(), f_color->a());
    ((Text*)node)->setTextColor(color);

    label->setUnifySizeEnabled(false);

    bool IsCustomSize = options->isCustomSize() != 0;
    label->ignoreContentAdaptWithSize(!IsCustomSize);

    auto widgetOptions = options->widgetOptions();
    if (!label->isIgnoreContentAdaptWithSize())
    {
        Size contentSize(widgetOptions->size()->width(), widgetOptions->size()->height());
        label->setContentSize(contentSize);
    }

    auto labelRenderer = dynamic_cast<ax::Label*>(label->getVirtualRenderer());
    if (options->boldEnabled())
        labelRenderer->enableBold();
    if (options->underlineEnabled())
        labelRenderer->enableUnderline();
    if (options->italicsEnabled())
        labelRenderer->enableItalics();
    if (options->strikethroughEnabled())
        labelRenderer->enableStrikethrough();
}

Node* TextReader::createNodeWithFlatBuffers(const flatbuffers::Table* textOptions)
{
    Text* text = Text::create();

    setPropsWithFlatBuffers(text, (Table*)textOptions);

    return text;
}

}  // namespace cocostudio
