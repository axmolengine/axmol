#ifndef __TRANSLATIONHELPER_H_
#define __TRANSLATIONHELPER_H_

#include "FairyGUIMacros.h"
#include "axmol/cocos2d.h"

NS_FGUI_BEGIN

class PackageItem;

class TranslationHelper
{
public:
    static axstd::string_map<axstd::string_map<std::string>> strings;

    static void loadFromXML(const char *xmlString, size_t nBytes);
    static void translateComponent(PackageItem* item);
};

NS_FGUI_END

#endif
