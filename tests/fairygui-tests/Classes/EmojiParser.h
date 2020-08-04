#ifndef __EMOJIPARSER_H__
#define __EMOJIPARSER_H__

#include "FairyGUI.h"
#include "utils/UBBParser.h"

USING_NS_FGUI;

class EmojiParser : public UBBParser
{
public:
    static EmojiParser* getInstance() { return _inst; }
    EmojiParser();

protected:
    void onTag_Emoji(const std::string& tagName, bool end, const std::string& attr, std::string& replacement);

private:
    static EmojiParser* _inst;
};

#endif