/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.
 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <EffekseerRendererCommon/EffekseerRenderer.CommonUtils.h>
#include <EffekseerRendererCommon/EffekseerRenderer.ShaderBase.h>
#include "axmol/rhi/ProgramState.h"
#include <array>
#include <memory>

namespace ax { class CustomEventListener; class EventDispatcher; }
namespace EffekseerRendererAxmol
{
// Common command/texture submission interface. Sprite and model shaders have
// different constant-buffer ABIs and vertex layouts, not different RHI commands.
class DrawShader : public EffekseerRenderer::ShaderBase
{
public:
    virtual ax::rhi::ProgramState* programState() const = 0;
    virtual ax::rhi::VertexLayout* vertexLayout(int32_t stride) = 0;
    virtual void setTexture(int slot, ax::rhi::Texture* texture) = 0;
    virtual void setSampler(int slot, const ax::rhi::SamplerDesc& desc) = 0;
    virtual void setDefaultTextures(ax::rhi::Texture* texture) = 0;
};

ax::rhi::VertexLayout* createModelLayout(Effekseer::Backend::GraphicsDeviceRef device, ax::rhi::Program* program);

class ModelShaderCache
{
public:
    ModelShaderCache();
    ~ModelShaderCache();
    DrawShader* acquire(EffekseerRenderer::RendererShaderType type, Effekseer::Backend::GraphicsDeviceRef device);

private:
    struct Entry
    {
        std::unique_ptr<DrawShader> shader;
        bool attempted = false;
    };
    std::array<Entry, 6> _entries;
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    ax::EventDispatcher* _eventDispatcher = nullptr;
    ax::CustomEventListener* _recreated = nullptr;
#endif
};
} // namespace EffekseerRendererAxmol
