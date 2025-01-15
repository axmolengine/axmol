#ifndef __TOOLSET_H__
#define __TOOLSET_H__

#include "cocos2d.h"
#include "FairyGUI.h"

NS_FGUI_BEGIN

class ToolSet
{
public:
    static ax::Color32 hexToColor(const char* str);
    static ax::Color3B intToColor(unsigned int rgb);
    static unsigned int colorToInt(const ax::Color3B& color);

    static ax::Rect intersection(const ax::Rect& rect1, const ax::Rect& rect2);
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
