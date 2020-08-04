#ifndef __BITMAPFONT_H__
#define __BITMAPFONT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "PackageItem.h"

NS_FGUI_BEGIN

class BitmapFont : public cocos2d::Font
{
public:
    BitmapFont();
    virtual ~BitmapFont();

    static BitmapFont* create();

    virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
    virtual cocos2d::FontAtlas *createFontAtlas() override { return _fontAtlas; }
    void releaseAtlas() { _fontAtlas->release(); }
    void setFontSize(float fontSize) {}
    int getOriginalFontSize()const { return _originalFontSize; }
    bool isResizable() { return _resizable; }
    bool canTint() { return _canTint; }

private:
    float _originalFontSize;
    bool _resizable;
    bool _canTint;
    cocos2d::FontAtlas* _fontAtlas;

    friend class UIPackage;
};

NS_FGUI_END

#endif