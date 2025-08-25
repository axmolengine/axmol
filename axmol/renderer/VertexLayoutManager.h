#pragma once

#include "axmol/base/axstd.h"
#include "axmol/base/hlookup.h"
#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/VertexLayout.h"

namespace ax
{
class ProgramManager;

AX_DLL class VertexLayoutManager
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
    tsl::robin_map<uint32_t, VertexLayout*> _customVertexLayouts;

    std::vector<VertexLayoutDesc> _vertexLayoutDescPool;
};
}  // namespace ax

#define axvlm ax::VertexLayoutManager::getInstance()
