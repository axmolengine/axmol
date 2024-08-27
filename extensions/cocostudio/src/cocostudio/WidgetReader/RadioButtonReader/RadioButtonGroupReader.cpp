

#include "RadioButtonGroupReader.h"

#include "ui/UIRadioButton.h"
#include "platform/FileUtils.h"
#include "2d/SpriteFrame.h"
#include "2d/SpriteFrameCache.h"
#include "cocostudio/CocoLoader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/FlatBuffersSerialize.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax;
using namespace ui;
using namespace flatbuffers;

namespace cocostudio
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
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

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

}  // namespace cocostudio
