

#include "ImageViewReader.h"

#include "ui/UIImageView.h"
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

static ImageViewReader* instanceImageViewReader = nullptr;

IMPLEMENT_CLASS_NODE_READER_INFO(ImageViewReader)

ImageViewReader::ImageViewReader() {}

ImageViewReader::~ImageViewReader() {}

ImageViewReader* ImageViewReader::getInstance()
{
    if (!instanceImageViewReader)
    {
        instanceImageViewReader = new ImageViewReader();
    }
    return instanceImageViewReader;
}

void ImageViewReader::destroyInstance()
{
    AX_SAFE_DELETE(instanceImageViewReader);
}

void ImageViewReader::setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* cocoNode)
{
    WidgetReader::setPropsFromBinary(widget, cocoLoader, cocoNode);

    ImageView* imageView = static_cast<ImageView*>(widget);
    this->beginSetBasicProperties(widget);
    float capsx = 0.0f, capsy = 0.0, capsWidth = 0.0, capsHeight = 0.0f;

    stExpCocoNode* stChildArray = cocoNode->GetChildArray(cocoLoader);

    for (int i = 0; i < cocoNode->GetChildNum(); ++i)
    {
        std::string key   = stChildArray[i].GetName(cocoLoader);
        std::string value = stChildArray[i].GetValue(cocoLoader);

        // read all basic properties of widget
        AX_BASIC_PROPERTY_BINARY_READER
        // read all color related properties of widget
        AX_COLOR_PROPERTY_BINARY_READER

        else if (key == P_Scale9Enable) { imageView->setScale9Enabled(valueToBool(value)); }
        else if (key == P_FileNameData)
        {
            stExpCocoNode* backGroundChildren = stChildArray[i].GetChildArray(cocoLoader);
            std::string resType               = backGroundChildren[2].GetValue(cocoLoader);

            Widget::TextureResType imageFileNameType = (Widget::TextureResType)valueToInt(resType);

            std::string backgroundValue = this->getResourcePath(cocoLoader, &stChildArray[i], imageFileNameType);
            imageView->loadTexture(backgroundValue, imageFileNameType);
        }
        else if (key == P_Scale9Width)
        {
            imageView->setContentSize(Size(valueToFloat(value), imageView->getContentSize().height));
        }
        else if (key == P_Scale9Height)
        {
            imageView->setContentSize(Size(imageView->getContentSize().width, valueToFloat(value)));
        }
        else if (key == P_CapInsetsX) { capsx = valueToFloat(value); }
        else if (key == P_CapInsetsY) { capsy = valueToFloat(value); }
        else if (key == P_CapInsetsWidth) { capsWidth = valueToFloat(value); }
        else if (key == P_CapInsetsHeight) { capsHeight = valueToFloat(value); }

    }  // end of for loop

    if (imageView->isScale9Enabled())
    {
        imageView->setCapInsets(Rect(capsx, capsy, capsWidth, capsHeight));
    }

    this->endSetBasicProperties(widget);
}

void ImageViewReader::setPropsFromJsonDictionary(Widget* widget, const rapidjson::Value& options)
{
    WidgetReader::setPropsFromJsonDictionary(widget, options);

    ImageView* imageView = static_cast<ImageView*>(widget);

    const rapidjson::Value& imageFileNameDic = DICTOOL->getSubDictionary_json(options, P_FileNameData);
    int imageFileNameType                    = DICTOOL->getIntValue_json(imageFileNameDic, P_ResourceType);
    std::string_view imageFilePath           = DICTOOL->getStringValue_json(imageFileNameDic, P_Path);

    if (!imageFilePath.empty())
    {
        std::string imageFileName =
            this->getResourcePath(imageFileNameDic, P_Path, (Widget::TextureResType)imageFileNameType);
        imageView->loadTexture(imageFileName, (Widget::TextureResType)imageFileNameType);
    }

    bool scale9EnableExist = DICTOOL->checkObjectExist_json(options, P_Scale9Enable);
    bool scale9Enable      = false;
    if (scale9EnableExist)
    {
        scale9Enable = DICTOOL->getBooleanValue_json(options, P_Scale9Enable);
    }
    imageView->setScale9Enabled(scale9Enable);

    if (scale9Enable)
    {

        float swf = DICTOOL->getFloatValue_json(options, P_Scale9Width, 80.0f);
        float shf = DICTOOL->getFloatValue_json(options, P_Scale9Height, 80.0f);
        imageView->setContentSize(Size(swf, shf));

        float cx = DICTOOL->getFloatValue_json(options, P_CapInsetsX);
        float cy = DICTOOL->getFloatValue_json(options, P_CapInsetsY);
        float cw = DICTOOL->getFloatValue_json(options, P_CapInsetsWidth, 1.0f);
        float ch = DICTOOL->getFloatValue_json(options, P_CapInsetsHeight, 1.0f);

        imageView->setCapInsets(Rect(cx, cy, cw, ch));
    }

    WidgetReader::setColorPropsFromJsonDictionary(widget, options);
}

Offset<Table> ImageViewReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                            flatbuffers::FlatBufferBuilder* builder)
{
    auto temp          = WidgetReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto widgetOptions = *(Offset<WidgetOptions>*)(&temp);

    bool scale9Enabled = false;
    Rect capInsets;
    ax::Size scale9Size;

    std::string path;
    std::string plistFile;
    int resourceType = 0;

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

    CapInsets f_capInsets(capInsets.origin.x, capInsets.origin.y, capInsets.size.width, capInsets.size.height);
    FlatSize f_scale9Size(scale9Size.width, scale9Size.height);

    auto options = CreateImageViewOptions(
        *builder, widgetOptions,
        CreateResourceData(*builder, builder->CreateString(path), builder->CreateString(plistFile), resourceType),
        &f_capInsets, &f_scale9Size, scale9Enabled);

    return *(Offset<Table>*)(&options);
}

void ImageViewReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* imageViewOptions)
{
    ImageView* imageView = static_cast<ImageView*>(node);
    auto options         = (ImageViewOptions*)imageViewOptions;

    bool fileExist = false;
    std::string errorFilePath;
    auto imageFileNameDic     = (options->fileNameData());
    int imageFileNameType     = imageFileNameDic->resourceType();
    std::string imageFileName = imageFileNameDic->path()->c_str();
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
        imageView->loadTexture(imageFileName, (Widget::TextureResType)imageFileNameType);
    }

    bool scale9Enabled = options->scale9Enabled() != 0;
    imageView->setScale9Enabled(scale9Enabled);

    auto widgetReader = WidgetReader::getInstance();
    widgetReader->setPropsWithFlatBuffers(node, (Table*)options->widgetOptions());

    if (scale9Enabled)
    {
        imageView->setUnifySizeEnabled(false);
        imageView->ignoreContentAdaptWithSize(false);

        auto f_scale9Size = options->scale9Size();
        Size scale9Size(f_scale9Size->width(), f_scale9Size->height());
        imageView->setContentSize(scale9Size);

        auto f_capInset = options->capInsets();
        Rect capInsets(f_capInset->x(), f_capInset->y(), f_capInset->width(), f_capInset->height());
        imageView->setCapInsets(capInsets);
    }
    else
    {
        Size contentSize(options->widgetOptions()->size()->width(), options->widgetOptions()->size()->height());
        imageView->setContentSize(contentSize);
    }
}

Node* ImageViewReader::createNodeWithFlatBuffers(const flatbuffers::Table* imageViewOptions)
{
    ImageView* imageView = ImageView::create();

    setPropsWithFlatBuffers(imageView, (Table*)imageViewOptions);

    return imageView;
}

int ImageViewReader::getResourceType(std::string_view key)
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
