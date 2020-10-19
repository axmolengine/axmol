
#include "libeditor/WidgetCallBackHandlerProtocol.h"

namespace cocostudio {
    WidgetCallBackHandlerProtocol::~WidgetCallBackHandlerProtocol(){}
    cocos2d::ui::Widget::ccWidgetTouchCallback WidgetCallBackHandlerProtocol::onLocateTouchCallback(const std::string& callBackName){}
    cocos2d::ui::Widget::ccWidgetClickCallback WidgetCallBackHandlerProtocol::onLocateClickCallback(const std::string& callBackName){}
    cocos2d::ui::Widget::ccWidgetEventCallback WidgetCallBackHandlerProtocol::onLocateEventCallback(const std::string& callBackName){}
}
