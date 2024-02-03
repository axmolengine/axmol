#ifndef __BITMAPFONT_H__
#define __BITMAPFONT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "PackageItem.h"

NS_FGUI_BEGIN

class BitmapFont : public ax::Font
{
public:
    BitmapFont();
    virtual ~BitmapFont();

    static BitmapFont* create();

    virtual int* getHorizontalKerningForTextUTF32(const std::u32string& text, int &outNumLetters) const override;
#    if defined(AX_VERSION)
    virtual ax::FontAtlas* newFontAtlas() override { return new ax::FontAtlas(this); }
#else
    virtual ax::FontAtlas* createFontAtlas() override { return new ax::FontAtlas(*this); }
#endif
    ax::FontAtlas* resetFontAtlas(ax::FontAtlas* fontAtlas)
    {
        if (_fontAtlas != fontAtlas)
        {
            AX_SAFE_RELEASE(_fontAtlas);
            _fontAtlas = fontAtlas;
        }
        return _fontAtlas;
    }
    ax::FontAtlas* getFontAtlas() const { return _fontAtlas; }
    void releaseAtlas() { resetFontAtlas(nullptr); }
    void setFontSize(float fontSize) {}
    int getOriginalFontSize()const { return _originalFontSize; }
    bool isResizable() { return _resizable; }
    bool canTint() { return _canTint; }

private:
    float _originalFontSize;
    bool _resizable;
    bool _canTint;
    ax::FontAtlas* _fontAtlas;

    friend class UIPackage;
};

NS_FGUI_END

#endif