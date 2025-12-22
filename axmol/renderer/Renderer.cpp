/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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
#include "axmol/renderer/Renderer.h"

#include <algorithm>

#include "axmol/renderer/TrianglesCommand.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/renderer/CallbackCommand.h"
#include "axmol/renderer/GroupCommand.h"
#include "axmol/renderer/MeshCommand.h"
#include "axmol/renderer/Material.h"
#include "axmol/renderer/Technique.h"
#include "axmol/renderer/Pass.h"
#include "axmol/renderer/Texture2D.h"

#include "axmol/base/Environment.h"
#include "axmol/base/Director.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventListenerCustom.h"
#include "axmol/base/EventType.h"
#include "axmol/2d/Camera.h"
#include "axmol/2d/Scene.h"
#include "xxhash/xxhash.h"

#include "axmol/rhi/axmol-rhi.h"
#include "axmol/rhi/RenderTarget.h"

#if (AX_RENDER_API == AX_RENDER_API_MTL || AX_RENDER_API == AX_RENDER_API_VK || AX_RENDER_API == AX_RENDER_API_D3D12)
#    define _AX_RENDER_API_MODERN 1
#else
#    define _AX_RENDER_API_MODERN 0
#endif

namespace ax
{

// helper
static bool compareRenderCommand(RenderCommand* a, RenderCommand* b)
{
    return a->getGlobalOrder() < b->getGlobalOrder();
}

static bool compare3DCommand(RenderCommand* a, RenderCommand* b)
{
    return a->getDepth() > b->getDepth();
}

// queue
RenderQueue::RenderQueue() {}

void RenderQueue::emplace_back(RenderCommand* command)
{
    float z = command->getGlobalOrder();
    if (z < 0)
    {
        _commands[QUEUE_GROUP::GLOBALZ_NEG].emplace_back(command);
    }
    else if (z > 0)
    {
        _commands[QUEUE_GROUP::GLOBALZ_POS].emplace_back(command);
    }
    else
    {
        if (command->is3D())
        {
            if (command->isTransparent())
            {
                _commands[QUEUE_GROUP::TRANSPARENT_3D].emplace_back(command);
            }
            else
            {
                _commands[QUEUE_GROUP::OPAQUE_3D].emplace_back(command);
            }
        }
        else
        {
            _commands[QUEUE_GROUP::GLOBALZ_ZERO].emplace_back(command);
        }
    }
}

ssize_t RenderQueue::size() const
{
    ssize_t result(0);
    for (int index = 0; index < QUEUE_GROUP::QUEUE_COUNT; ++index)
    {
        result += _commands[index].size();
    }

    return result;
}

void RenderQueue::sort()
{
    // Don't sort _queue0, it already comes sorted
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::TRANSPARENT_3D]),
                     std::end(_commands[QUEUE_GROUP::TRANSPARENT_3D]), compare3DCommand);
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_NEG]), std::end(_commands[QUEUE_GROUP::GLOBALZ_NEG]),
                     compareRenderCommand);
    std::stable_sort(std::begin(_commands[QUEUE_GROUP::GLOBALZ_POS]), std::end(_commands[QUEUE_GROUP::GLOBALZ_POS]),
                     compareRenderCommand);
}

RenderCommand* RenderQueue::operator[](ssize_t index) const
{
    for (int queIndex = 0; queIndex < QUEUE_GROUP::QUEUE_COUNT; ++queIndex)
    {
        if (index < static_cast<ssize_t>(_commands[queIndex].size()))
            return _commands[queIndex][index];
        else
        {
            index -= _commands[queIndex].size();
        }
    }

    AXASSERT(false, "invalid index");
    return nullptr;
}

void RenderQueue::clear()
{
    for (int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
    {
        _commands[i].clear();
    }
}

void RenderQueue::realloc(size_t reserveSize)
{
    for (int i = 0; i < QUEUE_GROUP::QUEUE_COUNT; ++i)
    {
        _commands[i].clear();
        _commands[i].reserve(reserveSize);
    }
}

//
//
//
static const int DEFAULT_RENDER_QUEUE = 0;

//
// constructors, destructor, init
//
Renderer::Renderer()
{
    _groupCommandManager = new GroupCommandManager();

    _commandGroupStack.push(DEFAULT_RENDER_QUEUE);

    _renderGroups.emplace_back();
    _queuedTriangleCommands.reserve(BATCH_TRIAGCOMMAND_RESERVED_SIZE);

    // for the batched TriangleCommand
    _triBatchesToDraw = (TriBatchToDraw*)malloc(sizeof(_triBatchesToDraw[0]) * _triBatchesToDrawCapacity);
}

Renderer::~Renderer()
{
    _renderGroups.clear();

    for (auto&& clearCommand : _callbackCommandsPool)
        delete clearCommand;
    _callbackCommandsPool.clear();

    for (auto&& clearCommand : _groupCommandPool)
        delete clearCommand;
    _groupCommandPool.clear();

    _groupCommandManager->release();

    free(_triBatchesToDraw);

    AX_SAFE_RELEASE(_offscreenRT);
    AX_SAFE_RELEASE(_depthStencilState);
    AX_SAFE_RELEASE(_renderPipeline);
    AX_SAFE_RELEASE(_context);
}

void Renderer::init()
{
    // Should invoke _triangleCommandBufferManager.init() first.
    _triangleCommandBufferManager.init();
    _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
    _indexBuffer  = _triangleCommandBufferManager.getIndexBuffer();

    auto driver        = axdrv;
    auto nativeDisplay = Director::getInstance()->getRenderView()->getNativeDisplay();
    _context           = driver->createRenderContext(nativeDisplay);
    _dsDesc.flags      = DepthStencilFlags::ALL;
    _currentRT = _defaultRT = _context->getScreenRenderTarget();

    _renderPipeline = driver->createRenderPipeline();
    _context->setRenderPipeline(_renderPipeline);

    _depthStencilState = driver->createDepthStencilState();
    _context->setDepthStencilState(_depthStencilState);
}

rhi::RenderTarget* Renderer::getOffscreenRenderTarget()
{
    if (_offscreenRT != nullptr)
        return _offscreenRT;
    return (_offscreenRT = axdrv->createRenderTarget());
}

void Renderer::addCallbackCommand(std::function<void()> func, float globalZOrder)
{
    auto cmd = nextCallbackCommand();
    cmd->init(globalZOrder);
    cmd->func = std::move(func);
    addCommand(cmd);
}

void Renderer::addCommand(RenderCommand* command)
{
    int renderQueueID = _commandGroupStack.top();
    addCommand(command, renderQueueID);
}

void Renderer::addCommand(RenderCommand* command, int renderQueueID)
{
    AXASSERT(!_isRendering, "Cannot add command while rendering");
    AXASSERT(renderQueueID >= 0, "Invalid render queue");
    AXASSERT(command->getType() != RenderCommand::Type::UNKNOWN_COMMAND, "Invalid Command Type");

#ifndef NDEBUG
    AXASSERT(command->checkPSVL(), "Command pipelineDesc incomplete");
#endif

    _renderGroups[renderQueueID].emplace_back(command);
}

GroupCommand* Renderer::getNextGroupCommand()
{
    if (_groupCommandPool.empty())
    {
        return new GroupCommand();
    }

    auto* command = _groupCommandPool.back();
    _groupCommandPool.pop_back();
    command->reset();

    return command;
}

void Renderer::pushGroup(int renderQueueID)
{
    AXASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.push(renderQueueID);
}

void Renderer::popGroup()
{
    AXASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.pop();
}

int Renderer::createRenderQueue()
{
    RenderQueue newRenderQueue;
    _renderGroups.emplace_back(newRenderQueue);
    return (int)_renderGroups.size() - 1;
}

void Renderer::processGroupCommand(GroupCommand* command)
{
    flush();

    int renderQueueID = ((GroupCommand*)command)->getRenderQueueID();

    visitRenderQueue(_renderGroups[renderQueueID]);
}

void Renderer::processRenderCommand(RenderCommand* command)
{
    auto commandType = command->getType();
    switch (commandType)
    {
    case RenderCommand::Type::TRIANGLES_COMMAND:
    {
        // flush other queues
        flush3D();

        auto cmd = static_cast<TrianglesCommand*>(command);

        // flush own queue when buffer is full
        if (_queuedTotalVertexCount + cmd->getVertexCount() > VBO_SIZE ||
            _queuedTotalIndexCount + cmd->getIndexCount() > INDEX_VBO_SIZE)
        {
            AXASSERT(cmd->getVertexCount() >= 0 && cmd->getVertexCount() < VBO_SIZE,
                     "VBO for vertex is not big enough, please break the data down or use customized render command");
            AXASSERT(cmd->getIndexCount() >= 0 && cmd->getIndexCount() < INDEX_VBO_SIZE,
                     "VBO for index is not big enough, please break the data down or use customized render command");
            drawBatchedTriangles();

            _queuedTotalIndexCount = _queuedTotalVertexCount = 0;
#if _AX_RENDER_API_MODERN
            _queuedIndexCount = _queuedVertexCount = 0;
            _triangleCommandBufferManager.prepareNextBuffer();
            _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
            _indexBuffer  = _triangleCommandBufferManager.getIndexBuffer();
#endif
        }

        // queue it
        _queuedTriangleCommands.emplace_back(cmd);
#if _AX_RENDER_API_MODERN
        _queuedIndexCount += cmd->getIndexCount();
        _queuedVertexCount += cmd->getVertexCount();
#endif
        _queuedTotalVertexCount += cmd->getVertexCount();
        _queuedTotalIndexCount += cmd->getIndexCount();
    }
    break;
    case RenderCommand::Type::MESH_COMMAND:
        flush2D();
        drawMeshCommand(command);
        break;
    case RenderCommand::Type::GROUP_COMMAND:
        processGroupCommand(static_cast<GroupCommand*>(command));
        _groupCommandPool.emplace_back(static_cast<GroupCommand*>(command));
        break;
    case RenderCommand::Type::CUSTOM_COMMAND:
        flush();
        drawCustomCommand(command);
        break;
    case RenderCommand::Type::CALLBACK_COMMAND:
        flush();
        static_cast<CallbackCommand*>(command)->execute();
        _callbackCommandsPool.emplace_back(static_cast<CallbackCommand*>(command));
        break;
    default:
        assert(false);
        break;
    }
}

void Renderer::visitRenderQueue(RenderQueue& queue)
{
    pushStateBlock();

    // Apply default state for all render queues
    setDepthTest(false);
    setDepthWrite(false);
    setCullMode(rhi::CullMode::NONE);

    //
    // Process Global-Z < 0 Objects
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_NEG));

    //
    // Process Opaque Object
    //
    pushStateBlock();
    setDepthTest(true);  // enable depth test in 3D queue by default
    setDepthWrite(true);
    setCullMode(rhi::CullMode::BACK);
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::OPAQUE_3D));

    //
    // Process 3D Transparent object
    //
    setDepthWrite(false);
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::TRANSPARENT_3D));
    popStateBlock();

    //
    // Process Global-Z = 0 Queue
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO));

    //
    // Process Global-Z > 0 Queue
    //
    doVisitRenderQueue(queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_POS));

    popStateBlock();
}

void Renderer::doVisitRenderQueue(const std::vector<RenderCommand*>& renderCommands)
{
    for (const auto& command : renderCommands)
    {
        processRenderCommand(command);
    }
    flush();
}

void Renderer::render()
{
    // TODO: setup camera or MVP
    _isRendering = true;

    // Process render commands
    // 1. Sort render commands based on ID
    for (auto&& renderqueue : _renderGroups)
    {
        renderqueue.sort();
    }
    visitRenderQueue(_renderGroups[0]);

    clean();
    _isRendering = false;
}

bool Renderer::beginFrame()
{
    return _context->beginFrame();
}

void Renderer::endFrame()
{
    _context->endFrame();

#if _AX_RENDER_API_MODERN
    _triangleCommandBufferManager.putbackAllBuffers();
    _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
    _indexBuffer  = _triangleCommandBufferManager.getIndexBuffer();
#endif
    _queuedTotalIndexCount  = 0;
    _queuedTotalVertexCount = 0;
}

void Renderer::clean()
{
    // Clear render group
    for (size_t j = 0, size = _renderGroups.size(); j < size; j++)
    {
        // commands are owned by nodes
        //  for (const auto &cmd : _renderGroups[j])
        //  {
        //      cmd->releaseToCommandPool();
        //  }
        _renderGroups[j].clear();
    }

    // Clear batch commands
    _queuedTriangleCommands.clear();
}

void Renderer::setDepthTest(bool value)
{
    if (value)
        _dsDesc.addFlag(DepthStencilFlags::DEPTH_TEST);
    else
        _dsDesc.removeFlag(DepthStencilFlags::DEPTH_TEST);
}

void Renderer::setStencilTest(bool value)
{
    if (value)
        _dsDesc.addFlag(DepthStencilFlags::STENCIL_TEST);
    else
        _dsDesc.removeFlag(DepthStencilFlags::STENCIL_TEST);
}

void Renderer::setDepthWrite(bool value)
{
    if (value)
        _dsDesc.addFlag(DepthStencilFlags::DEPTH_WRITE);
    else
        _dsDesc.removeFlag(DepthStencilFlags::DEPTH_WRITE);
}

void Renderer::setDepthCompareFunc(rhi::CompareFunc func)
{
    _dsDesc.depthCompareFunc = func;
}

rhi::CompareFunc Renderer::getDepthCompareFunc() const
{
    return _dsDesc.depthCompareFunc;
}

bool Renderer::Renderer::getDepthTest() const
{
    return bitmask::any(_dsDesc.flags, DepthStencilFlags::DEPTH_TEST);
}

bool Renderer::getStencilTest() const
{
    return bitmask::any(_dsDesc.flags, DepthStencilFlags::STENCIL_TEST);
}

bool Renderer::getDepthWrite() const
{
    return bitmask::any(_dsDesc.flags, DepthStencilFlags::DEPTH_WRITE);
}

void Renderer::setStencilCompareFunc(rhi::CompareFunc func, unsigned int ref, unsigned int readMask)
{
    _dsDesc.frontFaceStencil.stencilCompareFunc = func;
    _dsDesc.backFaceStencil.stencilCompareFunc  = func;

    _dsDesc.frontFaceStencil.readMask = readMask;
    _dsDesc.backFaceStencil.readMask  = readMask;

    _stencilRef = ref;
}

void Renderer::setStencilOp(rhi::StencilOp stencilFailureOp,
                            rhi::StencilOp depthFailureOp,
                            rhi::StencilOp stencilDepthPassOp)
{
    _dsDesc.frontFaceStencil.stencilFailureOp = stencilFailureOp;
    _dsDesc.backFaceStencil.stencilFailureOp  = stencilFailureOp;

    _dsDesc.frontFaceStencil.depthFailureOp = depthFailureOp;
    _dsDesc.backFaceStencil.depthFailureOp  = depthFailureOp;

    _dsDesc.frontFaceStencil.depthStencilPassOp = stencilDepthPassOp;
    _dsDesc.backFaceStencil.depthStencilPassOp  = stencilDepthPassOp;
}

void Renderer::setStencilWriteMask(unsigned int mask)
{
    _dsDesc.frontFaceStencil.writeMask = mask;
    _dsDesc.backFaceStencil.writeMask  = mask;
}

rhi::StencilOp Renderer::getStencilFailureOp() const
{
    return _dsDesc.frontFaceStencil.stencilFailureOp;
}

rhi::StencilOp Renderer::getStencilPassDepthFailureOp() const
{
    return _dsDesc.frontFaceStencil.depthFailureOp;
}

rhi::StencilOp Renderer::getStencilDepthPassOp() const
{
    return _dsDesc.frontFaceStencil.depthStencilPassOp;
}

rhi::CompareFunc Renderer::getStencilCompareFunc() const
{
    return _dsDesc.depthCompareFunc;
}

unsigned int Renderer::getStencilReadMask() const
{
    return _dsDesc.frontFaceStencil.readMask;
}

unsigned int Renderer::getStencilWriteMask() const
{
    return _dsDesc.frontFaceStencil.writeMask;
}

unsigned int Renderer::getStencilReferenceValue() const
{
    return _stencilRef;
}

void Renderer::setDepthStencilDesc(const rhi::DepthStencilDesc& dsDesc)
{
    _dsDesc = dsDesc;
}

const rhi::DepthStencilDesc& Renderer::getDepthStencilDesc() const
{
    return _dsDesc;
}

void Renderer::fillVerticesAndIndices(const TrianglesCommand* cmd, unsigned int vertexBufferOffset)
{
    auto destVertices = &_verts[_filledVertex];
    auto srcVertices  = cmd->getVertices();
    auto vertexCount  = cmd->getVertexCount();
    auto&& modelView  = cmd->getModelView();
    MathUtil::transformVertices(destVertices, srcVertices, vertexCount, modelView);

    auto destIndices = &_indices[_filledIndex];
    auto srcIndices  = cmd->getIndices();
    auto indexCount  = cmd->getIndexCount();
    auto offset      = vertexBufferOffset + _filledVertex;
    MathUtil::transformIndices(destIndices, srcIndices, indexCount, int(offset));

    _filledVertex += vertexCount;
    _filledIndex += indexCount;
}

void Renderer::drawBatchedTriangles()
{
    if (_queuedTriangleCommands.empty())
        return;

    /************** 1: Setup up vertices/indices *************/
#if _AX_RENDER_API_MODERN
    unsigned int vertexBufferFillOffset = _queuedTotalVertexCount - _queuedVertexCount;
    unsigned int indexBufferFillOffset  = _queuedTotalIndexCount - _queuedIndexCount;
#else
    unsigned int vertexBufferFillOffset = 0;
    unsigned int indexBufferFillOffset  = 0;
#endif

    _triBatchesToDraw[0].offset        = indexBufferFillOffset;
    _triBatchesToDraw[0].indicesToDraw = 0;
    _triBatchesToDraw[0].cmd           = nullptr;

    int batchesTotal        = 0;
    uint32_t prevMaterialID = 0;
    bool firstCommand       = true;

    _filledVertex = 0;
    _filledIndex  = 0;

    for (const auto& cmd : _queuedTriangleCommands)
    {
        auto currentMaterialID = cmd->getMaterialID();
        const bool batchable   = !cmd->isSkipBatching();

        fillVerticesAndIndices(cmd, vertexBufferFillOffset);

        // in the same batch ?
        if (batchable && (prevMaterialID == currentMaterialID || firstCommand))
        {
            AX_ASSERT((firstCommand || _triBatchesToDraw[batchesTotal].cmd->getMaterialID() == cmd->getMaterialID()) &&
                      "argh... error in logic");
            _triBatchesToDraw[batchesTotal].indicesToDraw += cmd->getIndexCount();
            _triBatchesToDraw[batchesTotal].cmd = cmd;
        }
        else
        {
            // is this the first one?
            if (!firstCommand)
            {
                batchesTotal++;
                _triBatchesToDraw[batchesTotal].offset =
                    _triBatchesToDraw[batchesTotal - 1].offset + _triBatchesToDraw[batchesTotal - 1].indicesToDraw;
            }

            _triBatchesToDraw[batchesTotal].cmd           = cmd;
            _triBatchesToDraw[batchesTotal].indicesToDraw = (int)cmd->getIndexCount();

            // is this a single batch ? Prevent creating a batch group then
            if (!batchable)
                currentMaterialID = 0;
        }

        // capacity full ?
        if (batchesTotal + 1 >= _triBatchesToDrawCapacity)
        {
            _triBatchesToDrawCapacity *= 1.4;
            _triBatchesToDraw =
                (TriBatchToDraw*)realloc(_triBatchesToDraw, sizeof(_triBatchesToDraw[0]) * _triBatchesToDrawCapacity);
        }

        prevMaterialID = currentMaterialID;
        firstCommand   = false;
    }
    batchesTotal++;
#if _AX_RENDER_API_MODERN
    _vertexBuffer->updateSubData(_verts, vertexBufferFillOffset * sizeof(_verts[0]), _filledVertex * sizeof(_verts[0]));
    _indexBuffer->updateSubData(_indices, indexBufferFillOffset * sizeof(_indices[0]),
                                _filledIndex * sizeof(_indices[0]));
#else
    _vertexBuffer->updateData(_verts, _filledVertex * sizeof(_verts[0]));
    _indexBuffer->updateData(_indices, _filledIndex * sizeof(_indices[0]));
#endif

    /************** 2: Draw *************/
    beginRenderPass();

    _context->setVertexBuffer(_vertexBuffer);
    _context->setIndexBuffer(_indexBuffer);

    for (int i = 0; i < batchesTotal; ++i)
    {
        auto& drawInfo = _triBatchesToDraw[i];
        _context->updatePipelineState(_currentRT, drawInfo.cmd->getPipelineDesc(), rhi::PrimitiveType::TRIANGLE);
        _context->drawElements(rhi::IndexFormat::U_SHORT, drawInfo.indicesToDraw,
                               drawInfo.offset * sizeof(_indices[0]));

        _drawnBatches++;
        _drawnVertices += _triBatchesToDraw[i].indicesToDraw;
    }

    endRenderPass();

    /************** 3: Cleanup *************/
    _queuedTriangleCommands.clear();

#if _AX_RENDER_API_MODERN
    _queuedIndexCount  = 0;
    _queuedVertexCount = 0;
#endif
}

void Renderer::drawCustomCommand(RenderCommand* command)
{
    auto cmd = static_cast<CustomCommand*>(command);

    if (cmd->getBeforeCallback())
        cmd->getBeforeCallback()();

    beginRenderPass();
    _context->setVertexBuffer(cmd->getVertexBuffer());

    const auto primitiveType = cmd->getPrimitiveType();
    _context->updatePipelineState(_currentRT, cmd->getPipelineDesc(), primitiveType);

    auto drawType = cmd->getDrawType();
    switch (drawType)
    {
    case CustomCommand::DrawType::ELEMENT:
        _context->setIndexBuffer(cmd->getIndexBuffer());
        _context->drawElements(cmd->getIndexFormat(), cmd->getIndexDrawCount(), cmd->getIndexDrawOffset(),
                               cmd->isWireframe());
        _drawnVertices += cmd->getIndexDrawCount();
        break;
    case CustomCommand::DrawType::ELEMENT_INSTANCED:
        _context->setIndexBuffer(cmd->getIndexBuffer());
        _context->setInstanceBuffer(cmd->getInstanceBuffer());
        _context->drawElementsInstanced(cmd->getIndexFormat(), cmd->getIndexDrawCount(), cmd->getIndexDrawOffset(),
                                        cmd->getInstanceCount(), cmd->isWireframe());
        _drawnVertices += cmd->getIndexDrawCount() * cmd->getInstanceCount();
        break;
    case CustomCommand::DrawType::ARRAY:
        _context->drawArrays(cmd->getVertexDrawStart(), cmd->getVertexDrawCount(), cmd->isWireframe());
        _drawnVertices += cmd->getVertexDrawCount();
        break;
    case CustomCommand::DrawType::ARRAY_INSTANCED:
        _context->setInstanceBuffer(cmd->getInstanceBuffer());
        _context->drawArraysInstanced(cmd->getVertexDrawStart(), cmd->getVertexDrawCount(), cmd->getInstanceCount(),
                                      cmd->isWireframe());
        _drawnVertices += cmd->getVertexDrawCount() * cmd->getInstanceCount();
        break;
    default:;
    }
    _drawnBatches++;
    endRenderPass();

    if (cmd->getAfterCallback())
        cmd->getAfterCallback()();
}

void Renderer::drawMeshCommand(RenderCommand* command)
{
    // MeshCommand and CustomCommand are identical while rendering.
    drawCustomCommand(command);
}

void Renderer::flush()
{
    flush2D();
    flush3D();
}

void Renderer::flush2D()
{
    flushTriangles();
}

void Renderer::flush3D()
{
    // TODO 3d instanced rendering
    // https://learnopengl.com/Advanced-OpenGL/Instancing
}

void Renderer::flushTriangles()
{
    drawBatchedTriangles();
}

// helpers
bool Renderer::checkVisibility(const Mat4& transform, const Vec2& size)
{
    auto director = Director::getInstance();
    auto scene    = director->getRunningScene();

    // If draw to Rendertexture, return true directly.
    //  only cull the default camera. The culling algorithm is valid for default camera.
    if (!scene || (scene->_defaultCamera != Camera::getVisitingCamera()))
        return true;

    Rect visibleRect(director->getVisibleOrigin(), director->getVisibleSize());

    // transform center point to screen space
    float hSizeX = size.width / 2;
    float hSizeY = size.height / 2;
    Vec3 v3p(hSizeX, hSizeY, 0);
    transform.transformPoint(&v3p);
    Vec2 v2p = Camera::getVisitingCamera()->projectGL(v3p);

    // convert content size to world coordinates
    float wshw = std::max(fabsf(hSizeX * transform.m[0] + hSizeY * transform.m[4]),
                          fabsf(hSizeX * transform.m[0] - hSizeY * transform.m[4]));
    float wshh = std::max(fabsf(hSizeX * transform.m[1] + hSizeY * transform.m[5]),
                          fabsf(hSizeX * transform.m[1] - hSizeY * transform.m[5]));

    // enlarge visible rect half size in screen coord
    visibleRect.origin.x -= wshw;
    visibleRect.origin.y -= wshh;
    visibleRect.size.width += wshw * 2;
    visibleRect.size.height += wshh * 2;
    bool ret = visibleRect.containsPoint(v2p);
    return ret;
}

void Renderer::readPixels(rhi::RenderTarget* rt,
                          bool preserveAxisHint,
                          std::function<void(const rhi::PixelBufferDesc&)> callback)
{
    assert(!!rt);
    // read pixels from screen, metal renderer backend: screen texture must not be a framebufferOnly
    if (rt == _defaultRT)
        _context->setFrameBufferOnly(false);

    _context->readPixels(rt, preserveAxisHint, std::move(callback));
}

void Renderer::updateSurface(void* surface, uint32_t width, uint32_t height)
{
    if (_context)
        _context->updateSurface(surface, width, height);
}

void Renderer::beginRenderPass()
{
    _context->beginRenderPass(_currentRT, _renderPassDesc);

    // Disable depth/stencil access if render target has no relevant attachments.
    auto depthStencil = _dsDesc;
    if (!_currentRT->isDefaultRenderTarget())
    {
        if (!_currentRT->_depthStencil)
            depthStencil.removeFlag(DepthStencilFlags::DEPTH_TEST | DepthStencilFlags::DEPTH_WRITE |
                                    DepthStencilFlags::STENCIL_TEST);
    }

    _context->setStencilReferenceValue(_stencilRef);
    _context->updateDepthStencilState(depthStencil);

    _context->setViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
    _context->setCullMode(_cullMode);
    _context->setWinding(_winding);
    _context->setScissorRect(_scissorState.isEnabled, _scissorState.rect.x, _scissorState.rect.y,
                             _scissorState.rect.width, _scissorState.rect.height);
}

void Renderer::endRenderPass()
{
    _context->endRenderPass();
}

void Renderer::clear(ClearFlag flags, const Color& color, float depth, unsigned int stencil, float globalOrder)
{
    _clearFlag = flags;

    CallbackCommand* command = nextCallbackCommand();
    command->init(globalOrder);
    command->func = [this, flags, color, depth, stencil]() -> void {
        rhi::RenderPassDesc descriptor;

        descriptor.flags.clear = flags;
        if (bitmask::any(flags, ClearFlag::COLOR))
        {
            _clearColor                = color;
            descriptor.clearColorValue = {color.r, color.g, color.b, color.a};
        }

        if (bitmask::any(flags, ClearFlag::DEPTH))
            descriptor.clearDepthValue = depth;
        else
            descriptor.flags.discardStart |= TargetBufferFlags::DEPTH;

        if (bitmask::any(flags, ClearFlag::STENCIL))
            descriptor.clearStencilValue = stencil;
        else
            descriptor.flags.discardStart |= TargetBufferFlags::STENCIL;

        _context->beginRenderPass(_currentRT, descriptor);
        _context->endRenderPass();
    };
    addCommand(command);
}

CallbackCommand* Renderer::nextCallbackCommand()
{
    CallbackCommand* cmd = nullptr;
    if (!_callbackCommandsPool.empty())
    {
        cmd = _callbackCommandsPool.back();
        cmd->reset();
        _callbackCommandsPool.pop_back();
    }
    else
        cmd = new CallbackCommand();
    return cmd;
}

const Color& Renderer::getClearColor() const
{
    return _clearColor;
}

float Renderer::getClearDepth() const
{
    return _renderPassDesc.clearDepthValue;
}

unsigned int Renderer::getClearStencil() const
{
    return _renderPassDesc.clearStencilValue;
}

ClearFlag Renderer::getClearFlag() const
{
    return _clearFlag;
}

void Renderer::setScissorTest(bool enabled)
{
    _scissorState.isEnabled = enabled;
}

bool Renderer::getScissorTest() const
{
    return _scissorState.isEnabled;
}

const ScissorRect& Renderer::getScissorRect() const
{
    return _scissorState.rect;
}

void Renderer::setScissorRect(float x, float y, float w, float h)
{
    _scissorState.rect.x      = x;
    _scissorState.rect.y      = y;
    _scissorState.rect.width  = w;
    _scissorState.rect.height = h;
}

void Renderer::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    _viewport.x      = x;
    _viewport.y      = y;
    _viewport.width  = w;
    _viewport.height = h;
}

// TriangleCommandBufferManager
Renderer::TriangleCommandBufferManager::~TriangleCommandBufferManager()
{
    for (auto&& vertexBuffer : _vertexBufferPool)
        vertexBuffer->release();

    for (auto&& indexBuffer : _indexBufferPool)
        indexBuffer->release();
}

void Renderer::TriangleCommandBufferManager::init()
{
    createBuffer();
}

void Renderer::TriangleCommandBufferManager::putbackAllBuffers()
{
    _currentBufferIndex = 0;
}

void Renderer::TriangleCommandBufferManager::prepareNextBuffer()
{
    if (_currentBufferIndex < (int)_vertexBufferPool.size() - 1)
    {
        ++_currentBufferIndex;
        return;
    }

    createBuffer();
    ++_currentBufferIndex;
}

rhi::Buffer* Renderer::TriangleCommandBufferManager::getVertexBuffer() const
{
    return _vertexBufferPool[_currentBufferIndex];
}

rhi::Buffer* Renderer::TriangleCommandBufferManager::getIndexBuffer() const
{
    return _indexBufferPool[_currentBufferIndex];
}

void Renderer::TriangleCommandBufferManager::createBuffer()
{
    auto driver = axdrv;

    // Not initializing the buffer before passing it to updateData for Android/OpenGL ES.
    // This change does fix the Android/OpenGL ES performance problem
    // If for some reason we get reports of performance issues on OpenGL implementations,
    // then we can just add pre-processor checks for OpenGL and have the updateData() allocate the full size after
    // buffer creation.
    auto vertexBuffer = driver->createBuffer(Renderer::VBO_SIZE * sizeof(_verts[0]), rhi::BufferType::VERTEX,
                                             rhi::BufferUsage::DYNAMIC);
    if (!vertexBuffer)
        return;

    auto indexBuffer = driver->createBuffer(Renderer::INDEX_VBO_SIZE * sizeof(_indices[0]), rhi::BufferType::INDEX,
                                            rhi::BufferUsage::DYNAMIC);
    if (!indexBuffer)
    {
        vertexBuffer->release();
        return;
    }

    _vertexBufferPool.emplace_back(vertexBuffer);
    _indexBufferPool.emplace_back(indexBuffer);
}

void Renderer::pushStateBlock()
{
    StateBlock block;
    block.depthTest  = getDepthTest();
    block.depthWrite = getDepthWrite();
    block.cullMode   = getCullMode();
    _stateBlockStack.emplace_back(block);
}

void Renderer::popStateBlock()
{
    auto& block = _stateBlockStack.back();
    setDepthTest(block.depthTest);
    setDepthWrite(block.depthWrite);
    setCullMode(block.cullMode);
    _stateBlockStack.pop_back();
}

uint64_t Renderer::getCompletedFenceValue() const
{
    return _context->getCompletedFenceValue();
}

}  // namespace ax
