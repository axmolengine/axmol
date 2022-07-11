
#include "WidgetCallBackHandlerProtocol.h"

namespace cocostudio
{
WidgetCallBackHandlerProtocol::~WidgetCallBackHandlerProtocol() {}
axis::ui::Widget::ccWidgetTouchCallback WidgetCallBackHandlerProtocol::onLocateTouchCallback(std::string_view)
{
    return nullptr;
}
axis::ui::Widget::ccWidgetClickCallback WidgetCallBackHandlerProtocol::onLocateClickCallback(std::string_view)
{
    return nullptr;
}
axis::ui::Widget::ccWidgetEventCallback WidgetCallBackHandlerProtocol::onLocateEventCallback(std::string_view)
{
    return nullptr;
}
}  // namespace cocostudio
