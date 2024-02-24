#ifndef __TRANSLATIONHELPER_H_
#define __TRANSLATIONHELPER_H_

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class PackageItem;

class TranslationHelper
{
public:
    static hlookup::string_map<hlookup::string_map<std::string>> strings;

    static void loadFromXML(const char *xmlString, size_t nBytes);
    static void translateComponent(PackageItem* item);
};

NS_FGUI_END

#endif
