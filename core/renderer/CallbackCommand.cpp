/****************************************************************************
 Copyright (c) 2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
#include "renderer/CallbackCommand.h"
#include "renderer/backend/DriverBase.h"

namespace ax
{

static uint64_t s_nextId = 1;

CallbackCommand::CallbackCommand()
{
    _type = RenderCommand::Type::CALLBACK_COMMAND;
}

void CallbackCommand::init(float globalOrder)
{
    _globalOrder = globalOrder;
}

void CallbackCommand::init(float globalOrder, const Mat4& transform, unsigned int flags)
{
    RenderCommand::init(globalOrder, transform, flags);
}

void CallbackCommand::reset()
{
    _globalOrder = 0.0f;
    _isTransparent = true;
    _skipBatching = false;
    _is3D = false;
    _depth = 0.0f;
}

void CallbackCommand::execute()
{
    if (func)
        func();
}

}
