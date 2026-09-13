/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/axmol.h"
#include "../BaseTest.h"

/**
 * @brief Renders a fullscreen quad colored by a storage buffer written by a
 * compute shader, verifying compute dispatch -> storage write -> vertex read.
 */
class ComputeDispatchTest : public TestCase
{
public:
    CREATE_FUNC(ComputeDispatchTest);

    ComputeDispatchTest();
    ~ComputeDispatchTest() override;

    bool init() override;
    std::string title() const override;

    void visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

private:
    bool dispatchCompute(ax::Renderer* renderer);
    void setupDrawCommand(ax::Renderer* renderer);

    ax::rhi::Program* _computeProgram          = nullptr;
    ax::rhi::ProgramState* _computeState       = nullptr;
    ax::rhi::ComputePipeline* _computePipeline = nullptr;
    ax::rhi::Program* _renderProgram           = nullptr;
    ax::rhi::ProgramState* _renderState        = nullptr;
    ax::rhi::Buffer* _storageBuffer            = nullptr;
    ax::rhi::Buffer* _vertexBuffer             = nullptr;
    ax::rhi::Buffer* _indexBuffer              = nullptr;
    ax::rhi::VertexLayout* _vertexLayout       = nullptr;

    ax::CustomCommand _dispatchCommand;
    ax::CustomCommand _drawCommand;
};

class ComputeShaderTests : public TestSuite
{
public:
    ComputeShaderTests();

private:
    std::vector<std::string> _searchPaths;
};
