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

#ifndef __X365LIBS__RadioButtonGroupReader__
#define __X365LIBS__RadioButtonGroupReader__

#include "cocostudio/WidgetReader/WidgetReader.h"
#include "cocostudio/CocosStudioExport.h"

namespace cocostudio
{
class CCS_DLL RadioButtonGroupReader : public WidgetReader
{
    DECLARE_CLASS_NODE_READER_INFO

public:
    RadioButtonGroupReader();
    virtual ~RadioButtonGroupReader();

    static RadioButtonGroupReader* getInstance();
    static void destroyInstance();

    flatbuffers::Offset<flatbuffers::Table> createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                                         flatbuffers::FlatBufferBuilder* builder);
    void setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table*);
    ax::Node* createNodeWithFlatBuffers(const flatbuffers::Table*);

    // int getResourceType(std::string_view key);
};
}  // namespace cocostudio

#endif /* defined(__X365LIBS__RadioButtonGroupReader__) */
