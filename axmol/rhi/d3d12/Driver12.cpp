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
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/d3d12/RenderContext12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/ShaderModule12.h"
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/rhi/d3d12/Sampler12.h"
#include "axmol/base/Logging.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/SamplerCache.h"
#include "yasio/sz.hpp"
#include "d3dx12.h"

#include "ntcvt/ntcvt.hpp"

#include <d3dcompiler.h>

#include <algorithm>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace ax::rhi
{
DriverBase* DriverBase::getInstance()
{
    if (!_instance)
    {
        _instance = new d3d12::DriverImpl();
        static_cast<d3d12::DriverImpl*>(_instance)->init();
    }
    return _instance;
}

void DriverBase::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}
}  // namespace ax::rhi

namespace ax::rhi::d3d12
{

static std::string_view kCSGenerateMipsHLSL = R"(
// mipmaps generation compute shader
#ifndef THREADS_X
#define THREADS_X 8
#endif
#ifndef THREADS_Y
#define THREADS_Y 8
#endif

// Macro to switch between Texture2D and Texture2DArray
#ifdef USE_ARRAY
    Texture2DArray<float4> gSrc : register(t0);
    RWTexture2DArray<float4> gDst : register(u0);
#else
    Texture2D<float4> gSrc : register(t0);
    RWTexture2D<float4> gDst : register(u0);
#endif

SamplerState gSampler : register(s0);

cbuffer MipConstants : register(b0)
{
    uint2  SrcDim;      // source mip width/height
    uint2  DstDim;      // destination mip width/height
    float2 InvSrcDim;   // 1.0 / SrcDim
#ifdef USE_ARRAY
    uint   FirstSlice;  // starting slice index
    uint   SliceCount;  // number of slices
#endif
}

[numthreads(THREADS_X, THREADS_Y, 1)]
void main(uint3 tid : SV_DispatchThreadID)
{
    if (tid.x >= DstDim.x || tid.y >= DstDim.y)
        return;

#ifdef USE_ARRAY
    if (tid.z >= SliceCount)
        return;
    uint slice = FirstSlice + tid.z;
#endif

    // Compute base UV in source mip
    float2 srcUVBase = (float2(tid.xy) * 2.0f + 0.5f) * InvSrcDim;

#ifdef USE_ARRAY
    float4 c0 = gSrc.SampleLevel(gSampler, float3(srcUVBase + float2(0,0) * InvSrcDim, slice), 0);
    float4 c1 = gSrc.SampleLevel(gSampler, float3(srcUVBase + float2(1,0) * InvSrcDim, slice), 0);
    float4 c2 = gSrc.SampleLevel(gSampler, float3(srcUVBase + float2(0,1) * InvSrcDim, slice), 0);
    float4 c3 = gSrc.SampleLevel(gSampler, float3(srcUVBase + float2(1,1) * InvSrcDim, slice), 0);

    gDst[uint3(tid.xy, slice)] = (c0 + c1 + c2 + c3) * 0.25f;
#else
    float4 c0 = gSrc.SampleLevel(gSampler, srcUVBase + float2(0,0) * InvSrcDim, 0);
    float4 c1 = gSrc.SampleLevel(gSampler, srcUVBase + float2(1,0) * InvSrcDim, 0);
    float4 c2 = gSrc.SampleLevel(gSampler, srcUVBase + float2(0,1) * InvSrcDim, 0);
    float4 c3 = gSrc.SampleLevel(gSampler, srcUVBase + float2(1,1) * InvSrcDim, 0);

    gDst[tid.xy] = (c0 + c1 + c2 + c3) * 0.25f;
#endif
}
)";

static inline bool checkHR(HRESULT hr, const char* msg = nullptr)
{
    if (FAILED(hr))
    {
        if (msg)
            AXLOGE("D3D12 ERROR: {} hr=0x{:X}", msg, hr);
        else
            AXLOGE("D3D12 ERROR: hr=0x{:X}", hr);

        AXASSERT(false, "D3D12 call failed!");
        return false;
    }
    return true;
}

static int evalulateMaxMsaaSamples(ID3D12Device* device, DXGI_FORMAT format)
{
    uint32_t best = 1;
    for (UINT sampleCount = 2; sampleCount <= D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT; sampleCount *= 2)
    {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels = {};
        qualityLevels.Format                                        = DXGI_FORMAT_R8G8B8A8_UNORM;
        qualityLevels.SampleCount                                   = sampleCount;
        qualityLevels.Flags                                         = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;

        if (SUCCEEDED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels,
                                                  sizeof(qualityLevels))))
        {
            if (qualityLevels.NumQualityLevels > 0)
                best = sampleCount;
        }
    }
    return static_cast<int>(best);
}

static inline std::wstring_view stageToProfileDXC(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return L"vs_6_0"sv;
    case ShaderStage::FRAGMENT:
        return L"ps_6_0"sv;
    case ShaderStage::COMPUTE:
        return L"cs_6_0"sv;
    default:
        return L"vs_6_0"sv;
    }
}

static inline const char* stageToProfileFXC(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return "vs_5_1";
    case ShaderStage::FRAGMENT:
        return "ps_5_1";
    case ShaderStage::COMPUTE:
        return "cs_5_1";
    default:
        return "vs_5_1";
    }
}

static constexpr auto kUploadQueueType = D3D12_COMMAND_LIST_TYPE_DIRECT;

void IsolateSubmission::create(ID3D12Device* device)
{
    // Create allocator
    device->CreateCommandAllocator(kUploadQueueType, IID_PPV_ARGS(&allocator));

    // Create command list
    device->CreateCommandList(0, kUploadQueueType, allocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));

    // Command lists are created in "open" state; ready for record.
    // Create (or reuse) a fence for this isolated submission
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fenceValue = 1;
}

void IsolateSubmission::reset()
{
    allocator->Reset();
    cmdList->Reset(allocator.Get(), nullptr);
    ++fenceValue;
}

uint64_t IsolateSubmission::submit(ComPtr<ID3D12CommandQueue>& queue)
{
    // Close list
    cmdList->Close();

    // Execute on same graphics queue
    ID3D12CommandList* lists[] = {cmdList.Get()};
    queue->ExecuteCommandLists(1, lists);

    // Signal fence
    queue->Signal(fence.Get(), fenceValue);

    return fenceValue;
}

void IsolateSubmission::waitGPU()
{

    if (fence->GetCompletedValue() < fenceValue)
    {
        if (!event)
            event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        fence->SetEventOnCompletion(fenceValue, event);
        WaitForSingleObject(event, INFINITE);
    }
}

void IsolateSubmission::release()
{
    waitGPU();

    fence.Reset();
    cmdList.Reset();
    allocator.Reset();

    if (event)
    {
        CloseHandle(event);
        event = nullptr;
    }
}

DriverImpl::DriverImpl() {}
DriverImpl::~DriverImpl()
{
    AX_SAFE_RELEASE_NULL(_currentRenderContext);

    cleanPendingResources();

    _srvAllocator.reset();
    _rtvAllocator.reset();
    _dsvAllocator.reset();
    _samplerAllocator.reset();

    _mipmapSrvHeap.Reset();
    _mipmapRootSig.Reset();
    _mipmapPSO2D.Reset();
    _mipmapPSOArray.Reset();

    _dxcLibrary.Reset();
    _dxcCompiler.Reset();
    _dxcArguments.clear();

    _isolateSubmission.release();

    _uploadQueue.Reset();
    _gfxQueue.Reset();

    _uploadBufferAllocator.reset();

    ComPtr<ID3D12DebugDevice> debugDevice;
    _device->QueryInterface(IID_PPV_ARGS(&debugDevice));

    _device.Reset();
    _adapter.Reset();
    _dxgiFactory.Reset();

    SafeRelease(_idleFence);
    if (_idleEvent)
    {
        CloseHandle(_idleEvent);
        _idleEvent = nullptr;
    }

    if (debugDevice)
        debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
}

void DriverImpl::init()
{
    initializeDevice();
    createDescriptorAllocators();

    // check device feature level
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
                                         D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {};
    featLevels.NumFeatureLevels                  = _countof(featureLevels);
    featLevels.pFeatureLevelsRequested           = featureLevels;

    HRESULT hr = _device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));

    if (SUCCEEDED(hr))
    {
        _featureLevel = featLevels.MaxSupportedFeatureLevel;
    }

    AXLOGI("D3D12 Feature level: 0x{:04x}", static_cast<int>(_featureLevel));

    // adapter version
    LARGE_INTEGER version;
    hr = _adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version);
    if (SUCCEEDED(hr))
    {
        _driverVersion = version;
    }
    else
    {
        _driverVersion.reset();
        AXLOGW("Error querying driver version from DXGI Adapter.");
    }

    // adapter desc
    _adapter->GetDesc(&_adapterDesc);

    // caps
    _caps.maxAttributes   = D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;     // 32
    _caps.maxTextureUnits = D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;  // 128
    _caps.maxTextureSize  = dxutils::evalulateMaxTexSize(_featureLevel);

    _caps.maxSamplesAllowed = evalulateMaxMsaaSamples(_device.Get(), DXGI_FORMAT_R8G8B8A8_UNORM);

    // descriptor stdie
    _srvDescriptorStride     = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _samplerDescriptorStride = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    // sync objects
    _idleEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_idleFence));
    AXASSERT(_idleEvent && _idleFence, "Create sync objects failed");

    // init DXC instances once
    DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&_dxcLibrary));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_dxcCompiler));

#if defined(NDEBUG)
    // Release build arguments
    _dxcArguments = {L"-O2", L"-Qstrip_debug"};
#else
    // Debug build arguments
    _dxcArguments = {L"-Zi", L"-Od"};
#endif

    _dxcAvailable = detectDXCAvailability();
}

void DriverImpl::initializeDevice()
{
    // Create factory
    auto& contextAttrs = Application::getContextAttrs();
    UINT createFlags   = 0;

    if (contextAttrs.debugLayerEnabled)
    {
        // Enable debug layer if available
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            createFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }

    HRESULT hr = CreateDXGIFactory2(createFlags, IID_PPV_ARGS(&_dxgiFactory));
    AXASSERT(SUCCEEDED(hr), "CreateDXGIFactory2 failed");

    // Enum adapter and create physical device
    const auto powerPreferrence = contextAttrs.powerPreference;
    DXGI_GPU_PREFERENCE gpuPref = DXGI_GPU_PREFERENCE_UNSPECIFIED;
    if (powerPreferrence == PowerPreference::HighPerformance)
        gpuPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    else if (powerPreferrence == PowerPreference::LowPower)
        gpuPref = DXGI_GPU_PREFERENCE_MINIMUM_POWER;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;
    hr = _dxgiFactory->QueryInterface(IID_PPV_ARGS(&factory6));
    if (SUCCEEDED(hr))
    {
        // IDXGIFactory6 is not availablee on all versions of windows 10, If it is available, use it
        // to enumerate the adapters based on the desired power preference.
        for (uint32_t adapterIndex = 0;; ++adapterIndex)
        {
            hr = factory6->EnumAdapterByGpuPreference(adapterIndex, gpuPref,
                                                      IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()));
            if (FAILED(hr))  // when start with Visual Studio graphics debugging, will fail
                break;
            hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
            if (SUCCEEDED(hr))
            {
                _adapter = std::move(adapter);
                break;
            }
        }
    }
    else
    {  // fallback: win10 lower versions
        std::vector<std::pair<int, ComPtr<IDXGIAdapter1>>> adapters;
        uint32_t adapterIndex{0};
        for (uint32_t adapterIndex = 0;; ++adapterIndex)
        {
            hr = _dxgiFactory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf());
            if (FAILED(hr))
                break;

            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);

            int score = 0;

            // Skip Microsoft Basic Render Driver (software adapter)
            if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
                continue;

            // 1. Base score by GPU type
            bool isDiscrete = desc.DedicatedVideoMemory > 0;
            if (isDiscrete)
                score += 1000;  // Higher base score for discrete GPU
            else
                score += 500;  // Lower base score for integrated GPU

            score += static_cast<int>(desc.DedicatedVideoMemory / (1024 * 1024));

            adapters.emplace_back(score, adapter);
        }

        if (!adapters.empty())
        {
            if (powerPreferrence == PowerPreference::HighPerformance)
            {
                std::stable_sort(adapters.begin(), adapters.end(),
                                 [](auto& lhs, auto& rhs) { return lhs.first > rhs.first; });
            }
            else if (powerPreferrence == PowerPreference::LowPower)
            {
                std::stable_sort(adapters.begin(), adapters.end(),
                                 [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
            }

            for (auto& [_, adapter] : adapters)
            {
                hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
                if (SUCCEEDED(hr))
                {
                    _adapter = std::move(adapter);
                    break;
                }
            }
        }
    }

    if (!_adapter)
    {
        _AXASSERT_HR(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device)));
        _AXASSERT_HR(_dxgiFactory->EnumAdapterByLuid(_device->GetAdapterLuid(), IID_PPV_ARGS(&_adapter)));
    }

    // Create graphics queue
    D3D12_COMMAND_QUEUE_DESC qdesc{};
    qdesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qdesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    qdesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qdesc.NodeMask = 0;
    hr             = _device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&_gfxQueue));
    AXASSERT(SUCCEEDED(hr), "CreateCommandQueue failed");

    // Create upload (copy) queue
    qdesc.Type = kUploadQueueType;
    hr         = _device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&_uploadQueue));
    AXASSERT(SUCCEEDED(hr), "CreateCommandQueue failed");

    const auto uploadBufferSize = (std::max)(8 * 1024 * 1024u, contextAttrs.uploadBufferSize);
    _uploadBufferAllocator      = std::make_unique<UploadBufferAllocator>(_device.Get(), uploadBufferSize);
}

void DriverImpl::createDescriptorAllocators()
{
    _srvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8192u, false);
    _samplerAllocator = std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
                                                                  SamplerCache::MAX_SAMPLER_COUNT, true);
    _rtvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024u, false);
    _dsvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 512u, false);

    _samplerAllocator->setAllowGrow(false);
}

RenderContext* DriverImpl::createRenderContext(SurfaceHandle surface)
{
    auto context = new RenderContextImpl(this, surface);
    Object::assign(_currentRenderContext, context);
    return context;
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial)
{
    return new BufferImpl(this, size, type, usage, initial);
}

Texture* DriverImpl::createTexture(const TextureDesc& descriptor)
{
    return new TextureImpl(this, descriptor);
}

RenderTarget* DriverImpl::createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment)
{
    auto rt = new RenderTargetImpl(this, false);
    rt->setColorTexture(colorAttachment);
    rt->setDepthStencilTexture(depthStencilAttachment);
    return rt;
}

DepthStencilState* DriverImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl();
}

RenderPipeline* DriverImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(this);
}

Program* DriverImpl::createProgram(Data vsData, Data fsData)
{
    return new ProgramImpl(vsData, fsData);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, Data& chunk)
{
    return new ShaderModuleImpl(this, stage, chunk);
}

SamplerHandle DriverImpl::createSampler(const SamplerDesc& desc)
{
    static constexpr D3D12_FILTER kFilterTable[8] = {D3D12_FILTER_MIN_MAG_MIP_POINT,
                                                     D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,
                                                     D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
                                                     D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,
                                                     D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT,
                                                     D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
                                                     D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
                                                     D3D12_FILTER_MIN_MAG_MIP_LINEAR};

    static constexpr D3D12_FILTER kCmpFilterTable[8] = {D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
                                                        D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
                                                        D3D12_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
                                                        D3D12_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
                                                        D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
                                                        D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
                                                        D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
                                                        D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR};

    static constexpr D3D12_TEXTURE_ADDRESS_MODE kWrapTbl[4] = {
        D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_MIRROR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER};

    static constexpr D3D12_COMPARISON_FUNC kCompareFunTbl[] = {
        D3D12_COMPARISON_FUNC_NEVER,          // CompareFunc::NEVER
        D3D12_COMPARISON_FUNC_LESS,           // CompareFunc::LESS
        D3D12_COMPARISON_FUNC_EQUAL,          // CompareFunc::EQUAL
        D3D12_COMPARISON_FUNC_LESS_EQUAL,     // CompareFunc::LESS_EQUAL
        D3D12_COMPARISON_FUNC_GREATER,        // CompareFunc::GREATER
        D3D12_COMPARISON_FUNC_NOT_EQUAL,      // CompareFunc::NOT_EQUAL
        D3D12_COMPARISON_FUNC_GREATER_EQUAL,  // CompareFunc::GREATER_EQUAL
        D3D12_COMPARISON_FUNC_ALWAYS          // CompareFunc::ALWAYS
    };

    D3D12_SAMPLER_DESC sd = {};

    // --- Filter ---
    if (desc.minFilter == SamplerFilter::MIN_ANISOTROPIC)
    {
        if (desc.compareFunc != CompareFunc::NEVER)
            sd.Filter = D3D12_FILTER_COMPARISON_ANISOTROPIC;
        else
            sd.Filter = D3D12_FILTER_ANISOTROPIC;

        sd.MaxAnisotropy = std::clamp(desc.anisotropy + 1u, 1u, 16u);
    }
    else
    {
        const auto minL = ((int)desc.minFilter & (int)SamplerFilter::MIN_LINEAR);
        const auto magL = ((int)desc.magFilter & (int)SamplerFilter::MAG_LINEAR);
        const auto mipL = ((int)desc.mipFilter & (int)SamplerFilter::MIP_LINEAR);

        const int idx = (minL << 2) | (magL << 1) | (mipL ? 1 : 0);

        if (desc.compareFunc != CompareFunc::NEVER)
            sd.Filter = kCmpFilterTable[idx];
        else
            sd.Filter = kFilterTable[idx];

        sd.MaxAnisotropy = 1;
    }

    // --- Wrap ---
    sd.AddressU = kWrapTbl[(int)desc.sAddressMode];
    sd.AddressV = kWrapTbl[(int)desc.tAddressMode];
    sd.AddressW = kWrapTbl[(int)desc.wAddressMode];

    // --- Compare ---
    sd.ComparisonFunc = kCompareFunTbl[(UINT)desc.compareFunc];

    sd.MinLOD         = 0.0f;
    sd.MaxLOD         = D3D12_FLOAT32_MAX;
    sd.MipLODBias     = 0.0f;
    sd.BorderColor[0] = sd.BorderColor[1] = sd.BorderColor[2] = sd.BorderColor[3] = 0.0f;

    auto handle = allocateDescriptor(DisposableResource::Type::SamplerView);
    _device->CreateSampler(&sd, handle->cpu);

    return SamplerHandle(handle);
}

void DriverImpl::destroySampler(SamplerHandle& h)
{
    deallocateDescriptor(static_cast<DescriptorHandle*>(h), DisposableResource::Type::SamplerView);
    h = nullptr;
}

DescriptorHandle* DriverImpl::createSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
{
    // Allocate descriptor from SRV heap
    DescriptorHandle* handle = allocateDescriptor(DisposableResource::Type::ShaderResourceView);
    AXASSERT(handle, "Failed to allocate SRV descriptor");

    // Create SRV into CPU handle
    _device->CreateShaderResourceView(resource, desc, handle->cpu);
    return handle;
}

DescriptorHandle* DriverImpl::createUAV(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
{
    // Allocate descriptor from SRV/UAV heap (same allocator as SRV)
    DescriptorHandle* handle = allocateDescriptor(DisposableResource::Type::ShaderResourceView);
    AXASSERT(handle, "Failed to allocate UAV descriptor");

    // Create UAV into CPU handle
    _device->CreateUnorderedAccessView(resource, nullptr, desc, handle->cpu);
    return handle;
}

DescriptorHandle* DriverImpl::allocateDescriptor(DisposableResource::Type type)
{
    switch (type)
    {
    case DisposableResource::Type::ShaderResourceView:
        return _srvAllocator->allocate();
    case DisposableResource::Type::SamplerView:
        return _samplerAllocator->allocate();
    case DisposableResource::Type::RenderTargetView:
        return _rtvAllocator->allocate();
    case DisposableResource::Type::DepthStencilView:
        return _dsvAllocator->allocate();
    default:
        return nullptr;
    }
}

void DriverImpl::deallocateDescriptor(DescriptorHandle* handle, DisposableResource::Type type)
{
    switch (type)
    {
    case DisposableResource::Type::ShaderResourceView:
        _srvAllocator->deallocate(handle);
        break;
    case DisposableResource::Type::SamplerView:
        _samplerAllocator->deallocate(handle);
        break;
    case DisposableResource::Type::RenderTargetView:
        _rtvAllocator->deallocate(handle);
        break;
    case DisposableResource::Type::DepthStencilView:
        _dsvAllocator->deallocate(handle);
        break;
    default:
        assert(false);
        break;
    }
}

VertexLayout* DriverImpl::createVertexLayout(VertexLayoutDesc&& desc)
{
    return new VertexLayoutImpl(std::move(desc));
}

std::string DriverImpl::getVendor() const
{
    return std::string{RHIUtils::vendorToString(_adapterDesc.VendorId)};
}

std::string DriverImpl::getVersion() const
{
    if (_driverVersion.has_value())
    {
        uint64_t intVersion        = static_cast<uint64_t>(_driverVersion->QuadPart);
        constexpr uint64_t kMask16 = std::numeric_limits<uint16_t>::max();

        return fmt::format("D3D12-{}.{}.{}.{}", (intVersion >> 48) & kMask16, (intVersion >> 32) & kMask16,
                           (intVersion >> 16) & kMask16, intVersion & kMask16);
    }
    else
    {
        return "D3D12"s;
    }
}

std::string DriverImpl::getRenderer() const
{
    auto desc = ntcvt::from_chars(_adapterDesc.Description);
    if (_dxcAvailable)
        return fmt::format("{} D3D12 DXC SM6.0", desc);
    else
        return fmt::format("{} D3D12 vs_5_1 ps_5_1", desc);
}

std::string DriverImpl::getShaderVersion() const
{
    if (_dxcAvailable)
        return "HLSL Shader Model 6.0 (DXC)"s;
    else
        return "D3D12 HLSL vs_5_1 ps_5_1"s;
}

IsolateSubmission& DriverImpl::startIsolateSubmission()
{
    if (_isolateSubmission)
    {  // reuse isolate submission
        _isolateSubmission.waitGPU();

        // Reset allocator and command list
        _isolateSubmission.reset();
    }
    else
    {
        _isolateSubmission.create(_device.Get());
    }

    return _isolateSubmission;
}

uint64_t DriverImpl::finishIsolateSubmission(IsolateSubmission& submission, bool waitForCompletion)
{
    if (!submission)
        return 0;

    auto fenceValue = submission.submit(_uploadQueue);
    if (waitForCompletion)
        submission.waitGPU();

    return fenceValue;
}

void DriverImpl::queueDisposal(ID3D12Resource* res, uint64_t fenceValue)
{
    queueDisposalInternal(
        DisposableResource{.type = DisposableResource::Type::Resource, .fenceValue = fenceValue, .resource = res});
}

void DriverImpl::queueDisposal(DescriptorHandle* handle, DisposableResource::Type type, uint64_t fenceValue)
{
    queueDisposalInternal(DisposableResource{.type = type, .fenceValue = fenceValue, .handle = handle});
}

void DriverImpl::processDisposalQueue(uint64_t completeFence)
{
    if (!_disposalQueue.empty())
    {
        for (size_t i = 0; i < _disposalQueue.size();)
        {
            auto& res = _disposalQueue[i];
            if (res.fenceValue <= completeFence)
            {
                if (res.type == DisposableResource::Type::Resource)
                {
                    SafeRelease(res.resource);
                }
                else
                {
                    deallocateDescriptor(res.handle, res.type);
                }

                _disposalQueue[i] = _disposalQueue.back();
                _disposalQueue.pop_back();
            }
            else
            {
                ++i;
            }
        }
    }
}

void DriverImpl::cleanPendingResources()
{
    waitForGPU();
    processDisposalQueue(UINT64_MAX);
}

void DriverImpl::queueDisposalInternal(DisposableResource&& disposal)
{
    std::lock_guard<std::mutex> lk(_disposalMutex);
    _disposalQueue.emplace_back(std::move(disposal));
}

bool DriverImpl::compileShader(std::span<uint8_t> shaderCode, ShaderStage stage, D3D12BlobHandle& outHandle)
{
    if (stage == ShaderStage::FRAGMENT)
    {
        _shaderCompileBuffer.clear();
        _shaderCompileBuffer.reserve(shaderCode.size() + BuiltinSamplers.size());
        _shaderCompileBuffer += BuiltinSamplers;
        _shaderCompileBuffer += shaderCode;
        shaderCode = _shaderCompileBuffer;
    }
    if (_dxcAvailable)
    {
        ComPtr<IDxcBlobEncoding> sourceBlob;
        _dxcLibrary->CreateBlobWithEncodingOnHeapCopy(shaderCode.data(), static_cast<UINT32>(shaderCode.size()),
                                                      CP_UTF8, &sourceBlob);

        std::wstring_view entryPoint = L"main";
        std::wstring_view profile    = stageToProfileDXC(stage);

        ComPtr<IDxcOperationResult> result;
        HRESULT hr = _dxcCompiler->Compile(sourceBlob.Get(),
                                           nullptr,            // source file name
                                           entryPoint.data(),  // entry point
                                           profile.data(),     // target profile
                                           _dxcArguments.data(), (UINT)_dxcArguments.size(), nullptr, 0,  // defines
                                           nullptr,  // include handler
                                           &result);

        if (FAILED(hr))
        {
            AXLOGE("axmol:ERROR: DXC compile failed, hr:{}", hr);
            AXASSERT(false, "Shader compile failed!");
            return false;
        }

        HRESULT status;
        result->GetStatus(&status);
        if (FAILED(status))
        {
            ComPtr<IDxcBlobEncoding> errors;
            result->GetErrorBuffer(&errors);
            std::string_view errorDetail =
                errors ? std::string_view((const char*)errors->GetBufferPointer(), errors->GetBufferSize())
                       : "Unknown compile error"sv;
            AXLOGE("axmol:ERROR: Failed to compile shader, hr:{},{}", status, errorDetail);
            AXASSERT(false, "Shader compile failed!");
            return false;
        }

        ComPtr<IDxcBlob> blob;
        result->GetResult(&blob);

        outHandle.blob = blob;
        outHandle.view = std::span<uint8_t>((uint8_t*)blob->GetBufferPointer(), blob->GetBufferSize());

        return true;
    }
    else
    {
        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> errorBlob;
        UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_ENABLE_STRICTNESS;
#if !defined(NDEBUG)
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
        HRESULT hr = D3DCompile(shaderCode.data(), shaderCode.size(), nullptr, nullptr, nullptr, "main",
                                stageToProfileFXC(stage), flags, 0, &blob, &errorBlob);
        if (SUCCEEDED(hr))
        {
            outHandle.blob = blob;
            outHandle.view = std::span<uint8_t>((uint8_t*)blob->GetBufferPointer(), blob->GetBufferSize());
            return true;
        }

        std::string_view errorDetail =
            errorBlob ? std::string_view((const char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize())
                      : "Unknown compile error"sv;
        AXLOGE("axmol:ERROR: Failed to compile shader, hr:{},{}", hr, errorDetail);
        AXASSERT(false, "Shader compile failed!");

        return false;
    }
}

bool DriverImpl::generateMipmaps(ID3D12GraphicsCommandList* cmd, ID3D12Resource* texture)
{
    if (!cmd || !texture)
        return false;

    const auto desc       = texture->GetDesc();
    const UINT mipCount   = desc.MipLevels;
    const UINT arrayCount = desc.DepthOrArraySize;

    // Nothing to generate if only one mip level
    if (mipCount <= 1)
        return true;

    const bool isArray = (arrayCount > 1);
    ensureMipmapPipeline(isArray);  // lazy init PSO + root signature

    // Build SRV/UAV descriptors for each mip (CPU-only descriptors in your existing pools)
    tlx::pod_vector<DescriptorHandle*> mipSrvs(mipCount);
    tlx::pod_vector<DescriptorHandle*> mipUavs(mipCount);

    for (UINT m = 0; m < mipCount; ++m)
    {
        // SRV for mip m (one-level SRV)
        D3D12_SHADER_RESOURCE_VIEW_DESC srv{};
        srv.Format                  = desc.Format;
        srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        if (isArray)
        {
            srv.ViewDimension                  = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
            srv.Texture2DArray.MostDetailedMip = m;
            srv.Texture2DArray.MipLevels       = 1;
            srv.Texture2DArray.FirstArraySlice = 0;
            srv.Texture2DArray.ArraySize       = arrayCount;
        }
        else
        {
            srv.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
            srv.Texture2D.MostDetailedMip = m;
            srv.Texture2D.MipLevels       = 1;
        }
        mipSrvs[m] = createSRV(texture, &srv);  // returns a handle from CPU-only heap

        // UAV for mip m (we write mip m+1, but preparing for all m simplifies indexing)
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav{};
        uav.Format = dxutils::getUAVCompatibleFormat(desc.Format);
        if (isArray)
        {
            uav.ViewDimension                  = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            uav.Texture2DArray.MipSlice        = m;
            uav.Texture2DArray.FirstArraySlice = 0;
            uav.Texture2DArray.ArraySize       = arrayCount;
        }
        else
        {
            uav.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
            uav.Texture2D.MipSlice = m;
        }
        mipUavs[m] = createUAV(texture, &uav);  // returns a handle from CPU-only heap
    }

    // Precompute per-pass GPU descriptor handles by copying from CPU-only heaps into the shader-visible heap.
    // For each pass m, we need SRV of mip m and UAV of mip (m+1). Total passes: (mipCount-1), total descriptors:
    // 2*(mipCount-1).
    const UINT passCount = (mipCount > 0) ? (mipCount - 1) : 0;
    struct ViewPairGPU
    {
        D3D12_GPU_DESCRIPTOR_HANDLE srv;
        D3D12_GPU_DESCRIPTOR_HANDLE uav;
    };
    tlx::pod_vector<ViewPairGPU> passViews(passCount);

    const auto cpuStart = _mipmapSrvHeap->GetCPUDescriptorHandleForHeapStart();
    const auto gpuStart = _mipmapSrvHeap->GetGPUDescriptorHandleForHeapStart();

    // Safety: ensure we do not overflow the heap; early-out if capacity insufficient
    const UINT requiredDescriptors = passCount * 2;
    const UINT heapCapacity        = _mipmapSrvHeap->GetDesc().NumDescriptors;
    if (requiredDescriptors > heapCapacity)
    {
        // If ever hit, adjust heap sizing policy above
        AXLOGE("Mipmap SRV heap capacity (%u) insufficient, required: %u", heapCapacity, requiredDescriptors);
        return false;
    }

    for (UINT m = 0; m < passCount; ++m)
    {
        // Destination indices in the shader-visible heap:
        // slot = 2*m for SRV, slot = 2*m+1 for UAV
        const UINT srvIndex = 2 * m;
        const UINT uavIndex = srvIndex + 1;

        const auto dstSrvCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, srvIndex, _srvDescriptorStride);
        const auto dstUavCPU = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, uavIndex, _srvDescriptorStride);

        // Source CPU-only descriptors from your DescriptorHandle abstraction
        const auto srcSrvCPU = mipSrvs[m]->cpu;
        const auto srcUavCPU = mipUavs[m + 1]->cpu;  // writes to mip (m+1)

        // Copy into shader-visible heap
        getDevice()->CopyDescriptorsSimple(1, dstSrvCPU, srcSrvCPU, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        getDevice()->CopyDescriptorsSimple(1, dstUavCPU, srcUavCPU, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        // Compute GPU handles for binding
        passViews[m].srv = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuStart, srvIndex, _srvDescriptorStride);
        passViews[m].uav = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuStart, uavIndex, _srvDescriptorStride);
    }

    // Create upload buffer for per-dispatch constants
    struct MipConstants
    {
        uint32_t SrcDim[2];
        uint32_t DstDim[2];
        float InvSrcDim[2];
        uint32_t FirstSlice;
        uint32_t SliceCount;
    };
    const UINT constStride    = 256;  // CB alignment
    const UINT totalConstSize = passCount * constStride;

    ComPtr<ID3D12Resource> constUpload;
    {
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto bufDesc = CD3DX12_RESOURCE_DESC::Buffer(totalConstSize);
        if (!checkHR(_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                      IID_PPV_ARGS(&constUpload)),
                     "Create constant upload buffer"))
            return false;
    }

    // Bind descriptor heaps (sampler is static in RS)
    {
        ID3D12DescriptorHeap* heaps[] = {_mipmapSrvHeap.Get()};
        cmd->SetDescriptorHeaps(1, heaps);
    }

    // Bind root signature and PSO
    cmd->SetComputeRootSignature(_mipmapRootSig.Get());
    cmd->SetPipelineState(isArray ? _mipmapPSOArray.Get() : _mipmapPSO2D.Get());

    // Transition texture to non-pixel-shader-readable baseline
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource   = texture;
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        b.Transition.StateAfter  = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }

    const UINT baseW = static_cast<UINT>(desc.Width);
    const UINT baseH = desc.Height;

    for (UINT m = 0; m < passCount; ++m)
    {
        const UINT srcW = std::max(1u, baseW >> m);
        const UINT srcH = std::max(1u, baseH >> m);
        const UINT dstW = std::max(1u, baseW >> (m + 1));
        const UINT dstH = std::max(1u, baseH >> (m + 1));

        // Transition destination mip (m+1) to UAV
        {
            D3D12_RESOURCE_BARRIER bar{};
            bar.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            bar.Transition.pResource   = texture;
            bar.Transition.Subresource = D3D12CalcSubresource(m + 1, 0, 0, mipCount, arrayCount);
            bar.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            bar.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            cmd->ResourceBarrier(1, &bar);
        }

        // Bind SRV/UAV for current pass (from the shader-visible heap we filled)
        cmd->SetComputeRootDescriptorTable(0, passViews[m].srv);
        cmd->SetComputeRootDescriptorTable(1, passViews[m].uav);

        // Upload constants for current mip
        uint8_t* mapped = nullptr;
        D3D12_RANGE rr{0, 0};
        constUpload->Map(0, &rr, reinterpret_cast<void**>(&mapped));
        auto* c         = reinterpret_cast<MipConstants*>(mapped + m * constStride);
        c->SrcDim[0]    = srcW;
        c->SrcDim[1]    = srcH;
        c->DstDim[0]    = dstW;
        c->DstDim[1]    = dstH;
        c->InvSrcDim[0] = 1.0f / float(srcW);
        c->InvSrcDim[1] = 1.0f / float(srcH);
        c->FirstSlice   = 0;
        c->SliceCount   = arrayCount;
        constUpload->Unmap(0, nullptr);

        D3D12_GPU_VIRTUAL_ADDRESS cbAddr = constUpload->GetGPUVirtualAddress() + m * constStride;
        cmd->SetComputeRootConstantBufferView(2, cbAddr);

        // Dispatch compute shader
        const UINT tgX = 8, tgY = 8;  // match HLSL THREADS_X/Y
        const UINT groupsX = (dstW + tgX - 1) / tgX;
        const UINT groupsY = (dstH + tgY - 1) / tgY;
        cmd->Dispatch(groupsX, groupsY, isArray ? arrayCount : 1);

        // Transition destination mip back to non-pixel-shader-readable
        {
            D3D12_RESOURCE_BARRIER bar{};
            bar.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            bar.Transition.pResource   = texture;
            bar.Transition.Subresource = D3D12CalcSubresource(m + 1, 0, 0, mipCount, arrayCount);
            bar.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            bar.Transition.StateAfter  = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            cmd->ResourceBarrier(1, &bar);
        }
    }

    // Transition whole texture back to pixel-shader-readable
    {
        D3D12_RESOURCE_BARRIER b{};
        b.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        b.Transition.pResource   = texture;
        b.Transition.StateBefore = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
        b.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
        b.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        cmd->ResourceBarrier(1, &b);
    }

    // Dispose the temporary CPU-only descriptors and upload buffer
    queueDisposal(constUpload.Detach(), 1);
    for (auto h : mipSrvs)
        queueDisposal(h, DisposableResource::Type::ShaderResourceView, 1);
    for (auto h : mipUavs)
        queueDisposal(h, DisposableResource::Type::ShaderResourceView, 1);

    return true;
}

void DriverImpl::ensureMipmapPipeline(bool isArray)
{
    if (!_mipmapRootSig)
    {
        CD3DX12_DESCRIPTOR_RANGE srvRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        CD3DX12_DESCRIPTOR_RANGE uavRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

        CD3DX12_ROOT_PARAMETER params[3];
        params[0].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_ALL);
        params[1].InitAsDescriptorTable(1, &uavRange, D3D12_SHADER_VISIBILITY_ALL);
        params[2].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

        CD3DX12_STATIC_SAMPLER_DESC staticSampler(
            0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 0.0f, 16, D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_ALL, 0);

        CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
        rsDesc.Init(_countof(params), params, 1, &staticSampler, D3D12_ROOT_SIGNATURE_FLAG_NONE);

        ComPtr<ID3DBlob> rsBlob, rsErr;
        checkHR(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &rsErr));
        checkHR(getDevice()->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(),
                                                 IID_PPV_ARGS(&_mipmapRootSig)));
    }

    ComPtr<ID3D12PipelineState>& targetPSO = isArray ? _mipmapPSOArray : _mipmapPSO2D;
    if (!targetPSO)
    {
        auto csBlob = compileMipmapCS(isArray);
        D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.pRootSignature     = _mipmapRootSig.Get();
        psoDesc.CS.pShaderBytecode = csBlob.view.data();
        psoDesc.CS.BytecodeLength  = csBlob.view.size();
        checkHR(getDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&targetPSO)));
    }

    // Create (or ensure) a shader-visible SRV/UAV heap that we can copy CPU-only descriptors into.
    // We size it to a safe upper bound (aligned to 64) based on device caps, and reuse across calls.
    if (!_mipmapSrvHeap)
    {
        const auto maxMips                = RHIUtils::computeMipLevels(_caps.maxTextureSize, _caps.maxTextureSize);
        const auto mipmapSrvDescriptorMax = YASIO_SZ_ALIGN(maxMips * 2, 64);  // 2 descriptors per pass (SRV+UAV)

        D3D12_DESCRIPTOR_HEAP_DESC dh{};
        dh.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        dh.NumDescriptors = mipmapSrvDescriptorMax;
        dh.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        _device->CreateDescriptorHeap(&dh, IID_PPV_ARGS(&_mipmapSrvHeap));

        // Cache descriptor size increment for handle offset calculations
        auto stride = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        assert(_srvDescriptorStride == stride);
    }
}

D3D12BlobHandle DriverImpl::compileMipmapCS(bool isArray)
{
    tlx::byte_buffer hlslSource;
    if (isArray)
        hlslSource += "#define USE_ARRAY 1\n"sv;
    hlslSource += kCSGenerateMipsHLSL;
    D3D12BlobHandle csBlob;
    bool ok = compileShader(hlslSource, ShaderStage::COMPUTE, csBlob);
    AXASSERT(ok, "Compute shader compile failed");
    return csBlob;
}

void DriverImpl::waitForGPU()
{
    if (_idleFence && _idleEvent && _gfxQueue)
    {
        ++_idleFenceValue;
        _gfxQueue->Signal(_idleFence, _idleFenceValue);
        _idleFence->SetEventOnCompletion(_idleFenceValue, _idleEvent);
        WaitForSingleObject(_idleEvent, INFINITE);
    }
}

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
    D3D12_FEATURE_DATA_FORMAT_SUPPORT support = {format};
    HRESULT hr = _device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &support, sizeof(support));
    return SUCCEEDED(hr) && (support.Support1 & D3D12_FORMAT_SUPPORT1_TEXTURE2D) != 0;
}

bool DriverImpl::detectDXCAvailability()
{
    if (!_device || !_dxcLibrary || !_dxcCompiler)
    {
        AXLOGW("DXC unavailable: missing compiler or library.");
        return false;
    }

    // --- 1. Minimal compute shader ---
    static constexpr std::string_view kCS = R"(
        [numthreads(1,1,1)]
        void main() {}
    )"sv;

    // --- 2. Compile CS via DXC ---
    ComPtr<IDxcBlobEncoding> srcBlob;
    HRESULT hr = _dxcLibrary->CreateBlobWithEncodingOnHeapCopy(kCS.data(), (UINT)kCS.size(), CP_UTF8, &srcBlob);

    if (FAILED(hr))
    {
        AXLOGW("DXC unavailable: failed to create source blob.");
        return false;
    }

    ComPtr<IDxcOperationResult> result;
    hr = _dxcCompiler->Compile(srcBlob.Get(), nullptr, L"main", L"cs_6_0", _dxcArguments.data(),
                               (UINT)_dxcArguments.size(), nullptr, 0, nullptr, &result);

    if (FAILED(hr))
    {
        AXLOGW("DXC unavailable: compile invocation failed.");
        return false;
    }

    HRESULT status = E_FAIL;
    result->GetStatus(&status);
    if (FAILED(status))
    {
        AXLOGW("DXC unavailable: compile failed.");
        return false;
    }

    ComPtr<IDxcBlob> csBlob;
    result->GetResult(&csBlob);
    if (!csBlob)
    {
        AXLOGW("DXC unavailable: no DXIL blob returned.");
        return false;
    }

    // --- 3. Create minimal root signature (empty) ---
    D3D12_ROOT_SIGNATURE_DESC rsDesc{};
    rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

    ComPtr<ID3DBlob> sigBlob, errBlob;
    hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    if (FAILED(hr))
    {
        AXLOGW("DXC unavailable: root signature serialization failed.");
        return false;
    }

    ComPtr<ID3D12RootSignature> rootSig;
    hr = _device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSig));

    if (FAILED(hr))
    {
        AXLOGW("DXC unavailable: root signature creation failed.");
        return false;
    }

    // --- 4. Build minimal compute PSO ---
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = rootSig.Get();
    psoDesc.CS             = {csBlob->GetBufferPointer(), csBlob->GetBufferSize()};

    ComPtr<ID3D12PipelineState> pso;
    hr = _device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pso));

    if (FAILED(hr))
    {
        AXLOGW("DXC unavailable: driver rejected DXIL, hr=0x{:08x}", (unsigned)hr);
        return false;
    }

    AXLOGI("DXC is available and DXIL is accepted by driver.");
    return true;
}

}  // namespace ax::rhi::d3d12
