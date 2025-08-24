/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/rhi/ProgramState.h"
#include "axmol/rhi/Program.h"
#include "axmol/rhi/Texture.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/EventType.h"
#include "axmol/base/Director.h"
#include <algorithm>
#include "xxhash/xxhash.h"

namespace ax::rhi {

// static field
std::vector<ProgramState::AutoBindingResolver*> ProgramState::_customAutoBindingResolvers;

TextureBindingInfo::TextureBindingInfo(const TextureBindingInfo& other)
{
    this->assign(other);
}

TextureBindingInfo::TextureBindingInfo(TextureBindingInfo&& other) noexcept
{
    this->assign(std::move(other));
}

TextureBindingInfo& TextureBindingInfo::operator=(const TextureBindingInfo& other) noexcept
{
    this->assign(other);
    return *this;
}

TextureBindingInfo& TextureBindingInfo::operator=(TextureBindingInfo&& other) noexcept
{
    this->assign(std::move(other));
    return *this;
}

void TextureBindingInfo::assign(const TextureBindingInfo& other)
{
    if (this != &other)
    {
        AX_SAFE_RELEASE(this->tex);
        this->slot = other.slot;
        this->tex  = other.tex;
        AX_SAFE_RETAIN(this->tex);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        location = other.location;
#endif
    }
}

void TextureBindingInfo::assign(TextureBindingInfo&& other)
{
    if (this != &other)
    {
        std::swap(slot, other.slot);
        std::swap(tex, other.tex);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        std::swap(location, other.location);
#endif
    }
}

void TextureBindingInfo::setTexture(int location_, int slot_, rhi::Texture* tex_)
{
    AX_SAFE_RELEASE(this->tex);

    this->slot = slot_;
    this->tex  = tex_;

    AX_SAFE_RETAIN(this->tex);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    this->location = location_;
#endif
}

TextureBindingInfo::~TextureBindingInfo()
{
    AX_SAFE_RELEASE(this->tex);
}

/* CLASS ProgramState */
ProgramState::ProgramState(Program* program)
{
    init(program);
}

bool ProgramState::init(Program* program)
{
    AX_SAFE_RETAIN(program);
    _program = program;

#if AX_RENDER_API == AX_RENDER_API_GL
    auto uboSize = _program->getUniformBufferSize(ShaderStage::DEFAULT);
    _uniformBuffer.resize((std::max)(uboSize, (size_t)1), 0);
#else
    auto fragUboSize = _program->getUniformBufferSize(ShaderStage::FRAGMENT);
    auto vertUboSize  = _program->getUniformBufferSize(ShaderStage::VERTEX);

    _uniformBuffer.resize((std::max)(vertUboSize + fragUboSize, (size_t)1), 0);
    _vertexUniformBufferStart = fragUboSize;
#endif

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _backToForegroundListener =
        EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) { this->resetUniforms(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif

    const auto programId = program->getProgramId();
    if (programId < ProgramType::BUILTIN_COUNT)
    {
        this->_batchId = programId;
        this->_isBatchable = true;
    }

    return true;
}

void ProgramState::updateBatchId()
{
    _batchId = XXH64(_uniformBuffer.data(), _uniformBuffer.size(), _program->getProgramId());
    _isBatchable = true;
}

void ProgramState::resetUniforms()
{
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    if (_program == nullptr)
        return;

    const auto& uniformLocation = _program->getAllUniformsLocation();
    for (const auto& uniform : uniformLocation)
    {
        auto location       = uniform.second;
        auto mappedLocation = _program->getMappedLocation(location);

        // check if current location had been set before
        if (_textureBindingInfos.find(location) != _textureBindingInfos.end())
        {
            _textureBindingInfos[location].location = mappedLocation;
        }
    }
#endif
}

ProgramState::~ProgramState()
{
    AX_SAFE_RELEASE(_program);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

ProgramState* ProgramState::clone() const
{
    ProgramState* cp          = new ProgramState(_program);
    cp->_textureBindingInfos  = _textureBindingInfos;

    cp->_uniformBuffer = _uniformBuffer;
#if AX_RENDER_API != AX_RENDER_API_GL
    cp->_vertexUniformBufferStart = _vertexUniformBufferStart;
#endif

    cp->_batchId = this->_batchId;
    cp->_isBatchable = this->_isBatchable;
    return cp;
}

rhi::UniformLocation ProgramState::getUniformLocation(rhi::Uniform name) const
{
    return _program->getUniformLocation(name);
}

rhi::UniformLocation ProgramState::getUniformLocation(std::string_view uniform) const
{
    return _program->getUniformLocation(uniform);
}

void ProgramState::setCallbackUniform(const rhi::UniformLocation& uniformLocation, const UniformCallback& callback)
{
    _callbackUniforms[uniformLocation] = callback;
}

void ProgramState::setUniform(const rhi::UniformLocation& uniformLocation, const void* data, std::size_t size)
{
    if (uniformLocation.stages[0])
        setUniform(uniformLocation.stages[0].location, data, size, uniformLocation.stages[0].offset, 0);
#if AX_RENDER_API != AX_RENDER_API_GL
    if (uniformLocation.stages[1])
        setUniform(uniformLocation.stages[1].location, data, size, uniformLocation.stages[1].offset,
                   _vertexUniformBufferStart);
#endif
}

void ProgramState::setUniform(int location, const void* data, std::size_t size, int offset, int start)
{
    assert(offset >= 0);
    assert(start + location + offset + size <= _uniformBuffer.size());
    memcpy(_uniformBuffer.data() + start + location + offset, data, size);
}

void ProgramState::setTexture(rhi::Texture* texture)
{
    auto location = getUniformLocation((rhi::Uniform)(rhi::Uniform::TEXTURE));
    setTexture(location, 0, texture);
}

void ProgramState::setTexture(const rhi::UniformLocation& uniformLocation, int slot, rhi::Texture* texture)
{
    auto location = uniformLocation.stages[0].location;
    if (location >= 0)
    {
        auto& textureBinding = _textureBindingInfos[location];
        textureBinding.setTexture(location, slot, texture);
    }
}

void ProgramState::setParameterAutoBinding(std::string_view uniform, std::string_view autoBinding)
{
    _autoBindings.emplace(uniform, autoBinding);
    applyAutoBinding(uniform, autoBinding);
}

void ProgramState::applyAutoBinding(std::string_view uniformName, std::string_view autoBinding)
{
    for (const auto resolver : _customAutoBindingResolvers)
    {
        if (resolver->resolveAutoBinding(this, uniformName, autoBinding))
            break;
    }
}

ProgramState::AutoBindingResolver::AutoBindingResolver()
{
    _customAutoBindingResolvers.emplace_back(this);
}

ProgramState::AutoBindingResolver::~AutoBindingResolver()
{
    auto& list = _customAutoBindingResolvers;
    list.erase(std::remove(list.begin(), list.end(), this), list.end());
}

}
