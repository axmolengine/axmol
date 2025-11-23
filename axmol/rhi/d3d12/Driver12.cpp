/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors

 https://axmol.dev/
 ****************************************************************************/
#include "axmol/rhi/d3d12/Driver12.h"
#include "axmol/rhi/d3d12/RenderContext12.h"
#include "axmol/rhi/d3d12/Buffer12.h"
#include "axmol/rhi/d3d12/Texture12.h"
#include "axmol/rhi/d3d12/Program12.h"
#include "axmol/rhi/d3d12/ShaderModule12.h"  // if you have separate module; otherwise Program12 handles blobs
#include "axmol/rhi/d3d12/RenderTarget12.h"
#include "axmol/rhi/d3d12/RenderPipeline12.h"
#include "axmol/rhi/d3d12/DepthStencilState12.h"
#include "axmol/rhi/d3d12/VertexLayout12.h"
#include "axmol/base/Logging.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/rhi/DXUtils.h"
#include "axmol/rhi/d3d12/Utils12.h"
#include "ntcvt/ntcvt.hpp"

#include <algorithm>

#define _AX_USE_DXC 1

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

#if _AX_USE_DXC
#    pragma comment(lib, "dxcompiler.lib")
#else
#    pragma comment(lib, "d3dcompiler.lib")
#endif

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

#if _AX_USE_DXC
static inline std::wstring_view stageToProfile(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return L"vs_6_0"sv;
    case ShaderStage::FRAGMENT:
        return L"ps_6_0"sv;
    default:
        return L"vs_6_0"sv;
    }
}
#else
static inline const char* stageToProfile(ShaderStage s)
{
    switch (s)
    {
    case ShaderStage::VERTEX:
        return "vs_5_1";
    case ShaderStage::FRAGMENT:
        return "ps_5_1";
    default:
        return "vs_5_1";
    }
}
#endif

DriverImpl::DriverImpl() {}
DriverImpl::~DriverImpl()
{
    cleanPendingResources();
    destroySwapchainAttachments();
    _gfxQueue.Reset();
    _device.Reset();
    _dxgiFactory.Reset();
}

void DriverImpl::init()
{
    initializeAdapter();
    initializeDevice();
    createDescriptorHeaps();

    // check device feature level
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1,
                                         D3D_FEATURE_LEVEL_11_0};
    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels = {};
    featLevels.NumFeatureLevels                  = _countof(featureLevels);
    featLevels.pFeatureLevelsRequested           = featureLevels;

    HRESULT hr = _device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));

    if (SUCCEEDED(hr))
    {
        _featureLevel = featLevels.MaxSupportedFeatureLevel;
    }

    // ensure adapter
    if (!_adapter)
    {
        ComPtr<IDXGIDevice> dxgiDevice;
        AX_D3D_FAST_FAIL(
            hr = _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf())));

        AX_D3D_FAST_FAIL(hr = dxgiDevice->GetAdapter(_adapter.GetAddressOf()));

        AX_D3D_FAST_FAIL(
            hr = _adapter->GetParent(__uuidof(IDXGIFactory1), (void**)_dxgiFactory.ReleaseAndGetAddressOf()));
    }

    // adapter version
    LARGE_INTEGER version;
    hr = _adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version);
    if (FAILED(hr))
    {
        _driverVersion.reset();
        AXLOGW("Error querying driver version from DXGI Adapter.");
    }
    else
    {
        _driverVersion = version;
    }

    // adapter desc
    _adapter->GetDesc(&_adapterDesc);

    // caps
    _caps.maxAttributes   = D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;     // 32
    _caps.maxTextureUnits = D3D12_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;  // 128
    _caps.maxTextureSize  = dxutils::evalulateMaxTexSize(_featureLevel);

    _caps.maxSamplesAllowed = evalulateMaxMsaaSamples(_device.Get(), DXGI_FORMAT_R8G8B8A8_UNORM);

#if _AX_USE_DXC
    // init DXC instances once
    DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&_dxcLibrary));
    DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&_dxcCompiler));

#    if defined(NDEBUG)
    // Release build arguments
    _dxcArguments = {L"-O2", L"-Qstrip_debug"};
#    else
    // Debug build arguments
    _dxcArguments = {L"-Zi", L"-Od"};
#    endif

#endif
}

void DriverImpl::initializeAdapter()
{
    auto& contextAttrs = Application::getContextAttrs();
    UINT createFlags   = 0;

    if (contextAttrs.debugLayerEnabled)
    {
        // Enable debug layer if available
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            createFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }

    HRESULT hr = CreateDXGIFactory2(createFlags, IID_PPV_ARGS(&_dxgiFactory));
    AXASSERT(SUCCEEDED(hr), "CreateDXGIFactory2 failed");

    // Choose adapter
    const auto powerPreferrence = contextAttrs.powerPreference;
    if (powerPreferrence == PowerPreference::Auto)
        return;

    DXGI_GPU_PREFERENCE gpuPref = DXGI_GPU_PREFERENCE_UNSPECIFIED;
    if (powerPreferrence == PowerPreference::HighPerformance)
        gpuPref = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    else if (powerPreferrence == PowerPreference::LowPower)
        gpuPref = DXGI_GPU_PREFERENCE_MINIMUM_POWER;

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0;
         _dxgiFactory->EnumAdapterByGpuPreference(i, gpuPref, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
            _adapter = adapter;
            break;
        }
    }

    if (!_adapter)
    {
        // Fallback: WARP
        _dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&_adapter));
    }
}

void DriverImpl::initializeDevice()
{
    // Create D3D12 device
    HRESULT hr = D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&_device));
    AXASSERT(SUCCEEDED(hr), "D3D12CreateDevice failed");

    // Create graphics queue
    D3D12_COMMAND_QUEUE_DESC qdesc{};
    qdesc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    qdesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    qdesc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    qdesc.NodeMask = 0;
    hr             = _device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&_gfxQueue));
    AXASSERT(SUCCEEDED(hr), "CreateCommandQueue failed");
}

void DriverImpl::createDescriptorHeaps()
{
    _srvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8192u, true);
    _samplerAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 256u, true);
    _rtvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1024u, false);
    _dsvAllocator =
        std::make_unique<DescriptorHeapAllocator>(_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 512u, false);
}

RenderContext* DriverImpl::createRenderContext(void* surfaceContext)
{
    _lastRenderContext = new RenderContextImpl(this, surfaceContext);
    return _lastRenderContext;
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage, const void* initial)
{
    return new BufferImpl(this, size, type, usage, initial);
}

Texture* DriverImpl::createTexture(const TextureDesc& descriptor)
{
    return new TextureImpl(this, descriptor);
}

RenderTarget* DriverImpl::createDefaultRenderTarget()
{
    return new RenderTargetImpl(this, true);
}

RenderTarget* DriverImpl::createRenderTarget(Texture* colorAttachment, Texture* depthStencilAttachment)
{
    auto rt = new RenderTargetImpl(this, false);
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    rt->setColorAttachment(colors);
    rt->setDepthStencilAttachment(depthStencilAttachment);
    return rt;
}

DepthStencilState* DriverImpl::createDepthStencilState()
{
    return new DepthStencilStateImpl();
}

RenderPipeline* DriverImpl::createRenderPipeline()
{
    return new RenderPipelineImpl(_device.Get());
}

Program* DriverImpl::createProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramImpl(vertexShader, fragmentShader);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleImpl(_device.Get(), stage, source);
}

SamplerHandle DriverImpl::createSampler(const SamplerDesc& desc)
{

    D3D12_SAMPLER_DESC sd = {};

    // --- Filter ---
    if (desc.minFilter == SamplerFilter::MIN_ANISOTROPIC)
    {
        sd.Filter        = D3D12_FILTER_ANISOTROPIC;
        sd.MaxAnisotropy = desc.anisotropy ? desc.anisotropy : 1;
    }
    else
    {
        const auto minL = ((int)desc.minFilter & (int)SamplerFilter::MIN_LINEAR);
        const auto magL = ((int)desc.magFilter & (int)SamplerFilter::MAG_LINEAR);
        const auto mipL = ((int)desc.mipFilter & (int)SamplerFilter::MIP_LINEAR);

        // minL<<2 | magL<<1 | mipL
        static const D3D12_FILTER filterTable[8] = {
            D3D12_FILTER_MIN_MAG_MIP_POINT,                // 000
            D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR,         // 001
            D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,   // 010
            D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR,         // 011
            D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT,         // 100
            D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR,  // 101
            D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,         // 110
            D3D12_FILTER_MIN_MAG_MIP_LINEAR                // 111
        };

        const int idx = (minL << 2) | (magL << 1) | (mipL ? 1 : 0);
        sd.Filter     = filterTable[idx];

        sd.MaxAnisotropy = 1;
    }

    // --- Wrap ---
    static const D3D12_TEXTURE_ADDRESS_MODE wrapTbl[4] = {
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,    // REPEAT
        D3D12_TEXTURE_ADDRESS_MODE_MIRROR,  // MIRROR
        D3D12_TEXTURE_ADDRESS_MODE_CLAMP,   // CLAMP
        D3D12_TEXTURE_ADDRESS_MODE_BORDER   // BORDER
    };
    sd.AddressU = wrapTbl[static_cast<int>(desc.sAddressMode)];
    sd.AddressV = wrapTbl[static_cast<int>(desc.tAddressMode)];
    sd.AddressW = wrapTbl[static_cast<int>(desc.wAddressMode)];

    // --- Compare ---
    sd.ComparisonFunc =
        static_cast<D3D12_COMPARISON_FUNC>(D3D12_COMPARISON_FUNC_NEVER + static_cast<int>(desc.compareFunc));

    sd.MinLOD         = 0.0f;
    sd.MaxLOD         = D3D12_FLOAT32_MAX;
    sd.MipLODBias     = 0.0f;
    sd.BorderColor[0] = 0.0f;
    sd.BorderColor[1] = 0.0f;
    sd.BorderColor[2] = 0.0f;
    sd.BorderColor[3] = 0.0f;

    // --- Allocate a slot in Sampler Heap ---
    auto handle = allocSampler();

    _device->CreateSampler(&sd, handle.cpu);

    return reinterpret_cast<SamplerHandle>(new D3D12SamplerHandle{handle});
}

void DriverImpl::destroySampler(SamplerHandle& h)
{
    auto wrapper = reinterpret_cast<D3D12SamplerHandle*>(h);
    _samplerAllocator->free(wrapper->handle);
    delete wrapper;
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

#if _AX_USE_DXC
    return fmt::format("{} D3D12 DXC SM6.0", desc);
#else
    return fmt::format("{} D3D12 vs_5_1 ps_5_1", desc);
#endif
}

std::string DriverImpl::getShaderVersion() const
{
#if _AX_USE_DXC
    return "HLSL Shader Model 6.0 (DXC)"s;
#else
    return "D3D12 HLSL vs_5_1 ps_5_1"s;
#endif
}

bool DriverImpl::checkForFeatureSupported(FeatureType feature)
{
    // Basic, conservative feature checks; extend with CheckFeatureSupport if needed.
    switch (feature)
    {
    case FeatureType::VAO:
    case FeatureType::VERTEX_ATTRIB_BINDING:
        return true;  // D3D12 uses input layout + PSO (no VAO concept)
    case FeatureType::DEPTH24:
    case FeatureType::PACKED_DEPTH_STENCIL:
    case FeatureType::IMG_FORMAT_BGRA8888:
    case FeatureType::S3TC:
    case FeatureType::ASTC:
        // Depending on runtime and WDDM, formats may vary. Return true conservatively or query.
        return true;
    default:
        return false;
    }
}

void DriverImpl::cleanPendingResources()
{
    // In D3D12, ComPtr takes care of Release(). If using fence-gated disposal, drain here.
    std::lock_guard<std::mutex> lk(_disposalMutex);
    _disposalQueue.clear();
}

void DriverImpl::queueDisposalInternal(DisposableResource&& res)
{
    std::lock_guard<std::mutex> lk(_disposalMutex);
    _disposalQueue.emplace_back(std::move(res));
}

ID3D12GraphicsCommandList* DriverImpl::startIsolateSubmission()
{
    // Create allocator
    _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_isolateSubmission.allocator));

    // Create command list
    _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _isolateSubmission.allocator.Get(), nullptr,
                               IID_PPV_ARGS(&_isolateSubmission.cmdList));

    // Command lists are created in "open" state; ready for record.

    // Create (or reuse) a fence for this isolated submission
    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_isolateSubmission.fence));
    _isolateSubmission.fenceValue = 1;

    // Optional: set descriptor heaps consistent with your binding model
    // ID3D12DescriptorHeap* heaps[] = { srvUavCbvHeap, samplerHeap };
    // _iso.cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

    return _isolateSubmission.cmdList.Get();
}

void DriverImpl::finishIsolateSubmission(bool waitForCompletion)
{
    if (!_isolateSubmission.cmdList)
        return;

    // Close list
    _isolateSubmission.cmdList->Close();

    // Execute on same graphics queue
    ID3D12CommandList* lists[] = {_isolateSubmission.cmdList.Get()};
    _gfxQueue->ExecuteCommandLists(1, lists);

    // Signal fence
    _gfxQueue->Signal(_isolateSubmission.fence.Get(), _isolateSubmission.fenceValue);

    if (waitForCompletion)
    {
        if (_isolateSubmission.fence->GetCompletedValue() < _isolateSubmission.fenceValue)
        {
            if (!_fenceEvent)
                _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

            _isolateSubmission.fence->SetEventOnCompletion(_isolateSubmission.fenceValue, _fenceEvent);
            WaitForSingleObject(_fenceEvent, INFINITE);
        }
    }

    // Reset transient objects for reuse by COM refcount or free them by scope
    _isolateSubmission.cmdList.Reset();
    _isolateSubmission.allocator.Reset();
    _isolateSubmission.fence.Reset();
    _isolateSubmission.fenceValue = 0;
}

void DriverImpl::processDisposalResources(uint64_t completedFence) {}

void DriverImpl::destroySwapchainAttachments()
{
    if (!_swapchainColorAttachments.empty())
    {
        for (auto tex : _swapchainColorAttachments)
            delete tex;
        _swapchainColorAttachments.clear();
    }
    if (_swapchainDepthStencilAttachment)
    {
        delete _swapchainDepthStencilAttachment;
        _swapchainDepthStencilAttachment = nullptr;
    }
}

void DriverImpl::rebuildSwapchainAttachments(IDXGISwapChain4* swapchain, uint32_t width, uint32_t height)
{
    destroySwapchainAttachments();

    // Create color attachments wrapping swapchain buffers
    const UINT bufferCount = RenderContextImpl::MAX_FRAMES_IN_FLIGHT;
    _swapchainColorAttachments.reserve(bufferCount);

    for (UINT i = 0; i < bufferCount; ++i)
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
        HRESULT hr = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
        AXASSERT(SUCCEEDED(hr), "SwapChain GetBuffer failed");

        TextureDesc colorDesc{};
        colorDesc.textureType  = TextureType::TEXTURE_2D;
        colorDesc.width        = static_cast<uint16_t>(width);
        colorDesc.height       = static_cast<uint16_t>(height);
        colorDesc.arraySize    = 1;
        colorDesc.mipLevels    = 1;
        colorDesc.pixelFormat  = PixelFormat::RGBA8;  // matches DXGI_FORMAT_R8G8B8A8_UNORM
        colorDesc.textureUsage = TextureUsage::RENDER_TARGET;

        auto tex = new TextureImpl(this, colorDesc);
        // Adopt existing resource, TOOD:
        // tex->internalHandle().resource = backBuffer;  // wrap swapchain buffer

        _swapchainColorAttachments.push_back(tex);
    }

    // Create depth-stencil attachment
    createDepthStencilAttachment(width, height);
}

void DriverImpl::createDepthStencilAttachment(UINT width, UINT height)
{
    // Create a D24S8 texture as default depth-stencil
    TextureDesc depthDesc{};
    depthDesc.textureType  = TextureType::TEXTURE_2D;
    depthDesc.width        = static_cast<uint16_t>(width);
    depthDesc.height       = static_cast<uint16_t>(height);
    depthDesc.arraySize    = 1;
    depthDesc.mipLevels    = 1;
    depthDesc.pixelFormat  = PixelFormat::D24S8;
    depthDesc.textureUsage = TextureUsage::RENDER_TARGET;

    auto tex = new TextureImpl(this, depthDesc);
    tex->updateData(nullptr, width, height, 0);  // initialize resource

    // Create DSV in DSV heap
    // D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;

    // _device->CreateDepthStencilView(tex->internalHandle().resource.Get(), &dsvDesc, dsvHandle);
    _swapchainDepthStencilAttachment = tex;
}

TextureImpl* DriverImpl::getSwapchainColorAttachment()
{
    if (_swapchainColorAttachments.empty())
        return nullptr;
    return _swapchainColorAttachments[_currentBackBufferIndex];
}

TextureImpl* DriverImpl::getSwapchainDepthStencilAttachment()
{
    return _swapchainDepthStencilAttachment;
}

bool DriverImpl::compileShader(std::string_view shaderSource, ShaderStage stage, D3D12BlobHandle& outHandle)
{
#if _AX_USE_DXC
    ComPtr<IDxcBlobEncoding> sourceBlob;
    _dxcLibrary->CreateBlobWithEncodingOnHeapCopy(shaderSource.data(), static_cast<UINT32>(shaderSource.size()),
                                                  CP_UTF8, &sourceBlob);

    std::wstring_view entryPoint = L"main";
    std::wstring_view profile    = stageToProfile(stage);

    ComPtr<IDxcOperationResult> result;
    HRESULT hr = _dxcCompiler->Compile(sourceBlob.Get(),
                                       nullptr,            // source file name
                                       entryPoint.data(),  // entry point
                                       profile.data(),     // target profile
                                       _dxcArguments.data(), (UINT)_dxcArguments.size(), nullptr, 0,  // defines
                                       nullptr,                                                       // include handler
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
#else
    ComPtr<ID3DBlob> blob;
    ComPtr<ID3DBlob> errorBlob;
    UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL2 | D3DCOMPILE_ENABLE_STRICTNESS;
#    if !defined(NDEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#    endif
    HRESULT hr = D3DCompile(shaderSource.data(), shaderSource.size(), nullptr, nullptr, nullptr, "main",
                            stageToProfile(stage), flags, 0, &blob, &errorBlob);
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
#endif
}

void DriverImpl::waitDeviceIdle()
{
    // Create a temporary fence to flush queue
    Microsoft::WRL::ComPtr<ID3D12Fence> fence;
    UINT64 value = 1;
    HANDLE evt   = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    HRESULT hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    AXASSERT(SUCCEEDED(hr), "CreateFence failed");

    _gfxQueue->Signal(fence.Get(), value);
    fence->SetEventOnCompletion(value, evt);
    WaitForSingleObject(evt, INFINITE);
    CloseHandle(evt);
}

}  // namespace ax::rhi::d3d12
