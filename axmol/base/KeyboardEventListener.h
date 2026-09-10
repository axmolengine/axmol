/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT

 ****************************************************************************/

#pragma once

#include "axmol/base/EventListener.h"
#include "axmol/base/KeyboardEvent.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

/** @class KeyboardEventListener
 * @brief Keyboard event listener.
 */
class AX_DLL KeyboardEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    /** Create a keyboard event listener.
     *
     * @return An autoreleased KeyboardEventListener object.
     */
    static KeyboardEventListener* create();

    /// Overrides
    KeyboardEventListener* clone() override;
    bool checkAvailable() override;

    std::function<void(KeyboardEvent*)> onKeyPressed;
    std::function<void(KeyboardEvent*)> onKeyReleased;
    std::function<void(KeyboardEvent*)> onKeyRepeat;
    KeyboardEventListener();
    bool init();
};

// deprecated
using EventListenerKeyboard = KeyboardEventListener;

}  // namespace ax

// end of base group
/// @}
