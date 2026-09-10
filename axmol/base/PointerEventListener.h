/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT

 ****************************************************************************/

#pragma once

#include "axmol/base/EventListener.h"
#include "axmol/math/Vec3.h"
#include <vector>

/**
 * @addtogroup base
 * @{
 */

namespace ax
{

class PointerEvent;

/** @class PointerEventListener
 * @brief Single touch event listener.
 */
class AX_DLL PointerEventListener : public EventListener
{
public:
    static const std::string_view LISTENER_ID;

    /** Create a one by one touch event listener.
     */
    static PointerEventListener* create();

    /**
     * Destructor.
     */
    virtual ~PointerEventListener();

    /// Overrides
    PointerEventListener* clone() override;
    bool checkAvailable() override;
    //

public:
    // Hit-test callback invoked only for scene-graph listeners
    std::function<bool(PointerEvent*, Vec3*)> onPointerHitTest;
    std::function<bool(PointerEvent*)> onPointerDown;
    std::function<void(PointerEvent*)> onPointerMove;
    std::function<void(PointerEvent*)> onPointerUp;
    std::function<void(PointerEvent*)> onPointerCancel;
    std::function<bool(PointerEvent*)> onPointerScroll;

    PointerEventListener();
    bool init();

private:
    friend class EventDispatcher;
};

}  // namespace ax

// end of base group
/// @}
