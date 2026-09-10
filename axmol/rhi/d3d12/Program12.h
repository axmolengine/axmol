/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "axmol/rhi/Program.h"
#include "axmol/rhi/d3d12/ShaderModule12.h"

namespace ax::rhi::d3d12
{
class BufferImpl;

/**
 * @brief A d3d12-based ShaderProgram implementation
 */
class ProgramImpl : public Program
{
public:
    ProgramImpl(Data& vsData, Data& fsData);

    /**
     * @param csData Specifies the compute shader source.
     */
    explicit ProgramImpl(Data& csData);

    ~ProgramImpl() override;

    std::span<uint8_t> getVSBlob() const { return static_cast<ShaderModuleImpl*>(_vsModule)->getBlob(); }
    std::span<uint8_t> getPSBlob() const { return static_cast<ShaderModuleImpl*>(_fsModule)->getBlob(); }
    std::span<uint8_t> getCSBlob() const { return static_cast<ShaderModuleImpl*>(_csModule)->getBlob(); }
};

}  // namespace ax::rhi::d3d12
