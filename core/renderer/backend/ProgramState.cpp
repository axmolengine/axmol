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

#include "renderer/backend/ProgramState.h"
#include "renderer/backend/Program.h"
#include "renderer/backend/Texture.h"
#include "renderer/backend/Types.h"
#include "base/EventDispatcher.h"
#include "base/EventType.h"
#include "base/Director.h"
#include <algorithm>
#include "xxhash/xxhash.h"

#include "glslcc/sgs-spec.h"

NS_AX_BACKEND_BEGIN

// static field
std::vector<ProgramState::AutoBindingResolver*> ProgramState::_customAutoBindingResolvers;

TextureInfo::TextureInfo(std::vector<int>&& _slots, std::vector<backend::TextureBackend*>&& _textures)
    : TextureInfo(std::move(_slots), std::vector<int>(_slots.size(), 0), std::move(_textures))
{}

TextureInfo::TextureInfo(std::vector<int>&& _slots,
                         std::vector<int>&& _indexs,
                         std::vector<backend::TextureBackend*>&& _textures)
    : slots(std::move(_slots)), indexs(std::move(_indexs)), textures(std::move(_textures))
{
    retainTextures();
}

/* CLASS TextureInfo */
TextureInfo::TextureInfo(const TextureInfo& other)
{
    this->assign(other);
}

TextureInfo::TextureInfo(TextureInfo&& other)
{
    this->assign(std::move(other));
}

TextureInfo::~TextureInfo()
{
    releaseTextures();
}

void TextureInfo::retainTextures()
{
    for (auto&& texture : textures)
        AX_SAFE_RETAIN(texture);
}

void TextureInfo::releaseTextures()
{
    for (auto&& texture : textures)
        AX_SAFE_RELEASE(texture);
    textures.clear();
}

TextureInfo& TextureInfo::operator=(const TextureInfo& other) noexcept
{
    this->assign(other);
    return *this;
}

TextureInfo& TextureInfo::operator=(TextureInfo&& other) noexcept
{
    this->assign(std::move(other));
    return *this;
}

void TextureInfo::assign(const TextureInfo& other)
{
    if (this != &other)
    {
        releaseTextures();

        indexs   = other.indexs;
        slots    = other.slots;
        textures = other.textures;
        retainTextures();

#if AX_ENABLE_CACHE_TEXTURE_DATA
        location = other.location;
#endif
    }
}

void TextureInfo::assign(TextureInfo&& other)
{
    if (this != &other)
    {
        releaseTextures();

        indexs   = std::move(other.indexs);
        slots    = std::move(other.slots);
        textures = std::move(other.textures);

#if AX_ENABLE_CACHE_TEXTURE_DATA
        location       = other.location;
        other.location = -1;
#endif
    }
}

void TextureInfo::assign(int slot, int index, backend::TextureBackend* texture)
{
    if (textures.size() != 1 or textures[0] != texture or slots[0] != slot or indexs[0] != index)
    {
        releaseTextures();
        indexs.resize(1);
        indexs[0] = index;
        slots.resize(1);
        slots[0] = slot;
        textures.resize(1);
        textures[0] = texture;
        AX_SAFE_RETAIN(texture);

#if AX_ENABLE_CACHE_TEXTURE_DATA
        location = -1;
#endif
    }
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

    _vertexLayout            = program->getVertexLayout();
    _ownVertexLayout         = false;
    _vertexUniformBufferSize = _program->getUniformBufferSize(ShaderStage::VERTEX);

#ifdef AX_USE_METAL
    _fragmentUniformBufferSize = _program->getUniformBufferSize(ShaderStage::FRAGMENT);
#endif

    _uniformBuffers.resize((std::max)(_vertexUniformBufferSize + _fragmentUniformBufferSize, (size_t)1), 0);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener =
        EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) { this->resetUniforms(); });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif

    const auto programId = program->getProgramId();
    if (programId < ProgramType::BUILTIN_COUNT)
        this->_batchId = programId;

    return true;
}

void ProgramState::updateBatchId()
{
    _batchId = XXH64(_uniformBuffers.data(), _uniformBuffers.size(), _program->getProgramId());
}

void ProgramState::resetUniforms()
{
#if AX_ENABLE_CACHE_TEXTURE_DATA
    if (_program == nullptr)
        return;

    const auto& uniformLocation = _program->getAllUniformsLocation();
    for (const auto& uniform : uniformLocation)
    {
        auto location       = uniform.second;
        auto mappedLocation = _program->getMappedLocation(location);

        // check if current location had been set before
        if (_vertexTextureInfos.find(location) != _vertexTextureInfos.end())
        {
            _vertexTextureInfos[location].location = mappedLocation;
        }
    }
#endif
}

ProgramState::~ProgramState()
{
    AX_SAFE_RELEASE(_program);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif

    if (_ownVertexLayout)
        AX_SAFE_DELETE(_vertexLayout);
}

ProgramState* ProgramState::clone() const
{
    ProgramState* cp          = new ProgramState(_program);
    cp->_vertexTextureInfos   = _vertexTextureInfos;
    cp->_fragmentTextureInfos = _fragmentTextureInfos;
    cp->_uniformBuffers       = _uniformBuffers;

    cp->_ownVertexLayout = _ownVertexLayout;
    cp->_vertexLayout    = !_ownVertexLayout ? _vertexLayout : new VertexLayout(*_vertexLayout);

    cp->_batchId = this->_batchId;
    return cp;
}

backend::UniformLocation ProgramState::getUniformLocation(backend::Uniform name) const
{
    return _program->getUniformLocation(name);
}

backend::UniformLocation ProgramState::getUniformLocation(std::string_view uniform) const
{
    return _program->getUniformLocation(uniform);
}

void ProgramState::setCallbackUniform(const backend::UniformLocation& uniformLocation, const UniformCallback& callback)
{
    _callbackUniforms[uniformLocation] = callback;
}

void ProgramState::setUniform(const backend::UniformLocation& uniformLocation, const void* data, std::size_t size)
{
    if (uniformLocation.vertStage)
        setVertexUniform(uniformLocation.vertStage.location, data, size, uniformLocation.vertStage.offset);
#ifdef AX_USE_METAL
    if (uniformLocation.fragStage)
        setFragmentUniform(uniformLocation.fragStage.location, data, size, uniformLocation.fragStage.offset);
#endif
}

void ProgramState::setVertexUniform(int location, const void* data, std::size_t size, std::size_t offset)
{
    if (location < 0)
        return;
#if AX_GLES_PROFILE != 200
    assert(location + offset + size <= _vertexUniformBufferSize);
    memcpy(_uniformBuffers.data() + location + offset, data, size);
#else
    assert(offset + size <= _vertexUniformBufferSize);
    memcpy(_uniformBuffers.data() + offset, data, size);
#endif
}

#ifdef AX_USE_METAL
void ProgramState::setFragmentUniform(int location, const void* data, std::size_t size, std::size_t offset)
{
    if (location < 0)
        return;

    memcpy(_uniformBuffers.data() + _vertexUniformBufferSize + location + offset, data, size);
}
#endif

#ifndef AX_CORE_PROFILE
void ProgramState::setVertexAttrib(std::string_view name,
                                   std::size_t index,
                                   VertexFormat format,
                                   std::size_t offset,
                                   bool needToBeNormallized)
{
    ensureVertexLayoutMutable();

    _vertexLayout->setAttrib(name, index, format, offset, needToBeNormallized);
}

void ProgramState::setVertexStride(uint32_t stride)
{
    ensureVertexLayoutMutable();
    _vertexLayout->setStride(stride);
}
#endif
void ProgramState::validateSharedVertexLayout(VertexLayoutType vlt)
{
    if (!_ownVertexLayout && !_vertexLayout->isValid())
        _program->setupVertexLayout(vlt);
}

void ProgramState::ensureVertexLayoutMutable()
{
    if (!_ownVertexLayout)
    {
        _vertexLayout    = new VertexLayout();
        _ownVertexLayout = true;
    }
}

VertexLayout* ProgramState::getMutableVertexLayout()
{
    if (_ownVertexLayout || !_vertexLayout->isValid())
        return _vertexLayout;

    _ownVertexLayout     = true;
    return _vertexLayout = new VertexLayout();
}

void ProgramState::setSharedVertexLayout(VertexLayout* vertexLayout)
{
    if (_ownVertexLayout)
        delete _vertexLayout;
    _ownVertexLayout = false;
    _vertexLayout    = vertexLayout;
}

void ProgramState::setTexture(backend::TextureBackend* texture)
{
    for (int slot = 0; slot < texture->getCount() && slot < AX_META_TEXTURES; ++slot)
    {
        auto location = getUniformLocation((backend::Uniform)(backend::Uniform::TEXTURE + slot));
        setTexture(location, slot, slot, texture);
    }
}

void ProgramState::setTexture(const backend::UniformLocation& uniformLocation,
                              int slot,
                              backend::TextureBackend* texture)
{
    setTexture(uniformLocation, slot, 0, texture);
}

void ProgramState::setTexture(const backend::UniformLocation& uniformLocation,
                              int slot,
                              int index,
                              backend::TextureBackend* texture)
{
    if (uniformLocation.vertStage)
        setTexture(uniformLocation.vertStage.location, slot, index, texture, _vertexTextureInfos);
#ifdef AX_USE_METAL
    if (uniformLocation.fragStage)
        setTexture(uniformLocation.fragStage.location, slot, index, texture, _fragmentTextureInfos);
#endif
}

void ProgramState::setTextureArray(const backend::UniformLocation& uniformLocation,
                                   std::vector<int> slots,
                                   std::vector<backend::TextureBackend*> textures)
{
    if (uniformLocation.vertStage)
        setTextureArray(uniformLocation.vertStage.location, std::move(slots), std::move(textures), _vertexTextureInfos);
#ifdef AX_USE_METAL
    if (uniformLocation.fragStage)
        setTextureArray(uniformLocation.fragStage.location, std::move(slots), std::move(textures), _fragmentTextureInfos);
#endif
}

void ProgramState::setTexture(int location,
                              int slot,
                              int index,
                              backend::TextureBackend* texture,
                              std::unordered_map<int, TextureInfo>& textureInfo)
{
    if (location < 0)
        return;

    auto& info = textureInfo[location];
    info.assign(slot, index, texture);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    info.location = location;
#endif
}

void ProgramState::setTextureArray(int location,
                                   std::vector<int> slots,
                                   std::vector<backend::TextureBackend*> textures,
                                   std::unordered_map<int, TextureInfo>& textureInfo)
{
    assert(slots.size() == textures.size());

    auto& info = textureInfo[location];
    info       = {std::move(slots), std::move(textures)};

#if AX_ENABLE_CACHE_TEXTURE_DATA
    info.location = location;
#endif
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

const char* ProgramState::getVertexUniformBuffer(std::size_t& size) const
{
    size = _vertexUniformBufferSize;
    return _uniformBuffers.data();
}

const char* ProgramState::getFragmentUniformBuffer(std::size_t& size) const
{
    size = _fragmentUniformBufferSize;
    return _uniformBuffers.data() + _vertexUniformBufferSize;
}

NS_AX_BACKEND_END
