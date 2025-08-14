#if defined(_WIN32)

#include "MFUtils.h"

#include <windows.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>

#pragma comment(lib, "mf")
#pragma comment(lib, "Mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")

#    define __TEXTSV(quote) quote##sv
#    define TEXTSV(str) __TEXTSV(str)

namespace MFUtils
{
struct MFMedia
{
    MFMedia() : _hr(MFStartup(MF_VERSION)) {}
    ~MFMedia() { MFShutdown(); }
    HRESULT _hr;
};

HRESULT InitializeMFOnce()
{
    static MFMedia s_mfMedia;
    return s_mfMedia._hr;
}

std::string_view GetVideoTypeName(const GUID& SubType)
{
    using namespace std::string_view_literals;

    if (SubType == GUID_NULL)
        return TEXTSV("Null");

    // image formats
    if (SubType == MFImageFormat_JPEG)
        return TEXTSV("Jpeg");
    if (SubType == MFImageFormat_RGB32)
        return TEXTSV("RGB32");

    // stream formats
    if (SubType == MFStreamFormat_MPEG2Transport)
        return TEXTSV("MPEG-2 Transport");
    if (SubType == MFStreamFormat_MPEG2Program)
        return TEXTSV("MPEG-2 Program");

    // video formats
    if (SubType == MFVideoFormat_RGB32)
        return TEXTSV("RGB32");
    if (SubType == MFVideoFormat_ARGB32)
        return TEXTSV("ARGB32");
    if (SubType == MFVideoFormat_RGB24)
        return TEXTSV("RGB24");
    if (SubType == MFVideoFormat_RGB555)
        return TEXTSV("RGB525");
    if (SubType == MFVideoFormat_RGB565)
        return TEXTSV("RGB565");
    if (SubType == MFVideoFormat_RGB8)
        return TEXTSV("RGB8");
    if (SubType == MFVideoFormat_AI44)
        return TEXTSV("AI44");
    if (SubType == MFVideoFormat_AYUV)
        return TEXTSV("AYUV");
    if (SubType == MFVideoFormat_YUY2)
        return TEXTSV("YUY2");
    if (SubType == MFVideoFormat_YVYU)
        return TEXTSV("YVYU");
    if (SubType == MFVideoFormat_YVU9)
        return TEXTSV("YVU9");
    if (SubType == MFVideoFormat_UYVY)
        return TEXTSV("UYVY");
    if (SubType == MFVideoFormat_NV11)
        return TEXTSV("NV11");
    if (SubType == MFVideoFormat_NV12)
        return TEXTSV("NV12");
    if (SubType == MFVideoFormat_YV12)
        return TEXTSV("YV12");
    if (SubType == MFVideoFormat_I420)
        return TEXTSV("I420");
    if (SubType == MFVideoFormat_IYUV)
        return TEXTSV("IYUV");
    if (SubType == MFVideoFormat_Y210)
        return TEXTSV("Y210");
    if (SubType == MFVideoFormat_Y216)
        return TEXTSV("Y216");
    if (SubType == MFVideoFormat_Y410)
        return TEXTSV("Y410");
    if (SubType == MFVideoFormat_Y416)
        return TEXTSV("Y416");
    if (SubType == MFVideoFormat_Y41P)
        return TEXTSV("Y41P");
    if (SubType == MFVideoFormat_Y41T)
        return TEXTSV("Y41T");
    if (SubType == MFVideoFormat_Y42T)
        return TEXTSV("Y42T");
    if (SubType == MFVideoFormat_P210)
        return TEXTSV("P210");
    if (SubType == MFVideoFormat_P216)
        return TEXTSV("P216");
    if (SubType == MFVideoFormat_P010)
        return TEXTSV("P010");
    if (SubType == MFVideoFormat_P016)
        return TEXTSV("P016");
    if (SubType == MFVideoFormat_v210)
        return TEXTSV("v210");
    if (SubType == MFVideoFormat_v216)
        return TEXTSV("v216");
    if (SubType == MFVideoFormat_v410)
        return TEXTSV("v410");
    if (SubType == MFVideoFormat_MP43)
        return TEXTSV("MP43");
    if (SubType == MFVideoFormat_MP4S)
        return TEXTSV("MP4S");
    if (SubType == MFVideoFormat_M4S2)
        return TEXTSV("M4S2");
    if (SubType == MFVideoFormat_MP4V)
        return TEXTSV("MP4V");
    if (SubType == MFVideoFormat_WMV1)
        return TEXTSV("WMV1");
    if (SubType == MFVideoFormat_WMV2)
        return TEXTSV("WMV2");
    if (SubType == MFVideoFormat_WMV3)
        return TEXTSV("WMV3");
    if (SubType == MFVideoFormat_WVC1)
        return TEXTSV("WVC1");
    if (SubType == MFVideoFormat_MSS1)
        return TEXTSV("MSS1");
    if (SubType == MFVideoFormat_MSS2)
        return TEXTSV("MSS2");
    if (SubType == MFVideoFormat_MPG1)
        return TEXTSV("MPG1");
    if (SubType == MFVideoFormat_DVSL)
        return TEXTSV("DVSL");
    if (SubType == MFVideoFormat_DVSD)
        return TEXTSV("DVSD");
    if (SubType == MFVideoFormat_DVHD)
        return TEXTSV("DVHD");
    if (SubType == MFVideoFormat_DV25)
        return TEXTSV("DV25");
    if (SubType == MFVideoFormat_DV50)
        return TEXTSV("DV50");
    if (SubType == MFVideoFormat_DVH1)
        return TEXTSV("DVH1");
    if (SubType == MFVideoFormat_DVC)
        return TEXTSV("DVC");
    if (SubType == MFVideoFormat_H264)
        return TEXTSV("H264");
    if (SubType == MFVideoFormat_MJPG)
        return TEXTSV("MJPG");
    if (SubType == MFVideoFormat_420O)
        return TEXTSV("420O");
    if (SubType == MFVideoFormat_HEVC)
        return TEXTSV("HEVC");
    if (SubType == MFVideoFormat_HEVC_ES)
        return TEXTSV("HEVC ES");

#    if (WINVER >= _WIN32_WINNT_WIN8)
    if (SubType == MFVideoFormat_H263)
        return TEXTSV("H263");
#    endif

    if (SubType == MFVideoFormat_H264_ES)
        return TEXTSV("H264 ES");
    if (SubType == MFVideoFormat_MPEG2)
        return TEXTSV("MPEG-2");
    return "UNKNOWN"sv;
}
}  // namespace MFUtils
#endif
