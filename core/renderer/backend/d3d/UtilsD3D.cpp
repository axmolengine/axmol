//
// Direct3D / DXGI equivalent of the OpenGL `s_textureFormats` table.
// For compressed formats that D3D11/D3D12 do **not** natively support (PVRTC, ETC*, ATC, ASTC, …),
// we fall back to the closest broadly-available format so the rest of the engine can still
// create a valid texture object and sample from it.
//   • Lossy RGB‐only schemes → BC1 (a.k.a. DXT1) or R8G8B8A8_UNORM as a generic 32-bit placeholder
//   • Lossy RGBA schemes     → BC3 (DXT5) or R8G8B8A8_UNORM
//   • High-quality ASTC      → BC7 (best quality on PC GPUs)
//
// The **`type`** column has no real DXGI equivalent; it is kept for structural parity and
// set to `DXGI_FORMAT_UNKNOWN`.
//

#include "UtilsD3D.h"
#include "DriverD3D.h"
#include <dxgiformat.h>
#include <wrl/client.h>

#include "base/Logging.h"

#include "renderer/backend/PixelFormatUtils.h"

using namespace Microsoft::WRL;

namespace ax::backend::d3d
{

#define DXGI_FORMAT_ASTC_4X4_TYPELESS     DXGI_FORMAT(133)
#define DXGI_FORMAT_ASTC_4X4_UNORM        DXGI_FORMAT(134)
#define DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   DXGI_FORMAT(135)
#define DXGI_FORMAT_ASTC_5X4_TYPELESS     DXGI_FORMAT(137)
#define DXGI_FORMAT_ASTC_5X4_UNORM        DXGI_FORMAT(138)
#define DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   DXGI_FORMAT(139)
#define DXGI_FORMAT_ASTC_5X5_TYPELESS     DXGI_FORMAT(141)
#define DXGI_FORMAT_ASTC_5X5_UNORM        DXGI_FORMAT(142)
#define DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   DXGI_FORMAT(143)
#define DXGI_FORMAT_ASTC_6X5_TYPELESS     DXGI_FORMAT(145)
#define DXGI_FORMAT_ASTC_6X5_UNORM        DXGI_FORMAT(146)
#define DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   DXGI_FORMAT(147)
#define DXGI_FORMAT_ASTC_6X6_TYPELESS     DXGI_FORMAT(149)
#define DXGI_FORMAT_ASTC_6X6_UNORM        DXGI_FORMAT(150)
#define DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   DXGI_FORMAT(151)
#define DXGI_FORMAT_ASTC_8X5_TYPELESS     DXGI_FORMAT(153)
#define DXGI_FORMAT_ASTC_8X5_UNORM        DXGI_FORMAT(154)
#define DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   DXGI_FORMAT(155)
#define DXGI_FORMAT_ASTC_8X6_TYPELESS     DXGI_FORMAT(157)
#define DXGI_FORMAT_ASTC_8X6_UNORM        DXGI_FORMAT(158)
#define DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   DXGI_FORMAT(159)
#define DXGI_FORMAT_ASTC_8X8_TYPELESS     DXGI_FORMAT(161)
#define DXGI_FORMAT_ASTC_8X8_UNORM        DXGI_FORMAT(162)
#define DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   DXGI_FORMAT(163)
#define DXGI_FORMAT_ASTC_10X5_TYPELESS    DXGI_FORMAT(165)
#define DXGI_FORMAT_ASTC_10X5_UNORM       DXGI_FORMAT(166)
#define DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  DXGI_FORMAT(167)
#define DXGI_FORMAT_ASTC_10X6_TYPELESS    DXGI_FORMAT(169)
#define DXGI_FORMAT_ASTC_10X6_UNORM       DXGI_FORMAT(170)
#define DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  DXGI_FORMAT(171)
#define DXGI_FORMAT_ASTC_10X8_TYPELESS    DXGI_FORMAT(173)
#define DXGI_FORMAT_ASTC_10X8_UNORM       DXGI_FORMAT(174)
#define DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  DXGI_FORMAT(175)
#define DXGI_FORMAT_ASTC_10X10_TYPELESS   DXGI_FORMAT(177)
#define DXGI_FORMAT_ASTC_10X10_UNORM      DXGI_FORMAT(178)
#define DXGI_FORMAT_ASTC_10X10_UNORM_SRGB DXGI_FORMAT(179)
#define DXGI_FORMAT_ASTC_12X10_TYPELESS   DXGI_FORMAT(181)
#define DXGI_FORMAT_ASTC_12X10_UNORM      DXGI_FORMAT(182)
#define DXGI_FORMAT_ASTC_12X10_UNORM_SRGB DXGI_FORMAT(183)
#define DXGI_FORMAT_ASTC_12X12_TYPELESS   DXGI_FORMAT(185)
#define DXGI_FORMAT_ASTC_12X12_UNORM      DXGI_FORMAT(186)
#define DXGI_FORMAT_ASTC_12X12_UNORM_SRGB DXGI_FORMAT(187)

struct GPUTextureFormatInfo
{
    DXGI_FORMAT internalFmt;      // Creation format
    DXGI_FORMAT internalFmtSrgb;  // Creation format (sRGB)
    DXGI_FORMAT srvFmt;           // View format for SRV/UAV
    DXGI_FORMAT srvFmtSrgb;       // View format for SRV/UAV (sRGB)
    DXGI_FORMAT typelessFmt;      // Typeless fallback for Copy/DSV/RTV aliasing
};

// clang-format off
static GPUTextureFormatInfo s_textureFormatsD3D[] =
{
    /* pvrtc v1 ------------------------------------------------------------ */
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,       DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN }, // PVRTC4
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,       DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN }, // PVRTC4A
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,       DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN }, // PVRTC2
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,       DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN,          DXGI_FORMAT_UNKNOWN }, // PVRTC2A

    /* etc ----------------------------------------------------------------- */
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ETC1
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ETC2
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ETC2A

    /* s3tc / bc ----------------------------------------------------------- */
    { DXGI_FORMAT_BC1_UNORM,            DXGI_FORMAT_BC1_UNORM_SRGB,            DXGI_FORMAT_BC1_UNORM,               DXGI_FORMAT_BC1_UNORM,               DXGI_FORMAT_UNKNOWN }, // S3TC_DXT1
    { DXGI_FORMAT_BC2_UNORM,            DXGI_FORMAT_BC2_UNORM_SRGB,            DXGI_FORMAT_BC2_UNORM,               DXGI_FORMAT_BC2_UNORM,               DXGI_FORMAT_UNKNOWN }, // S3TC_DXT3
    { DXGI_FORMAT_BC3_UNORM,            DXGI_FORMAT_BC3_UNORM_SRGB,            DXGI_FORMAT_BC3_UNORM,               DXGI_FORMAT_BC3_UNORM,               DXGI_FORMAT_UNKNOWN }, // S3TC_DXT5

    /* atc ----------------------------------------------------------------- */
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ATC
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ATCE
    { DXGI_FORMAT_UNKNOWN,              DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN }, // ATCI

    /* astc ---------------------------------------------------------------- */
    { DXGI_FORMAT_ASTC_4X4_UNORM,       DXGI_FORMAT_ASTC_4X4_UNORM_SRGB,            DXGI_FORMAT_ASTC_4X4_UNORM,               DXGI_FORMAT_ASTC_4X4_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC4×4
    { DXGI_FORMAT_ASTC_5X5_UNORM,       DXGI_FORMAT_ASTC_5X5_UNORM_SRGB,            DXGI_FORMAT_ASTC_5X5_UNORM,               DXGI_FORMAT_ASTC_5X5_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC5×5
    { DXGI_FORMAT_ASTC_6X6_UNORM,       DXGI_FORMAT_ASTC_6X6_UNORM_SRGB,            DXGI_FORMAT_ASTC_6X6_UNORM,               DXGI_FORMAT_ASTC_6X6_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC6×6
    { DXGI_FORMAT_ASTC_8X5_UNORM,       DXGI_FORMAT_ASTC_8X5_UNORM_SRGB,            DXGI_FORMAT_ASTC_8X5_UNORM,               DXGI_FORMAT_ASTC_8X5_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC8×5
    { DXGI_FORMAT_ASTC_8X6_UNORM,       DXGI_FORMAT_ASTC_8X6_UNORM_SRGB,            DXGI_FORMAT_ASTC_8X6_UNORM,               DXGI_FORMAT_ASTC_8X6_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC8×6
    { DXGI_FORMAT_ASTC_8X8_UNORM,       DXGI_FORMAT_ASTC_8X8_UNORM_SRGB,            DXGI_FORMAT_ASTC_8X8_UNORM,               DXGI_FORMAT_ASTC_8X8_UNORM,               DXGI_FORMAT_UNKNOWN }, // ASTC8×8
    { DXGI_FORMAT_ASTC_10X5_UNORM,      DXGI_FORMAT_ASTC_10X5_UNORM_SRGB,           DXGI_FORMAT_ASTC_10X5_UNORM,              DXGI_FORMAT_ASTC_10X5_UNORM,              DXGI_FORMAT_UNKNOWN }, // ASTC10×5

    /* uncompressed -------------------------------------------------------- */
    { DXGI_FORMAT_R8G8B8A8_UNORM,       DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,       DXGI_FORMAT_R8G8B8A8_UNORM,          DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,     DXGI_FORMAT_R8G8B8A8_TYPELESS }, // RGBA8
    { DXGI_FORMAT_B8G8R8A8_UNORM,       DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,       DXGI_FORMAT_B8G8R8A8_UNORM,          DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,     DXGI_FORMAT_B8G8R8A8_TYPELESS }, // BGRA8
    { DXGI_FORMAT_R8G8B8A8_UNORM,       DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,       DXGI_FORMAT_R8G8B8A8_UNORM,          DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,     DXGI_FORMAT_R8G8B8A8_TYPELESS }, // RGB8 (no native 24-bit; upgrade)
    { DXGI_FORMAT_B5G6R5_UNORM,         DXGI_FORMAT_B5G6R5_UNORM,              DXGI_FORMAT_B5G6R5_UNORM,            DXGI_FORMAT_B5G6R5_UNORM,            DXGI_FORMAT_UNKNOWN             }, // RGB565
    { DXGI_FORMAT_B4G4R4A4_UNORM,       DXGI_FORMAT_B4G4R4A4_UNORM,            DXGI_FORMAT_B4G4R4A4_UNORM,          DXGI_FORMAT_B4G4R4A4_UNORM,          DXGI_FORMAT_UNKNOWN             }, // RGBA4
    { DXGI_FORMAT_B5G5R5A1_UNORM,       DXGI_FORMAT_B5G5R5A1_UNORM,            DXGI_FORMAT_B5G5R5A1_UNORM,          DXGI_FORMAT_B5G5R5A1_UNORM,          DXGI_FORMAT_UNKNOWN             }, // RGB5A1

    { DXGI_FORMAT_R8_UNORM,             DXGI_FORMAT_R8_UNORM,                  DXGI_FORMAT_R8_UNORM,                DXGI_FORMAT_R8_UNORM,                DXGI_FORMAT_R8_TYPELESS }, // R8
    { DXGI_FORMAT_R8G8_UNORM,           DXGI_FORMAT_R8G8_UNORM,                DXGI_FORMAT_R8G8_UNORM,              DXGI_FORMAT_R8G8_UNORM,              DXGI_FORMAT_R8G8_TYPELESS }, // RG8
    { DXGI_FORMAT_R32G32B32A32_FLOAT,   DXGI_FORMAT_R32G32B32A32_FLOAT,        DXGI_FORMAT_R32G32B32A32_FLOAT,      DXGI_FORMAT_R32G32B32A32_FLOAT,      DXGI_FORMAT_R32G32B32A32_FLOAT }, // RGBA32F

    /* depth-stencil ------------------------------------------------------- */
    { DXGI_FORMAT_D24_UNORM_S8_UINT,    DXGI_FORMAT_D24_UNORM_S8_UINT,         DXGI_FORMAT_D24_UNORM_S8_UINT,       DXGI_FORMAT_D24_UNORM_S8_UINT,       DXGI_FORMAT_R24G8_TYPELESS }, // D24S8
};
// clang-format on

static_assert(AX_ARRAYSIZE(s_textureFormatsD3D) == static_cast<int>(PixelFormat::COUNT),
              "The Direct3D GPU texture format info table is incomplete!");

static TextureImpl* s_defaultColorAttachment;
static TextureImpl* s_defaultDepthStencilAttachment;

void UtilsD3D::toD3DTypes(PixelFormat pf, DXGI_FORMAT& dxgiFormat, bool& isCompressed)
{
    if (AX_LIKELY(pf < PixelFormat::COUNT))
    {
        auto& info   = s_textureFormatsD3D[(int)pf];
        dxgiFormat   = info.internalFmt;
        isCompressed = PixelFormatUtils::isCompressed(pf);
    }
}

void UtilsD3D::updateDefaultRenderTargetAttachments(DriverImpl* driverImpl, IDXGISwapChain* swapChain)
{
    AX_SAFE_RELEASE_NULL(s_defaultColorAttachment);
    AX_SAFE_RELEASE_NULL(s_defaultDepthStencilAttachment);

    if (swapChain)
    {
        auto device  = driverImpl->getDevice();
        auto context = driverImpl->getContext();

        // swap chain only contains color attachment
        ID3D11RenderTargetView* rtv{nullptr};
        ComPtr<ID3D11Texture2D> backBuffer = nullptr;
        HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
        hr         = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &rtv);
        s_defaultColorAttachment = new TextureImpl(device, backBuffer.Get());

        UINT fmtSupport = 0;
        device->CheckFormatSupport(DXGI_FORMAT_D24_UNORM_S8_UINT, &fmtSupport);
        assert(fmtSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL);

        // create depth-stencil attachment
        D3D11_TEXTURE2D_DESC depthDesc = {};
        depthDesc.Width                = s_defaultColorAttachment->getWidth();
        depthDesc.Height               = s_defaultColorAttachment->getHeight();
        depthDesc.MipLevels            = 1;
        depthDesc.ArraySize            = 1;
        depthDesc.Format               = DXGI_FORMAT_D24_UNORM_S8_UINT;  // 24-bit depth, 8-bit stencil
        depthDesc.Usage                = D3D11_USAGE_DEFAULT;
        depthDesc.BindFlags            = D3D11_BIND_DEPTH_STENCIL;
        depthDesc.SampleDesc.Count     = 1;
        depthDesc.SampleDesc.Quality   = 0;

        ComPtr<ID3D11Texture2D> depthStencilTexture;
        hr = device->CreateTexture2D(&depthDesc, nullptr, depthStencilTexture.GetAddressOf());

        ComPtr<ID3D11DepthStencilView> depthStencilView = nullptr;
        hr = device->CreateDepthStencilView(depthStencilTexture.Get(), nullptr, depthStencilView.GetAddressOf());

        context->OMSetRenderTargets(1, &rtv, depthStencilView.Get());

        s_defaultDepthStencilAttachment = new TextureImpl(device, depthStencilTexture.Get());
    }
}

TextureImpl* UtilsD3D::getDefaultColorAttachment()
{
    return s_defaultColorAttachment;
}
TextureImpl* UtilsD3D::getDefaultDepthStencilAttachment()
{
    return s_defaultDepthStencilAttachment;
}

}  // namespace ax::backend::d3d
