/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __AX_FRAMEWORK_COMCONTAINER_H__
#define __AX_FRAMEWORK_COMCONTAINER_H__

/// @cond DO_NOT_SHOW

#include "base/Map.h"
#include <string>

namespace ax
{

class Component;
class Node;

class AX_DLL ComponentContainer
{
protected:
    /**
     * @js ctor
     */
    ComponentContainer(Node* node);

public:
    /**
     * @js NA
     * @lua NA
     */
    ~ComponentContainer();

    /**
     * @js getComponent
     */
    Component* get(std::string_view name) const;

    bool add(Component* com);
    bool remove(std::string_view name);
    bool remove(Component* com);
    void removeAll();
    void visit(float delta);

    void onEnter();
    void onExit();

    bool isEmpty() const { return _componentMap.empty(); }

private:
    hlookup::string_map<Component*> _componentMap;
    Node* _owner;

    friend class Node;
};

}

/// @endcond
#endif  // __AX_FRAMEWORK_COMCONTAINER_H__
