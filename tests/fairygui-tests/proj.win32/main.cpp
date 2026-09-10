/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "main.h"
#include "AppDelegate.h"

// Uncomment to enable win32 console
// #define USE_WIN32_CONSOLE

using namespace ax;
static int axmol_main(int argc, TCHAR** argv)
{
    // create the application instance
    AppDelegate app;
    return Application::getInstance()->launch(argc, argv);
}

#if !defined(_CONSOLE)
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
#    ifdef USE_WIN32_CONSOLE
#        include "axmol/platform/win32/EmbedConsole.h"
#    endif

    // create the application instance
    return axmol_main(__argc, __targv);
}
#else
int _tmain(int argc, TCHAR** argv)
{
    return axmol_main(argc, argv);
}
#endif
