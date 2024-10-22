

#include "ScrollViewReader.h"

#include "ui/UIScrollView.h"
#include "platform/FileUtils.h"
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
static const char* P_InnerWidth   = "innerWidth";
static const char* P_InnerHeight  = "innerHeight";
static const char* P_Direction    = "direction";
static const char* P_BounceEnable = "bounceEnable";

static ScrollViewReader* instanceScrollViewReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(ScrollViewReader)

ScrollViewReader::ScrollViewReader() {}

ScrollViewReader::~ScrollViewReader() {}

ScrollViewReader* ScrollViewReader::getInstance()
{
    if (!instanceScrollViewReader)
    {
        instanceScrollViewReader = new ScrollViewReader();
    }
    return instanceScrollViewReader;
}

void ScrollViewReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceScrollViewReader);
}

void ScrollViewReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    // TODO: need to refactor...
    LayoutReader::setPropsFromBinary(widget, cocoLoader, cocoNode);

    ScrollView* scrollView = static_cast<ScrollView*>(widget);

    stExpCocoNode* stChildArray = cocoNode->GetChildArray(cocoLoader);
    float innerWidth;
    float innerHeight;
    for (int i = 0; i < cocoNode->GetChildNum(); ++i)
    {
        std::string key   = stChildArray[i].GetName(cocoLoader);
        std::string value = stChildArray[i].GetValue(cocoLoader);
        if (key == P_InnerWidth)
        {
            innerWidth = valueToFloat(value);
        }
        else if (key == P_InnerHeight)
        {
            innerHeight = valueToFloat(value);
        }
        else if (key == P_Direction)
        {
            scrollView->setDirection((ScrollView::Direction)valueToInt(value));
        }
        else if (key == P_BounceEnable)
        {
            scrollView->setBounceEnabled(valueToBool(value));
        }

    }  // end of for loop
    scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));
}

void ScrollViewReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    LayoutReader::setPropsFromJsonDictionary(widget, options);

    ScrollView* scrollView = static_cast<ScrollView*>(widget);
    float innerWidth       = DICTOOL->getFloatValue_json(options, P_InnerWidth, 200);
    float innerHeight      = DICTOOL->getFloatValue_json(options, P_InnerHeight, 200);
    scrollView->setInnerContainerSize(Size(innerWidth, innerHeight));
    int direction = DICTOOL->getFloatValue_json(options, P_Direction, 1);
    scrollView->setDirection((ScrollView::Direction)direction);
    scrollView->setBounceEnabled(DICTOOL->getBooleanValue_json(options, P_BounceEnable));

    LayoutReader::setColorPropsFromJsonDictionary(widget, options);
}

Offset<Table> ScrollViewReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                             flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string path;
    std::string plistFile;
    int resourceType = 0;

    bool clipEnabled = false;
    Color3B bgColor;
    Color3B bgStartColor;
    Color3B bgEndColor;
    int colorType          = 0;
    uint8_t bgColorOpacity = 255;
    Vec2 colorVector(0.0f, -0.5f);
    Rect capInsets;
    Size scale9Size;
    bool backGroundScale9Enabled = false;
    Size innerSize(200, 300);
    int direction               = 0;
    bool bounceEnabled          = false;
    bool scrollbarEnabled       = true;
    bool scrollbarAutoHide      = true;
    float scrollbarAutoHideTime = 0.2f;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "ClipAble")
        {
            clipEnabled = FLATSTR_TO_BOOL(value);
        }
        else if (name == "ComboBoxIndex")
        {
            colorType = atoi(value.data());
        }
        else if (name == "BackColorAlpha")
        {
            bgColorOpacity = atoi(value.data());
        }
        else if (name == "Scale9Enable")
        {
            backGroundScale9Enabled = FLATSTR_TO_BOOL(value);
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
        else if (name == "ScrollDirectionType")
        {
            if (value == "Vertical")
            {
                direction = 1;
            }
            else if (value == "Horizontal")
            {
                direction = 2;
            }
            else if (value == "Vertical_Horizontal")
            {
                direction = 3;
            }
        }
        else if (name == "IsBounceEnabled")
        {
            bounceEnabled = FLATSTR_TO_BOOL(value);
        }
        else if (name.compare("BarEnabled") == 0)
        {
            scrollbarEnabled = FLATSTR_TO_BOOL(value);
        }
        else if (name.compare("BarAutoHide") == 0)
        {
            scrollbarAutoHide = FLATSTR_TO_BOOL(value);
        }
        else if (name.compare("BarAutoHideTime") == 0)
        {
            scrollbarAutoHideTime = atof(value.data());
        }
        attribute = attribute.next_attribute();
    }

    // child elements
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "InnerNodeSize")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Width")
                {
                    innerSize.width = atof(value.data());
                }
                else if (name == "Height")
                {
                    innerSize.height = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "Size" && backGroundScale9Enabled)
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
        else if (name == "SingleColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "R")
                {
                    bgColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    bgColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    bgColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "EndColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "R")
                {
                    bgEndColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    bgEndColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    bgEndColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "FirstColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "R")
                {
                    bgStartColor.r = atoi(value.data());
                }
                else if (name == "G")
                {
                    bgStartColor.g = atoi(value.data());
                }
                else if (name == "B")
                {
                    bgStartColor.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "ColorVector")
        {
            attribute = child.first_attribute();
            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "ScaleX")
                {
                    colorVector.x = atof(value.data());
                }
                else if (name == "ScaleY")
                {
                    colorVector.y = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (name == "FileData")
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
                    path = value;
                }
                else if (name == "Type")
                {
                    resourceType = getResourceType(value);
                }
                else if (name == "Plist")
                {
                    plistFile = value;
                    texture   = value;
                }

                attribute = attribute.next_attribute();
            }

            if (resourceType == 1)
            {
                FlatBuffersSerialize* fbs = FlatBuffersSerialize::getInstance();
                fbs->_textures.emplace_back(builder->CreateString(texture));
            }
        }

        child = child.next_sibling();
    }

    FColor f_bgColor(255, bgColor.r, bgColor.g, bgColor.b);
    FColor f_bgStartColor(255, bgStartColor.r, bgStartColor.g, bgStartColor.b);
    FColor f_bgEndColor(255, bgEndColor.r, bgEndColor.g, bgEndColor.b);
    FVec2 f_colorVector(colorVector.x, colorVector.y);
    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);
    FlatSize f_innerSize(innerSize.width, innerSize.height);

    auto options = CreateScrollViewOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        clipEnabled, &f_bgColor, &f_bgStartColor, &f_bgEndColor, colorType, bgColorOpacity, &f_colorVector,
        &f_capInsets, &f_scale9Size, backGroundScale9Enabled, &f_innerSize, direction, bounceEnabled, scrollbarEnabled,
        scrollbarAutoHide, scrollbarAutoHideTime);

    return *(Offset<Table>*)(&options);
}

void ScrollViewReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* scrollViewOptions)
{
    ScrollView* scrollView = static_cast<ScrollView*>(node);
    auto options           = (ScrollViewOptions*)scrollViewOptions;

    bool clipEnabled = options->clipEnabled() != 0;
    scrollView->setClippingEnabled(clipEnabled);

    bool backGroundScale9Enabled = options->backGroundScale9Enabled() != 0;
    scrollView->setBackGroundImageScale9Enabled(backGroundScale9Enabled);

    auto f_bgColor = options->bgColor();
    Color3B bgColor(f_bgColor->r(), f_bgColor->g(), f_bgColor->b());
    auto f_bgStartColor = options->bgStartColor();
    Color3B bgStartColor(f_bgStartColor->r(), f_bgStartColor->g(), f_bgStartColor->b());
    auto f_bgEndColor = options->bgEndColor();
    Color3B bgEndColor(f_bgEndColor->r(), f_bgEndColor->g(), f_bgEndColor->b());

    auto f_colorVecor = options->colorVector();
    Vec2 colorVector(f_colorVecor->x(), f_colorVecor->y());
    scrollView->setBackGroundColorVector(colorVector);

    int bgColorOpacity = options->bgColorOpacity();

    int colorType = options->colorType();
    scrollView->setBackGroundColorType(Layout::BackGroundColorType(colorType));

    scrollView->setBackGroundColor(bgStartColor, bgEndColor);
    scrollView->setBackGroundColor(bgColor);
    scrollView->setBackGroundColorOpacity(bgColorOpacity);

    bool fileExist = false;
    std::string errorFilePath;
    auto imageFileNameDic     = (options->backGroundImageData());
    int imageFileNameType     = imageFileNameDic->resourceType();
    std::string imageFileName = imageFileNameDic->path()->c_str();
    if (imageFileName != "")
    {
        switch (imageFileNameType)
        {
        case 0:
        {
            if (FileUtils::getInstance()->isFileExist(imageFileName))
            {
                fileExist = true;
            }
            else
            {
                errorFilePath = imageFileName;
                fileExist     = false;
            }
            break;
        }

        case 1:
        {
            std::string plist        = imageFileNameDic->plistFile()->c_str();
            SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->findFrame(imageFileName);
            if (spriteFrame)
            {
                fileExist = true;
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
                        errorFilePath = textureFileName;
                    }
                }
                else
                {
                    errorFilePath = plist;
                }
                fileExist = false;
            }
            break;
        }

        default:
            break;
        }
        if (fileExist)
        {
            scrollView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        }
    }

    auto widgetOptions = options->widgetOptions();
    auto f_color       = widgetOptions->color();
    Color3B color(f_color->r(), f_color->g(), f_color->b());
    scrollView->setColor(color);

    int opacity = widgetOptions->alpha();
    scrollView->setOpacity(opacity);

    auto f_innerSize = options->innerSize();
    Size innerSize(f_innerSize->width(), f_innerSize->height());
    scrollView->setInnerContainerSize(innerSize);
    int direction = options->direction();
    scrollView->setDirection((ScrollView::Direction)direction);
    bool bounceEnabled = options->bounceEnabled() != 0;
    scrollView->setBounceEnabled(bounceEnabled);

    bool scrollbarEnabled = options->scrollbarEnabeld() != 0;
    scrollView->setScrollBarEnabled(scrollbarEnabled);
    if (scrollbarEnabled)
    {
        bool scrollbarAutoHide = options->scrollbarAutoHide() != 0;
        scrollView->setScrollBarAutoHideEnabled(scrollbarAutoHide);
        float barAutoHideTime = options->scrollbarAutoHideTime();
        scrollView->setScrollBarAutoHideTime(barAutoHideTime);
    }

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    if (backGroundScale9Enabled)
    {
        auto f_capInsets = options->capInsets();
        Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
        scrollView->setBackGroundImageCapInsets(capInsets);

        auto f_scale9Size = options->scale9Size();
        Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
        scrollView->setContentSize(scale9Size);
    }
    else
    {
        if (!scrollView->isIgnoreContentAdaptWithSize())
        {
            Size contentSize(widgetOptions->size()->width(), widgetOptions->size()->height());
            scrollView->setContentSize(contentSize);
        }
    }
}

Node* ScrollViewReader::createNodeWithFlatBuffers(const flatbuffers::Table* scrollViewOptions)
{
    ScrollView* scrollView = ScrollView::create();

    setPropsWithFlatBuffers(scrollView, (Table*)scrollViewOptions);

    return scrollView;
}

int ScrollViewReader::getResourceType(std::string_view key)
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
