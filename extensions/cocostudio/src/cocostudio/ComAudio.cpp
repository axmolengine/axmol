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

#include "ComAudio.h"
#include "platform/FileUtils.h"

namespace cocostudio
{

IMPLEMENT_CLASS_COMPONENT_INFO(ComAudio)

const std::string ComAudio::COMPONENT_NAME = "CCComAudio";

ComAudio::ComAudio() : _filePath(""), _loop(false), _startedSoundId(0)
{
    _name = COMPONENT_NAME;
}

ComAudio::~ComAudio() {}

bool ComAudio::init()
{
    return true;
}

void ComAudio::onEnter() {}

void ComAudio::onExit()
{
    stopBackgroundMusic(true);
    stopAllEffects();
}

void ComAudio::onAdd() {}

void ComAudio::onRemove()
{
    stopBackgroundMusic(true);
    stopAllEffects();
}

bool ComAudio::serialize(void* r)
{
    bool ret = false;
    do
    {
        AX_BREAK_IF(r == nullptr);
        SerData* serData          = (SerData*)(r);
        const rapidjson::Value* v = serData->_rData;
        stExpCocoNode* cocoNode   = serData->_cocoNode;
        CocoLoader* cocoLoader    = serData->_cocoLoader;
        const char* className     = nullptr;
        const char* comName       = nullptr;
        const char* file          = nullptr;
        std::string filePath;
        int resType = 0;
        bool loop   = false;
        if (v != nullptr)
        {
            className = DICTOOL->getStringValue_json(*v, "classname");
            AX_BREAK_IF(className == nullptr);
            comName                          = DICTOOL->getStringValue_json(*v, "name");
            const rapidjson::Value& fileData = DICTOOL->getSubDictionary_json(*v, "fileData");
            AX_BREAK_IF(!DICTOOL->checkObjectExist_json(fileData));
            file = DICTOOL->getStringValue_json(fileData, "path");
            AX_BREAK_IF(file == nullptr);
            resType = DICTOOL->getIntValue_json(fileData, "resourceType", -1);
            AX_BREAK_IF(resType != 0);
            loop = DICTOOL->getIntValue_json(*v, "loop") != 0 ? true : false;
        }
        else if (cocoNode != nullptr)
        {
            className = cocoNode[1].GetValue(cocoLoader);
            AX_BREAK_IF(className == nullptr);
            comName                  = cocoNode[2].GetValue(cocoLoader);
            stExpCocoNode* pfileData = cocoNode[4].GetChildArray(cocoLoader);
            AX_BREAK_IF(!pfileData);
            file = pfileData[0].GetValue(cocoLoader);
            AX_BREAK_IF(file == nullptr);
            resType = atoi(pfileData[2].GetValue(cocoLoader));
            AX_BREAK_IF(resType != 0);
            loop = atoi(cocoNode[5].GetValue(cocoLoader)) != 0 ? true : false;
            ret  = true;
        }
        if (comName != nullptr)
        {
            setName(comName);
        }
        else
        {
            setName(className);
        }
        if (file != nullptr)
        {
            if (strcmp(file, "") == 0)
            {
                continue;
            }
            filePath.assign(ax::FileUtils::getInstance()->fullPathForFilename(file));
        }
        if (strcmp(className, "CCBackgroundAudio") == 0)
        {
            preloadBackgroundMusic(filePath.c_str());
            setLoop(loop);
            playBackgroundMusic(filePath.c_str(), loop);
        }
        else if (strcmp(className, COMPONENT_NAME.c_str()) == 0)
        {
            preloadEffect(filePath.c_str());
        }
        else
        {
            AX_BREAK_IF(true);
        }
        ret = true;
    } while (0);
    return ret;
}

ComAudio* ComAudio::create()
{
    ComAudio* pRet = new ComAudio();
    if (pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(pRet);
    }
    return pRet;
}

void ComAudio::end() {}

void ComAudio::preloadBackgroundMusic(const char* pszFilePath)
{
    // TODO: use audio engine to play
    // CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic(pszFilePath);
    setFile(pszFilePath);
    setLoop(false);
}

void ComAudio::playBackgroundMusic(const char* pszFilePath, bool loop)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(pszFilePath, loop);
}

void ComAudio::playBackgroundMusic(const char* pszFilePath)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(pszFilePath);
}

void ComAudio::playBackgroundMusic()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
}

void ComAudio::stopBackgroundMusic(bool bReleaseData)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(bReleaseData);
}

void ComAudio::stopBackgroundMusic()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void ComAudio::pauseBackgroundMusic()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void ComAudio::resumeBackgroundMusic()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

void ComAudio::rewindBackgroundMusic()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->rewindBackgroundMusic();
}

bool ComAudio::willPlayBackgroundMusic()
{
    return false;  // CocosDenshion::SimpleAudioEngine::getInstance()->willPlayBackgroundMusic();
}

bool ComAudio::isBackgroundMusicPlaying()
{
    return false;  // CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

float ComAudio::getBackgroundMusicVolume()
{
    return 0;  // CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicVolume();
}

void ComAudio::setBackgroundMusicVolume(float volume)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

float ComAudio::getEffectsVolume()
{
    return 0;  // CocosDenshion::SimpleAudioEngine::getInstance()->getEffectsVolume();
}

void ComAudio::setEffectsVolume(float volume)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->setEffectsVolume(volume);
}

unsigned int ComAudio::playEffect(const char* pszFilePath, bool loop)
{
    return 0;  // CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(pszFilePath, loop);
}

unsigned int ComAudio::playEffect(const char* pszFilePath)
{
    return 0;  // CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(pszFilePath);
}

unsigned int ComAudio::playEffect()
{
    return 0;  // CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(_filePath.c_str(), _loop);
}

void ComAudio::pauseEffect(unsigned int nSoundId)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->pauseEffect(nSoundId);
}

void ComAudio::pauseAllEffects()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
}

void ComAudio::resumeEffect(unsigned int nSoundId)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->resumeEffect(nSoundId);
}

void ComAudio::resumeAllEffects()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
}

void ComAudio::stopEffect(unsigned int nSoundId)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(nSoundId);
}

void ComAudio::stopAllEffects()
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
}

void ComAudio::preloadEffect(const char* pszFilePath)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(pszFilePath);
    setFile(pszFilePath);
    setLoop(false);
}

void ComAudio::unloadEffect(const char* pszFilePath)
{
    // CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(pszFilePath);
}

void ComAudio::setFile(const char* pszFilePath)
{
    _filePath.assign(pszFilePath);
}

void ComAudio::setLoop(bool loop)
{
    _loop = loop;
}

const char* ComAudio::getFile()
{
    return _filePath.c_str();
}

bool ComAudio::isLoop()
{
    return _loop;
}

void ComAudio::start()
{
    _startedSoundId = playEffect();
}

void ComAudio::stop()
{
    stopEffect(_startedSoundId);
}
}  // namespace cocostudio
