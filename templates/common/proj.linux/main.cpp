/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "AppDelegate.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

using namespace ax;

int axmol_main()
{
    // create the application instance
    AppDelegate app;
    return Application::getInstance()->run();
}

int main(int argc, char** argv)
{
    auto result = axmol_main();

#if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#endif

    return result;
}
