/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __AX_EXTENTIONS_CCCOMRENDER_H__
#define __AX_EXTENTIONS_CCCOMRENDER_H__

#include "ComBase.h"
#include "2d/Component.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

class CCS_DLL ComRender : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO

public:
    const static std::string COMPONENT_NAME;

    static ComRender* create();
    static ComRender* create(ax::Node* node, const char* comName);

    /**
     * @js NA
     * @lua NA
     */
    virtual void onEnter() override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void onExit() override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void onAdd() override;
    /**
     * @js NA
     * @lua NA
     */
    virtual void onRemove() override;
    virtual bool serialize(void* r) override;
    virtual ax::Node* getNode();
    virtual void setNode(ax::Node* node);
    /**
     *  @js ctor
     */
    ComRender();
    ComRender(ax::Node* node, const char* comName);
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ComRender();

private:
    bool readJson(std::string_view fileName, rapidjson::Document& doc);

    ax::Node* _render;
};

}  // namespace cocostudio
#endif  // __AX_EXTENTIONS_CCCOMRENDER_H__
