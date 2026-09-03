if nil == axui then
    return
end

LAYOUT_COLOR_NONE                      = axui.LayoutBackGroundColorType.none
LAYOUT_COLOR_SOLID                     = axui.LayoutBackGroundColorType.solid
LAYOUT_COLOR_GRADIENT                  = axui.LayoutBackGroundColorType.gradient

LAYOUT_ABSOLUTE                        = axui.LayoutType.ABSOLUTE
LAYOUT_LINEAR_VERTICAL                 = axui.LayoutType.VERTICAL
LAYOUT_LINEAR_HORIZONTAL               = axui.LayoutType.HORIZONTAL
LAYOUT_RELATIVE                        = axui.LayoutType.RELATIVE

BRIGHT_NONE                            = axui.BrightStyle.none
BRIGHT_NORMAL                          = axui.BrightStyle.normal
BRIGHT_HIGHLIGHT                       = axui.BrightStyle.highlight

UI_TEX_TYPE_LOCAL                      = axui.TextureResType.localType
UI_TEX_TYPE_PLIST                      = axui.TextureResType.plistType

TOUCH_EVENT_BEGAN                      = axui.TouchEventType.began
TOUCH_EVENT_MOVED                      = axui.TouchEventType.moved
TOUCH_EVENT_ENDED                      = axui.TouchEventType.ended
TOUCH_EVENT_CANCELED                   = axui.TouchEventType.canceled

SIZE_ABSOLUTE                          = axui.SizeType.absolute
SIZE_PERCENT                           = axui.SizeType.percent

POSITION_ABSOLUTE                      = axui.PositionType.absolute
POSITION_PERCENT                       = axui.PositionType.percent

CHECKBOX_STATE_EVENT_SELECTED          = axui.CheckBoxEventType.selected
CHECKBOX_STATE_EVENT_UNSELECTED        = axui.CheckBoxEventType.unselected

CHECKBOX_STATE_EVENT_SELECTED          = axui.CheckBoxEventType.selected
CHECKBOX_STATE_EVENT_UNSELECTED        = axui.CheckBoxEventType.unselected

LoadingBarTypeLeft                     = axui.LoadingBarDirection.LEFT
LoadingBarTypeRight                    = axui.LoadingBarDirection.RIGHT

LoadingBarTypeRight                    = axui.SliderEventType.percent_changed

TEXTFIELD_EVENT_ATTACH_WITH_IME        = axui.TextFiledEventType.attach_with_ime
TEXTFIELD_EVENT_DETACH_WITH_IME        = axui.TextFiledEventType.detach_with_ime
TEXTFIELD_EVENT_INSERT_TEXT            = axui.TextFiledEventType.insert_text
TEXTFIELD_EVENT_DELETE_BACKWARD        = axui.TextFiledEventType.delete_backward

SCROLLVIEW_EVENT_SCROLL_TO_TOP         = axui.ScrollViewDir.none
SCROLLVIEW_DIR_VERTICAL                = axui.ScrollViewDir.vertical
SCROLLVIEW_DIR_HORIZONTAL              = axui.ScrollViewDir.horizontal
SCROLLVIEW_DIR_BOTH                    = axui.ScrollViewDir.both

SCROLLVIEW_EVENT_SCROLL_TO_TOP         = axui.ScrollviewEventType.scrollToTop
SCROLLVIEW_EVENT_SCROLL_TO_BOTTOM      = axui.ScrollviewEventType.scrollToBottom
SCROLLVIEW_EVENT_SCROLL_TO_LEFT        = axui.ScrollviewEventType.scrollToLeft
SCROLLVIEW_EVENT_SCROLL_TO_RIGHT       = axui.ScrollviewEventType.scrollToRight
SCROLLVIEW_EVENT_SCROLLING             = axui.ScrollviewEventType.scrolling
SCROLLVIEW_EVENT_BOUNCE_TOP            = axui.ScrollviewEventType.bounceTop
SCROLLVIEW_EVENT_BOUNCE_BOTTOM         = axui.ScrollviewEventType.bounceBottom
SCROLLVIEW_EVENT_BOUNCE_LEFT           = axui.ScrollviewEventType.bounceLeft
SCROLLVIEW_EVENT_BOUNCE_RIGHT          = axui.ScrollviewEventType.bounceRight
SCROLLVIEW_EVENT_CONTAINER_MOVED       = axui.ScrollviewEventType.containerMoved
SCROLLVIEW_EVENT_AUTOSCROLL_ENDED      = axui.ScrollviewEventType.autoscrollEnded

PAGEVIEW_EVENT_TURNING                 = axui.PageViewEventType.turning

PAGEVIEW_TOUCHLEFT                     = axui.PVTouchDir.touch_left
PAGEVIEW_TOUCHRIGHT                    = axui.PVTouchDir.touch_right

LISTVIEW_DIR_NONE                      = axui.ListViewDirection.none
LISTVIEW_DIR_VERTICAL                  = axui.ListViewDirection.vertical
LISTVIEW_DIR_HORIZONTAL                = axui.ListViewDirection.horizontal

LISTVIEW_MOVE_DIR_NONE                 = axui.ListViewMoveDirection.none
LISTVIEW_MOVE_DIR_UP                   = axui.ListViewMoveDirection.up
LISTVIEW_MOVE_DIR_DOWN                 = axui.ListViewMoveDirection.down
LISTVIEW_MOVE_DIR_LEFT                 = axui.ListViewMoveDirection.left
LISTVIEW_MOVE_DIR_RIGHT                = axui.ListViewMoveDirection.right

LISTVIEW_EVENT_INIT_CHILD              = axui.ListViewEventType.init_child
LISTVIEW_EVENT_UPDATE_CHILD            = axui.ListViewEventType.update_child

LAYOUT_PARAMETER_NONE                  = axui.LayoutParameterType.none
LAYOUT_PARAMETER_LINEAR                = axui.LayoutParameterType.linear
LAYOUT_PARAMETER_RELATIVE              = axui.LayoutParameterType.relative

axui.LoadingBarType                    = axui.LoadingBarDirection
axui.LoadingBarType.left               = axui.LoadingBarDirection.LEFT
axui.LoadingBarType.right              = axui.LoadingBarDirection.RIGHT

axui.LayoutType.absolute               = axui.LayoutType.ABSOLUTE
axui.LayoutType.linearVertical         = axui.LayoutType.VERTICAL
axui.LayoutType.linearHorizontal       = axui.LayoutType.HORIZONTAL
axui.LayoutType.relative               = axui.LayoutType.RELATIVE

axui.ListViewEventType.onsSelectedItem = axui.ListViewEventType.ONSELECTEDITEM_START
