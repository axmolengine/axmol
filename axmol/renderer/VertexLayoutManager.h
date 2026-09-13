/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/tlx/vector.hpp"
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
    ~VertexLayoutManager();

    /**
     * @brief Gets builtin vertex layout
     *
     * @param kind
     * @return VertexLayout*
     */
    VertexLayout* getBuiltinVertexLayout(VertexLayoutKind kind);

    /**
     * @brief Gets vertex layout by desc
     *
     * @return VertexLayout*
     */
    VertexLayout* getVertexLayout(VertexLayoutDesc&&);

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
    VertexLayout* getBuiltinVertexLayout(VertexLayoutKind kind, rhi::Program* prog);

    tlx::pod_vector<VertexLayout*> _builtinVertexLayouts;
    tlx::hash_map<uint32_t, VertexLayout*> _customVertexLayouts;

    std::vector<VertexLayoutDesc> _vertexLayoutDescPool;
};
}  // namespace ax

#define axvlm ax::VertexLayoutManager::getInstance()
