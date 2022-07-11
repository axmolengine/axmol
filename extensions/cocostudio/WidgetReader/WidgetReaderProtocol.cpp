#include "WidgetReader/WidgetReaderProtocol.h"

namespace cocostudio
{
WidgetReaderProtocol::~WidgetReaderProtocol() {}
void WidgetReaderProtocol::setPropsFromProtocolBuffers(axis::ui::Widget* widget,
                                                       const protocolbuffers::NodeTree& nodeTree)
{}
void WidgetReaderProtocol::setPropsFromXML(axis::ui::Widget* widget, pugi::xml_node objectData) {}
}  // namespace cocostudio
