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

#ifndef _COMPUTESHADERTEST_H_
#define _COMPUTESHADERTEST_H_

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

    ax::rhi::Program* _computeProgram  = nullptr;
    ax::rhi::ProgramState* _computeState = nullptr;
    ax::rhi::ComputePipeline* _computePipeline = nullptr;
    ax::rhi::Program* _renderProgram   = nullptr;
    ax::rhi::ProgramState* _renderState  = nullptr;
    ax::rhi::Buffer* _storageBuffer    = nullptr;
    ax::rhi::Buffer* _vertexBuffer     = nullptr;
    ax::rhi::Buffer* _indexBuffer      = nullptr;
    ax::rhi::VertexLayout* _vertexLayout = nullptr;

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

#endif  // _COMPUTESHADERTEST_H_
