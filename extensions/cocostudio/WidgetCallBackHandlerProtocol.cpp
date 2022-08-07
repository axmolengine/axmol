
#include "WidgetCallBackHandlerProtocol.h"

namespace cocostudio
{
WidgetCallBackHandlerProtocol::~WidgetCallBackHandlerProtocol() {}
ax::ui::Widget::ccWidgetTouchCallback WidgetCallBackHandlerProtocol::onLocateTouchCallback(std::string_view)
{
    return nullptr;
}
ax::ui::Widget::ccWidgetClickCallback WidgetCallBackHandlerProtocol::onLocateClickCallback(std::string_view)
{
    return nullptr;
}
ax::ui::Widget::ccWidgetEventCallback WidgetCallBackHandlerProtocol::onLocateEventCallback(std::string_view)
{
    return nullptr;
}
}  // namespace cocostudio
