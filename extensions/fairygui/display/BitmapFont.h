#ifndef __BITMAPFONT_H__
#define __BITMAPFONT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "PackageItem.h"

NS_FGUI_BEGIN

class BitmapFont : public axis::Font
{
public:
    BitmapFont();
    virtual ~BitmapFont();

    static BitmapFont* create();

    virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
#    if defined(AXYS_VERSION)
    virtual axis::FontAtlas* newFontAtlas() override { return new axis::FontAtlas(this); }
#else
    virtual axis::FontAtlas* createFontAtlas() override { return new axis::FontAtlas(*this); }
#endif
    axis::FontAtlas* resetFontAtlas(axis::FontAtlas* fontAtlas)
    {
        if (_fontAtlas != fontAtlas)
        {
            AX_SAFE_RELEASE(_fontAtlas);
            _fontAtlas = fontAtlas;
        }
        return _fontAtlas;
    }
    axis::FontAtlas* getFontAtlas() const { return _fontAtlas; }
    void releaseAtlas() { resetFontAtlas(nullptr); }
    void setFontSize(float fontSize) {}
    int getOriginalFontSize()const { return _originalFontSize; }
    bool isResizable() { return _resizable; }
    bool canTint() { return _canTint; }

private:
    float _originalFontSize;
    bool _resizable;
    bool _canTint;
    axis::FontAtlas* _fontAtlas;

    friend class UIPackage;
};

NS_FGUI_END

#endif