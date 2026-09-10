/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "AppDelegate.h"
#include "axmol/axmol.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>

using namespace ax;

namespace
{
std::unique_ptr<AppDelegate> appDelegate;
}

void axmol_wasm_app_exit()
{
    appDelegate = nullptr;

#if AX_OBJECT_LEAK_DETECTION
    Object::printLeaks();
#endif
}

int main(int argc, char** argv)
{
    // create the application instance
    appDelegate.reset(new AppDelegate());
    return Application::getInstance()->run();
}
