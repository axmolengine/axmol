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

#include "sceneio/WidgetReader/RichTextReader/RichTextReader.h"

#include "axmol/2d/FontAtlasCache.h"
#include "axmol/ui/RichText.h"
#include "axmol/platform/FileUtils.h"
#include "sceneext/CocoLoader.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/LocalizationManager.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax::ui;
using namespace flatbuffers;

namespace ax::ext
{
static RichTextReader* instanceTextBMFontReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(RichTextReader)

RichTextReader::RichTextReader() {}

RichTextReader::~RichTextReader() {}

RichTextReader* RichTextReader::getInstance()
{
    if (!instanceTextBMFontReader)
    {
        instanceTextBMFontReader = new RichTextReader();
    }
    return instanceTextBMFontReader;
}

void RichTextReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceTextBMFontReader);
}

Offset<Table> RichTextReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                           flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string xmlDefinition = "RichText";
    // bool isLocalized = false;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name  = attribute.name();
        auto value = attribute.value();

        if (name == "XmlDefinition")
        {
            xmlDefinition = value;
            break;
        }
        /*else if (name == "IsLocalized")
        {
            isLocalized = (value == "True") ? true : false;
        }*/

        attribute = attribute.next_attribute();
    }

    auto options = CreateRichTextOptions(*builder,
                                               widgetOptions,
            builder->CreateString(xmlDefinition)/*,
                                               isLocalized*/);

    return *(Offset<Table>*)(&options);
}

void RichTextReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* richTextOptions)
{
    auto ppnode  = (Node**)(node);
    auto options = (RichTextOptions*)richTextOptions;

    auto implNode = ui::RichText::createWithXML(options->xmlDefinition()->c_str());
    *ppnode       = implNode;

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(implNode, (Table*)options->widgetOptions());

    // implNode->ignoreContentAdaptWithSize(true);
}

Node* RichTextReader::createNodeWithFlatBuffers(const flatbuffers::Table* options)
{
    Node* node = nullptr;
    // #Hack
    setPropsWithFlatBuffers((Node*)(&node), (Table*)options);

    return node;
}

}  // namespace ax::ext
