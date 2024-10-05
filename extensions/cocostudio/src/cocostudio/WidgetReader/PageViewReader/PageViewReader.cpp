

#include "PageViewReader.h"

#include "ui/UIPageView.h"
#include "ui/UILayout.h"
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
static PageViewReader* instancePageViewReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(PageViewReader)

PageViewReader::PageViewReader() {}

PageViewReader::~PageViewReader() {}

PageViewReader* PageViewReader::getInstance()
{
    if (!instancePageViewReader)
    {
        instancePageViewReader = new PageViewReader();
    }
    return instancePageViewReader;
}

void PageViewReader::destroyInstance()
{
    AX_SAFE_DELETE(instancePageViewReader);
}

void PageViewReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    LayoutReader::setPropsFromBinary(widget, cocoLoader, cocoNode);
}

void PageViewReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    LayoutReader::setPropsFromJsonDictionary(widget, options);
}

Offset<Table> PageViewReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
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

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view name  = attribute.name();
        std::string_view value = attribute.value();

        if (name == "ClipAble")
        {
            clipEnabled = (value == "True") ? true : false;
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
            if (value == "True")
            {
                backGroundScale9Enabled = true;
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

        attribute = attribute.next_attribute();
    }

    // child elements
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "Size" && backGroundScale9Enabled)
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

    auto options = CreatePageViewOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        clipEnabled, &f_bgColor, &f_bgStartColor, &f_bgEndColor, colorType, bgColorOpacity, &f_colorVector,
        &f_capInsets, &f_scale9Size, backGroundScale9Enabled);

    return *(Offset<Table>*)(&options);
}

void PageViewReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* pageViewOptions)
{
    PageView* pageView = static_cast<PageView*>(node);
    auto options       = (PageViewOptions*)pageViewOptions;

    bool clipEnabled = options->clipEnabled() != 0;
    pageView->setClippingEnabled(clipEnabled);

    bool backGroundScale9Enabled = options->backGroundScale9Enabled() != 0;
    pageView->setBackGroundImageScale9Enabled(backGroundScale9Enabled);

    auto f_bgColor = options->bgColor();
    Color3B bgColor(f_bgColor->r(), f_bgColor->g(), f_bgColor->b());
    auto f_bgStartColor = options->bgStartColor();
    Color3B bgStartColor(f_bgStartColor->r(), f_bgStartColor->g(), f_bgStartColor->b());
    auto f_bgEndColor = options->bgEndColor();
    Color3B bgEndColor(f_bgEndColor->r(), f_bgEndColor->g(), f_bgEndColor->b());

    auto f_colorVecor = options->colorVector();
    Vec2 colorVector(f_colorVecor->x(), f_colorVecor->y());
    pageView->setBackGroundColorVector(colorVector);

    int bgColorOpacity = options->bgColorOpacity();

    int colorType = options->colorType();
    pageView->setBackGroundColorType(Layout::BackGroundColorType(colorType));

    pageView->setBackGroundColor(bgStartColor, bgEndColor);
    pageView->setBackGroundColor(bgColor);
    pageView->setBackGroundColorOpacity(bgColorOpacity);

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
            pageView->setBackGroundImage(imageFileName, (Widget::TextureResType)imageFileNameType);
        }
    }

    auto widgetOptions = options->widgetOptions();
    auto f_color       = widgetOptions->color();
    Color3B color(f_color->r(), f_color->g(), f_color->b());
    pageView->setColor(color);

    int opacity = widgetOptions->alpha();
    pageView->setOpacity(opacity);

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    if (backGroundScale9Enabled)
    {
        auto f_capInsets = options->capInsets();
        Rect capInsets(f_capInsets->x(), f_capInsets->y(), f_capInsets->width(), f_capInsets->height());
        pageView->setBackGroundImageCapInsets(capInsets);

        auto f_scale9Size = options->scale9Size();
        Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
        pageView->setContentSize(scale9Size);
    }
    else
    {
        if (!pageView->isIgnoreContentAdaptWithSize())
        {
            Size contentSize(widgetOptions->size()->width(), widgetOptions->size()->height());
            pageView->setContentSize(contentSize);
        }
    }
}

Node* PageViewReader::createNodeWithFlatBuffers(const flatbuffers::Table* pageViewOptions)
{
    PageView* pageView = PageView::create();

    setPropsWithFlatBuffers(pageView, (Table*)pageViewOptions);

    return pageView;
}

int PageViewReader::getResourceType(std::string_view key)
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
