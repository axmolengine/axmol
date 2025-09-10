/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"
#include "axmol/base/Director.h"
#include "LAppPal.hpp"

LAppSprite::LAppSprite(rhi::Program* program)
{
    _program = program;

    std::fill_n(_spriteColor, 4, 1);
}

LAppSprite::~LAppSprite()
{
}

void LAppSprite::RenderImmidiate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer,
                                 rhi::Texture* texture,
                                 float uvVertex[8]) const
{
    Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* drawCommandBuffer = CSM_NEW Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();
    auto cmd = drawCommandBuffer->GetCommandDraw()->GetCommand();
    rhi::BlendDesc* blendDescriptor = drawCommandBuffer->GetCommandDraw()->GetBlendDescriptor();
    rhi::ProgramState* programState = cmd->unsafePS();

    drawCommandBuffer->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
    drawCommandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::rhi::PrimitiveType::TRIANGLE);
    drawCommandBuffer->CreateVertexBuffer(sizeof(float) * 2, 4 * 2);
    drawCommandBuffer->CreateIndexBuffer(6);

    // 画面サイズを取得する
    ax::Size visibleSize = ax::Director::getInstance()->getVisibleSize();
    ax::Size winSize = ax::Director::getInstance()->getLogicalSize();

    // 頂点データ
    float positionVertex[] =
    {
        visibleSize.width / winSize.width, visibleSize.height / winSize.height,
       -visibleSize.width / winSize.width, visibleSize.height / winSize.height,
       -visibleSize.width / winSize.width,-visibleSize.height / winSize.height,
        visibleSize.width / winSize.width,-visibleSize.height / winSize.height,
    };

    short positionIndex[] =
    {
        0,1,2,
        0,2,3
    };

    drawCommandBuffer->UpdateVertexBuffer(positionVertex, uvVertex, 4);
    drawCommandBuffer->UpdateIndexBuffer(positionIndex, 6);
    drawCommandBuffer->CommitVertexBuffer();

    bool needInit = !programState;
    if (needInit)
    {
        programState = new ax::rhi::ProgramState(_program);
    }

    rhi::VertexLayoutDesc desc = axvlm->allocateVertexLayoutDesc();
    // attribute属性を登録
    desc.startLayout(2);
    desc.addAttrib("a_position", _program->getVertexInputDesc("a_position"), rhi::VertexFormat::FLOAT2, 0, false);
    desc.addAttrib("a_texCoord", _program->getVertexInputDesc("a_texCoord"), rhi::VertexFormat::FLOAT2,
                            sizeof(float) * 2, false);

    // uniform属性の登録
    programState->setTexture(_program->getUniformLocation("u_tex0"), 0, texture);

    programState->setUniform(_program->getUniformLocation("baseColor"), _spriteColor, sizeof(float) * 4);

    desc.endLayout(sizeof(float) * 4);

    blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::ONE;
    blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
    blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->blendEnabled = true;

    if (needInit)
        cmd->setOwnPSVL(programState, axvlm->acquireVertexLayout(std::move(desc)),
                        RenderCommand::ADOPT_FLAG_ALL);

    // モデルの描画
    commandBuffer->AddDrawCommand(drawCommandBuffer->GetCommandDraw());

}

void LAppSprite::SetColor(float r, float g, float b, float a)
{
    _spriteColor[0] = r;
    _spriteColor[1] = g;
    _spriteColor[2] = b;
    _spriteColor[3] = a;
}
