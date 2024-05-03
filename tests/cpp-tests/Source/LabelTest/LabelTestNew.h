/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef _ATLAS_TEST_NEW_H_
#define _ATLAS_TEST_NEW_H_

#include "../BaseTest.h"
#include "renderer/CustomCommand.h"
#include "ui/CocosGUI.h"
#include "extensions/axmol-ext.h"
#include "cocostudio/LocalizationManager.h"

DEFINE_TEST_SUITE(NewLabelTests);

class AtlasDemoNew : public TestCase
{};

class LabelFNTColorAndOpacity : public AtlasDemoNew
{
    float _time;

public:
    CREATE_FUNC(LabelFNTColorAndOpacity);

    LabelFNTColorAndOpacity();

    virtual void step(float dt);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTSpriteActions : public AtlasDemoNew
{
    float _time;

public:
    CREATE_FUNC(LabelFNTSpriteActions);

    LabelFNTSpriteActions();
    virtual void step(float dt);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTPadding : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTPadding);

    LabelFNTPadding();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTOffset : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTOffset);

    LabelFNTOffset();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTColor : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTColor);

    LabelFNTColor();
    virtual std::string title() const override;

    virtual std::string subtitle() const override;
};

class LabelFNTOpacity : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTOpacity);

    LabelFNTOpacity();
    virtual std::string title() const override;

    virtual std::string subtitle() const override;
};

class LabelFNTHundredLabels : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTHundredLabels);

    LabelFNTHundredLabels();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTMultiLine : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTMultiLine);

    LabelFNTMultiLine();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTandTTFEmpty : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTandTTFEmpty);

    LabelFNTandTTFEmpty();
    void updateStrings(float dt);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    bool setEmpty;
};

class LabelFNTRetina : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTRetina);

    LabelFNTRetina();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTGlyphDesigner : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTGlyphDesigner);

    LabelFNTGlyphDesigner();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTMultiLineAlignment : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTMultiLineAlignment);

    virtual bool init() override;
    void snapArrowsToEdge();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    void stringChanged(ax::Object* sender);
    void alignmentChanged(ax::Object* sender);
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void selectAlignmentItem(ax::MenuItemFont* item);
    void selectSentenceItem(ax::MenuItemFont* item);
    virtual std::string getItemString(ax::MenuItemFont* item);

protected:
    ax::Label* _label      = nullptr;
    ax::Sprite* _arrowsBar = nullptr;
    ax::Sprite* _arrows    = nullptr;
    std::vector<ax::MenuItemFont*> _menuItems;
    ax::MenuItemFont* _lastSentenceItem  = nullptr;
    ax::MenuItemFont* _lastAlignmentItem = nullptr;
    bool _drag                                = false;
};

class LabelFNTMultiLineAlignmentUNICODE : public LabelFNTMultiLineAlignment
{
public:
    CREATE_FUNC(LabelFNTMultiLineAlignmentUNICODE);

    bool init() override;

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual std::string getItemString(ax::MenuItemFont* item) override;
};

class LabelFNTUNICODELanguages : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTUNICODELanguages);

    LabelFNTUNICODELanguages();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTBounds : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTBounds);

    LabelFNTBounds();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTMultiFontAtlasNoRotation : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTMultiFontAtlasNoRotation);

    LabelFNTMultiFontAtlasNoRotation();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFNTMultiFontAtlasWithRotation : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFNTMultiFontAtlasWithRotation);

    LabelFNTMultiFontAtlasWithRotation();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFLongLineWrapping : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFLongLineWrapping);

    LabelTTFLongLineWrapping();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFColor : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFColor);

    LabelTTFColor();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFUnicodeNew : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFUnicodeNew);

    LabelTTFUnicodeNew();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFDynamicAlignment : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFDynamicAlignment);

    LabelTTFDynamicAlignment();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    void setAlignmentLeft(ax::Object* sender);
    void setAlignmentCenter(ax::Object* sender);
    void setAlignmentRight(ax::Object* sender);
    void updateAlignment();

    ax::Label* _label;
    ax::TextHAlignment _horizAlign;
};

class LabelTTFEmoji : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFEmoji);

    LabelTTFEmoji();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFCJKWrappingTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFCJKWrappingTest);

    LabelTTFCJKWrappingTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
};

class LabelTTFFontsTestNew : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFFontsTestNew);

    LabelTTFFontsTestNew();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFontDefTestNew : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFontDefTestNew);

    LabelFontDefTestNew();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
};

class LabelTTFDistanceField : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFDistanceField);

    LabelTTFDistanceField();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelOutlineAndGlowTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelOutlineAndGlowTest);

    LabelOutlineAndGlowTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelShadowTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelShadowTest);

    LabelShadowTest();

    virtual void onEnter() override;
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void sliderEvent(ax::Object* sender, ax::ui::Slider::EventType type);

private:
    ax::Label* shadowLabelOutline;
    ax::Label* shadowLabelTTF;
    ax::Label* shadowLabelGrow;
    ax::Label* shadowLabelBMFont;
};

class LabelCharMapTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelCharMapTest);

    LabelCharMapTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void step(float dt);

private:
    float _time;
};

class LabelCharMapColorTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelCharMapColorTest);

    LabelCharMapColorTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void step(float dt);
    void actionFinishCallback();

private:
    float _time;
};

class LabelCrashTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelCrashTest);

    LabelCrashTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelTTFOldNew : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelTTFOldNew);

    LabelTTFOldNew();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelFontNameTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelFontNameTest);

    LabelFontNameTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelAlignmentTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelAlignmentTest);

    LabelAlignmentTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    void setAlignmentLeft(ax::Object* sender);
    void setAlignmentCenter(ax::Object* sender);
    void setAlignmentRight(ax::Object* sender);
    void setAlignmentTop(ax::Object* sender);
    void setAlignmentMiddle(ax::Object* sender);
    void setAlignmentBottom(ax::Object* sender);

    ax::Label* _label;
};

class LabelIssue4428Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue4428Test);

    LabelIssue4428Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue4999Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue4999Test);

    LabelIssue4999Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelLineHeightTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelLineHeightTest);

    LabelLineHeightTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void sliderEvent(ax::Object* sender, ax::ui::Slider::EventType type);

private:
    ax::Label* label;
};

class LabelAdditionalKerningTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelAdditionalKerningTest);

    LabelAdditionalKerningTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    void sliderEvent(ax::Object* sender, ax::ui::Slider::EventType type);

private:
    ax::Label* label;
};

class LabelIssue8492Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue8492Test);

    LabelIssue8492Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelMultilineWithOutline : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelMultilineWithOutline);

    LabelMultilineWithOutline();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

// we don't support linebreak mode

class LabelIssue9255Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue9255Test);

    LabelIssue9255Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelSmallDimensionsTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelSmallDimensionsTest);

    LabelSmallDimensionsTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue10089Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue10089Test);

    LabelIssue10089Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelSystemFontColor : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelSystemFontColor);

    LabelSystemFontColor();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue10773Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue10773Test);

    LabelIssue10773Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue11576Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue11576Test);

    LabelIssue11576Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue11699Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue11699Test);

    LabelIssue11699Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue12409Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue12409Test);

    LabelIssue12409Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelAddChildTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelAddChildTest);

    LabelAddChildTest();

    virtual std::string title() const override;
};

class LabelIssue12775Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue12775Test);

    LabelIssue12775Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue11585Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue11585Test);

    LabelIssue11585Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue10688Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue10688Test);

    LabelIssue10688Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue13202Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue13202Test);

    LabelIssue13202Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue9500Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue9500Test);

    LabelIssue9500Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelLayoutBaseTest : public AtlasDemoNew
{
public:
    LabelLayoutBaseTest();
    void updateDrawNodeSize(const ax::Size& drawNodeSize);
    ax::extension::ControlStepper* makeControlStepper();
    void valueChanged(ax::Object* sender, ax::extension::Control::EventType controlEvent);

protected:
    void setAlignmentLeft(ax::Object* sender);
    void setAlignmentCenter(ax::Object* sender);
    void setAlignmentRight(ax::Object* sender);
    void setAlignmentTop(ax::Object* sender);
    void setAlignmentMiddle(ax::Object* sender);
    void setAlignmentBottom(ax::Object* sender);

    void initWrapOption(const ax::Size& size);
    void initToggleLabelTypeOption(const ax::Size& size);
    void initAlignmentOption(const ax::Size& size);
    void initFontSizeChange(const ax::Size& size);
    void initSliders(const ax::Size& size);
    void initTestLabel(const ax::Size& size);
    void initDrawNode(const ax::Size& size);
    ax::DrawNode* _drawNode;
    ax::Label* _label;
    int _labelType;
};

class LabelWrapByWordTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelWrapByWordTest);

    LabelWrapByWordTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelWrapByCharTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelWrapByCharTest);

    LabelWrapByCharTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelWrapNoBreakSpaceTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelWrapNoBreakSpaceTest);

    LabelWrapNoBreakSpaceTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelShrinkByWordTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelShrinkByWordTest);

    LabelShrinkByWordTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelShrinkByCharTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelShrinkByCharTest);

    LabelShrinkByCharTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelResizeTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelResizeTest);

    LabelResizeTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelToggleTypeTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelToggleTypeTest);

    LabelToggleTypeTest();
    void initToggleCheckboxes();
    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelSystemFontTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelSystemFontTest);

    LabelSystemFontTest();
    void initToggleCheckboxes();
    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelCharMapFontTest : public LabelLayoutBaseTest
{
public:
    CREATE_FUNC(LabelCharMapFontTest);

    LabelCharMapFontTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue13846Test : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue13846Test);

    LabelIssue13846Test();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelRichText : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelRichText);

    LabelRichText();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelItalics : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelItalics);

    LabelItalics();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1a;
    ax::Label* _label2a;
};

class LabelBold : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelBold);

    LabelBold();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1a;
    ax::Label* _label2a;
};

class LabelUnderline : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelUnderline);

    LabelUnderline();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1a;
    ax::Label* _label2a;
};

class LabelUnderlineMultiline : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelUnderlineMultiline);

    LabelUnderlineMultiline();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1a;
    ax::Label* _label2a;
};

class LabelStrikethrough : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelStrikethrough);

    LabelStrikethrough();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1a;
    ax::Label* _label2a;
};

class LabelLocalizationTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelLocalizationTest);

    LabelLocalizationTest();
    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

    ax::Label* _label1;
    ax::Label* _label2;
    cocostudio::ILocalizationManager* _localizationJson;
    cocostudio::ILocalizationManager* _localizationBin;
};

class LabelIssue15214 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue15214);

    LabelIssue15214();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue16293 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue16293);

    LabelIssue16293();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue16471 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue16471);

    LabelIssue16471();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue16717 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue16717);

    LabelIssue16717();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssueLineGap : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssueLineGap);

    LabelIssueLineGap();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue17902 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue17902);

    LabelIssue17902();

    virtual void purgeCachedData();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class LabelIssue20523 : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelIssue20523);

    LabelIssue20523();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
    int _i = 0;
};

class LabelLetterColorsTest : public AtlasDemoNew
{
public:
    CREATE_FUNC(LabelLetterColorsTest);

    LabelLetterColorsTest();

    virtual std::string title() const override;
    virtual std::string subtitle() const override;

private:
    static void setLetterColors(ax::Label* label, const ax::Color3B& color);
};

#endif
