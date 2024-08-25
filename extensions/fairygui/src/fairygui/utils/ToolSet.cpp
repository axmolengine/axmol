#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;
using namespace std;

Color4B ToolSet::hexToColor(const char* str)
{
    ssize_t len = strlen(str);
    if (len < 7 || str[0] != '#')
        return Color4B::BLACK;

    char temp[3];
    memset(temp, 0, 3);

    if (len == 9)
    {
        return Color4B(strtol(strncpy(temp, str + 3, 2), NULL, 16),
            strtol(strncpy(temp, str + 5, 2), NULL, 16),
            strtol(strncpy(temp, str + 7, 2), NULL, 16),
            strtol(strncpy(temp, str + 1, 2), NULL, 16));
    }
    else
    {
        return Color4B(strtol(strncpy(temp, str + 1, 2), NULL, 16),
            strtol(strncpy(temp, str + 3, 2), NULL, 16),
            strtol(strncpy(temp, str + 5, 2), NULL, 16),
            255);
    }
}

ax::Color3B ToolSet::intToColor(unsigned int rgb)
{
    return Color3B((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

unsigned int ToolSet::colorToInt(const ax::Color3B& color)
{
    return (color.r << 16) + (color.g << 8) + color.b;
}

Rect ToolSet::intersection(const Rect& rect1, const Rect& rect2)
{
    if (rect1.size.width == 0 || rect1.size.height == 0 || rect2.size.width == 0 || rect2.size.height == 0)
        return Rect(0, 0, 0, 0);

    float left = rect1.getMinX() > rect2.getMinX() ? rect1.getMinX() : rect2.getMinX();
    float right = rect1.getMaxX() < rect2.getMaxX() ? rect1.getMaxX() : rect2.getMaxX();
    float top = rect1.getMinY() > rect2.getMinY() ? rect1.getMinY() : rect2.getMinY();
    float bottom = rect1.getMaxY() < rect2.getMaxY() ? rect1.getMaxY() : rect2.getMaxY();

    if (left > right || top > bottom)
        return Rect(0, 0, 0, 0);
    else
        return Rect(left, top, right - left, bottom - top);
}

int ToolSet::findInStringArray(const std::vector<std::string>& arr, const std::string& str)
{
    auto iter = std::find(arr.begin(), arr.end(), str);
    if (iter != arr.end())
        return (int)(iter - arr.begin());

    return -1;
}

bool ToolSet::isFileExist(const std::string & fileName)
{
    bool tmp = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    bool ret = FileUtils::getInstance()->isFileExist(fileName);
    FileUtils::getInstance()->setPopupNotify(tmp);
    return ret;
}

FastSplitter::FastSplitter() : data(nullptr), dataLength(-1), delimiter('\0')
{
}

void FastSplitter::start(const char* data, ssize_t dataLength, char delimiter)
{
    this->data = data;
    this->dataLength = dataLength;
    this->delimiter = delimiter;
    this->textLength = -1;
}

bool FastSplitter::next()
{
    if (dataLength < 0)
        return false;

    if (dataLength == 0)
    {
        dataLength = -1;
        textLength = 0;
        return true;
    }

    data += textLength + 1;
    char* found = (char*)memchr(data, (int)delimiter, dataLength);
    if (found)
        textLength = found - data;
    else
        textLength = dataLength;
    dataLength -= (textLength + 1);

    return true;
}

const char* FastSplitter::getText()
{
    if (textLength > 0)
        return data;
    else
        return nullptr;
}

ssize_t FastSplitter::getTextLength()
{
    return textLength;
}

void FastSplitter::getKeyValuePair(char* keyBuf, ssize_t keyBufSize, char* valueBuf, ssize_t valueBufSize)
{
    if (textLength == 0)
    {
        keyBuf[0] = '\0';
        valueBuf[0] = '\0';
    }
    else
    {
        char* found = (char*)memchr(data, (int)'=', textLength);
        if (found)
        {
            ssize_t len = MIN(keyBufSize - 1, found - data);
            memcpy(keyBuf, data, len);
            keyBuf[len] = '\0';

            len = MIN(valueBufSize - 1, textLength - (found - data) - 1);
            memcpy(valueBuf, found + 1, len);
            valueBuf[len] = '\0';
        }
        else
        {
            ssize_t len = MIN(valueBufSize - 1, textLength);
            memcpy(keyBuf, data, len);
            keyBuf[len] = '\0';
            valueBuf[0] = '\0';
        }
    }
}

NS_FGUI_END
