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
#include "axmol/rhi/SamplerRegistry.h"
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
        setTextureArray(other.runtimeLocation, other.slots, other.texs);
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

StorageBufferBindingSet::StorageBufferBindingSet(const StorageBufferBindingSet& other)
{
    assign(other);
}

StorageBufferBindingSet::StorageBufferBindingSet(StorageBufferBindingSet&& other) noexcept
{
    swap(other);
}

StorageBufferBindingSet& StorageBufferBindingSet::operator=(const StorageBufferBindingSet& other) noexcept
{
    assign(other);
    return *this;
}

StorageBufferBindingSet& StorageBufferBindingSet::operator=(StorageBufferBindingSet&& other) noexcept
{
    swap(other);
    return *this;
}

StorageBufferBindingSet::~StorageBufferBindingSet()
{
    releaseBuffer();
}

void StorageBufferBindingSet::assign(const StorageBufferBindingSet& other)
{
    if (this != &other)
        setBuffer(other.runtimeLocation, other.buffer, other.access);
}

void StorageBufferBindingSet::swap(StorageBufferBindingSet& other)
{
    if (this != &other)
    {
        std::swap(runtimeLocation, other.runtimeLocation);
        std::swap(buffer, other.buffer);
        std::swap(access, other.access);
    }
}

void StorageBufferBindingSet::setBuffer(int slot, rhi::Buffer* newBuffer, BufferAccess newAccess)
{
    AX_SAFE_RETAIN(newBuffer);
    releaseBuffer();
    runtimeLocation = slot;
    buffer          = newBuffer;
    access          = newAccess;
}

void StorageBufferBindingSet::releaseBuffer()
{
    AX_SAFE_RELEASE(buffer);
    runtimeLocation = -1;
    access          = BufferAccess::READ_ONLY;
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
    _backToForegroundListener = CustomEventListener::create(
        EVENT_RENDERER_RECREATED, [this](CustomEvent*) { this->remapTextureRuntimeLocations(); });
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
    cp->_storageBufferBindingSets = _storageBufferBindingSets;
    cp->_samplerOverrides = _samplerOverrides;

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

void ProgramState::setUniform(const rhi::UniformLocation& uniformLocation, const void* data, size_t size)
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

void ProgramState::setUniformBlock(int binding, const void* data, size_t size)
{
    if (!data || size == 0)
        return;

    const auto& blocks = _program->getActiveUniformBlockInfos();
    auto it = std::find_if(blocks.begin(), blocks.end(), [binding](const UniformBlockInfo& info) {
        return info.binding == binding;
    });
    if (it == blocks.end())
        return;

    const auto copySize = (std::min)(size, static_cast<size_t>(it->sizeBytes));
    assert(it->cpuOffset + copySize <= _uniformBuffer.size());
    memcpy(_uniformBuffer.data() + it->cpuOffset, data, copySize);
}

bool ProgramState::setUniformBlock(ShaderStage stage, std::string_view blockName, const void* data, size_t size)
{
    if (!data || size == 0 || blockName.empty())
        return false;

    for (const auto& block : _program->getActiveUniformBlockInfos())
    {
        if (block.stage != stage)
            continue;

        auto reflectedName = block.name;
        constexpr std::string_view generatedPrefixes[] = {"type_", "type."};
        for (const auto prefix : generatedPrefixes)
        {
            if (reflectedName.starts_with(prefix))
            {
                reflectedName.remove_prefix(prefix.size());
                break;
            }
        }
        if (reflectedName != blockName)
            continue;

        const auto copySize = (std::min)(size, static_cast<size_t>(block.sizeBytes));
        assert(block.cpuOffset + copySize <= _uniformBuffer.size());
        memcpy(_uniformBuffer.data() + block.cpuOffset, data, copySize);
        return true;
    }

    return false;
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

void ProgramState::setStorageBuffer(int binding, rhi::Buffer* buffer, BufferAccess access)
{
    if (binding < 0)
        return;

    auto& bindingSet = _storageBufferBindingSets[binding];
    bindingSet.setBuffer(binding, buffer, access);
}

void ProgramState::setSampler(const rhi::SamplerLocation& samplerLocation, const SamplerDesc& desc)
{
    if (!samplerLocation)
        return;

    const auto& samplers = _program->getActiveSamplerInfos();
    auto it = std::find_if(samplers.begin(), samplers.end(), [samplerLocation](const SamplerBindingInfo& info) {
        return info.binding == samplerLocation.binding && info.space == samplerLocation.space;
    });
    if (it == samplers.end() || it->presetIndex >= 0)
        return;

    auto samplerId = SamplerRegistry::getInstance()->registerSampler(desc);
    _samplerOverrides[samplerLocation.binding] = SamplerId{static_cast<uint16_t>(samplerId)};
}

void ProgramState::setSampler(std::string_view name, const SamplerDesc& desc)
{
    setSampler(_program->getSamplerLocation(name), desc);
}

SamplerId ProgramState::getSamplerOverride(int binding) const
{
    auto it = _samplerOverrides.find(binding);
    return it != _samplerOverrides.end() ? it->second : SamplerId{};
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
