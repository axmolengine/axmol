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

namespace ax::rhi
{

// static field
std::vector<ProgramState::AutoBindingResolver*> ProgramState::_customAutoBindingResolvers;

TextureBindingSet::TextureBindingSet(const TextureBindingSet& other)
{
    this->assign(other);
}

TextureBindingSet::TextureBindingSet(TextureBindingSet&& other) noexcept
{
    this->swap(other);
}

TextureBindingSet& TextureBindingSet::operator=(const TextureBindingSet& other) noexcept
{
    this->assign(other);
    return *this;
}

TextureBindingSet& TextureBindingSet::operator=(TextureBindingSet&& other) noexcept
{
    this->swap(other);
    return *this;
}

TextureBindingSet::~TextureBindingSet()
{
    releaseTextures();
}

void TextureBindingSet::assign(const TextureBindingSet& other)
{
    if (this != &other)
    {
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        setTextureArray(other.loc, other.slots, other.texs);
#else
        setTextureArray(-1, other.slots, other.texs);
#endif
    }
}

void TextureBindingSet::swap(TextureBindingSet& other)
{
    if (this != &other)
    {
        std::swap(slots, other.slots);
        std::swap(texs, other.texs);

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
        std::swap(loc, other.loc);
#endif
    }
}

void TextureBindingSet::setTexture(int location, int slot, rhi::Texture* tex)
{
    if (tex && slot >= 0)
    {
        tex->retain();
        releaseTextures();
        this->slots.push_back(slot);
        this->texs.push_back(tex);
    }
}

void TextureBindingSet::setTextureArray(int location, std::span<const TextureBinding> units)
{
    for (auto& unit : units)
        AX_SAFE_RETAIN(unit.tex);

    releaseTextures();

    if (!units.empty())
    {
        const auto count = units.size();
        this->texs.resize(count);
        this->slots.resize(count);
        for (int i = 0; i < count; ++i)
        {
            this->texs[i]  = units[i].tex;
            this->slots[i] = units[i].slot;
        }
    }

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    this->loc = location;
#endif
}

void TextureBindingSet::setTextureArray(int location, std::span<const int> slots, std::span<rhi::Texture*> texs)
{
    bool retain = !slots.empty() && (slots.size() == texs.size());

    if (retain)
    {
        for (auto tex : texs)
            AX_SAFE_RETAIN(tex);
    }

    releaseTextures();

    if (retain)
    {
        this->slots.resize(slots.size());
        this->texs.resize(slots.size());

        memcpy(this->slots.data(), slots.data(), slots.size_bytes());
        memcpy(this->texs.data(), texs.data(), texs.size_bytes());
    }

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    this->loc = location;
#endif
}

void TextureBindingSet::releaseTextures()
{
    for (auto& tex : this->texs)
        AX_SAFE_RELEASE(tex);
    this->texs.clear();
    this->slots.clear();
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
    auto vertUboSize = _program->getUniformBufferSize(ShaderStage::VERTEX);

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
        this->_batchId     = programId;
        this->_isBatchable = true;
    }

    return true;
}

void ProgramState::updateBatchId()
{
    _batchId     = XXH64(_uniformBuffer.data(), _uniformBuffer.size(), _program->getProgramId());
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
        auto it = _textureBindingSets.find(location);
        if (it != _textureBindingSets.end())
            it->second.loc = mappedLocation;
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
    ProgramState* cp        = new ProgramState(_program);
    cp->_textureBindingSets = _textureBindingSets;

    cp->_uniformBuffer = _uniformBuffer;
#if AX_RENDER_API != AX_RENDER_API_GL
    cp->_vertexUniformBufferStart = _vertexUniformBufferStart;
#endif

    cp->_batchId     = this->_batchId;
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
        auto& bindingSet = _textureBindingSets[location];
        bindingSet.setTexture(location, slot, texture);
    }
}

void ProgramState::setTextureArray(const rhi::UniformLocation& uniformLocation, std::span<TextureBinding> units)
{
    auto location = uniformLocation.stages[0].location;
    if (location >= 0)
    {
        auto& bindingSet = _textureBindingSets[location];
        bindingSet.setTextureArray(location, units);
    }
}

void ProgramState::setTextureArray(const rhi::UniformLocation& uniformLocation,
                                   std::span<int> slots,
                                   std::span<rhi::Texture*> textures)
{
    auto location = uniformLocation.stages[0].location;
    if (location >= 0)
    {
        auto& bindingSet = _textureBindingSets[location];
        bindingSet.setTextureArray(location, slots, textures);
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

}  // namespace ax::rhi
