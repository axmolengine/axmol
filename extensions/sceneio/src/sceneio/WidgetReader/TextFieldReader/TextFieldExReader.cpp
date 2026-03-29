/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "sceneio/WidgetReader/TextFieldReader/TextFieldExReader.h"

#include "axmol/ui/UITextFieldEx.h"
#include "axmol/platform/FileUtils.h"
#include "sceneext/CocoLoader.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/LocalizationManager.h"

#include "sceneio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax::ui;
using namespace flatbuffers;

inline ax::Color32 Color32FromFb(const FColor* pColor)
{
    return ax::Color32(pColor->r(), pColor->g(), pColor->b(), pColor->a());
}

namespace ax::ext
{
static TextFieldExReader* instanceTextFieldExReader = nullptr;

static const char* P_PlaceHolder       = "placeHolder";
static const char* P_Text              = "text";
static const char* P_FontSize          = "fontSize";
static const char* P_FontName          = "fontName";
static const char* P_TouchSizeWidth    = "touchSizeWidth";
static const char* P_TouchSizeHeight   = "touchSizeHeight";
static const char* P_MaxLengthEnable   = "maxLengthEnable";
static const char* P_MaxLength         = "maxLength";
static const char* P_PasswordEnable    = "passwordEnable";
static const char* P_PasswordStyleText = "passwordStyleText";

IMPLEMENT_CLASS_NODE_READER_INFO(TextFieldExReader)

TextFieldExReader::TextFieldExReader() {}

TextFieldExReader::~TextFieldExReader() {}

TextFieldExReader* TextFieldExReader::getInstance()
{
    if (!instanceTextFieldExReader)
    {
        instanceTextFieldExReader = new TextFieldExReader();
    }
    return instanceTextFieldExReader;
}

void TextFieldExReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceTextFieldExReader);
}

Offset<Table> TextFieldExReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                              flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string path;
    std::string plistFile;
    int resourceType = 0;

    std::string fontName;
    int fontSize = 20;
    std::string text;
    bool isLocalized              = false;
    std::string placeHolder       = "Text Field Extend";
    bool passwordEnabled          = false;
    std::string passwordStyleText = "*";
    bool maxLengthEnabled         = false;
    int maxLength                 = 10;
    bool isEnabled                = true;
    bool isEditable               = true;
    Color32 textColor;
    Color32 placeholderColor;
    Color32 cursorColor;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name  = attribute.name();
        auto value = attribute.value();

        if (name == "PlaceHolderText")
        {
            placeHolder = value;
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
        else if (name == "MaxLengthText")
        {
            maxLength = atoi(value.data());
        }
        else if (name == "PasswordEnable")
        {
            passwordEnabled = (value == "True") ? true : false;
        }
        else if (name == "PasswordStyleText")
        {
            passwordStyleText = value;
        }
        else if (name == "Enabled")
        {
            isEnabled = (value == "True") ? true : false;
        }
        else if (name == "Editable")
        {
            isEditable = (value == "True") ? true : false;
        }

        attribute = attribute.next_attribute();
    }

    // child elements
    auto child = objectData.first_child();
    while (child)
    {
        auto name = child.name();

        if (name == "FontResource")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

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
        else if (name == "TextColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

                if (name == "A")
                {
                    textColor.a = atoi(value.data());
                }
                else if (name == "R")
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
        else if (name == "PlaceholderColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

                if (name == "A")
                {
                    placeholderColor.a = atoi(value.data());
                }
                else if (name == "R")
                {
                    placeholderColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    placeholderColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    placeholderColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "CursorColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name       = attribute.name();
                auto value = attribute.value();

                if (name == "A")
                {
                    cursorColor.a = atoi(value.data());
                }
                else if (name == "R")
                {
                    cursorColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    cursorColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    cursorColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    FColor f_textColor(textColor.a, textColor.r, textColor.g, textColor.b);
    FColor f_placeholderColor(placeholderColor.a, placeholderColor.r, placeholderColor.g, placeholderColor.b);
    FColor f_cursorColor(255, cursorColor.r, cursorColor.g, cursorColor.b);
    auto options = CreateTextFieldExOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        builder->CreateString(fontName), fontSize, builder->CreateString(text), builder->CreateString(placeHolder),
        passwordEnabled, builder->CreateString(passwordStyleText), maxLength, &f_textColor, &f_placeholderColor,
        &f_cursorColor, isEnabled, isEditable, isLocalized);

    return *(Offset<Table>*)(&options);
}

void TextFieldExReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* textFieldOptions)
{
    TextFieldEx** pTextField = (TextFieldEx**)(node);
    auto options             = (TextFieldExOptions*)textFieldOptions;

    std::string placeholder = options->placeholderText()->c_str();

    std::string text = options->text()->c_str();
    bool isLocalized = options->isLocalized();
    if (isLocalized)
    {
        ILocalizationManager* lm            = LocalizationHelper::getCurrentManager();
        std::string localizedTxt            = lm->getLocalizationString(text);
        std::string::size_type newlineIndex = localizedTxt.find("\n");
        if (newlineIndex != std::string::npos)
            localizedTxt = localizedTxt.substr(0, newlineIndex);
        text = (localizedTxt);
    }

    int fontSize = options->fontSize();

    std::string fontName = options->fontName()->c_str();

    bool fileExist = false;
    std::string errorFilePath;
    auto resourceDataDic = (options->fontResource());
    std::string path     = resourceDataDic->path()->c_str();
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
            fontName = (path);
        }
    }

    // bool maxLengthEnabled = options->maxLengthEnabled() != 0;
    // textField->setMaxLengthEnabled(maxLengthEnabled);
    auto textField = TextFieldEx::create(placeholder, fontName, fontSize);

    *pTextField = textField;

    int maxLength = options->maxLength();
    textField->setMaxLength(maxLength);
    bool passwordEnabled = options->passwordEnabled() != 0;

    textField->setPasswordEnabled(passwordEnabled);

    /*if (passwordEnabled)
    {
        std::string passwordStyleText = options->passwordStyleText()->c_str();
        textField->setPasswordStyleText(passwordStyleText.c_str());
    }*/
    textField->setString(text);

    textField->setTextColor(Color32FromFb(options->textColor()));
    textField->setPlaceholderColor(Color32FromFb(options->placeholderColor()));
    textField->setCursorColor(Color32FromFb(options->cursorColor()));

    auto widgetReader = NodeReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(textField, (Table*)options->nodeOptions());

    // textField->setUnifySizeEnabled(false);
    // textField->ignoreContentAdaptWithSize(false);

    auto widgetOptions = options->nodeOptions();

    Size contentSize(widgetOptions->size()->width(), widgetOptions->size()->height());
    textField->setContentSize(contentSize);

    textField->setEnabled(options->enabled());
    textField->setEditable(options->editable());

    textField->enableIME(nullptr);
    /*if (!textField->isIgnoreContentAdaptWithSize())
    {
        ((Label*)(textField->getVirtualRenderer()))->setLineBreakWithoutSpace(true);
    }*/
}

Node* TextFieldExReader::createNodeWithFlatBuffers(const flatbuffers::Table* textFieldOptions)
{
    TextFieldEx* textField = nullptr;  // TextFieldEx::create("dummy", "Courier New", 18);

    setPropsWithFlatBuffers((Node*)&textField, (Table*)textFieldOptions);

    return textField;
}

}  // namespace ax::ext
