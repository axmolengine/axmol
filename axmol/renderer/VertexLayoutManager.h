/****************************************************************************
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
#pragma once

#include "axmol/tlx/pod_vector.hpp"
#include "axmol/tlx/hlookup.hpp"
#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/VertexLayout.h"

namespace ax
{
class ProgramManager;

class AX_DLL VertexLayoutManager
{
    friend class ::ax::ProgramManager;

public:
    static VertexLayoutManager* getInstance();
    static void destroyInstance();

    VertexLayoutManager();

    /**
     * @brief acquire builtin vertex layout
     *
     * @param kind
     * @return VertexLayout*
     */
    VertexLayout* acquireBuiltinVertexLayout(VertexLayoutKind kind);

    /**
     * @brief acquire vertex layout by desc
     *
     * @return VertexLayout*
     */
    VertexLayout* acquireVertexLayout(VertexLayoutDesc&&);

    /**
     * @brief Remove unused vertex layout
     * @remark: only remove custom
     */
    void removeUnusedVertexLayouts();

    /**
     * @brief allocate vertex layout desc fast
     *
     * @return VertexLayoutDesc
     */
    VertexLayoutDesc allocateVertexLayoutDesc();

private:
    VertexLayout* acquireBuiltinVertexLayout(VertexLayoutKind kind, rhi::Program* prog);

    axstd::pod_vector<VertexLayout*> _builtinVertexLayouts;
    axstd::hash_map<uint32_t, VertexLayout*> _customVertexLayouts;

    std::vector<VertexLayoutDesc> _vertexLayoutDescPool;
};
}  // namespace ax

#define axvlm ax::VertexLayoutManager::getInstance()
