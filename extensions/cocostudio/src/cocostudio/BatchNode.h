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

#ifndef __CCBATCHNODE_H__
#define __CCBATCHNODE_H__

#include "2d/Node.h"
#include "ArmatureDefine.h"
#include "CocosStudioExport.h"

namespace ax
{

class GroupCommand;

}

namespace cocostudio
{

class CCS_DLL BatchNode : public ax::Node
{
public:
    static BatchNode* create();

public:
    /**
     * @js ctor
     */
    BatchNode();
    /**
     * @ js NA
     * @ lua NA
     */
    ~BatchNode();
    /**
     *  @js NA
     */
    virtual bool init() override;
    using Node::addChild;
    virtual void addChild(ax::Node* pChild, int zOrder, int tag) override;
    virtual void addChild(ax::Node* pChild, int zOrder, std::string_view name) override;
    virtual void removeChild(ax::Node* child, bool cleanup) override;
    virtual void visit(ax::Renderer* renderer,
                       const ax::Mat4& parentTransform,
                       uint32_t parentFlags) override;
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;

protected:
    void generateGroupCommand();

};

}  // namespace cocostudio

#endif /*__CCBATCHNODE_H__*/
