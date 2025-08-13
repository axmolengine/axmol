#pragma once

#include "../VertexLayout.h"
#include "../RHITypes.h"
#include <d3d11.h>

namespace ax::rhi::d3d
{
class VertexLayoutImpl : public VertexLayout
{
public:
    ~VertexLayoutImpl() override;

    void apply(ID3D11DeviceContext* context, Program* program) const;
    VertexLayoutImpl* clone() override;

private:

    mutable ID3D11InputLayout* _d3dVL{nullptr};
};
}  // namespace ax::rhi::d3d
