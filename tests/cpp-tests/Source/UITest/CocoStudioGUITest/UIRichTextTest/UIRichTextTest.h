/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef __TestCpp__UIRichTextTest__
#define __TestCpp__UIRichTextTest__

#include "../UIScene.h"

DEFINE_TEST_SUITE(UIRichTextTests);

class UIRichTextTestBase : public UIScene
{
public:
    UIRichTextTestBase()           = default;
    ~UIRichTextTestBase() override = 0;  // Pure virtual to prevent instancing this class

    bool init() override;
    virtual void touchEvent(Object* pSender, ax::ui::Widget::TouchEventType type);
    virtual void switchWrapMode(ax::Object* sender, ax::ui::Widget::TouchEventType type);
    virtual void switchHorizontalAlignment(ax::Object* sender, ax::ui::Widget::TouchEventType type);
    virtual void switchVerticalAlignment(ax::Object* sender, ax::ui::Widget::TouchEventType type);

protected:
    virtual void createButtonPanel();

    ax::ui::RichText* _richText{};
    ax::Vec2 _defaultContentSize;
};

inline UIRichTextTestBase::~UIRichTextTestBase() = default;

class UIRichTextTest : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextTest);

    bool init() override;
};

class UIRichTextXMLBasic : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLBasic);

    bool init() override;
};

class UIRichTextXMLSmallBig : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLSmallBig);

    bool init() override;
};

class UIRichTextXMLColor : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLColor);

    bool init() override;
};

class UIRichTextXMLSUIB : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLSUIB);

    bool init() override;
};

class UIRichTextXMLSUIB2 : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLSUIB2);

    bool init() override;
};

class UIRichTextXMLSUIB3 : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLSUIB3);

    bool init() override;
};

class UIRichTextXMLImg : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLImg);

    bool init() override;
};

class UIRichTextXMLUrl : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLUrl);

    bool init() override;
};

class UIRichTextXMLUrlImg : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLUrlImg);

    bool init() override;
};

class UIRichTextXMLFace : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLFace);

    bool init() override;
};

class UIRichTextXMLBR : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLBR);

    bool init() override;
};

class UIRichTextXMLInvalid : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLInvalid);

    bool init() override;
};

class UIRichTextXMLOutline : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLOutline);

    bool init() override;
};

class UIRichTextXMLShadow : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLShadow);

    bool init() override;
};

class UIRichTextXMLGlow : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLGlow);

    bool init() override;
};

class UIRichTextXMLExtend : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLExtend);

    bool init() override;
};

class UIRichTextXMLSpace : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextXMLSpace);

    bool init() override;
};

class UIRichTextNewline : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextNewline);

    bool init() override;
};

class UIRichTextHeadings : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextHeadings);

    bool init() override;
};

class UIRichTextDynamicFontSize : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextDynamicFontSize);

    bool init() override;
};

class UIRichTextParagraph : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextParagraph);

    bool init() override;
};

class UIRichTextScrollTo : public UIRichTextTestBase
{
public:
    CREATE_FUNC(UIRichTextScrollTo);

    bool init() override;

protected:
    void updateScrollViewSize();

    ax::ui::ScrollView* _scrollView;
};

#endif /* defined(__TestCpp__UIRichTextTest__) */
