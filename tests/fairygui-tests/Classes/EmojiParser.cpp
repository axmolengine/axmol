#include "EmojiParser.h"

EmojiParser* EmojiParser::_inst = new EmojiParser();

EmojiParser::EmojiParser()
{
    std::vector<std::string> tags({ "88","am","bs","bz","ch","cool","dhq","dn","fd","gz","han","hx","hxiao","hxiu" });

    for (auto &str : tags)
        _handlers[":" + str] = UBB_TAG_HANDLER(EmojiParser::onTag_Emoji, this);
}

void EmojiParser::onTag_Emoji(const std::string & tagName, bool end, const std::string & attr, std::string & replacement)
{
    std::string str = tagName.substr(1);
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    replacement = "<img src='ui://Emoji/" + str + "'/>";
}
