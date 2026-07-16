/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "axmol/axmol.h"
#include "axmol/rhi/Texture.h"

using namespace ax;

namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

static inline ax::Renderer* GetCocos2dRenderer()
{
    return ax::Director::getInstance()->getRenderer();
}

class CubismCommandBuffer_Axmol
{
public:
    class DrawCommandBuffer
    {
    public:
        class DrawCommand
        {
        public:
            DrawCommand();
            virtual ~DrawCommand();

            ax::rhi::BlendDesc* GetBlendDescriptor();
            ax::CustomCommand* GetCommand();

        private:
            DrawCommand& operator=(const DrawCommand&);
            ax::CustomCommand _command;
        };

        DrawCommandBuffer();
        virtual ~DrawCommandBuffer();


        void CreateVertexBuffer(csmSizeInt stride, csmSizeInt count);
        void CreateIndexBuffer(csmSizeInt count);
        void UpdateVertexBuffer(const void* data, const void* uvData, csmSizeInt count);
        void UpdateIndexBuffer(const void* data, csmSizeInt count);
        void CommitVertexBuffer();

        DrawCommand* GetCommandDraw();

    private:
        DrawCommand _drawCommandDraw;
        csmSizeInt _vbStride;
        csmSizeInt _vbCount;
        csmSizeInt _ibCount;
        csmUint8* _drawBuffer;
    };

    union OperationStateDataArgument
    {
        csmBool    bl;
        csmInt32   i32;
        csmUint32  u32;
        csmFloat32 f32;
    };

    struct OperationStateData
    {
        OperationStateData()
        : Enabled(false)
        {
            Arg0.i32 = 0;
            Arg1.i32 = 0;
            Arg2.i32 = 0;
            Arg3.i32 = 0;
        }

        csmBool Enabled;
        OperationStateDataArgument Arg0;
        OperationStateDataArgument Arg1;
        OperationStateDataArgument Arg2;
        OperationStateDataArgument Arg3;
    };

    enum OperationType
    {
        OperationType_ScissorTest = 0,
        OperationType_DepthTest,
        OperationType_StencilTest,
        OperationType_Culling,
        OperationType_Winding,
        OperationType_TypeMax,
    };

    enum CullType
    {
        CullType_Front,
        CullType_Back,
    };

    enum WindingType
    {
        WindingType_ClockWise,
        WindingType_CounterClockWise,
    };

    CubismCommandBuffer_Axmol();
    virtual ~CubismCommandBuffer_Axmol();

    void PushCommandGroup();
    void PopCommandGroup();

    void SetOperationEnable(OperationType operationType, csmBool enabled);
    csmBool GetOperationEnabled(OperationType operationType);

    void SetCullMode(CullType cullType);
    void SetWindingMode(WindingType windingType);

    void Clear(csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a);

    void Viewport(csmFloat32 x, csmFloat32 y, csmFloat32 w, csmFloat32 h);

    void SetColorBuffer(rhi::Texture* colorBuffer);
    rhi::Texture* GetColorBuffer();

    void AddDrawCommand(DrawCommandBuffer::DrawCommand* drawCommand);

private:
    void AddCommand(const std::function<void()>& fn);

    rhi::Texture* _currentColorBuffer;
    rhi::RenderTarget* _offscreenRT = nullptr;
    OperationStateData _operationStateArray[OperationType_TypeMax];
};

}}}}
