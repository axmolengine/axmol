#include "cocostudio/WidgetReader/ArmatureNodeReader/ArmatureNodeReader.h"

#include "platform/FileUtils.h"

#include "flatbuffers/flatbuffers.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/ArmatureNodeReader/CSArmatureNode_generated.h"
#include "cocostudio/Armature.h"


using namespace ax;
using namespace cocostudio;
using namespace flatbuffers;

IMPLEMENT_CLASS_NODE_READER_INFO(ArmatureNodeReader)

ArmatureNodeReader::ArmatureNodeReader() {}

ArmatureNodeReader::~ArmatureNodeReader() {}

static ArmatureNodeReader* _instanceArmatureNodeReader = nullptr;

ArmatureNodeReader* ArmatureNodeReader::getInstance()
{
    if (_instanceArmatureNodeReader == nullptr)
    {
        _instanceArmatureNodeReader = new ArmatureNodeReader();
    }
    return _instanceArmatureNodeReader;
}

void ArmatureNodeReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceArmatureNodeReader);
}

Offset<Table> ArmatureNodeReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                               flatbuffers::FlatBufferBuilder* builder)
{

    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    bool isloop     = false;
    bool isAutoPlay = false;
    std::string currentAnimationName;
    std::string currentArmatureName;

    int type = 0;
    std::string path;

    float armatureScale = 1.0f;
    float timeScale     = 1.0f;

    /*int textureInfoFileType = 0;
    std::string textureInfoFilePath;*/

    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "IsLoop")
        {
            isloop = (value == "True") ? true : false;
        }
        else if (attriname == "IsAutoPlay")
        {
            isAutoPlay = (value == "True") ? true : false;
        }
        else if (attriname == "CurrentAnimationName")
        {
            currentAnimationName = value;
        }
        else if (attriname == "CurrentArmatureName")
        {
            currentArmatureName = value;
        }
        else if (attriname == "ArmatureScale")
        {
            armatureScale = atof(value.data());
        }
        else if (attriname == "TimeScale")
        {
            timeScale = atof(value.data());
        }

        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        std::string_view attriname = child.name();
        if (attriname == "FileData")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname         = attribute.name();
                std::string_view value = attribute.value();

                if (attriname == "Type")
                {
                    type = 0;
                }
                else if (attriname == "Path")
                {
                    path = value;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    auto options = CreateCSArmatureNodeOption(*builder, nodeOptions,
                                              CreateResourceItemData(*builder, type, builder->CreateString(path)),
                                              isloop, isAutoPlay, builder->CreateString(currentAnimationName),
                                              builder->CreateString(currentArmatureName), timeScale, armatureScale);

    return *(Offset<Table>*)(&options);
}

void ArmatureNodeReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* nodeOptions)
{
    Node** ppResult = (Node**)(node);
    auto options    = (flatbuffers::CSArmatureNodeOption*)nodeOptions;

    bool fileExist = false;
    std::string errorFilePath;

    std::string filepath(options->fileData()->path()->c_str());

    if (FileUtils::getInstance()->isFileExist(filepath))
    {
        fileExist = true;
        {
            std::string fullpath = FileUtils::getInstance()->fullPathForFilename(filepath);

            std::string dirpath = fullpath.substr(0, fullpath.find_last_of('/'));
            FileUtils::getInstance()->addSearchPath(dirpath);

            ArmatureDataManager::getInstance()->addArmatureFileInfo(filepath);
            auto armatureNode       = Armature::create(getArmatureName(filepath));
            std::string currentname = options->currentAnimationName()->c_str();
            if (options->isAutoPlay())
                armatureNode->getAnimation()->play(currentname, -1, options->isLoop());
            else
            {
                armatureNode->getAnimation()->play(currentname);
                armatureNode->getAnimation()->gotoAndPause(0);
            }

            *ppResult = armatureNode;
        }
    }
    else
    {
        *ppResult = Node::create();

        errorFilePath = filepath;
        fileExist     = false;
    }
}

ax::Node* ArmatureNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table* nodeOptions)
{
    Node* node = nullptr;  // auto node = Armature::create();

    // self
    auto options = (flatbuffers::CSArmatureNodeOption*)nodeOptions;
    setPropsWithFlatBuffers((Node*)(&node), (Table*)options);

    // super node
    auto NodeReader = NodeReader::getInstance();
    NodeReader->setPropsWithFlatBuffers(node, (Table*)options->nodeOptions());

    return node;
}

std::string ArmatureNodeReader::getArmatureName(std::string_view exporJsonPath)
{
    // FileUtils.getFileData(exporJsonPath, "r", size)   // need read armature name in exportJsonPath
    size_t end    = exporJsonPath.find_last_of(".");
    size_t start  = exporJsonPath.find_last_of('\\') + 1;
    size_t start1 = exporJsonPath.find_last_of('/') + 1;
    if (start < start1)
        start = start1;

    if (start == -1)
        start = 0;
    return std::string{exporJsonPath.substr(start, end - start)};
}
