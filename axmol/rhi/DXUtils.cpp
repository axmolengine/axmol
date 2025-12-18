#include "axmol/rhi/DXUtils.h"
#include "axmol/platform/Common.h"

namespace ax::rhi::dxutils
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

// clang-format off
static PixelFormatInfo s_pixelFormatInfos[] =
{
    /* ---- format ----------------------- fmtSvr -----------------------fmtDsv --------------------- fmtSrgb -------------------*/
    /* pvrtc v1 ------------------------------------------------------------ */
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // PVRTC4  (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // PVRTC4A (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // PVRTC2  (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // PVRTC2A (NA)

    /* etc ----------------------------------------------------------------- */
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ETC1  (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ETC2  (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ETC2A (NA)

    /* s3tc / bc ----------------------------------------------------------- */
    { DXGI_FORMAT_BC1_UNORM,          DXGI_FORMAT_BC1_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC1_UNORM_SRGB        }, // S3TC_DXT1
    { DXGI_FORMAT_BC2_UNORM,          DXGI_FORMAT_BC2_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC2_UNORM_SRGB        }, // S3TC_DXT3
    { DXGI_FORMAT_BC3_UNORM,          DXGI_FORMAT_BC3_UNORM,             DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_BC3_UNORM_SRGB        }, // S3TC_DXT5

    /* atc ----------------------------------------------------------------- */
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ATC  (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ATCE (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // ATCI (NA)

    /* astc ---------------------------------------------------------------- */
    { DXGI_FORMAT_ASTC_4X4_UNORM,     DXGI_FORMAT_ASTC_4X4_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_4X4_UNORM_SRGB   }, // ASTC4×4
    { DXGI_FORMAT_ASTC_5X4_UNORM,     DXGI_FORMAT_ASTC_5X4_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_5X4_UNORM_SRGB   }, // ASTC5×4
    { DXGI_FORMAT_ASTC_5X5_UNORM,     DXGI_FORMAT_ASTC_5X5_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_5X5_UNORM_SRGB   }, // ASTC5×5
    { DXGI_FORMAT_ASTC_6X5_UNORM,     DXGI_FORMAT_ASTC_6X5_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_6X5_UNORM_SRGB   }, // ASTC6×5
    { DXGI_FORMAT_ASTC_6X6_UNORM,     DXGI_FORMAT_ASTC_6X6_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_6X6_UNORM_SRGB   }, // ASTC6×6
    { DXGI_FORMAT_ASTC_8X5_UNORM,     DXGI_FORMAT_ASTC_8X5_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_8X5_UNORM_SRGB   }, // ASTC8×5
    { DXGI_FORMAT_ASTC_8X6_UNORM,     DXGI_FORMAT_ASTC_8X6_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_8X6_UNORM_SRGB   }, // ASTC8×6
    { DXGI_FORMAT_ASTC_8X8_UNORM,     DXGI_FORMAT_ASTC_8X8_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_8X8_UNORM_SRGB   }, // ASTC8×8
    { DXGI_FORMAT_ASTC_10X5_UNORM,    DXGI_FORMAT_ASTC_10X5_UNORM,       DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_10X5_UNORM_SRGB  }, // ASTC10×5
    { DXGI_FORMAT_ASTC_10X6_UNORM,    DXGI_FORMAT_ASTC_10X6_UNORM,       DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_10X6_UNORM_SRGB  }, // ASTC10×6
    { DXGI_FORMAT_ASTC_10X8_UNORM,    DXGI_FORMAT_ASTC_10X8_UNORM,       DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_10X8_UNORM_SRGB  }, // ASTC10×8
    { DXGI_FORMAT_ASTC_10X10_UNORM,   DXGI_FORMAT_ASTC_10X10_UNORM,      DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_10X10_UNORM_SRGB }, // ASTC10×10
    { DXGI_FORMAT_ASTC_12X10_UNORM,   DXGI_FORMAT_ASTC_12X10_UNORM,      DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_12X10_UNORM_SRGB }, // ASTC12×10
    { DXGI_FORMAT_ASTC_12X12_UNORM,   DXGI_FORMAT_ASTC_12X12_UNORM,      DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_ASTC_12X12_UNORM_SRGB }, // ASTC12×10

    /* uncompressed -------------------------------------------------------- */
    { DXGI_FORMAT_R8G8B8A8_UNORM,     DXGI_FORMAT_R8G8B8A8_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_R8G8B8A8_UNORM_SRGB   }, // RGBA8
    { DXGI_FORMAT_B8G8R8A8_UNORM,     DXGI_FORMAT_B8G8R8A8_UNORM,        DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_B8G8R8A8_UNORM_SRGB   }, // BGRA8
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // RGB8 (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // RGB565 (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // RGBA4 (NA)
    { DXGI_FORMAT_UNKNOWN,            DXGI_FORMAT_UNKNOWN,               DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // RGB5A1 (NA)

    { DXGI_FORMAT_R8_UNORM,           DXGI_FORMAT_R8_UNORM,              DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN,              }, // R8
    { DXGI_FORMAT_R8G8_UNORM,         DXGI_FORMAT_R8G8_UNORM,            DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN,              }, // RG8
    { DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT,    DXGI_FORMAT_UNKNOWN,           DXGI_FORMAT_UNKNOWN               }, // RGBA32F

    /* depth-stencil ------------------------------------------------------- */
    { DXGI_FORMAT_R24G8_TYPELESS,     DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN               }, // D24S8
};
// clang-format on

static_assert(AX_ARRAYSIZE(s_pixelFormatInfos) == static_cast<int>(PixelFormat::COUNT),
              "The Direct3D GPU texture format info table is incomplete!");

const PixelFormatInfo* toDxgiFormatInfo(PixelFormat pf)
{
    if (pf < PixelFormat::COUNT) [[likely]]
        return &s_pixelFormatInfos[(int)pf];
    return nullptr;
}

int evalulateMaxTexSize(D3D_FEATURE_LEVEL fl)
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

DXGI_FORMAT getUAVCompatibleFormat(DXGI_FORMAT format)
{
    switch (format)
    {
    // SRGB → UNORM
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_UNORM;

    // Typeless → UNORM/FLOAT
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;

    // Already UAV-compatible
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return format;

    // Compressed formats not UAV-compatible
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return DXGI_FORMAT_UNKNOWN;

    default:
        return format;  // fallback: assume it's already UAV-compatible
    }
}

void fatalError(std::string_view op, HRESULT hr)
{
    auto msg = fmt::format("{}: 0x{:08x}", op, static_cast<unsigned>(hr));
#if AX_RENDER_API == AX_RENDER_API_D3D12
    showAlert(msg, "axmol: D3D12: Fatal Error", AlertStyle::IconError | AlertStyle::RequireSync);
#else
    showAlert(msg, "axmol: D3D11: Fatal Error", AlertStyle::IconError | AlertStyle::RequireSync);
#endif
    utils::killCurrentProcess();  // kill current process, don't cause crash when driver issue.
}

}  // namespace ax::rhi::dxutils
