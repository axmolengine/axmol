#ifndef __FIELDTYPES_H__
#define __FIELDTYPES_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

enum class PackageItemType
{
    IMAGE,
    MOVIECLIP,
    SOUND,
    COMPONENT,
    ATLAS,
    FONT,
    SWF,
    MISC,
    UNKNOWN,
    SPINE,
    DRAGONBONES
};

enum class ObjectType
{
    IMAGE,
    MOVIECLIP,
    SWF,
    GRAPH,
    LOADER,
    GROUP,
    TEXT,
    RICHTEXT,
    INPUTTEXT,
    COMPONENT,
    LIST,
    LABEL,
    BUTTON,
    COMBOBOX,
    PROGRESSBAR,
    SLIDER,
    SCROLLBAR,
    TREE,
    LOADER3D
};

enum class ButtonMode
{
    COMMON,
    CHECK,
    RADIO
};

enum class ChildrenRenderOrder
{
    ASCENT,
    DESCENT,
    ARCH,
};

enum class OverflowType
{
    VISIBLE,
    HIDDEN,
    SCROLL
};

enum class ScrollType
{
    HORIZONTAL,
    VERTICAL,
    BOTH
};

enum class ScrollBarDisplayType
{
    DEFAULT,
    VISIBLE,
    AUTO,
    HIDDEN
};

enum class LoaderFillType
{
    NONE,
    SCALE,
    SCALE_MATCH_HEIGHT,
    SCALE_MATCH_WIDTH,
    SCALE_FREE,
    SCALE_NO_BORDER
};

enum class ProgressTitleType
{
    PERCENT,
    VALUE_MAX,
    VALUE,
    MAX
};

enum class ListLayoutType
{
    SINGLE_COLUMN,
    SINGLE_ROW,
    FLOW_HORIZONTAL,
    FLOW_VERTICAL,
    PAGINATION
};

enum class ListSelectionMode
{
    SINGLE,
    MULTIPLE,
    MULTIPLE_SINGLECLICK,
    NONE
};

enum class GroupLayoutType
{
    NONE,
    HORIZONTAL,
    VERTICAL
};

enum class PopupDirection
{
    AUTO,
    UP,
    DOWN
};

enum class AutoSizeType
{
    NONE,
    BOTH,
    HEIGHT,
    SHRINK
};

enum class FlipType
{
    NONE,
    HORIZONTAL,
    VERTICAL,
    BOTH
};

enum class TransitionActionType
{
    XY,
    Size,
    Scale,
    Pivot,
    Alpha,
    Rotation,
    Color,
    Animation,
    Visible,
    Sound,
    Transition,
    Shake,
    ColorFilter,
    Skew,
    Text,
    Icon,
    Unknown
};

enum class FillMethod
{
    None,
    Horizontal,
    Vertical,
    Radial90,
    Radial180,
    Radial360,
};

enum class FillOrigin
{
    Top,
    Bottom,
    Left,
    Right
};

enum class ObjectPropID {
    Text,
    Icon,
    Color,
    OutlineColor,
    Playing,
    Frame,
    DeltaTime,
    TimeScale,
    FontSize,
    Selected
};

NS_FGUI_END

#endif