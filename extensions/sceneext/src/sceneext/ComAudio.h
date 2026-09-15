/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "sceneext/ComBase.h"
#include "axmol/base/Protocols.h"
#include "axmol/scene/Component.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class SCNEXT_API ComAudio : public ax::Component, public ax::PlayableProtocol
{
    DECLARE_CLASS_COMPONENT_INFO
public:
    const static std::string COMPONENT_NAME;

    /**
     */
    ComAudio();
    /**
     * @lua NA
     */
    virtual ~ComAudio();

public:
    static ComAudio* create();

    bool init() override;
    /**
     * @lua NA
     */
    void onEnter() override;
    /**
     * @lua NA
     */
    void onExit() override;
    /**
     * @lua NA
     */
    void onAdd() override;
    /**
     * @lua NA
     */
    void onRemove() override;

    bool serialize(void* r) override;

public:
    /**
     * @lua endToLua
     */
    void end();
    void preloadBackgroundMusic(const char* pszFilePath);
    void playBackgroundMusic(const char* pszFilePath, bool bLoop);
    void playBackgroundMusic(const char* pszFilePath);
    void playBackgroundMusic();
    void stopBackgroundMusic(bool bReleaseData);
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    void rewindBackgroundMusic();
    bool willPlayBackgroundMusic();
    bool isBackgroundMusicPlaying();
    float getBackgroundMusicVolume();
    void setBackgroundMusicVolume(float volume);
    float getEffectsVolume();
    void setEffectsVolume(float volume);
    unsigned int playEffect(const char* pszFilePath, bool bLoop);
    unsigned int playEffect(const char* pszFilePath);
    unsigned int playEffect();
    void pauseEffect(unsigned int nSoundId);
    void pauseAllEffects();
    void resumeEffect(unsigned int nSoundId);
    void resumeAllEffects();
    void stopEffect(unsigned int nSoundId);
    void stopAllEffects();
    void preloadEffect(const char* pszFilePath);
    void unloadEffect(const char* pszFilePath);
    void setFile(const char* pszFilePath);
    const char* getFile();
    void setLoop(bool bLoop);
    bool isLoop();

    /// @{
    /// @name implement Playable Protocol
    // play the effect sound path in _filePath
    void start() override;
    // stop the effect sound which started with latest start()
    void stop() override;
    /// @} end of PlayableProtocol

private:
    std::string _filePath;
    bool _loop;

    unsigned int _startedSoundId;  // !playing sound id from start(), not playEffect
};

}  // namespace ax::ext
