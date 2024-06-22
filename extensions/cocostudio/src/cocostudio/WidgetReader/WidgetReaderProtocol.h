/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __TestCpp__WidgetReaderProtocol__
#define __TestCpp__WidgetReaderProtocol__

#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/CocosStudioExport.h"
#include "pugixml/pugixml.hpp"
#include "pugixml/pugiext.hpp"

// protobuf present at cocos2d-x-3.3rc1@libccs19
namespace protocolbuffers
{
class NodeTree;
class ResourceData;  // please match in CSParseBinary.pb
}  // namespace protocolbuffers

NS_AX_BEGIN
namespace ui
{
class Widget;
}
NS_AX_END  // namespace ax

namespace cocostudio
{
class CocoLoader;
struct stExpCocoNode;

class CCS_DLL WidgetReaderProtocol
{
public:
    virtual ~WidgetReaderProtocol();
    virtual void setPropsFromJsonDictionary(ax::ui::Widget* widget, const rapidjson::Value& options)          = 0;
    virtual void setPropsFromBinary(ax::ui::Widget* widget, CocoLoader* cocoLoader, stExpCocoNode* pCocoNode) = 0;
    virtual void setPropsFromProtocolBuffers(
        ax::ui::Widget* widget,
        const protocolbuffers::NodeTree& nodeTree);  // cocos2d-x-3.3rc1@libccs19 spec
    virtual void setPropsFromXML(ax::ui::Widget* widget,
                                 pugi::xml_node objectData);  // cocos2d-x-3.3rc1@libccs19 spec
};
}  // namespace cocostudio

#endif /* defined(__TestCpp__WidgetReaderProtocol__) */
