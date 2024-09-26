#include "UBBParser.h"
#include "base/format.h"

NS_FGUI_BEGIN
using namespace ax;
using namespace std;

UBBParser* UBBParser::_inst = nullptr;

UBBParser * UBBParser::getInstance()
{
    if (!_inst)
        _inst = new UBBParser();
    return _inst;
}

UBBParser::UBBParser() :
    defaultImgWidth(0),
    defaultImgHeight(0),
    _pString(nullptr),
    _readPos(0)
{
    _handlers["url"] = UBB_TAG_HANDLER(UBBParser::onTag_URL, this);
    _handlers["img"] = UBB_TAG_HANDLER(UBBParser::onTag_IMG, this);
    _handlers["b"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["i"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["u"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["sup"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["sub"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["color"] = UBB_TAG_HANDLER(UBBParser::onTag_COLOR, this);
    _handlers["font"] = UBB_TAG_HANDLER(UBBParser::onTag_FONT, this);
    _handlers["size"] = UBB_TAG_HANDLER(UBBParser::onTag_SIZE, this);
    _handlers["align"] = UBB_TAG_HANDLER(UBBParser::onTag_ALIGN, this);
}

UBBParser::~UBBParser()
{

}

std::string UBBParser::parse(const char * text, bool remove)
{
    _pString = text;
    _readPos = 0;
    lastColor.clear();
    lastFontSize.clear();

    ssize_t pos;
    bool end;
    string tag, attr;
    string repl;
    string out;

    while (*_pString != '\0')
    {
        const char* p = strchr(_pString, '[');
        if (!p)
        {
            out.append(_pString);
            break;
        }

        pos = p - _pString;
        if (pos > 0 && *(p - 1) == '\\')
        {
            out.append(_pString, pos - 1);
            out.append("[");
            _pString += pos + 1;
            continue;
        }

        out.append(_pString, pos);
        _pString += pos;

        p = strchr(_pString, ']');
        if (!p)
        {
            out.append(_pString);
            break;
        }

        pos = p - _pString;
        if (pos == 1)
        {
            out.append(_pString, 0, 2);
            _pString += 2;
            continue;
        }

        end = _pString[1] == '/';
        if (end)
            tag.assign(_pString + 2, pos - 2);
        else
            tag.assign(_pString + 1, pos - 1);
        _readPos = pos + 1;

        attr.clear();
        repl.clear();
        pos = tag.find('=');
        if (pos != -1)
        {
            attr = tag.substr(pos + 1);
            tag = tag.substr(0, pos);
        }
        transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
        auto it = _handlers.find(tag);
        if (it != _handlers.end())
        {
            it->second(tag, end, attr, repl);
            if (!remove)
                out.append(repl);
        }
        else
            out.append(_pString, _readPos);
        _pString += _readPos;
    }
    return out;
}

void UBBParser::getTagText(std::string& out, bool remove)
{
    const char* p = strchr(_pString + _readPos, '[');
    if (!p)
        return;

    ssize_t pos = p - _pString;
    out.assign(_pString, _readPos, pos - _readPos);
    if (remove)
        _readPos = pos;
}

void UBBParser::onTag_URL(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end)
    {
        if (!attr.empty())
            replacement = "<a href=\"" + attr + "\" target=\"_blank\">";
        else
        {
            string href;
            getTagText(href, false);
            replacement = "<a href=\"" + href + "\" target=\"_blank\">";
        }
    }
    else
        replacement = "</a>";
}

void UBBParser::onTag_IMG(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end)
    {
        std::string src;
        getTagText(src, true);

        if (src.empty())
            return;
        if (defaultImgWidth != 0)
            replacement = fmt::format(R"(<img src="{}" width="{}" height="{}"/>)", src, defaultImgWidth, defaultImgHeight);
        else
            replacement = "<img src=\"" + src + "\"/>";
    }
}

void UBBParser::onTag_Simple(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    replacement = end ? ("</" + tagName + ">") : ("<" + tagName + ">");
}

void UBBParser::onTag_COLOR(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end) {
        replacement = "<font color=\"" + attr + "\">";
        lastColor = attr;
    }
    else
        replacement = "</font>";
}

void UBBParser::onTag_FONT(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end)
        replacement = "<font face=\"" + attr + "\">";
    else
        replacement = "</font>";
}

void UBBParser::onTag_SIZE(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end) {
        replacement = "<font size=\"" + attr + "\">";
        lastFontSize = attr;
    }
    else
        replacement = "</font>";
}

void UBBParser::onTag_ALIGN(const std::string & tagName, bool end, const std::string & attr, std::string& replacement)
{
    if (!end)
        replacement = "<p align=\"" + attr + "\">";
    else
        replacement = "</p>";
}

NS_FGUI_END
