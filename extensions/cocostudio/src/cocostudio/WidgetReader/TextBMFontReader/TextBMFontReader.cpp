

#include "TextBMFontReader.h"

#include "2d/FontAtlasCache.h"
#include "ui/UITextBMFont.h"
#include "platform/FileUtils.h"
#include "cocostudio/CocoLoader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/LocalizationManager.h"

#include "flatbuffers/flatbuffers.h"

USING_NS_AX;
using namespace ui;
using namespace flatbuffers;

namespace cocostudio
{
static const char* P_FileNameData = "fileNameData";
static const char* P_Text         = "text";

static TextBMFontReader* instanceTextBMFontReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(TextBMFontReader)

TextBMFontReader::TextBMFontReader() {}

TextBMFontReader::~TextBMFontReader() {}

TextBMFontReader* TextBMFontReader::getInstance()
{
    if (!instanceTextBMFontReader)
    {
        instanceTextBMFontReader = new TextBMFontReader();
    }
    return instanceTextBMFontReader;
}

void TextBMFontReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceTextBMFontReader);
}

void TextBMFontReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    this->beginSetBasicProperties(widget);

    TextBMFont* labelBMFont = static_cast<TextBMFont*>(widget);

    stExpCocoNode* stChildArray = cocoNode->GetChildArray(cocoLoader);

    for (int i = 0; i < cocoNode->GetChildNum(); ++i)
    {
        std::string key   = stChildArray[i].GetName(cocoLoader);
        std::string value = stChildArray[i].GetValue(cocoLoader);
        // read all basic properties of widget
        AX_BASIC_PROPERTY_BINARY_READER
        // read all color related properties of widget
        AX_COLOR_PROPERTY_BINARY_READER

        else if (key == P_FileNameData)
        {
            stExpCocoNode* backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
            std::string resType               = backGroundChildren[2].GetValue(cocoLoader);

            Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);

            std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);

            if (imageFileNameType == (Widget::TextureResType)0)
            {
                labelBMFont->setFntFile(backgroundValue);
            }
        }
        else if (key == P_Text) { labelBMFont->setString(value); }
    }  // end of for loop
    this->endSetBasicProperties(widget);
}

void TextBMFontReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    WidgetReader::setPropsFromJsonDictionary(widget, options);

    std::string_view jsonPath = GUIReader::getInstance()->getFilePath();

    TextBMFont* labelBMFont = static_cast<TextBMFont*>(widget);

    const rapidjson::Value& cmftDic = DICTOOL->getSubDictionary_json(options, P_FileNameData);
    int cmfType                     = DICTOOL->getIntValue_json(cmftDic, P_ResourceType);
    switch (cmfType)
    {
    case 0:
    {
        std::string tp_c{jsonPath};
        const char* cmfPath = DICTOOL->getStringValue_json(cmftDic, P_Path);
        const char* cmf_tp  = tp_c.append(cmfPath).c_str();
        labelBMFont->setFntFile(cmf_tp);
        break;
    }
    case 1:
        AXLOGD("Wrong res type of LabelAtlas!");
        break;
    default:
        break;
    }

    const char* text = DICTOOL->getStringValue_json(options, P_Text, "Text Label");
    labelBMFont->setString(text);

    WidgetReader::setColorPropsFromJsonDictionary(widget, options);
}

Offset<Table> TextBMFontReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                             flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string text = "Fnt Text Label";
    bool isLocalized = false;

    std::string path;
    std::string plistFlie;
    int resourceType = 0;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "LabelText")
        {
            text = value;
        }
        else if (name == "IsLocalized")
        {
            isLocalized = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    // child elements
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "LabelBMFontFile_CNB")
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
                    plistFlie = value;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    auto options = CreateTextBMFontOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFlie), resourceType),
        builder->CreateString(text), isLocalized);

    return *(Offset<Table>*)(&options);
}

void TextBMFontReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textBMFontOptions)
{
    TextBMFont* labelBMFont = static_cast<TextBMFont*>(node);
    auto options            = (TextBMFontOptions*)textBMFontOptions;

    auto cmftDic   = (options->fileNameData());
    bool fileExist = false;
    std::string errorFilePath;
    std::string errorContent;
    std::string path = cmftDic->path()->c_str();
    int cmfType      = cmftDic->resourceType();
    switch (cmfType)
    {
    case 0:
    {
        if (FileUtils::getInstance()->isFileExist(path))
        {
            FontAtlas* newAtlas = FontAtlasCache::getFontAtlasFNT(path);
            if (newAtlas)
            {
                fileExist = true;
            }
            else
            {
                errorContent = "has problem";
                fileExist    = false;
            }
        }
        break;
    }

    default:
        break;
    }
    if (fileExist)
    {
        labelBMFont->setFntFile(path);
    }

    std::string text = options->text()->c_str();
    bool isLocalized = options->isLocalized() != 0;
    if (isLocalized)
    {
        ILocalizationManager* lm = LocalizationHelper::getCurrentManager();
        labelBMFont->setString(lm->getLocalizationString(text));
    }
    else
    {
        labelBMFont->setString(text);
    }

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    labelBMFont->ignoreContentAdaptWithSize(true);
}

Node* TextBMFontReader::createNodeWithFlatBuffers(const flatbuffers::Table* textBMFontOptions)
{
    TextBMFont* textBMFont = TextBMFont::create();

    setPropsWithFlatBuffers(textBMFont, (Table*)textBMFontOptions);

    return textBMFont;
}

}  // namespace cocostudio
