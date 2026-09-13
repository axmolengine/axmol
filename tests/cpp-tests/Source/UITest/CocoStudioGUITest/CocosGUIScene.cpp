/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "CocosGUIScene.h"

#include "UIButtonTest/UIButtonTest.h"
#include "UICheckBoxTest/UICheckBoxTest.h"
#include "UIRadioButtonTest/UIRadioButtonTest.h"
#include "UISliderTest/UISliderTest.h"
#include "UIImageViewTest/UIImageViewTest.h"
#include "UILoadingBarTest/UILoadingBarTest.h"
#include "UITextAtlasTest/UITextAtlasTest.h"
#include "UITextTest/UITextTest.h"
#include "UITextBMFontTest/UITextBMFontTest.h"
#include "UILayoutTest/UILayoutTest.h"
#include "UIScrollViewTest/UIScrollViewTest.h"
#include "UIPageViewTest/UIPageViewTest.h"
#include "UIListViewTest/UIListViewTest.h"
#include "UIWidgetAddNodeTest/UIWidgetAddNodeTest.h"
#include "UIRichTextTest/UIRichTextTest.h"
#include "UIFocusTest/UIFocusTest.h"
#include "UITabControlTest/UITabControlTest.h"
#include "UIInputFieldTest/UIInputFieldTest.h"

#if defined(AX_ENABLE_VIDEO)
#    include "UIVideoPlayerTest/UIVideoPlayerTest.h"
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
#    include "UIWebViewTest/UIWebViewTest.h"
#endif
#include "UIScale9SpriteTest.h"
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) ||                 \
    (AX_TARGET_PLATFORM == AX_PLATFORM_WASM) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || defined(_WIN32) || \
    defined(__linux__)
#    include "UIEditBoxTest.h"
#endif
#if ((AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX && defined(AX_HAVE_WEBKIT2GTK))
#    include "UIWebViewTest/UIWebViewTest.h"
#endif

GUIDynamicCreateTests::GUIDynamicCreateTests()
{
#if defined(AX_ENABLE_VIDEO)
    addTest("VideoPlayer Test", []() { return new VideoPlayerTests; });
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && \
        !defined(AX_TARGET_OS_TVOS) ||                                                      \
    (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 && defined(AX_ENABLE_MSEDGE_WEBVIEW2)) ||      \
    (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX && defined(AX_HAVE_WEBKIT2GTK))
    addTest("WebView Test", []() { return new WebViewTests; });
#endif
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID) ||                 \
    (AX_TARGET_PLATFORM == AX_PLATFORM_WASM) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC) || defined(_WIN32) || \
    defined(__linux__)
    addTest("EditBox Test", []() { return new UIEditBoxTests; });
#endif
    addTest("Focus Test", []() { return new UIFocusTests; });
    addTest("Scale9Sprite Test", []() { return new UIScale9SpriteTests; });
    addTest("Button Test", []() { return new UIButtonTests; });
    addTest("CheckBox Test", []() { return new UICheckBoxTests; });
    addTest("RadioButton Test", []() { return new UIRadioButtonTests; });
    addTest("Slider Test", []() { return new UISliderTests; });

    addTest("ImageView Test", []() { return new UIImageViewTests; });
    addTest("LoadingBar Test", []() { return new UILoadingBarTests; });
    addTest("TextAtlas Test", []() { return new UITextAtlasTests; });

    addTest("Text Test", []() { return new UITextTests; });
    addTest("TextBMFont Test", []() { return new UITextBMFontTests; });
    addTest("Layout Test", []() { return new UILayoutTests; });

    addTest("ScrollView Test", []() { return new UIScrollViewTests; });
    addTest("PageView Test", []() { return new UIPageViewTests; });
    addTest("ListView Test", []() { return new UIListViewTests; });

    addTest("WidgetAddNode Test", []() { return new UIWidgetAddNodeTests; });
    addTest("RichText Test", []() { return new UIRichTextTests; });

    addTest("TabControl Test", []() { return new UITabControlTests; });
    addTest("InputField Test", []() { return new UIInputFieldTests; });
}
