/****************************************************************************
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
#include "axmol/rhi/SamplerCache.h"
#include "axmol/rhi/DriverBase.h"
#include "yasio/singleton.hpp"

namespace ax::rhi
{
SamplerCache* SamplerCache::getInstance()
{
    return yasio::singleton<SamplerCache>::instance();
}
void SamplerCache::destroyInstance()
{
    yasio::singleton<SamplerCache>::destroy();
}

SamplerCache::SamplerCache()
{
    _driver = DriverBase::getInstance();

    createBuiltinSamplers();
}

SamplerCache::~SamplerCache()
{
    removeAllSamplers();
}

void SamplerCache::invalidateAll()
{
    removeAllSamplers();

    createBuiltinSamplers();
}

void SamplerCache::removeAllSamplers()
{

    for (auto& [_, sampler] : _customSamplers)
        _driver->destroySampler(sampler);
    for (auto sampler : _builtinSamplers)
        _driver->destroySampler(sampler);

    _customSamplers.clear();
    _builtinSamplers.clear();
    _samplersRegsitry.clear();

    _nextSamplerIndex = 0;
}

SamplerIndex::enum_type SamplerCache::registerSampler(const SamplerDesc& desc)
{
    if (_nextSamplerIndex >= MAX_SAMPLER_COUNT)
    {
        throw std::runtime_error("sampler registry is full");
    }

    auto key = *reinterpret_cast<const uint32_t*>(&desc);
    auto it  = _samplersRegsitry.find(key);
    if (it != _samplersRegsitry.end())
        return static_cast<SamplerIndex::enum_type>(it->second);

    auto samplerIndex  = _nextSamplerIndex++;
    auto samplerHandle = _driver->createSampler(desc);
    _customSamplers.emplace(static_cast<SamplerIndex::enum_type>(samplerIndex), samplerHandle);

    _samplersRegsitry.emplace(key, samplerIndex);

    return static_cast<SamplerIndex::enum_type>(samplerIndex);
}

SamplerHandle SamplerCache::getSampler(const SamplerDesc& desc)
{
    const auto samplerIndex = registerSampler(desc);
    return getSampler(samplerIndex);
}

SamplerHandle SamplerCache::getSampler(SamplerIndex::enum_type samplerIndex)
{
    if (samplerIndex < _builtinSamplers.size())
        return _builtinSamplers[samplerIndex];

    auto it = _customSamplers.find(samplerIndex);
    if (it != _customSamplers.end())
        return it->second;

    AXLOGE("The custom sampler(index: {}) not register, please register first!", (int)samplerIndex);
    AXASSERT(false, "The custom sampler not register, please register first!");
    return nullptr;
}

void SamplerCache::createBuiltinSamplers()
{
    _builtinSamplers.resize(SamplerIndex::Count);

    // --- Linear sampling ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_LINEAR;
        d.magFilter = SamplerFilter::MAG_LINEAR;
        d.mipFilter = SamplerFilter::MIP_DEFAULT;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::LinearClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        createBuiltinSampler(SamplerIndex::LinearWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        createBuiltinSampler(SamplerIndex::LinearMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        createBuiltinSampler(SamplerIndex::LinearBorder, d);
    }

    // --- Point sampling ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_NEAREST;
        d.magFilter = SamplerFilter::MAG_NEAREST;
        d.mipFilter = SamplerFilter::MIP_DEFAULT;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::PointClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        createBuiltinSampler(SamplerIndex::PointWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        createBuiltinSampler(SamplerIndex::PointMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        createBuiltinSampler(SamplerIndex::PointBorder, d);
    }

    // --- Linear + Mipmap ---
    {
        SamplerDesc d{};
        d.minFilter = SamplerFilter::MIN_LINEAR;
        d.magFilter = SamplerFilter::MAG_LINEAR;
        d.mipFilter = SamplerFilter::MIP_LINEAR;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        d.tAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::LinearMipClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        d.tAddressMode = SamplerAddressMode::REPEAT;
        createBuiltinSampler(SamplerIndex::LinearMipWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        d.tAddressMode = SamplerAddressMode::MIRROR;
        createBuiltinSampler(SamplerIndex::LinearMipMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        d.tAddressMode = SamplerAddressMode::BORDER;
        createBuiltinSampler(SamplerIndex::LinearMipBorder, d);
    }

    // --- Anisotropic filtering ---
    {
        SamplerDesc d{};
        d.minFilter  = SamplerFilter::MIN_ANISOTROPIC;
        d.magFilter  = SamplerFilter::MAG_LINEAR;
        d.mipFilter  = SamplerFilter::MIP_LINEAR;
        d.anisotropy = 16;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::AnisoClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        createBuiltinSampler(SamplerIndex::AnisoWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        createBuiltinSampler(SamplerIndex::AnisoMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        createBuiltinSampler(SamplerIndex::AnisoBorder, d);
    }

    // --- Depth comparison samplers ---
    {
        SamplerDesc d{};
        d.minFilter   = SamplerFilter::MIN_LINEAR;
        d.magFilter   = SamplerFilter::MAG_LINEAR;
        d.mipFilter   = SamplerFilter::MIP_DEFAULT;
        d.compareFunc = CompareFunc::LESS;

        d.sAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::ShadowCmpClamp, d);

        d.sAddressMode = SamplerAddressMode::REPEAT;
        createBuiltinSampler(SamplerIndex::ShadowCmpWrap, d);

        d.sAddressMode = SamplerAddressMode::MIRROR;
        createBuiltinSampler(SamplerIndex::ShadowCmpMirror, d);

        d.sAddressMode = SamplerAddressMode::BORDER;
        createBuiltinSampler(SamplerIndex::ShadowCmpBorder, d);
    }

    // --- Special cases ---
    {
        SamplerDesc d{};
        d.minFilter    = SamplerFilter::MIN_LINEAR;
        d.magFilter    = SamplerFilter::MAG_LINEAR;
        d.mipFilter    = SamplerFilter::MIP_NEAREST;  // no mip
        d.sAddressMode = SamplerAddressMode::CLAMP;
        createBuiltinSampler(SamplerIndex::LinearNoMipClamp, d);

        d.minFilter = SamplerFilter::MIN_NEAREST;
        d.magFilter = SamplerFilter::MAG_NEAREST;
        createBuiltinSampler(SamplerIndex::PointNoMipClamp, d);
    }

    _nextSamplerIndex = static_cast<uint32_t>(_builtinSamplers.size());
}

void SamplerCache::createBuiltinSampler(uint32_t samplerIndex, const SamplerDesc& desc)
{
    _builtinSamplers[samplerIndex] = _driver->createSampler(desc);

    auto key = *reinterpret_cast<const uint32_t*>(&desc);
    _samplersRegsitry.emplace(key, samplerIndex);
}

}  // namespace ax::rhi
