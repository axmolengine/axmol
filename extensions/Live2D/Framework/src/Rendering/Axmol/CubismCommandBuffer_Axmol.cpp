/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismCommandBuffer_Axmol.hpp"
#include "CubismFramework.hpp"
#include "axmol/rhi/RenderTarget.h"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {
CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand::DrawCommand()
{
    _command.init(0.0);
}

CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand::~DrawCommand()
{
}

ax::rhi::BlendDesc* CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand::GetBlendDescriptor()
{
    return &_command.blendDesc();
}

ax::CustomCommand* CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand::GetCommand()
{
    return &_command;
}

CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommandBuffer()
    : _vbStride(0)
    , _vbCount(0)
    , _ibCount(0)
    , _drawBuffer(nullptr)
{
}

CubismCommandBuffer_Axmol::DrawCommandBuffer::~DrawCommandBuffer()
{
    CSM_FREE(_drawBuffer);
}

void CubismCommandBuffer_Axmol::DrawCommandBuffer::CreateVertexBuffer(csmSizeInt stride, csmSizeInt count)
{
    _vbStride = stride;
    _vbCount = count;
    _drawBuffer = static_cast<csmUint8*>(CSM_MALLOC(stride * count));
    _drawCommandDraw.GetCommand()->createVertexBuffer(stride, count, ax::CustomCommand::BufferUsage::DYNAMIC);
}

void CubismCommandBuffer_Axmol::DrawCommandBuffer::CreateIndexBuffer(csmSizeInt count)
{
    _ibCount = count;
    _drawCommandDraw.GetCommand()->createIndexBuffer(ax::rhi::IndexFormat::U_SHORT, count, ax::CustomCommand::BufferUsage::DYNAMIC);
}

void CubismCommandBuffer_Axmol::DrawCommandBuffer::UpdateVertexBuffer(const void* data, const void* uvData, csmSizeInt count)
{
    csmSizeInt length = count * _vbStride;
    csmFloat32* dest = reinterpret_cast<csmFloat32*>(_drawBuffer);
    const csmFloat32* sourceVertices = reinterpret_cast<const csmFloat32*>(data);
    const csmFloat32* sourceUvs = reinterpret_cast<const csmFloat32*>(uvData);

    for (csmUint32 i = 0, j = 0; i < count; ++i)
    {
        *dest = *sourceVertices;
        dest++;
        sourceVertices++;
        *dest = *sourceVertices;
        dest++;
        sourceVertices++;

        *dest = *sourceUvs;
        dest++;
        sourceUvs++;
        *dest = *sourceUvs;
        dest++;
        sourceUvs++;
    }
}

void CubismCommandBuffer_Axmol::DrawCommandBuffer::UpdateIndexBuffer(const void* data, csmSizeInt count)
{
    csmSizeInt length = count * sizeof(csmInt16);

    _drawCommandDraw.GetCommand()->updateIndexBuffer(data, length);
}

void CubismCommandBuffer_Axmol::DrawCommandBuffer::CommitVertexBuffer() {

    csmSizeInt count = _vbCount * _vbStride;

    _drawCommandDraw.GetCommand()->updateVertexBuffer(_drawBuffer, count);
}

CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* CubismCommandBuffer_Axmol::DrawCommandBuffer::GetCommandDraw()
{
    return &_drawCommandDraw;
}

CubismCommandBuffer_Axmol::CubismCommandBuffer_Axmol()
    :_currentColorBuffer(NULL)
{
}

CubismCommandBuffer_Axmol::~CubismCommandBuffer_Axmol()
{
    AX_SAFE_RELEASE(_offscreenRT);
}

void CubismCommandBuffer_Axmol::PushCommandGroup()
{
    auto groupCommand = GetCocos2dRenderer()->getNextGroupCommand();
    groupCommand->init(0.0);
    GetCocos2dRenderer()->addCommand(groupCommand);
    GetCocos2dRenderer()->pushGroup(groupCommand->getRenderQueueID());
}

void CubismCommandBuffer_Axmol::PopCommandGroup()
{
    GetCocos2dRenderer()->popGroup();
}

void CubismCommandBuffer_Axmol::SetOperationEnable(OperationType operationType, csmBool enabled)
{
    _operationStateArray[operationType].Enabled = enabled;


    AddCommand
    (
        [=]() -> void
    {
        switch (operationType)
        {
        case OperationType_ScissorTest:
            GetCocos2dRenderer()->setScissorTest(enabled);
            break;
        case OperationType_DepthTest:
            GetCocos2dRenderer()->setDepthTest(enabled);
            break;
        case OperationType_StencilTest:
            GetCocos2dRenderer()->setStencilTest(enabled);
            break;
        case OperationType_Culling:
            if (!enabled)
            {
                GetCocos2dRenderer()->setCullMode(ax::CullMode::NONE);
            }
            else
            {
                switch (_operationStateArray[operationType].Arg0.i32)
                {
                case CullType_Front:
                    GetCocos2dRenderer()->setCullMode(ax::CullMode::FRONT);
                    break;
                case CullType_Back:
                    GetCocos2dRenderer()->setCullMode(ax::CullMode::BACK);
                    break;
                }
            }
            break;

        }
    }
    );
}

csmBool CubismCommandBuffer_Axmol::GetOperationEnabled(OperationType operationType)
{
    return _operationStateArray[operationType].Enabled;
}

void CubismCommandBuffer_Axmol::SetCullMode(CullType cullType)
{
    _operationStateArray[OperationType_Culling].Arg0.i32 = cullType;


    SetOperationEnable(OperationType_Culling, _operationStateArray[OperationType_Culling].Enabled);
}

void CubismCommandBuffer_Axmol::SetWindingMode(WindingType windingType)
{
    _operationStateArray[OperationType_Winding].Arg0.i32 = windingType;


    AddCommand
    (
        [=] () -> void
        {
            switch (_operationStateArray[OperationType_Winding].Arg0.i32)
            {
            case WindingType_ClockWise:
                GetCocos2dRenderer()->setWinding(ax::Winding::CLOCK_WISE);
                break;

            case  WindingType_CounterClockWise:
                GetCocos2dRenderer()->setWinding(ax::Winding::COUNTER_CLOCK_WISE);
                break;
            }
        }
    );
}

void CubismCommandBuffer_Axmol::Clear(csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a)
{
    // Add the callback command internally.
    GetCocos2dRenderer()->clear(ax::ClearFlag::COLOR, ax::Color(r, g, b, a), 0.0f, 0, 0.0f);
}

void CubismCommandBuffer_Axmol::Viewport(csmFloat32 x, csmFloat32 y, csmFloat32 w, csmFloat32 h)
{
    AddCommand
    (
        [=] () -> void
        {
            GetCocos2dRenderer()->setViewport(x, y, w, h);
        }
    );
}

void CubismCommandBuffer_Axmol::SetColorBuffer(rhi::Texture* colorBuffer)
{
    // nullptr means default render target.
    // Non-null means offscreen color texture.
    //
    // Do not pass the default render target's swapchain color texture here.
    // Swapchain images are presentable color attachments, not ordinary sampled
    // or offscreen textures.
    _currentColorBuffer = colorBuffer;

    AddCommand([this, colorBuffer]() -> void {
        rhi::RenderTarget* rt = nullptr;
        if (colorBuffer)
        {
            if (!_offscreenRT)
                _offscreenRT = axdrv->createRenderTarget();
            rt = _offscreenRT;
            rt->setColorTexture(colorBuffer);
        }
        else
        {
            rt = GetCocos2dRenderer()->getDefaultRenderTarget();
        }
        GetCocos2dRenderer()->setRenderTarget(rt);
    });
}

rhi::Texture* CubismCommandBuffer_Axmol::GetColorBuffer()
{
    return _currentColorBuffer;
}

void CubismCommandBuffer_Axmol::AddDrawCommand(DrawCommandBuffer::DrawCommand* drawCommand)
{
    GetCocos2dRenderer()->addCommand(drawCommand->GetCommand());
}

void CubismCommandBuffer_Axmol::AddCommand(const std::function<void()>& fn)
{
    ax::CallbackCommand* command = GetCocos2dRenderer()->nextCallbackCommand();

    command->init(0.0f);
    command->func = fn;

    GetCocos2dRenderer()->addCommand(command);
}

}}}}
