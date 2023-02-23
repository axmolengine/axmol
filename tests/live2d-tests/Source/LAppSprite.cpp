/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppSprite.hpp"
#include "base/CCDirector.h"
#include "LAppPal.hpp"

LAppSprite::LAppSprite(backend::Program* program)
{
    _program = program;

    std::fill_n(_spriteColor, 4, 1);
}

LAppSprite::~LAppSprite()
{
}

void LAppSprite::RenderImmidiate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer,
                                 backend::TextureBackend* texture,
                                 float uvVertex[8]) const
{
    Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* drawCommandBuffer = CSM_NEW Csm::Rendering::CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();
    PipelineDescriptor* pipelineDescriptor = drawCommandBuffer->GetCommandDraw()->GetPipelineDescriptor();
    backend::BlendDescriptor* blendDescriptor = drawCommandBuffer->GetCommandDraw()->GetBlendDescriptor();
    backend::ProgramState* programState = pipelineDescriptor->programState;

    drawCommandBuffer->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
    drawCommandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::backend::PrimitiveType::TRIANGLE);
    drawCommandBuffer->CreateVertexBuffer(sizeof(float) * 2, 4 * 2);
    drawCommandBuffer->CreateIndexBuffer(6);

    // 画面サイズを取得する
    ax::Size visibleSize = ax::Director::getInstance()->getVisibleSize();
    ax::Size winSize = ax::Director::getInstance()->getWinSize();

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

    if (!programState)
    {
        programState = new ax::backend::ProgramState(_program);
    }

    // attribute属性を登録
    programState->setVertexAttrib("position", _program->getAttributeLocation("position"), backend::VertexFormat::FLOAT2, 0, false);
    programState->setVertexAttrib("uv", _program->getAttributeLocation("uv"), backend::VertexFormat::FLOAT2, sizeof(float) * 2, false);

    // uniform属性の登録
    programState->setTexture(_program->getUniformLocation("texture"), 0, texture);

    programState->setUniform(_program->getUniformLocation("baseColor"), _spriteColor, sizeof(float) * 4);

    programState->setVertexStride(sizeof(float) * 4);

    blendDescriptor->sourceRGBBlendFactor = ax::backend::BlendFactor::ONE;
    blendDescriptor->destinationRGBBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->sourceAlphaBlendFactor = ax::backend::BlendFactor::ONE;
    blendDescriptor->destinationAlphaBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor->blendEnabled = true;

    pipelineDescriptor->programState = programState;

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
