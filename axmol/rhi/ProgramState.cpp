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

TextureBindingInfo::TextureBindingInfo(TextureBindingInfo&& other)
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

void TextureBindingInfo::reset(int location_, int slot_, rhi::Texture* tex_)
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

    _vertexLayout             = program->getVertexLayout();
    _ownVertexLayout          = false;
    _vertexUniformBufferSize  = _program->getUniformBufferSize(ShaderStage::VERTEX);

#if AX_RENDER_API == AX_RENDER_API_MTL || AX_RENDER_API == AX_RENDER_API_D3D
    _fragmentUniformBufferSize = _program->getUniformBufferSize(ShaderStage::FRAGMENT);
#endif

    _uniformBuffers.resize((std::max)(_vertexUniformBufferSize + _fragmentUniformBufferSize, (size_t)1), 0);

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
    _batchId = XXH64(_uniformBuffers.data(), _uniformBuffers.size(), _program->getProgramId());
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

    if (_ownVertexLayout)
        AX_SAFE_DELETE(_vertexLayout);
}

ProgramState* ProgramState::clone() const
{
    ProgramState* cp          = new ProgramState(_program);
    cp->_textureBindingInfos  = _textureBindingInfos;
    cp->_uniformBuffers       = _uniformBuffers;

    cp->_ownVertexLayout = _ownVertexLayout;
    cp->_vertexLayout    = !_ownVertexLayout ? _vertexLayout : _vertexLayout->clone(); // OPTIMIZE ME: make VertexLayout inherit from ax::Object, and just retain

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
    if (uniformLocation.vertStage)
        setVertexUniform(uniformLocation.vertStage.location, data, size, uniformLocation.vertStage.offset);
#if AX_RENDER_API == AX_RENDER_API_MTL || AX_RENDER_API == AX_RENDER_API_D3D
    if (uniformLocation.fragStage)
        setFragmentUniform(uniformLocation.fragStage.location, data, size, uniformLocation.fragStage.offset);
#endif
}

void ProgramState::setVertexUniform(int location, const void* data, std::size_t size, int offset)
{
    if (location < 0 || offset < 0)
        return;

    assert(location + offset + size <= _vertexUniformBufferSize);
    memcpy(_uniformBuffers.data() + location + offset, data, size);
}

#if AX_RENDER_API == AX_RENDER_API_MTL || AX_RENDER_API == AX_RENDER_API_D3D
void ProgramState::setFragmentUniform(int location, const void* data, std::size_t size, int offset)
{
    if (location < 0 || offset < 0)
        return;

    memcpy(_uniformBuffers.data() + _vertexUniformBufferSize + location + offset, data, size);
}
#endif

void ProgramState::validateSharedVertexLayout(VertexLayoutType vlt)
{
    if (!_ownVertexLayout && !_vertexLayout->isValid())
        _program->defineVertexLayout(vlt);
}

void ProgramState::ensureVertexLayoutMutable()
{
    if (!_ownVertexLayout)
    {
        if(_vertexLayout)
            _vertexLayout = _vertexLayout->clone();
        else
            _vertexLayout    = DriverBase::getInstance()->createVertexLayout();
        _ownVertexLayout = true;
    }
}

VertexLayout* ProgramState::getMutableVertexLayout()
{
    auto driver = DriverBase::getInstance();

    if (_ownVertexLayout || !_vertexLayout->isValid())
        return _vertexLayout;

    assert(_vertexLayout);
    _vertexLayout        = _vertexLayout->clone();
    _ownVertexLayout     = true;
    return _vertexLayout;
}

void ProgramState::setSharedVertexLayout(VertexLayout* vertexLayout)
{
    if (_ownVertexLayout)
        delete _vertexLayout;
    _ownVertexLayout = false;
    _vertexLayout    = vertexLayout;
}

void ProgramState::setTexture(rhi::Texture* texture)
{
    auto location = getUniformLocation((rhi::Uniform)(rhi::Uniform::TEXTURE));
    setTexture(location, 0, texture);
}

void ProgramState::setTexture(const rhi::UniformLocation& uniformLocation, int slot, rhi::Texture* texture)
{
    if (uniformLocation.fragStage)
    {
        auto& textureBinding = _textureBindingInfos[uniformLocation.fragStage.location];
        textureBinding.reset(uniformLocation.fragStage.location, slot, texture);
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

}
