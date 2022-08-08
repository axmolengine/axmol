#include "WidgetReader/WidgetReaderProtocol.h"

namespace cocostudio
{
WidgetReaderProtocol::~WidgetReaderProtocol() {}
void WidgetReaderProtocol::setPropsFromProtocolBuffers(ax::ui::Widget* widget,
                                                       const protocolbuffers::NodeTree& nodeTree)
{}
void WidgetReaderProtocol::setPropsFromXML(ax::ui::Widget* widget, pugi::xml_node objectData) {}
}  // namespace cocostudio
