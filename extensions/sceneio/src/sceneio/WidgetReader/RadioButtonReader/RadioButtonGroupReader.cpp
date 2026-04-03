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

#include "sceneio/WidgetReader/RadioButtonReader/RadioButtonGroupReader.h"

#include "axmol/ui/UIRadioButton.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/2d/SpriteFrame.h"
#include "axmol/2d/SpriteFrameCache.h"
#include "sceneext/CocoLoader.h"
#include "sceneio/CSParseBinary_generated.h"
#include "sceneio/FlatBuffersSerialize.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax::ui;
using namespace flatbuffers;

namespace ax::ext
{
static const char* P_Scale9Enable    = "scale9Enable";
static const char* P_FileNameData    = "fileNameData";
static const char* P_CapInsetsX      = "capInsetsX";
static const char* P_CapInsetsY      = "capInsetsY";
static const char* P_CapInsetsWidth  = "capInsetsWidth";
static const char* P_CapInsetsHeight = "capInsetsHeight";
static const char* P_Scale9Width     = "scale9Width";
static const char* P_Scale9Height    = "scale9Height";

static RadioButtonGroupReader* s_readerInstance = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(RadioButtonGroupReader)

RadioButtonGroupReader::RadioButtonGroupReader() {}

RadioButtonGroupReader::~RadioButtonGroupReader() {}

RadioButtonGroupReader* RadioButtonGroupReader::getInstance()
{
    if (!s_readerInstance)
    {
        s_readerInstance = new RadioButtonGroupReader();
    }
    return s_readerInstance;
}

void RadioButtonGroupReader::destroyInstance()
{
    AX_SAFE_DELETE(s_readerInstance);
}

Offset<Table> RadioButtonGroupReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                   flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    bool allowdNoSelection = false;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        auto name  = attribute.name();
        auto value = attribute.value();

        if (name == "AllowedNoSelection")
        {
            allowdNoSelection = (value == "True");
        }

        attribute = attribute.next_attribute();
    }

    auto options = CreateRadioButtonGroupOptions(*builder, widgetOptions, allowdNoSelection);

    return *(Offset<Table>*)(&options);
}

void RadioButtonGroupReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* params)
{
    auto radioButtonGroup = static_cast<ui::RadioButtonGroup*>(node);
    auto options          = (RadioButtonGroupOptions*)params;

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    radioButtonGroup->setAllowedNoSelection(options->allowedNoSelection());
}

Node* RadioButtonGroupReader::createNodeWithFlatBuffers(const flatbuffers::Table* options)
{
    auto node = ui::RadioButtonGroup::create();

    setPropsWithFlatBuffers(node, (Table*)options);

    return node;
}

/*int RadioButtonGroupReader::getResourceType(std::string_view key)
{
    if(key == "Normal" || key == "Default")
    {
        return 	0;
    }

    FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
    if(fbs->_isSimulator)
    {
        if(key == "MarkedSubImage")
        {
            return 0;
        }
    }
    return 1;
}*/

}  // namespace ax::ext
