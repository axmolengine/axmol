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

    VertexLayout* acquireBuiltinVertexLayout(VertexLayoutKind kind);

    /// @brief acquire vertex layout by description
    /// @return
    VertexLayout* acquireVertexLayout(VertexLayoutDesc&&);

    /// @brief release a vertex layout
    /// @param layout
    void releaseVertexLayout(VertexLayout*& layout);

    VertexLayoutDesc allocateVertexLayoutDesc();

private:
    VertexLayout* acquireBuiltinVertexLayout(VertexLayoutKind kind, rhi::Program* prog);

    axstd::pod_vector<VertexLayout*> _builtinVertexLayouts;
    tsl::robin_map<uint32_t, VertexLayout*> _customVertexLayouts;

    std::vector<VertexLayoutDesc> _vertexLayoutDescPool;
};
}  // namespace ax

#define axvlm ax::VertexLayoutManager::getInstance()
