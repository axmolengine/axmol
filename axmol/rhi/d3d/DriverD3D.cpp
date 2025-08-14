#include "DriverD3D.h"
#include "CommandBufferD3D.h"
#include "BufferD3D.h"
#include "TextureD3D.h"
#include "ProgramD3D.h"
#include "ShaderModuleD3D.h"
#include "RenderTargetD3D.h"
#include "RenderPipelineD3D.h"
#include "DepthStencilStateD3D.h"
#include "VertexLayoutD3D.h"
#include "base/Logging.h"
#include "ntcvt/ntcvt.hpp"

#include <wrl/client.h>

#pragma comment(lib, "D3D11.lib")

namespace ax::rhi
{
enum VendorID : uint32_t
{
    VENDOR_ID_UNKNOWN = 0x0,
    VENDOR_ID_AMD     = 0x1002,
    VENDOR_ID_APPLE   = 0x106B,
    VENDOR_ID_ARM     = 0x13B5,
    // Broadcom devices won't use PCI, but this is their Vulkan vendor id.
    VENDOR_ID_BROADCOM  = 0x14E4,
    VENDOR_ID_GOOGLE    = 0x1AE0,
    VENDOR_ID_INTEL     = 0x8086,
    VENDOR_ID_MESA      = 0x10005,
    VENDOR_ID_MICROSOFT = 0x1414,
    VENDOR_ID_NVIDIA    = 0x10DE,
    VENDOR_ID_POWERVR   = 0x1010,
    // This is Qualcomm PCI Vendor ID.
    // Android doesn't have a PCI bus, but all we need is a unique id.
    VENDOR_ID_QUALCOMM = 0x5143,
    VENDOR_ID_SAMSUNG  = 0x144D,
    VENDOR_ID_VIVANTE  = 0x9999,
    VENDOR_ID_VMWARE   = 0x15AD,
    VENDOR_ID_VIRTIO   = 0x1AF4,
};

std::string_view GetVendorString(uint32_t vendorId)
{
    switch (vendorId)
    {
    case VENDOR_ID_AMD:
        return "AMD"sv;
    case VENDOR_ID_ARM:
        return "ARM"sv;
    case VENDOR_ID_APPLE:
        return "Apple"sv;
    case VENDOR_ID_BROADCOM:
        return "Broadcom"sv;
    case VENDOR_ID_GOOGLE:
        return "Google"sv;
    case VENDOR_ID_INTEL:
        return "Intel"sv;
    case VENDOR_ID_MESA:
        return "Mesa"sv;
    case VENDOR_ID_MICROSOFT:
        return "Microsoft"sv;
    case VENDOR_ID_NVIDIA:
        return "NVIDIA"sv;
    case VENDOR_ID_POWERVR:
        return "Imagination Technologies"sv;
    case VENDOR_ID_QUALCOMM:
        return "Qualcomm"sv;
    case VENDOR_ID_SAMSUNG:
        return "Samsung Electronics Co., Ltd."sv;
    case VENDOR_ID_VIVANTE:
        return "Vivante"sv;
    case VENDOR_ID_VMWARE:
        return "VMware"sv;
    case VENDOR_ID_VIRTIO:
        return "VirtIO"sv;
    case 0:
        return "NULL"sv;
    default:
        // TODO(jmadill): More vendor IDs.
        // UNIMPLEMENTED();
        return "Unknown";
    }
}

DriverBase* DriverBase::getInstance()
{
    if (!_instance)
        _instance = new d3d::DriverImpl();

    return _instance;
}

void DriverBase::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}
}  // namespace ax::rhi

namespace ax::rhi::d3d
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

DriverImpl::DriverImpl()
{
#if 0  // TODO: enum adapter
    Microsoft::WRL::ComPtr<IDXGIFactory> factory;
    CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

    UINT i = 0;
    Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
    while (factory->EnumAdapters(i, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        // std::wcout << L"Adapter " << i << L": " << desc.Description << std::endl;

        // select best adapter
        ++i;
    }
#endif

    UINT createDeviceFlags = 0;

#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    auto requestDriverType = D3D_DRIVER_TYPE_HARDWARE;
    HRESULT hr = D3D11CreateDevice(nullptr,                   // Adapter
                                   requestDriverType,         // Driver Type
                                   nullptr,                   // Software
                                   createDeviceFlags,         // Flags
                                   featureLevels,             // Feature Levels
                                   ARRAYSIZE(featureLevels),  // Num Feature Levels
                                   D3D11_SDK_VERSION,         // SDK Version
                                   &_device,                  // Device
                                   &_featureLevel,             // Feature Level
                                   &_context);
    if (hr == DXGI_ERROR_UNSUPPORTED)
    {
        // Try again with software driver type
        requestDriverType = D3D_DRIVER_TYPE_WARP;
        hr                = D3D11CreateDevice(nullptr,                   // Adapter
                                              requestDriverType,         // Driver Type
                                              nullptr,                   // Software
                                              createDeviceFlags,         // Flags
                                              featureLevels,             // Feature Levels
                                              ARRAYSIZE(featureLevels),  // Num Feature Levels
                                              D3D11_SDK_VERSION,         // SDK Version
                                              &_device,                  // Device
                                              &_featureLevel,            // Feature Level
                                              &_context);
    }
    if (FAILED(hr))
    {
        AXLOGE("Failed to create D3D11 device.");
    }

    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    _device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));

    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    dxgiDevice->GetAdapter(&dxgiAdapter);

    dxgiAdapter->GetDesc(&_adapterDesc);

    LARGE_INTEGER version;
    hr = dxgiAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &version);
    if (FAILED(hr))
    {
        _driverVersion.reset();
        AXLOGE("Error querying driver version from DXGI Adapter.");
    }
    else
    {
        _driverVersion = version;
    }
    // _maxAttributes = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
    // _maxTextureSize    = 16384;
    // _maxTextureUnits = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;
    // _maxSamplesAllowed = 1;

    _maxAttributes = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;  // 16

    _maxTextureUnits = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;  // 128

    _maxTextureSize = EstimateMaxTexSize(_device->GetFeatureLevel());

    _maxSamplesAllowed = static_cast<int32_t>(FindMaxMsaaSamples(_device, DXGI_FORMAT_R8G8B8A8_UNORM));
}

DriverImpl::~DriverImpl()
{
    SafeRelease(_context);
    SafeRelease(_device);
}

CommandBuffer* DriverImpl::createCommandBuffer(void* hwnd)
{
    return new CommandBufferImpl(this, (HWND)hwnd);
}

Buffer* DriverImpl::createBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferImpl(_device, _context, size, type, usage);
}

/**
 * New a Texture object.
 * @param descriptor Specifies texture description.
 * @return A Texture object.
 */
Texture* DriverImpl::createTexture(const TextureDescriptor& descriptor)
{
    return new TextureImpl(_device, descriptor);
}

RenderTarget* DriverImpl::createDefaultRenderTarget()
{
    return new RenderTargetImpl(_device, true);
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

/**
 * This property controls whether or not the drawables'
 * MTLTextures may only be used for framebuffer attachments (YES) or
 * whether they may also be used for texture sampling and pixel
 * read/write operations (NO).
 * @param frameBufferOnly A value of YES allows CAMetalLayer to allocate the MTLTexture objects in ways that are
 * optimized for display purposes that makes them unsuitable for sampling. The recommended value for most
 * applications is YES.
 */
void DriverImpl::setFrameBufferOnly(bool frameBufferOnly) {}

Program* DriverImpl::createProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramImpl(vertexShader, fragmentShader);
}

ShaderModule* DriverImpl::createShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleImpl(_device, stage, source);
}

VertexLayout* DriverImpl::createVertexLayout()
{
    return new VertexLayoutImpl();
}

/// below is driver info

/// @name Setters & Getters
/**
 * Get vendor device name.
 * @return Vendor device name.
 */
std::string DriverImpl::getVendor() const
{
    return std::string{GetVendorString(_adapterDesc.VendorId)};
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
    //     ASTC
    // };

    switch (feature)
    {
    case FeatureType::DEPTH24:
    case FeatureType::VAO:
    case FeatureType::PACKED_DEPTH_STENCIL:
    case FeatureType::IMG_FORMAT_BGRA8888:
    case FeatureType::S3TC:
        return true;
    }
    return false;
}

}  // namespace ax::rhi::d3d
