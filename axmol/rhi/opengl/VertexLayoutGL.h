/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/VertexLayout.h"
#include "axmol/rhi/RHITypes.h"
#include "axmol/platform/GL.h"

namespace ax::rhi::gl
{
class BufferImpl;
class VertexLayoutImpl : public VertexLayout
{
public:
    explicit VertexLayoutImpl(VertexLayoutDesc&&);
    ~VertexLayoutImpl() override;

    void apply(BufferImpl* vertexBuffer, BufferImpl* instanceBuffer, uint32_t& usedBits) const;

    void invalidate()
    {
        _vao      = 0;
        _usedBits = 0;
    }

private:
    void setupVAO();

    mutable GLuint _vao{0};

    uint32_t _usedBits{0};
};
}  // namespace ax::rhi::gl
