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
        setTextureArray(this->runtimeLocation, other.slots, other.texs);
    }
}

void TextureBindingSet::swap(TextureBindingSet& other)
{
    if (this != &other)
    {
        std::swap(this->slots, other.slots);
        std::swap(this->texs, other.texs);
        std::swap(this->runtimeLocation, other.runtimeLocation);
    }
}

void TextureBindingSet::setTexture(int location, int slot, rhi::Texture* tex)
{
    if (tex && slot >= 0)
    {
        tex->retain();
        releaseTextures();
        this->runtimeLocation = location;
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
        this->runtimeLocation = location;
        const auto count      = units.size();
        this->texs.resize(count);
        this->slots.resize(count);
        for (int i = 0; i < count; ++i)
        {
            this->texs[i]  = units[i].tex;
            this->slots[i] = units[i].slot;
        }
    }
}

void TextureBindingSet::setTextureArray(int location, std::span<const int> slots, std::span<rhi::Texture* const> texs)
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
        this->runtimeLocation = location;
        this->slots.resize(slots.size());
        this->texs.resize(slots.size());

        memcpy(this->slots.data(), slots.data(), slots.size_bytes());
        memcpy(this->texs.data(), texs.data(), texs.size_bytes());
    }
}

void TextureBindingSet::releaseTextures()
{
    for (auto& tex : this->texs)
        AX_SAFE_RELEASE(tex);
    this->texs.clear();
    this->slots.clear();
    this->runtimeLocation = -1;
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

    auto uniformBufferSize = _program->getUniformBufferSize();
    _uniformBuffer.resize((std::max)(uniformBufferSize, (size_t)1), 0);

    const auto programId = program->getProgramId();
    if (programId < ProgramType::BUILTIN_COUNT)
    {
        this->_batchId     = programId;
        this->_isBatchable = true;
    }

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _backToForegroundListener = EventListenerCustom::create(
        EVENT_RENDERER_RECREATED, [this](EventCustom*) { this->remapTextureRuntimeLocations(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif

    return true;
}

void ProgramState::updateBatchId()
{
    _batchId     = XXH64(_uniformBuffer.data(), _uniformBuffer.size(), _program->getProgramId());
    _isBatchable = true;
}

void ProgramState::remapTextureRuntimeLocations()
{
    if (_program == nullptr)
        return;

    const auto& textureInfos = _program->getActiveTextureInfos();
    for (auto& entry : textureInfos)
    {
        // check if current location had been set before
        auto& textureInfo = entry.second;
        auto bindingIt    = _textureBindingSets.find(textureInfo->location);
        if (bindingIt != _textureBindingSets.end())
            bindingIt->second.runtimeLocation = textureInfo->runtimeLocation;
    }
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
    cp->_batchId       = this->_batchId;
    cp->_isBatchable   = this->_isBatchable;
    return cp;
}

rhi::UniformLocation ProgramState::getUniformLocation(rhi::Uniform kind) const
{
    return _program->getUniformLocation(kind);
}

rhi::UniformLocation ProgramState::getUniformLocation(std::string_view name) const
{
    return _program->getUniformLocation(name);
}

void ProgramState::getUniformLocations(std::string_view name, UniformLocationVector& outLocations) const
{
    _program->getUniformLocations(name, outLocations);
}

void ProgramState::setCallbackUniform(const rhi::UniformLocation& uniformLocation, const UniformCallback& callback)
{
    _callbackUniforms[uniformLocation] = callback;
}

void ProgramState::setUniform(const rhi::UniformLocation& uniformLocation, const void* data, std::size_t size)
{
    if (uniformLocation)
    {
        auto offset    = uniformLocation.offset;
        auto cpuOffset = uniformLocation.cpuOffset;
        assert(offset >= 0);
        assert(cpuOffset >= 0);
        assert(cpuOffset + offset + size <= _uniformBuffer.size());
        memcpy(_uniformBuffer.data() + cpuOffset + offset, data, size);
    }
}

void ProgramState::setTexture(rhi::Texture* texture)
{
    auto location = getUniformLocation(rhi::Uniform::TEXTURE);
    setTexture(location, 0, texture);
}

void ProgramState::setTexture(const rhi::UniformLocation& uniformLocation, int slot, rhi::Texture* texture)
{
    if (uniformLocation)
    {
        auto& bindingSet = _textureBindingSets[uniformLocation.location];
        bindingSet.setTexture(uniformLocation.runtimeLocation, slot, texture);
    }
}

void ProgramState::setTextureArray(const rhi::UniformLocation& uniformLocation, std::span<TextureBinding> units)
{
    if (uniformLocation)
    {
        auto& bindingSet = _textureBindingSets[uniformLocation.location];
        bindingSet.setTextureArray(uniformLocation.runtimeLocation, units);
    }
}

void ProgramState::setTextureArray(const rhi::UniformLocation& uniformLocation,
                                   std::span<int> slots,
                                   std::span<rhi::Texture*> textures)
{
    if (uniformLocation)
    {
        auto& bindingSet = _textureBindingSets[uniformLocation.location];
        bindingSet.setTextureArray(uniformLocation.runtimeLocation, slots, textures);
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
