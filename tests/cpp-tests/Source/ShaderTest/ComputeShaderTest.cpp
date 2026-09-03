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

#include "ComputeShaderTest.h"
#include "axmol/rhi/ComputePipeline.h"
#include "axmol/rhi/GraphicsCore.h"
#include "axmol/rhi/VertexLayout.h"
#include "axmol/renderer/Renderer.h"

using namespace ax;

namespace
{
constexpr int kColorCount = 16;

ax::Data loadArchive(std::string_view name)
{
    auto fileUtils = FileUtils::getInstance();
    auto fullPath  = fileUtils->fullPathForFilename(name);
    if (fullPath.empty())
        return {};
    return fileUtils->getDataFromFile(fullPath);
}

bool failInitialization(std::string_view reason)
{
    AXLOGE("ComputeDispatchTest initialization failed: {}", reason);
    return false;
}
}  // namespace

ComputeShaderTests::ComputeShaderTests()
{
    ADD_TEST_CASE(ComputeDispatchTest);
}

ComputeDispatchTest::ComputeDispatchTest() {}

ComputeDispatchTest::~ComputeDispatchTest()
{
    AX_SAFE_RELEASE(_indexBuffer);
    AX_SAFE_RELEASE(_vertexBuffer);
    AX_SAFE_RELEASE(_storageBuffer);
    AX_SAFE_RELEASE(_vertexLayout);
    AX_SAFE_RELEASE(_renderState);
    AX_SAFE_RELEASE(_computeState);
    AX_SAFE_RELEASE(_computePipeline);
    AX_SAFE_RELEASE(_computeProgram);

    ProgramManager::getInstance()->unloadProgram(_renderProgram);
}

bool ComputeDispatchTest::init()
{
    if (!TestCase::init())
        return false;

    auto device = rhi::GraphicsCore::device();
    if (!device || !device->checkForFeatureSupported(rhi::FeatureType::COMPUTE_SHADER))
    {
        AXLOGW("ComputeShaderTest skipped: the current backend does not support compute shaders.");
        return true;
    }
    if (!device->checkForFeatureSupported(rhi::FeatureType::STORAGE_BUFFER))
        return failInitialization("backend reports compute support without storage buffer support");

    // Compute program: writes a gradient into the storage buffer.
    auto computeArchive = loadArchive("custom/compute_dispatch_cs");
    if (computeArchive.isNull())
        return failInitialization("compute shader archive is missing");
    _computeProgram = device->createComputeProgram(std::move(computeArchive));
    if (!_computeProgram || !_computeProgram->isValid())
        return failInitialization("compute program creation failed");
    _computePipeline = device->createComputePipeline(_computeProgram);
    if (!_computePipeline)
        return failInitialization("compute pipeline creation failed");
    _computeState = new rhi::ProgramState(_computeProgram);
    _computeState->setStorageBuffer(0, nullptr, rhi::BufferAccess::READ_WRITE);

    // Render program: vertex shader reads the storage buffer to color a quad.
    _renderProgram = ProgramManager::getInstance()->loadProgram("custom/compute_dispatch_vs",
                                                                "custom/compute_dispatch_ps");
    if (!_renderProgram || !_renderProgram->isValid())
        return failInitialization("render program creation failed");
    _renderState = new rhi::ProgramState(_renderProgram);
    _renderState->setStorageBuffer(0, nullptr, rhi::BufferAccess::READ_ONLY);

    // Storage buffer: kColorCount float4.
    rhi::BufferDesc storageDesc;
    storageDesc.size   = static_cast<size_t>(kColorCount) * sizeof(float) * 4;
    storageDesc.stride = sizeof(float) * 4;
    storageDesc.type   = rhi::BufferType::STORAGE;
    storageDesc.usage  = rhi::BufferUsage::DYNAMIC;
    storageDesc.access = rhi::BufferAccess::READ_WRITE;
    _storageBuffer     = device->createBuffer(storageDesc);
    if (!_storageBuffer)
        return failInitialization("storage buffer creation failed");

    _computeState->setStorageBuffer(0, _storageBuffer, rhi::BufferAccess::READ_WRITE);
    _renderState->setStorageBuffer(0, _storageBuffer, rhi::BufferAccess::READ_ONLY);

    // Fullscreen quad: pos (float3) + id (float).
    const float verts[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 4.0f,
         1.0f,  1.0f, 0.0f, 8.0f,
        -1.0f,  1.0f, 0.0f, 12.0f,
    };
    _vertexBuffer = device->createBuffer(sizeof(verts), rhi::BufferType::VERTEX, rhi::BufferUsage::STATIC, verts);
    if (!_vertexBuffer)
        return failInitialization("vertex buffer creation failed");

    const unsigned short indices[] = {0, 1, 2, 0, 2, 3};
    _indexBuffer = device->createBuffer(sizeof(indices), rhi::BufferType::INDEX, rhi::BufferUsage::STATIC, indices);
    if (!_indexBuffer)
        return failInitialization("index buffer creation failed");

    rhi::VertexLayoutDesc layoutDesc;
    layoutDesc.startLayout(2);
    // Use the program's backend-reflected vertex input descriptions (semantic,
    // location, varType); manually filling location breaks on backends that
    // remap semantics (e.g. D3D12 TEXCOORD0 -> TEXCOORD1).
    auto posDesc = _renderProgram->getVertexInputDesc(rhi::VertexSemantic::POSITION);
    auto idDesc  = _renderProgram->getVertexInputDesc(rhi::VertexSemantic::TEXCOORD0);
    if (!posDesc || !idDesc)
        return failInitialization("render program vertex reflection is incomplete");
    layoutDesc.addAttrib(posDesc, rhi::VertexElementType::FLOAT3, 0, false);
    layoutDesc.addAttrib(idDesc, rhi::VertexElementType::FLOAT, 12, false);
    layoutDesc.endLayout(16);
    _vertexLayout = device->createVertexLayout(std::move(layoutDesc));
    if (!_vertexLayout)
        return failInitialization("vertex layout creation failed");

    return true;
}

std::string ComputeDispatchTest::title() const
{
    return "Compute Dispatch (storage buffer write -> vertex read)";
}

void ComputeDispatchTest::visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    auto renderer = state.getRenderer();
    if (renderer && _computeState && _storageBuffer && _renderState && _vertexLayout)
    {
        if (dispatchCompute(renderer))
            setupDrawCommand(renderer);
    }
    TestCase::visit(state, parentTransform, parentFlags);
}

bool ComputeDispatchTest::dispatchCompute(ax::Renderer* renderer)
{
    auto context = renderer->getContext();
    if (!context)
        return false;

    rhi::ComputeDispatchDesc desc;
    desc.pipeline     = _computePipeline;
    desc.programState = _computeState;
    desc.groupCountX  = 1;  // 16 threads in one group
    desc.groupCountY  = 1;
    desc.groupCountZ  = 1;

    const bool dispatched = context->dispatch(desc);
    AXASSERT(dispatched, "Compute dispatch failed");
    if (!dispatched)
        AXLOGE("ComputeDispatchTest dispatch failed");
    return dispatched;
}

void ComputeDispatchTest::setupDrawCommand(ax::Renderer* renderer)
{
    // VS constants: orthographic identity mvp + color count.
    struct
    {
        float mvp[16];
        int colorCount;
    } cb{};
    cb.mvp[0] = cb.mvp[5] = cb.mvp[10] = cb.mvp[15] = 1.0f;
    cb.colorCount = kColorCount;
    // Match by name: on Metal, axslcc shifts the UBO binding past the storage
    // buffer slot (0), so the integer-binding overload would silently fail.
    _renderState->setUniformBlock(rhi::ShaderStage::VERTEX, "VSConstants", &cb, sizeof(cb));

    _drawCommand.init(0.0f);
    _drawCommand.setOwnPSVL(_renderState, _vertexLayout);
    _drawCommand.setDrawType(ax::CustomCommand::DrawType::ELEMENT);
    _drawCommand.setPrimitiveType(ax::CustomCommand::PrimitiveType::TRIANGLE);
    _drawCommand.setTransparent(false);
    _drawCommand.setVertexBuffer(_vertexBuffer);
    _drawCommand.setIndexBuffer(_indexBuffer, ax::CustomCommand::IndexFormat::U_SHORT);
    _drawCommand.setIndexDrawInfo(0, 6);

    renderer->addCommand(&_drawCommand);
}
