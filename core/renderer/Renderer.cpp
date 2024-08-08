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
#include "renderer/Renderer.h"

#include <algorithm>

#include "renderer/TrianglesCommand.h"
#include "renderer/CustomCommand.h"
#include "renderer/CallbackCommand.h"
#include "renderer/GroupCommand.h"
#include "renderer/MeshCommand.h"
#include "renderer/Material.h"
#include "renderer/Technique.h"
#include "renderer/Pass.h"
#include "renderer/Texture2D.h"

#include "base/Configuration.h"
#include "base/Director.h"
#include "base/EventDispatcher.h"
#include "base/EventListenerCustom.h"
#include "base/EventType.h"
#include "2d/Camera.h"
#include "2d/Scene.h"
#include "xxhash.h"

#include "renderer/backend/Backend.h"
#include "renderer/backend/RenderTarget.h"

NS_AX_BEGIN

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

    AX_SAFE_RELEASE(_depthStencilState);
    AX_SAFE_RELEASE(_commandBuffer);
    AX_SAFE_RELEASE(_renderPipeline);
    AX_SAFE_RELEASE(_defaultRT);
    AX_SAFE_RELEASE(_offscreenRT);
}

void Renderer::init()
{
    // Should invoke _triangleCommandBufferManager.init() first.
    _triangleCommandBufferManager.init();
    _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
    _indexBuffer  = _triangleCommandBufferManager.getIndexBuffer();

    auto driver    = backend::DriverBase::getInstance();
    _commandBuffer = driver->newCommandBuffer();
    _dsDesc.flags = DepthStencilFlags::ALL;
    _defaultRT    = driver->newDefaultRenderTarget();

    _currentRT      = _defaultRT;
    _renderPipeline = driver->newRenderPipeline();
    _commandBuffer->setRenderPipeline(_renderPipeline);

    _depthStencilState = driver->newDepthStencilState();
    _commandBuffer->setDepthStencilState(_depthStencilState);
}

backend::RenderTarget* Renderer::getOffscreenRenderTarget() {
    if (_offscreenRT != nullptr) return _offscreenRT;
    return (_offscreenRT = backend::DriverBase::getInstance()->newRenderTarget());
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
#ifdef AX_USE_METAL
            _queuedIndexCount = _queuedVertexCount = 0;
            _triangleCommandBufferManager.prepareNextBuffer();
            _vertexBuffer = _triangleCommandBufferManager.getVertexBuffer();
            _indexBuffer  = _triangleCommandBufferManager.getIndexBuffer();
#endif
        }

        // queue it
        _queuedTriangleCommands.emplace_back(cmd);
#ifdef AX_USE_METAL
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
    setCullMode(backend::CullMode::NONE);

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
    setCullMode(backend::CullMode::BACK);
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
    //    if (_glViewAssigned)
    {
        // Process render commands
        // 1. Sort render commands based on ID
        for (auto&& renderqueue : _renderGroups)
        {
            renderqueue.sort();
        }
        visitRenderQueue(_renderGroups[0]);
    }
    clean();
    _isRendering = false;
}

bool Renderer::beginFrame()
{
    return _commandBuffer->beginFrame();
}

void Renderer::endFrame()
{
    _commandBuffer->endFrame();

#ifdef AX_USE_METAL
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

void Renderer::setDepthCompareFunction(backend::CompareFunction func)
{
    _dsDesc.depthCompareFunction = func;
}

backend::CompareFunction Renderer::getDepthCompareFunction() const
{
    return _dsDesc.depthCompareFunction;
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

void Renderer::setStencilCompareFunction(backend::CompareFunction func, unsigned int ref, unsigned int readMask)
{
    _dsDesc.frontFaceStencil.stencilCompareFunction = func;
    _dsDesc.backFaceStencil.stencilCompareFunction  = func;

    _dsDesc.frontFaceStencil.readMask = readMask;
    _dsDesc.backFaceStencil.readMask  = readMask;

    _stencilRef = ref;
}

void Renderer::setStencilOperation(backend::StencilOperation stencilFailureOp,
                                   backend::StencilOperation depthFailureOp,
                                   backend::StencilOperation stencilDepthPassOp)
{
    _dsDesc.frontFaceStencil.stencilFailureOperation = stencilFailureOp;
    _dsDesc.backFaceStencil.stencilFailureOperation  = stencilFailureOp;

    _dsDesc.frontFaceStencil.depthFailureOperation = depthFailureOp;
    _dsDesc.backFaceStencil.depthFailureOperation  = depthFailureOp;

    _dsDesc.frontFaceStencil.depthStencilPassOperation = stencilDepthPassOp;
    _dsDesc.backFaceStencil.depthStencilPassOperation  = stencilDepthPassOp;
}

void Renderer::setStencilWriteMask(unsigned int mask)
{
    _dsDesc.frontFaceStencil.writeMask = mask;
    _dsDesc.backFaceStencil.writeMask  = mask;
}

backend::StencilOperation Renderer::getStencilFailureOperation() const
{
    return _dsDesc.frontFaceStencil.stencilFailureOperation;
}

backend::StencilOperation Renderer::getStencilPassDepthFailureOperation() const
{
    return _dsDesc.frontFaceStencil.depthFailureOperation;
}

backend::StencilOperation Renderer::getStencilDepthPassOperation() const
{
    return _dsDesc.frontFaceStencil.depthStencilPassOperation;
}

backend::CompareFunction Renderer::getStencilCompareFunction() const
{
    return _dsDesc.depthCompareFunction;
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

void Renderer::setDepthStencilDesc(const backend::DepthStencilDescriptor& dsDesc)
{
    _dsDesc = dsDesc;
}

const backend::DepthStencilDescriptor& Renderer::getDepthStencilDesc() const
{
    return _dsDesc;
}

void Renderer::setViewPort(int x, int y, unsigned int w, unsigned int h)
{
    _viewport.x = x;
    _viewport.y = y;
    _viewport.width = w;
    _viewport.height = h;
}

void Renderer::fillVerticesAndIndices(const TrianglesCommand* cmd, unsigned int vertexBufferOffset)
{
    auto destVertices = &_verts[_filledVertex];
    auto srcVertices = cmd->getVertices();
    auto vertexCount = cmd->getVertexCount();
    auto&& modelView = cmd->getModelView();
    MathUtil::transformVertices(destVertices, srcVertices, vertexCount, modelView);

    auto destIndices = &_indices[_filledIndex];
    auto srcIndices = cmd->getIndices();
    auto indexCount = cmd->getIndexCount();
    auto offset = vertexBufferOffset + _filledVertex;
    MathUtil::transformIndices(destIndices, srcIndices, indexCount, int(offset));

    _filledVertex += vertexCount;
    _filledIndex += indexCount;
}

void Renderer::drawBatchedTriangles()
{
    if (_queuedTriangleCommands.empty())
        return;

        /************** 1: Setup up vertices/indices *************/
#ifdef AX_USE_METAL
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
#ifdef AX_USE_METAL
    _vertexBuffer->updateSubData(_verts, vertexBufferFillOffset * sizeof(_verts[0]), _filledVertex * sizeof(_verts[0]));
    _indexBuffer->updateSubData(_indices, indexBufferFillOffset * sizeof(_indices[0]),
                                _filledIndex * sizeof(_indices[0]));
#else
    _vertexBuffer->updateData(_verts, _filledVertex * sizeof(_verts[0]));
    _indexBuffer->updateData(_indices, _filledIndex * sizeof(_indices[0]));
#endif

    /************** 2: Draw *************/
    beginRenderPass();

    _commandBuffer->setVertexBuffer(_vertexBuffer);
    _commandBuffer->setIndexBuffer(_indexBuffer);

    for (int i = 0; i < batchesTotal; ++i)
    {
        auto& drawInfo = _triBatchesToDraw[i];
        _commandBuffer->updatePipelineState(_currentRT, drawInfo.cmd->getPipelineDescriptor());
        auto& pipelineDescriptor = drawInfo.cmd->getPipelineDescriptor();
        _commandBuffer->setProgramState(pipelineDescriptor.programState);
        _commandBuffer->drawElements(backend::PrimitiveType::TRIANGLE, backend::IndexFormat::U_SHORT,
                                     drawInfo.indicesToDraw, drawInfo.offset * sizeof(_indices[0]));

        _drawnBatches++;
        _drawnVertices += _triBatchesToDraw[i].indicesToDraw;
    }

    endRenderPass();

    /************** 3: Cleanup *************/
    _queuedTriangleCommands.clear();

#ifdef AX_USE_METAL
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
    _commandBuffer->setVertexBuffer(cmd->getVertexBuffer());

    _commandBuffer->updatePipelineState(_currentRT, cmd->getPipelineDescriptor());
    _commandBuffer->setProgramState(cmd->getPipelineDescriptor().programState);

    auto drawType = cmd->getDrawType();
    if (CustomCommand::DrawType::ELEMENT == drawType)
    {
        _commandBuffer->setIndexBuffer(cmd->getIndexBuffer());
        _commandBuffer->drawElements(cmd->getPrimitiveType(), cmd->getIndexFormat(), cmd->getIndexDrawCount(),
                                     cmd->getIndexDrawOffset(), cmd->isWireframe());
        _drawnVertices += cmd->getIndexDrawCount();
    }
    else if (CustomCommand::DrawType::ELEMENT_INSTANCE == drawType)
    {
        _commandBuffer->setIndexBuffer(cmd->getIndexBuffer());
        _commandBuffer->setInstanceBuffer(cmd->getInstanceBuffer());
        _commandBuffer->drawElementsInstanced(cmd->getPrimitiveType(), cmd->getIndexFormat(), cmd->getIndexDrawCount(),
                                              cmd->getIndexDrawOffset(), cmd->getInstanceCount(), cmd->isWireframe());
        _drawnVertices += cmd->getIndexDrawCount() * cmd->getInstanceCount();
    }
    else
    {
        _commandBuffer->drawArrays(cmd->getPrimitiveType(), cmd->getVertexDrawStart(), cmd->getVertexDrawCount(),
                                   cmd->isWireframe());
        _drawnVertices += cmd->getVertexDrawCount();
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
    if (!scene || (scene && scene->_defaultCamera != Camera::getVisitingCamera()))
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

void Renderer::readPixels(backend::RenderTarget* rt,
                          std::function<void(const backend::PixelBufferDescriptor&)> callback)
{
    assert(!!rt);
    if (rt ==
        _defaultRT)  // read pixels from screen, metal renderer backend: screen texture must not be a framebufferOnly
        backend::DriverBase::getInstance()->setFrameBufferOnly(false);

    _commandBuffer->readPixels(rt, std::move(callback));
}

void Renderer::beginRenderPass()
{
    _commandBuffer->beginRenderPass(_currentRT, _renderPassDesc);

    // Disable depth/stencil access if render target has no relevant attachments.
    auto depthStencil = _dsDesc;
    if (!_currentRT->isDefaultRenderTarget())
    {
        if (!_currentRT->_depth)
            depthStencil.removeFlag(DepthStencilFlags::DEPTH_TEST | DepthStencilFlags::DEPTH_WRITE);
        if (!_currentRT->_stencil)
            depthStencil.removeFlag(DepthStencilFlags::STENCIL_TEST);
    }

    _commandBuffer->updateDepthStencilState(depthStencil);
    _commandBuffer->setStencilReferenceValue(_stencilRef);

    _commandBuffer->setViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
    _commandBuffer->setCullMode(_cullMode);
    _commandBuffer->setWinding(_winding);
    _commandBuffer->setScissorRect(_scissorState.isEnabled, _scissorState.rect.x, _scissorState.rect.y,
                                   _scissorState.rect.width, _scissorState.rect.height);
}

void Renderer::endRenderPass()
{
    _commandBuffer->endRenderPass();
}

void Renderer::clear(ClearFlag flags, const Color4F& color, float depth, unsigned int stencil, float globalOrder)
{
    _clearFlag = flags;

    CallbackCommand* command = nextCallbackCommand();
    command->init(globalOrder);
    command->func = [this, flags, color, depth, stencil]() -> void {

        backend::RenderPassDescriptor descriptor;

        descriptor.flags.clear = flags;
        if (bitmask::any(flags, ClearFlag::COLOR))
        {
            _clearColor                = color;
            descriptor.clearColorValue = {color.r, color.g, color.b, color.a};
        }

        if (bitmask::any(flags, ClearFlag::DEPTH))
            descriptor.clearDepthValue = depth;

        if (bitmask::any(flags, ClearFlag::STENCIL))
            descriptor.clearStencilValue = stencil;

        _commandBuffer->setScissorRect(_scissorState.isEnabled, _scissorState.rect.x, _scissorState.rect.y,
                                       _scissorState.rect.width, _scissorState.rect.height);
        _commandBuffer->beginRenderPass(_currentRT, descriptor);
        _commandBuffer->endRenderPass();
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

const Color4F& Renderer::getClearColor() const
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

void Renderer::setScissorRect(float x, float y, float width, float height)
{
    _scissorState.rect.x      = x;
    _scissorState.rect.y      = y;
    _scissorState.rect.width  = width;
    _scissorState.rect.height = height;
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

backend::Buffer* Renderer::TriangleCommandBufferManager::getVertexBuffer() const
{
    return _vertexBufferPool[_currentBufferIndex];
}

backend::Buffer* Renderer::TriangleCommandBufferManager::getIndexBuffer() const
{
    return _indexBufferPool[_currentBufferIndex];
}

void Renderer::TriangleCommandBufferManager::createBuffer()
{
    auto driver = backend::DriverBase::getInstance();

    // Not initializing the buffer before passing it to updateData for Android/OpenGL ES.
    // This change does fix the Android/OpenGL ES performance problem
    // If for some reason we get reports of performance issues on OpenGL implementations,
    // then we can just add pre-processor checks for OpenGL and have the updateData() allocate the full size after buffer creation.
    auto vertexBuffer = driver->newBuffer(Renderer::VBO_SIZE * sizeof(_verts[0]), backend::BufferType::VERTEX,
                                          backend::BufferUsage::DYNAMIC);
    if (!vertexBuffer)
        return;

    auto indexBuffer = driver->newBuffer(Renderer::INDEX_VBO_SIZE * sizeof(_indices[0]), backend::BufferType::INDEX,
                                         backend::BufferUsage::DYNAMIC);
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

NS_AX_END
