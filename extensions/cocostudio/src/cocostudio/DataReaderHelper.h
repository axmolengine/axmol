/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __CCDATAREADERHELPER_H__
#define __CCDATAREADERHELPER_H__

#include "ArmatureDefine.h"
#include "Datas.h"
#include "Armature.h"
#include "DictionaryHelper.h"
#include "CocosStudioExport.h"

#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace cocostudio
{
class CocoLoader;
struct stExpCocoNode;
/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL DataReaderHelper : ax::Object
{
protected:
    enum ConfigType
    {
        DragonBone_XML,
        CocoStudio_JSON,
        CocoStudio_Binary
    };

    typedef struct _AsyncStruct
    {
        std::string filename;
        std::string fileContent;
        ConfigType configType;
        std::string baseFilePath;
        ax::Object* target;
        ax::SEL_SCHEDULE selector;
        bool autoLoadSpriteFile;

        std::string imagePath;
        std::string plistPath;
    } AsyncStruct;

    typedef struct _DataInfo
    {
        AsyncStruct* asyncStruct;
        std::queue<std::string> configFileQueue;
        float contentScale;
        std::string filename;
        std::string baseFilePath;
        float flashToolVersion;
        float cocoStudioVersion;
    } DataInfo;

public:

    static DataReaderHelper* getInstance();
    static void destroyInstance();

    /**
     * Scale the position data, used for multiresolution adapter
     * It won't effect the data already read.
     */
    static void setPositionReadScale(float scale);
    static float getPositionReadScale();

public:
    /**
     * @js ctor
     */
    DataReaderHelper();
    /**
     * @js NA
     * @lua NA
     */
    ~DataReaderHelper();

    void addDataFromFile(std::string_view filePath);
    void addDataFromFileAsync(std::string_view imagePath,
                              std::string_view plistPath,
                              std::string_view filePath,
                              ax::Object* target,
                              ax::SEL_SCHEDULE selector);

    void addDataAsyncCallBack(float dt);

    void removeConfigFile(std::string_view configFile);

public:
    /**
     * Translate XML export from Dragon Bone flash tool to datas, and save them.
     * When you add a new xml, the data already saved will be kept.
     *
     * @param xmlPath The cache of the xml
     */
    static void addDataFromCache(std::string_view pFileContent, DataInfo* dataInfo = nullptr);

    /**
     * Decode Armature Datas from xml export from Dragon Bone flash tool
     */
    static ArmatureData* decodeArmature(pugi::xml_node& armatureXML, DataInfo* dataInfo);
    static BoneData* decodeBone(pugi::xml_node& boneXML, pugi::xml_node& parentXML, DataInfo* dataInfo);
    static DisplayData* decodeBoneDisplay(pugi::xml_node& displayXML, DataInfo* dataInfo);

    /**
     * Decode ArmatureAnimation Datas from xml export from Dragon Bone flash tool
     */
    static AnimationData* decodeAnimation(pugi::xml_node& animationXML, DataInfo* dataInfo);
    static MovementData* decodeMovement(pugi::xml_node& movementXML, ArmatureData* armatureData, DataInfo* dataInfo);
    static MovementBoneData* decodeMovementBone(pugi::xml_node& movBoneXml,
                                                pugi::xml_node& parentXml,
                                                BoneData* boneData,
                                                DataInfo* dataInfo);
    static FrameData* decodeFrame(pugi::xml_node& frameXML,
                                  pugi::xml_node& parentFrameXml,
                                  BoneData* boneData,
                                  DataInfo* dataInfo);

    /**
     * Decode Texture Datas from xml export from Dragon Bone flash tool
     */
    static TextureData* decodeTexture(pugi::xml_node& textureXML, DataInfo* dataInfo);

    /**
     * Decode Contour Datas from xml export from Dragon Bone flash tool
     */
    static ContourData* decodeContour(pugi::xml_node& contourXML, DataInfo* dataInfo);

public:
    static void addDataFromJsonCache(std::string_view fileContent, DataInfo* dataInfo = nullptr);

    static ArmatureData* decodeArmature(const rapidjson::Value& json, DataInfo* dataInfo);
    static BoneData* decodeBone(const rapidjson::Value& json, DataInfo* dataInfo);
    static DisplayData* decodeBoneDisplay(const rapidjson::Value& json, DataInfo* dataInfo);

    static AnimationData* decodeAnimation(const rapidjson::Value& json, DataInfo* dataInfo);
    static MovementData* decodeMovement(const rapidjson::Value& json, DataInfo* dataInfo);
    static MovementBoneData* decodeMovementBone(const rapidjson::Value& json, DataInfo* dataInfo);
    static FrameData* decodeFrame(const rapidjson::Value& json, DataInfo* dataInfo);

    static TextureData* decodeTexture(const rapidjson::Value& json);

    static ContourData* decodeContour(const rapidjson::Value& json);

    static void decodeNode(BaseData* node, const rapidjson::Value& json, DataInfo* dataInfo);

    // for binary decode
public:
    static void addDataFromBinaryCache(const char* fileContent, DataInfo* dataInfo = nullptr);
    static ArmatureData* decodeArmature(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);
    static BoneData* decodeBone(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);
    static DisplayData* decodeBoneDisplay(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);
    static AnimationData* decodeAnimation(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);
    static MovementData* decodeMovement(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);

    static MovementBoneData* decodeMovementBone(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);
    static FrameData* decodeFrame(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);

    static TextureData* decodeTexture(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);
    static ContourData* decodeContour(CocoLoader* cocoLoader, stExpCocoNode* pCocoNode);

    static void decodeNode(BaseData* node, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode, DataInfo* dataInfo);

protected:
    void loadData();

    std::condition_variable _sleepCondition;

    std::thread* _loadingThread;

    std::mutex _sleepMutex;

    std::mutex _asyncStructQueueMutex;
    std::mutex _dataInfoMutex;

    std::mutex _addDataMutex;

    std::mutex _getFileMutex;

    unsigned long _asyncRefCount;
    unsigned long _asyncRefTotalCount;

    bool need_quit;

    std::queue<AsyncStruct*>* _asyncStructQueue;
    std::queue<DataInfo*>* _dataQueue;

    static std::vector<std::string> _configFileList;

    static DataReaderHelper* _dataReaderHelper;
};

}  // namespace cocostudio

#endif /*__CCDATAREADERHELPER_H__*/
