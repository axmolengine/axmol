#ifndef __FUILABEL_H__
#define __FUILABEL_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "TextFormat.h"

NS_FGUI_BEGIN

class FUILabel : public ax::Label
{
public:
    FUILabel();
    virtual ~FUILabel();

    CREATE_FUNC(FUILabel);

    std::string_view getText() const { return getString(); }
    void setText(std::string_view value);

    TextFormat* getTextFormat() const { return _textFormat; }
    void applyTextFormat();

    void setUnderlineColor(const ax::Color3B& value);

    virtual bool setBMFontFilePath(std::string_view bmfontFilePath, const ax::Vec2& imageOffset = ax::Vec2::ZERO, float fontSize = 0) override;

    void setGrayed(bool value);
protected:
    /*
    ע�⣡���������������˱��������Ҫ�޸�cocos2d��Դ�룬�ļ�2d/CCLabel.h����Լ��672�У�ΪupdateBMFontScale��������virtual���η���
    ��Ϊ�����������ǿ���������ָ��ΪFontFnt���͵Ĵ��룬�����ǲ�ʹ��FontFnt��FontFntֻ֧�ִ��ⲿ�ļ����������ã���������BMFontConfiguration�Ƕ�����cpp��ġ���
    ������Ҫ��д���������
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
