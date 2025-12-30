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
#include "axmol/rhi/d3d11/Driver11.h"
#include "axmol/rhi/d3d11/RenderContext11.h"
#include "axmol/rhi/d3d11/Buffer11.h"
#include "axmol/rhi/d3d11/Texture11.h"
#include "axmol/rhi/d3d11/Program11.h"
#include "axmol/rhi/d3d11/ShaderModule11.h"
#include "axmol/rhi/d3d11/RenderTarget11.h"
#include "axmol/rhi/d3d11/RenderPipeline11.h"
#include "axmol/rhi/d3d11/DepthStencilState11.h"
#include "axmol/rhi/d3d11/VertexLayout11.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/base/Logging.h"
#include "axmol/platform/Application.h"
#include "ntcvt/ntcvt.hpp"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

namespace ax::rhi
{

DriverBase* DriverBase::getInstance()
{
    if (!_instance)
    {
        _instance = new d3d11::DriverImpl();
        static_cast<d3d11::DriverImpl*>(_instance)->init();
    }

    return _instance;
}

void DriverBase::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}
}  // namespace ax::rhi

namespace ax::rhi::d3d11
{
namespace
{

static int32_t EstimateMaxTexSize(D3D_FEATURE_LEVEL fl)
{
    switch (fl)
    {
    case D3D_FEATURE_LEVEL_12_1:
    case D3D_FEATURE_LEVEL_12_0:
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
        return 16384;  // guaranteed
    case D3D_FEATURE_LEVEL_10_1:
    case D3D_FEATURE_LEVEL_10_0:
        return 8192;
    case D3D_FEATURE_LEVEL_9_3:
        return 4096;
    default:
        return 2048;  // Oldest device
    }
}

// iterating 1,2,4,8,16,32,64 to find max samples
static uint32_t FindMaxMsaaSamples(ID3D11Device* device, DXGI_FORMAT format)
{
    uint32_t best = 1;
    for (uint32_t s = 2; s <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; s <<= 1)
    {
        UINT quality = 0;
        if (SUCCEEDED(device->CheckMultisampleQualityLevels(format, s, &quality)) && quality > 0)
        {
            best = s;
        }
    }
    return best;
}
}  // namespace

DriverImpl::DriverImpl() {}

void DriverImpl::init()
{
    initializeAdapter();
    initializeDevice();

    HRESULT hr{E_FAIL};
    if (!_dxgiAdapter)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        _AXASSERT_HR(
            hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf())));

        _AXASSERT_HR(hr = dxgiDevice->GetAdapter(&_dxgiAdapter));

        _AXASSERT_HR(
            hr = _dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), (void**)_dxgiFactory.ReleaseAndGetAddressOf()));
    }

    LARGE_INTEGER version;
    hr = _dxgiAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version);
    if (FAILED(hr))
    {
        _driverVersion.reset();
        AXLOGW("Error querying driver version from DXGI Adapter.");
    }
    else
    {
        _driverVersion = version;
    }
    _dxgiAdapter->GetDesc(&_adapterDesc);

    _caps.maxAttributes = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;  // 16

    _caps.maxTextureUnits = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;  // 128

    _caps.maxTextureSize = EstimateMaxTexSize(_device->GetFeatureLevel());

    _caps.maxSamplesAllowed = static_cast<int32_t>(FindMaxMsaaSamples(_device, DXGI_FORMAT_R8G8B8A8_UNORM));
}

DriverImpl::~DriverImpl()
{
    SafeRelease(_context);

    ComPtr<ID3D11Debug> debug;
    _device->QueryInterface(IID_PPV_ARGS(&debug));

    SafeRelease(_device);

    _dxgiAdapter.Reset();
    _dxgiFactory.Reset();

    if (debug)
        debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}

void DriverImpl::initializeDevice()
{
    constexpr UINT releaseFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    constexpr UINT debugFlags   = releaseFlags | D3D11_CREATE_DEVICE_DEBUG;

    HRESULT hr              = E_FAIL;
    const bool isDebugLayer = Application::getContextAttrs().debugLayerEnabled;

    if (isDebugLayer) [[unlikely]]
    {
        hr = createD3DDevice(D3D_DRIVER_TYPE_HARDWARE, debugFlags);
        if (SUCCEEDED(hr))
            return;

        if (hr != DXGI_ERROR_UNSUPPORTED)
        {
            AXLOGI("Failed creating Debug D3D11 device - falling back to release runtime.");
            goto L_ReleaseRuntime;
        }
        else
        {
            goto L_WarpRuntime;
        }
    }

L_ReleaseRuntime:
    hr = createD3DDevice(D3D_DRIVER_TYPE_HARDWARE, releaseFlags);
    if (hr == DXGI_ERROR_UNSUPPORTED) [[unlikely]]
    {
    L_WarpRuntime:
        AXLOGI("Failed creating hardware D3D11 device - falling back to software runtime.");
        // Reset adapter to null before using D3D_DRIVER_TYPE_WARP,
        // otherwise D3D11CreateDevice will return E_INVALIDARG when both
        // a non-null adapter and a non-matching driver type are specified.
        _dxgiAdapter.Reset();
        hr = createD3DDevice(D3D_DRIVER_TYPE_WARP, releaseFlags);
    }

    if (FAILED(hr)) [[unlikely]]
        dxutils::fatalError("initializeDevice"sv, hr);
}

void DriverImpl::initializeAdapter()
{
    auto& contextAttrs          = Application::getContextAttrs();
    const auto powerPreferrence = contextAttrs.powerPreference;

    if (powerPreferrence == PowerPreference::Auto)
        return;

    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&_dxgiFactory)))
        return;

    ComPtr<IDXGIAdapter> bestAdapter;
    int bestScore = std::numeric_limits<int>::min();

    UINT i = 0;
    ComPtr<IDXGIAdapter> adapter;
    while (_dxgiFactory->EnumAdapters(i, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        // Skip Microsoft Basic Render Driver (software adapter)
        if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
        {
            ++i;
            continue;
        }

        int score = 0;

        // 1. Base score by GPU type
        bool isDiscrete = desc.DedicatedVideoMemory > 0;
        if (isDiscrete)
            score += 1000;  // Higher base score for discrete GPU
        else
            score += 500;  // Lower base score for integrated GPU

        // 2. Adjust score based on PowerPreference
        if (powerPreferrence == PowerPreference::HighPerformance && isDiscrete)
            score += 500;
        else if (powerPreferrence == PowerPreference::LowPower && !isDiscrete)
            score += 500;

        // 3. Adjust score based on VRAM size
        if (powerPreferrence == PowerPreference::HighPerformance)
            score += static_cast<int>(desc.DedicatedVideoMemory / (1024 * 1024));  // More VRAM = higher score
        else if (powerPreferrence == PowerPreference::LowPower)
            score -= static_cast<int>(desc.DedicatedVideoMemory / (1024 * 1024));  // Less VRAM = higher score

        // Keep the adapter with the highest score
        if (score > bestScore)
        {
            bestScore   = score;
            bestAdapter = adapter;
        }

        ++i;
    }

    _dxgiAdapter = std::move(bestAdapter);
}

HRESULT DriverImpl::createD3DDevice(int requestDriverType, int createFlags)
{
    if (_dxgiAdapter)
        requestDriverType = D3D_DRIVER_TYPE_UNKNOWN;
    return ::D3D11CreateDevice(_dxgiAdapter.Get(),                  // Adapter
                               (D3D_DRIVER_TYPE)requestDriverType,  // Driver Type
                               nullptr,                             // Software
                               createFlags,                         // Flags
                               DEFAULT_REATURE_LEVELS,              // Feature Levels
                               ARRAYSIZE(DEFAULT_REATURE_LEVELS),   // Num Feature Levels
                               D3D11_SDK_VERSION,                   // SDK Version
                               &_device,                            // Device
                               &_featureLevel,                      // Feature Level
                               &_context);
}

RenderContext* DriverImpl::createRenderContext(SurfaceHandle surface)
{
    return new RenderContextImpl(this, surface);
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial)
{
    return new BufferImpl(_device, _context, size, type, usage, initial);
}

/**
 * New a Texture object.
 * @param descriptor Specifies texture description.
 * @return A Texture object.
 */
Texture* DriverImpl::createTexture(const TextureDesc& descriptor)
{
    return new TextureImpl(_device, descriptor);
}

RenderTarget* DriverImpl::createRenderTarget(Texture* colorAttachment, Texture* depthAttachment)
{
    auto renderTarget = new RenderTargetImpl(_device, false);
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    renderTarget->setColorAttachment(colors);
    renderTarget->setDepthStencilAttachment(depthAttachment);
    return renderTarget;
}

/**
 * New a DepthStencilState object.
 */
DepthStencilState* DriverImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl(_device);
}

/**
 * New a RenderPipeline object.
 * @param descriptor Specifies render pipeline description.
 * @return A RenderPipeline object.
 */
RenderPipeline* DriverImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(_device, _context);
}

Program* DriverImpl::createProgram(Data vsData, Data fsData)
{
    return new ProgramImpl(vsData, fsData);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, Data& chunk)
{
    return new ShaderModuleImpl(_device, stage, chunk);
}

SamplerHandle DriverImpl::createSampler(const SamplerDesc& desc)
{
    D3D11_SAMPLER_DESC sd = {};

    // --- Filter ---
    if (desc.minFilter == SamplerFilter::MIN_ANISOTROPIC)
    {
        sd.Filter        = D3D11_FILTER_ANISOTROPIC;
        sd.MaxAnisotropy = std::clamp(desc.anisotropy + 1u, 1u, 16u);
    }
    else
    {
        const auto minL = ((int)desc.minFilter & (int)SamplerFilter::MIN_LINEAR);
        const auto magL = ((int)desc.magFilter & (int)SamplerFilter::MAG_LINEAR);
        const auto mipL = ((int)desc.mipFilter & (int)SamplerFilter::MIP_LINEAR);

        // minL<<2 | magL<<1 | mipL
        static const D3D11_FILTER filterTable[8] = {
            D3D11_FILTER_MIN_MAG_MIP_POINT,                // 000
            D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,         // 001
            D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,   // 010
            D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR,         // 011
            D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,         // 100
            D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,  // 101
            D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT,         // 110
            D3D11_FILTER_MIN_MAG_MIP_LINEAR                // 111
        };

        const int idx = (minL << 2) | (magL << 1) | (mipL ? 1 : 0);
        sd.Filter     = filterTable[idx];

        sd.MaxAnisotropy = 1;
    }

    // --- Wrap ---
    static const D3D11_TEXTURE_ADDRESS_MODE wrapTbl[4] = {
        D3D11_TEXTURE_ADDRESS_WRAP,    // REPEAT
        D3D11_TEXTURE_ADDRESS_MIRROR,  // MIRROR
        D3D11_TEXTURE_ADDRESS_CLAMP,   // CLAMP
        D3D11_TEXTURE_ADDRESS_BORDER   // BORDER
    };
    sd.AddressU = wrapTbl[static_cast<int>(desc.sAddressMode)];
    sd.AddressV = wrapTbl[static_cast<int>(desc.tAddressMode)];
    sd.AddressW = wrapTbl[static_cast<int>(desc.wAddressMode)];

    // --- Compare ---
    sd.ComparisonFunc = static_cast<D3D11_COMPARISON_FUNC>(D3D11_COMPARISON_NEVER + static_cast<int>(desc.compareFunc));

    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;

    ID3D11SamplerState* sampler{nullptr};
    _device->CreateSamplerState(&sd, &sampler);
    return SamplerHandle(sampler);
}

void DriverImpl::destroySampler(SamplerHandle& h)
{
    auto samplerState = static_cast<ID3D11SamplerState*>(h);
    SafeRelease(samplerState);
    h = nullptr;
}

VertexLayout* DriverImpl::createVertexLayout(VertexLayoutDesc&& desc)
{
    return new VertexLayoutImpl(std::move(desc));
}

/// below is driver info

/// @name Setters & Getters
/**
 * Get vendor device name.
 * @return Vendor device name.
 */
std::string DriverImpl::getVendor() const
{
    return std::string{RHIUtils::vendorToString(_adapterDesc.VendorId)};
}

/**
 * Get the full name of the vendor device.
 * @return The full name of the vendor device.
 */
std::string DriverImpl::getRenderer() const
{
    auto desc = ntcvt::from_chars(_adapterDesc.Description);
    return fmt::format("{} D3D11 vs_5_0 ps_5_0", desc);
}

/**
 * Get featureSet name.
 * @return FeatureSet name.
 */
std::string DriverImpl::getVersion() const
{
    if (_driverVersion.has_value())
    {
        uint64_t intVersion        = static_cast<uint64_t>(_driverVersion->QuadPart);
        constexpr uint64_t kMask16 = std::numeric_limits<uint16_t>::max();

        return fmt::format("D3D11-{}.{}.{}.{}", (intVersion >> 48) & kMask16, (intVersion >> 32) & kMask16,
                           (intVersion >> 16) & kMask16, intVersion & kMask16);
    }
    else
    {
        return "D3D11"s;
    }
}

std::string DriverImpl::getShaderVersion() const
{
    return "D3D11 HLSL vs_5_0 ps_5_0"s;
}

/**
 * Check if feature supported by Metal.
 * @param feature Specify feature to be query.
 * @return true if the feature is supported, false otherwise.
 */
bool DriverImpl::checkForFeatureSupported(FeatureType feature)
{
    // enum class FeatureType : uint32_t
    // {
    //     ETC1,
    //     ETC2,
    //     S3TC,
    //     AMD_COMPRESSED_ATC,
    //     PVRTC,
    //     IMG_FORMAT_BGRA8888,
    //     DISCARD_FRAMEBUFFER,
    //     PACKED_DEPTH_STENCIL,
    //     VAO,
    //     MAPBUFFER,
    //     DEPTH24,
    //     ASTC,
    //     VERTEX_ATTRIB_BINDING
    // };

    switch (feature)
    {
    case FeatureType::VAO:
    case FeatureType::VERTEX_ATTRIB_BINDING:
        return true;
    case FeatureType::DEPTH24:
    case FeatureType::PACKED_DEPTH_STENCIL:
        return checkFormatSupport(DXGI_FORMAT_D24_UNORM_S8_UINT);
    case FeatureType::IMG_FORMAT_BGRA8888:
        return checkFormatSupport(DXGI_FORMAT_B8G8R8A8_UNORM);
    case FeatureType::S3TC:
        return checkFormatSupport(DXGI_FORMAT_BC2_UNORM);
    case FeatureType::ASTC:
#define DXGI_FORMAT_ASTC_4X4_UNORM DXGI_FORMAT(134)
        return checkFormatSupport(DXGI_FORMAT_ASTC_4X4_UNORM);
    }
    return false;
}

bool DriverImpl::checkFormatSupport(DXGI_FORMAT format)
{
    UINT formatSupport = 0;
    HRESULT hr         = _device->CheckFormatSupport(format, &formatSupport);
    return SUCCEEDED(hr) && (formatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D);
}

}  // namespace ax::rhi::d3d11
