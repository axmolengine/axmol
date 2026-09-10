/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/ui/AbstractCheckButton.h"
#include "axmol/ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
namespace ax
{

namespace ui
{

/**
 *  Checkbox is a specific type of two-states button that can be either checked or unchecked.
 */
class AX_GUI_DLL CheckBox : public AbstractCheckButton
{

    DECLARE_CLASS_GUI_INFO

public:
    /**
     * CheckBox event type, currently only selected and unselected events are used.
     */
    enum class EventType
    {
        SELECTED,
        UNSELECTED
    };
    using CheckBoxCallback = std::function<void(Object*, EventType)>;

    /**
     * Default constructor.
     *
     * @lua new
     */
    CheckBox() = default;

    /**
     * Create and return a empty CheckBox instance pointer.
     */
    static CheckBox* create();

    /**
     * Create an checkbox with various images.
     *
     * @param backGround    backGround texture.
     * @param backGroundSelected    backGround selected state texture.
     * @param cross    cross texture.
     * @param backGroundDisabled    backGround disabled state texture.
     * @param frontCrossDisabled    cross dark state texture.
     * @param texType    @see `Widget::TextureResType`
     *
     * @return A CheckBox instance pointer.
     */
    static CheckBox* create(std::string_view backGround,
                            std::string_view backGroundSelected,
                            std::string_view cross,
                            std::string_view backGroundDisabled,
                            std::string_view frontCrossDisabled,
                            TextureResType texType = TextureResType::LOCAL);

    /**
     * Another factory method to create a CheckBox instance.
     * This method uses less resource to create a CheckBox.
     * @param backGround The background image name in `std::string`.
     * @param cross The cross image name in `std::string`.
     * @param texType  The texture's resource type in `Widget::TextureResType`.
     * @return A CheckBox instance pointer
     */
    static CheckBox* create(std::string_view backGround,
                            std::string_view cross,
                            TextureResType texType = TextureResType::LOCAL);

    /**
     * Add a callback function which would be called when CheckBox is selected or unselected.
     */
    void addEventListener(const CheckBoxCallback& callback);

    // override functions
    std::string getDescription() const override;

    void onPointerUp(PointerEvent* event) override;

protected:
    void dispatchSelectChangedEvent(bool selected) override;

    Widget* createCloneInstance() override;
    void copySpecialProperties(Widget* model) override;

    CheckBoxCallback _eventCallback;
};

}  // namespace ui

}  // namespace ax
// end of ui group
/// @}
