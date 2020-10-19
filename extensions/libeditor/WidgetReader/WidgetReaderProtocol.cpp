#include "libeditor/WidgetReader/WidgetReaderProtocol.h"

namespace cocostudio
{
    WidgetReaderProtocol::~WidgetReaderProtocol(){}
    void WidgetReaderProtocol::setPropsFromProtocolBuffers(cocos2d::ui::Widget* widget, const protocolbuffers::NodeTree& nodeTree){}
    void WidgetReaderProtocol::setPropsFromXML(cocos2d::ui::Widget* widget, pugi::xml_node objectData){}
}
