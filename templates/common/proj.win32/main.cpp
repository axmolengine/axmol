/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "main.h"
#include "AppDelegate.h"

// Uncomment to enable win32 console
#define USE_WIN32_CONSOLE

using namespace ax;

static int axmol_main(int argc, TCHAR** argv)
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    // create the application instance
    AppDelegate app;
    // Use launch instead of run if you want Axmol to parse builtin launch arguments.
    // Application::getInstance()->launch(argc, argv);
    return Application::getInstance()->run();
}

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#    ifdef USE_WIN32_CONSOLE
#        include "axmol/platform/win32/EmbedConsole.h"
#    endif

    auto result = axmol_main(__argc, __targv);

#    if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#    endif

    return result;
}
#else
int _tmain(int argc, TCHAR** argv)
{
    auto result = axmol_main(argc, argv);

#    if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#    endif

    return result;
}
#endif
