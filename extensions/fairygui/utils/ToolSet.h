#ifndef __TOOLSET_H__
#define __TOOLSET_H__

#include "cocos2d.h"
#include "FairyGUI.h"

NS_FGUI_BEGIN

class ToolSet
{
public:
    static cocos2d::Color4B hexToColor(const char* str);
    static cocos2d::Color3B intToColor(unsigned int rgb);
    static unsigned int colorToInt(const cocos2d::Color3B& color);

    static cocos2d::Rect intersection(const cocos2d::Rect& rect1, const cocos2d::Rect& rect2);
    static int findInStringArray(const std::vector<std::string>& arr, const std::string& str);

    static bool isFileExist(const std::string& fileName);
};

class FastSplitter
{
public:
    FastSplitter();
    void start(const char* data, ssize_t dataLength, char delimiter);
    bool next();
    const char* getText();
    ssize_t getTextLength();
    void getKeyValuePair(char* keyBuf, ssize_t keyBufSize, char* valueBuf, ssize_t valueBufSize);

private:
    const char* data;
    ssize_t dataLength;
    ssize_t textLength;
    char delimiter;
};

NS_FGUI_END

#endif
