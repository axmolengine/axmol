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

#ifndef __CCARMATUREDATAMANAGER_H__
#define __CCARMATUREDATAMANAGER_H__

#include "ArmatureDefine.h"
#include "Datas.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

struct RelativeData
{
    std::vector<std::string> plistFiles;
    std::vector<std::string> armatures;
    std::vector<std::string> animations;
    std::vector<std::string> textures;
};

/**
 *    @brief    format and manage armature configuration and armature animation
 */
class CCS_DLL ArmatureDataManager : public ax::Object
{
public:
    /** @deprecated Use getInstance() instead */
    AX_DEPRECATED_ATTRIBUTE static ArmatureDataManager* sharedArmatureDataManager()
    {
        return ArmatureDataManager::getInstance();
    }

    /** @deprecated Use destroyInstance() instead */
    AX_DEPRECATED_ATTRIBUTE static void purge() { ArmatureDataManager::destroyInstance(); };

    static ArmatureDataManager* getInstance();
    static void destroyInstance();

protected:
    /**
     * @js ctor
     */
    ArmatureDataManager(void);
    /**
     * @js NA
     * @lua NA
     */
    ~ArmatureDataManager(void);

public:
    /**
     * Init ArmatureDataManager
     */
    virtual bool init();

    /**
     * Add armature data
     * @param id The id of the armature data
     * @param armatureData ArmatureData *
     */
    void addArmatureData(std::string_view id, ArmatureData* armatureData, std::string_view configFilePath = "");

    /**
     *    @brief    get armature data
     *    @param    id the id of the armature data you want to get
     *  @return    ArmatureData *
     */
    ArmatureData* getArmatureData(std::string_view id);

    /**
     *    @brief    remove armature data
     *    @param    id the id of the armature data you want to get
     */
    void removeArmatureData(std::string_view id);

    /**
     *    @brief    add animation data
     *    @param     id the id of the animation data
     *  @return AnimationData *
     */
    void addAnimationData(std::string_view id, AnimationData* animationData, std::string_view configFilePath = "");

    /**
     *    @brief    get animation data from _animationDatas(Dictionary)
     *    @param     id the id of the animation data you want to get
     *  @return AnimationData *
     */
    AnimationData* getAnimationData(std::string_view id);

    /**
     *    @brief    remove animation data
     *    @param     id the id of the animation data
     */
    void removeAnimationData(std::string_view id);

    /**
     *    @brief    add texture data
     *    @param     id the id of the texture data
     *  @return TextureData *
     */
    void addTextureData(std::string_view id, TextureData* textureData, std::string_view configFilePath = "");

    /**
     *    @brief    get texture data
     *    @param     id the id of the texture data you want to get
     *  @return TextureData *
     */
    TextureData* getTextureData(std::string_view id);

    /**
     *    @brief    remove texture data
     *    @param     id the id of the texture data you want to get
     */
    void removeTextureData(std::string_view id);

    /**
     *    @brief    Add ArmatureFileInfo, it is managed by ArmatureDataManager.
     */
    void addArmatureFileInfo(std::string_view configFilePath);

    /**
     *    @brief    Add ArmatureFileInfo, it is managed by ArmatureDataManager.
     *            It will load data in a new thread
     */
    void addArmatureFileInfoAsync(std::string_view configFilePath,
                                  ax::Object* target,
                                  ax::SEL_SCHEDULE selector);

    /**
     *    @brief    Add ArmatureFileInfo, it is managed by ArmatureDataManager.
     */
    void addArmatureFileInfo(std::string_view imagePath, std::string_view plistPath, std::string_view configFilePath);

    /**
     *    @brief    Add ArmatureFileInfo, it is managed by ArmatureDataManager.
     *            It will load data in a new thread
     */
    void addArmatureFileInfoAsync(std::string_view imagePath,
                                  std::string_view plistPath,
                                  std::string_view configFilePath,
                                  ax::Object* target,
                                  ax::SEL_SCHEDULE selector);

    /**
     *    @brief    Add sprite frame to CCSpriteFrameCache, it will save display name and it's relative image name
     */
    void addSpriteFrameFromFile(std::string_view plistPath,
                                std::string_view imagePath,
                                std::string_view configFilePath = "");

    virtual void removeArmatureFileInfo(std::string_view configFilePath);

    /**
     *    @brief    Judge whether or not need auto load sprite file
     */
    bool isAutoLoadSpriteFile();

    const ax::StringMap<ArmatureData*>& getArmatureDatas() const;
    const ax::StringMap<AnimationData*>& getAnimationDatas() const;
    const ax::StringMap<TextureData*>& getTextureDatas() const;

public:
    void addRelativeData(std::string_view configFilePath);
    RelativeData* getRelativeData(std::string_view configFilePath);

private:
    /**
     *    @brief    save armature datas
     *  @key    std::string
     *  @value    ArmatureData *
     */
    ax::StringMap<ArmatureData*> _armarureDatas;

    /**
     *    @brief    save animation datas
     *  @key    std::string
     *  @value    AnimationData *
     */
    ax::StringMap<AnimationData*> _animationDatas;

    /**
     *    @brief    save texture datas
     *  @key    std::string
     *  @value    TextureData *
     */
    ax::StringMap<TextureData*> _textureDatas;

    bool _autoLoadSpriteFile;

    hlookup::string_map<RelativeData> _relativeDatas;
};

}  // namespace cocostudio

#endif /*__CCARMATUREDATAMANAGER_H__*/
