
#include "WidgetCallBackHandlerProtocol.h"

namespace cocostudio
{
WidgetCallBackHandlerProtocol::~WidgetCallBackHandlerProtocol() {}
cocos2d::ui::Widget::ccWidgetTouchCallback WidgetCallBackHandlerProtocol::onLocateTouchCallback(std::string_view)
{
    return nullptr;
}
cocos2d::ui::Widget::ccWidgetClickCallback WidgetCallBackHandlerProtocol::onLocateClickCallback(std::string_view)
{
    return nullptr;
}
cocos2d::ui::Widget::ccWidgetEventCallback WidgetCallBackHandlerProtocol::onLocateEventCallback(std::string_view)
{
    return nullptr;
}
}  // namespace cocostudio
