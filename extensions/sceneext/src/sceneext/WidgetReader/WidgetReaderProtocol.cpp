/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#include "sceneext/WidgetReader/WidgetReaderProtocol.h"

namespace ax::ext
{
WidgetReaderProtocol::~WidgetReaderProtocol() {}
void WidgetReaderProtocol::setPropsFromProtocolBuffers(ax::ui::Widget* widget,
                                                       const protocolbuffers::NodeTree& nodeTree)
{}
void WidgetReaderProtocol::setPropsFromXML(ax::ui::Widget* widget, pugi::xml_node objectData) {}
}  // namespace ax::ext
