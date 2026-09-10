/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "AppDelegate.h"
#include "axmol/axmol.h"

using namespace ax;

int axmol_main()
{
    // create the application instance
    AppDelegate app;
    return Application::getInstance()->run();
}

int main(int argc, char* argv[])
{
    auto result = axmol_main();

#if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#endif

    return result;
}
