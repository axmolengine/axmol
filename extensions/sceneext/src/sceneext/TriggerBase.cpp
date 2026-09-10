/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#include "sceneext/TriggerBase.h"
#include "axmol/base/CustomEvent.h"

using namespace ax;
using namespace ax::ext;

void sendEvent(unsigned int event)
{
    std::string custom_event_name = fmt::to_string(event);

    CustomEvent eventCustom(custom_event_name);
    TriggerMng::getInstance()->dispatchEvent(&eventCustom);
}
