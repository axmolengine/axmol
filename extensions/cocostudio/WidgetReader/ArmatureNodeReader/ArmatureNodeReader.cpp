#include "cocostudio/WidgetReader/ArmatureNodeReader/ArmatureNodeReader.h"

#include "platform/CCFileUtils.h"

#include "flatbuffers/flatbuffers.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"
#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/ArmatureNodeReader/CSArmatureNode_generated.h"
#include "cocostudio/CCArmature.h"
#if defined(CC_BUILD_WITH_DRANGBONES) && CC_BUILD_WITH_DRANGBONES
#include "dragonbones/CCDragonBonesHeaders.h"
#endif

USING_NS_CC;
using namespace cocostudio;
using namespace flatbuffers;

IMPLEMENT_CLASS_NODE_READER_INFO(ArmatureNodeReader)

ArmatureNodeReader::ArmatureNodeReader()
{

}

ArmatureNodeReader::~ArmatureNodeReader()
{

}

static ArmatureNodeReader* _instanceArmatureNodeReader = nullptr;

ArmatureNodeReader* ArmatureNodeReader::getInstance()
{
	if (_instanceArmatureNodeReader == nullptr)
	{
		_instanceArmatureNodeReader = new (std::nothrow) ArmatureNodeReader();
	}
	return _instanceArmatureNodeReader;
}

void ArmatureNodeReader::destroyInstance()
{
	CC_SAFE_DELETE(_instanceArmatureNodeReader);
}

Offset<Table> ArmatureNodeReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
	flatbuffers::FlatBufferBuilder *builder)
{

	auto temp = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
	auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

	bool isloop = false;
	bool isAutoPlay = false;
	std::string currentAnimationName;
    std::string currentArmatureName;

	int type = 0;
	std::string path;

    float armatureScale = 1.0f;
    float timeScale = 1.0f;

    /*int textureInfoFileType = 0;
    std::string textureInfoFilePath;*/

	auto attribute =  objectData.first_attribute();
	while (attribute)
	{
		std::string attriname = attribute.name();
		std::string value = attribute.value();

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
            armatureScale = atof(value.c_str());
        }
        else if (attriname == "TimeScale")
        {
            timeScale = atof(value.c_str());
        }

		attribute = attribute.next_attribute();
	}

	auto child = objectData.first_child();
	while (child)
	{
		std::string attriname = child.name();
		if (attriname == "FileData")
		{
			attribute = child.first_attribute();

			while (attribute)
			{
				attriname = attribute.name();
				std::string value = attribute.value();

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

	auto options = CreateCSArmatureNodeOption(*builder,
		nodeOptions,
		CreateResourceItemData(*builder,
			type,
			builder->CreateString(path)),
		isloop,
		isAutoPlay,
		builder->CreateString(currentAnimationName),
        builder->CreateString(currentArmatureName),
        timeScale,
        armatureScale);

	return *(Offset<Table>*)(&options);
}

void ArmatureNodeReader::setPropsWithFlatBuffers(cocos2d::Node *node,
	const flatbuffers::Table *nodeOptions)
{
	Node** ppResult = (Node**)(node);
	auto options = (flatbuffers::CSArmatureNodeOption*)nodeOptions;

	bool fileExist = false;
	std::string errorFilePath;

	std::string filepath(options->fileData()->path()->c_str());

	if (FileUtils::getInstance()->isFileExist(filepath))
	{
		fileExist = true;
#if defined(CC_BUILD_WITH_DRANGBONES) && CC_BUILD_WITH_DRANGBONES
		auto filep = filepath.rfind('.');
		if (filep != std::string::npos && strcmp(&filepath[filep], ".json") == 0)
		{ // Currently, adjust by file ext, regard as DragonBones 4.5/5.0
            // 4.5 texture info is fixed as texture.png, texture.json
            // 5.o texture info is _tex.json _tex.png
			auto sharedFactory = dragonBones::CCFactory::getFactory();
			const auto dragonBonesData = sharedFactory->loadDragonBonesData(filepath, filepath);

			if (dragonBonesData != nullptr) {
				auto slash = filepath.rfind('/');
				if (slash == std::string::npos)
					slash = filepath.rfind("\\");
				if (slash != std::string::npos) {
					auto folder = filepath.substr(0, slash + 1);
                    // try dragonBones 5.0 firstly;
                    std::string commonName;
                    auto _skePos = filepath.find("_ske");
                    if(_skePos != std::string::npos) {
                        commonName = filepath.substr(slash + 1, _skePos - slash - 1);
                    }
                    auto succeed = sharedFactory->loadTextureAtlasData(folder + commonName + "_tex.json", filepath) != nullptr;
					if (succeed || sharedFactory->loadTextureAtlasData(folder + "texture.json", filepath) != nullptr)
					{
                        std::string designArmatureName(options->currentArmatureName()->c_str());
						auto armatureNode = sharedFactory->buildArmatureDisplay(designArmatureName);
						
                        armatureNode->setScale(options->armatureScale());
                        armatureNode->getAnimation()->timeScale = options->timeScale();

						std::string currentname = options->currentAnimationName()->c_str();
						armatureNode->getAnimation()->play(currentname, options->isLoop() ? -1 : 1);

						*ppResult = armatureNode;
					}
				}
			}
		}
		else 
#endif
        {
			std::string fullpath = FileUtils::getInstance()->fullPathForFilename(filepath);

			std::string dirpath = fullpath.substr(0, fullpath.find_last_of('/'));
			FileUtils::getInstance()->addSearchPath(dirpath);

			ArmatureDataManager::getInstance()->addArmatureFileInfo(filepath);
			auto armatureNode = Armature::create(getArmatureName(filepath));
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
		fileExist = false;
	}
}

cocos2d::Node*  ArmatureNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table *nodeOptions)
{
	Node* node = nullptr;// auto node = Armature::create();

	// self
	auto options = (flatbuffers::CSArmatureNodeOption*)nodeOptions;
	setPropsWithFlatBuffers((Node*)(&node), (Table*)options);

	// super node
	auto NodeReader = NodeReader::getInstance();
	NodeReader->setPropsWithFlatBuffers(node, (Table*)options->nodeOptions());

	return node;
}

std::string ArmatureNodeReader::getArmatureName(const std::string& exporJsonPath)
{
	//FileUtils.getFileData(exporJsonPath, "r", size)   // need read armature name in exportJsonPath
	size_t end = exporJsonPath.find_last_of(".");
	size_t start = exporJsonPath.find_last_of('\\') + 1;
	size_t start1 = exporJsonPath.find_last_of('/') + 1;
	if (start < start1)
		start = start1;

	if (start == -1)
		start = 0;
	return  exporJsonPath.substr(start, end - start);
}
