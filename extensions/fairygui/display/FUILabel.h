#ifndef __FUILABEL_H__
#define __FUILABEL_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "TextFormat.h"

NS_FGUI_BEGIN

class FUILabel : public cocos2d::Label
{
public:
    FUILabel();
    virtual ~FUILabel();

    CREATE_FUNC(FUILabel);

    const std::string& getText() const { return getString(); }
    void setText(const std::string& value);

    TextFormat* getTextFormat() const { return _textFormat; }
    void applyTextFormat();

    void setUnderlineColor(const cocos2d::Color3B& value);

    virtual bool setBMFontFilePath(const std::string& bmfontFilePath, const cocos2d::Vec2& imageOffset = cocos2d::Vec2::ZERO, float fontSize = 0) override;

    void setGrayed(bool value);
protected:
    /*
    注意！！！如果这里出现了编译错误，需要修改cocos2d的源码，文件2d/CCLabel.h，大约在672行，为updateBMFontScale函数打上virtual修饰符。
    因为这个方法里有强制字体对象指针为FontFnt类型的代码，但我们不使用FontFnt（FontFnt只支持从外部文件中载入配置，更糟糕的是BMFontConfiguration是定义在cpp里的。）
    所以需要重写这个方法。
    */
    virtual void updateBMFontScale() override;

private:
    TextFormat* _textFormat;
    std::string _fontName;
    int _fontSize;
    bool _bmFontCanTint;
    bool _grayed;
};

NS_FGUI_END

#endif
