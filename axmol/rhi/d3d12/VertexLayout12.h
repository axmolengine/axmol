/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors

 https://axmol.dev/
 ****************************************************************************/
#pragma once

#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RHITypes.h"
#include <d3d12.h>
#include <vector>

namespace ax::rhi::d3d12
{
/**
 * @brief A D3D12-based VertexLayout implementation
 */
class VertexLayoutImpl : public VertexLayout
{
public:
    explicit VertexLayoutImpl(VertexLayoutDesc&&);
    ~VertexLayoutImpl() override = default;

    /**
     * @brief Get D3D12 input layout desc
     */
    const D3D12_INPUT_LAYOUT_DESC& getD3D12InputLayout() const { return _inputLayout; }

private:
    std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
    D3D12_INPUT_LAYOUT_DESC _inputLayout{};
};

}  // namespace ax::rhi::d3d12
