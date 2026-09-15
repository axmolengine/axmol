/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/EventListener.h"

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

namespace ui
{
class Widget;
}

/** @class FocusEventListener
 * @brief Focus event listener.
 */
class AX_DLL FocusEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    /** Create a focus event listener.
     *
     * @return An autoreleased FocusEventListener object.
     */
    static FocusEventListener* create();

    /** Destructor.
     */
    virtual ~FocusEventListener();

    /// Overrides
    FocusEventListener* clone() override;
    bool checkAvailable() override;
    //

public:
    std::function<void(ui::Widget*, ui::Widget*)> onFocusChanged;

    FocusEventListener();
    bool init();

    friend class EventDispatcher;
};

// deprecated alias
using EventListenerFocus = FocusEventListener;

}  // namespace ax

// end of base group
/// @}
