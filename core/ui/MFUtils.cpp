#include "MFUtils.h"

#include <windows.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>


// MPEG-2 audio sub-types
#define OTHER_FORMAT_MPEG2_AC3 0xe06d80e4         // MPEG-2 AC3
#define OTHER_FORMAT_MPEG2_AUDIO 0xe06d802b       // MPEG-2 Audio
#define OTHER_FORMAT_MPEG2_DOLBY_AC3 0xe06d802c   // Dolby AC3
#define OTHER_FORMAT_MPEG2_DTS 0xe06d8033         // MPEG-2 DTS
#define OTHER_FORMAT_MPEG2_LPCM_AUDIO 0xe06d8032  // DVD LPCM Audio
#define OTHER_FORMAT_MPEG2_SDDS 0xe06d8034        // SDDS

// MPEG-2 video sub-types
#define OTHER_FORMAT_MPEG2_DVD_SUBPICTURE 0xe06d802d  // DVD Sub-picture
#define OTHER_FORMAT_MPEG2_VIDEO 0xe06d80e3           // MPEG-2 Video

#if (WINVER < _WIN32_WINNT_WIN8)
const GUID MF_LOW_LATENCY = {0x9c27891a, 0xed7a, 0x40e1, {0x88, 0xe8, 0xb2, 0x27, 0x27, 0xa0, 0x24, 0xee}};
#endif

// common media formats not defined by WMF
const GUID OtherFormatMpeg2_Base    = {0x00000000, 0xdb46, 0x11cf, {0xb4, 0xd1, 0x00, 0x80, 0x05f, 0x6c, 0xbb, 0xea}};
const GUID OtherVideoFormat_LifeCam = {
    0x3032344d,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};  // Microsoft LifeCam UVC 1.0 video
const GUID OtherVideoFormat_QuickTime = {0x61766331,
                                         0x767a,
                                         0x494d,
                                         {0xb4, 0x78, 0xf2, 0x9d, 0x25, 0xdc, 0x90, 0x37}};  // 1cva

const char* SubTypeToString(const GUID& SubType)
{
    if (SubType == GUID_NULL)
        return ("Null");

    // image formats
    if (SubType == MFImageFormat_JPEG)
        return ("Jpeg");
    if (SubType == MFImageFormat_RGB32)
        return ("RGB32");

    // stream formats
    if (SubType == MFStreamFormat_MPEG2Transport)
        return ("MPEG-2 Transport");
    if (SubType == MFStreamFormat_MPEG2Program)
        return ("MPEG-2 Program");

    // video formats
    if (SubType == MFVideoFormat_RGB32)
        return ("RGB32");
    if (SubType == MFVideoFormat_ARGB32)
        return ("ARGB32");
    if (SubType == MFVideoFormat_RGB24)
        return ("RGB24");
    if (SubType == MFVideoFormat_RGB555)
        return ("RGB525");
    if (SubType == MFVideoFormat_RGB565)
        return ("RGB565");
    if (SubType == MFVideoFormat_RGB8)
        return ("RGB8");
    if (SubType == MFVideoFormat_AI44)
        return ("AI44");
    if (SubType == MFVideoFormat_AYUV)
        return ("AYUV");
    if (SubType == MFVideoFormat_YUY2)
        return ("YUY2");
    if (SubType == MFVideoFormat_YVYU)
        return ("YVYU");
    if (SubType == MFVideoFormat_YVU9)
        return ("YVU9");
    if (SubType == MFVideoFormat_UYVY)
        return ("UYVY");
    if (SubType == MFVideoFormat_NV11)
        return ("NV11");
    if (SubType == MFVideoFormat_NV12)
        return ("NV12");
    if (SubType == MFVideoFormat_YV12)
        return ("YV12");
    if (SubType == MFVideoFormat_I420)
        return ("I420");
    if (SubType == MFVideoFormat_IYUV)
        return ("IYUV");
    if (SubType == MFVideoFormat_Y210)
        return ("Y210");
    if (SubType == MFVideoFormat_Y216)
        return ("Y216");
    if (SubType == MFVideoFormat_Y410)
        return ("Y410");
    if (SubType == MFVideoFormat_Y416)
        return ("Y416");
    if (SubType == MFVideoFormat_Y41P)
        return ("Y41P");
    if (SubType == MFVideoFormat_Y41T)
        return ("Y41T");
    if (SubType == MFVideoFormat_Y42T)
        return ("Y42T");
    if (SubType == MFVideoFormat_P210)
        return ("P210");
    if (SubType == MFVideoFormat_P216)
        return ("P216");
    if (SubType == MFVideoFormat_P010)
        return ("P010");
    if (SubType == MFVideoFormat_P016)
        return ("P016");
    if (SubType == MFVideoFormat_v210)
        return ("v210");
    if (SubType == MFVideoFormat_v216)
        return ("v216");
    if (SubType == MFVideoFormat_v410)
        return ("v410");
    if (SubType == MFVideoFormat_MP43)
        return ("MP43");
    if (SubType == MFVideoFormat_MP4S)
        return ("MP4S");
    if (SubType == MFVideoFormat_M4S2)
        return ("M4S2");
    if (SubType == MFVideoFormat_MP4V)
        return ("MP4V");
    if (SubType == MFVideoFormat_WMV1)
        return ("WMV1");
    if (SubType == MFVideoFormat_WMV2)
        return ("WMV2");
    if (SubType == MFVideoFormat_WMV3)
        return ("WMV3");
    if (SubType == MFVideoFormat_WVC1)
        return ("WVC1");
    if (SubType == MFVideoFormat_MSS1)
        return ("MSS1");
    if (SubType == MFVideoFormat_MSS2)
        return ("MSS2");
    if (SubType == MFVideoFormat_MPG1)
        return ("MPG1");
    if (SubType == MFVideoFormat_DVSL)
        return ("DVSL");
    if (SubType == MFVideoFormat_DVSD)
        return ("DVSD");
    if (SubType == MFVideoFormat_DVHD)
        return ("DVHD");
    if (SubType == MFVideoFormat_DV25)
        return ("DV25");
    if (SubType == MFVideoFormat_DV50)
        return ("DV50");
    if (SubType == MFVideoFormat_DVH1)
        return ("DVH1");
    if (SubType == MFVideoFormat_DVC)
        return ("DVC");
    if (SubType == MFVideoFormat_H264)
        return ("H264");
    if (SubType == MFVideoFormat_MJPG)
        return ("MJPG");
    if (SubType == MFVideoFormat_420O)
        return ("420O");
    if (SubType == MFVideoFormat_HEVC)
        return ("HEVC");
    if (SubType == MFVideoFormat_HEVC_ES)
        return ("HEVC ES");

#if (WINVER >= _WIN32_WINNT_WIN8)
    if (SubType == MFVideoFormat_H263)
        return ("H263");
#endif

    if (SubType == MFVideoFormat_H264_ES)
        return ("H264 ES");
    if (SubType == MFVideoFormat_MPEG2)
        return ("MPEG-2");

    // common non-Windows formats
    if (SubType == OtherVideoFormat_LifeCam)
        return ("LifeCam");
    if (SubType == OtherVideoFormat_QuickTime)
        return ("QuickTime");

    if (memcmp(&SubType.Data2, &OtherFormatMpeg2_Base.Data2, 12) == 0)
    {
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_AC3)
            return ("MPEG-2 AC3");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_AUDIO)
            return ("MPEG-2 Audio");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_DOLBY_AC3)
            return ("Dolby AC-3");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_DTS)
            return ("DTS");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_LPCM_AUDIO)
            return ("DVD LPCM");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_SDDS)
            return ("SDDS");

        if (SubType.Data1 == OTHER_FORMAT_MPEG2_DVD_SUBPICTURE)
            return ("DVD Subpicture");
        if (SubType.Data1 == OTHER_FORMAT_MPEG2_VIDEO)
            return ("MPEG-2 Video");
    }

    // audio formats
    if ((memcmp(&SubType.Data2, &MFAudioFormat_Base.Data2, 12) == 0) ||
        (memcmp(&SubType.Data2, &MFMPEG4Format_Base.Data2, 12) == 0))
    {
        if (SubType.Data1 == WAVE_FORMAT_UNKNOWN)
            return ("Unknown Audio Format");
        if (SubType.Data1 == WAVE_FORMAT_PCM)
            return ("PCM");
        if (SubType.Data1 == WAVE_FORMAT_ADPCM)
            return ("ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_IEEE_FLOAT)
            return ("IEEE Float");
        if (SubType.Data1 == WAVE_FORMAT_VSELP)
            return ("VSELP");
        if (SubType.Data1 == WAVE_FORMAT_IBM_CVSD)
            return ("IBM CVSD");
        if (SubType.Data1 == WAVE_FORMAT_ALAW)
            return ("aLaw");
        if (SubType.Data1 == WAVE_FORMAT_MULAW)
            return ("uLaw");
        if (SubType.Data1 == WAVE_FORMAT_DTS)
            return ("DTS");
        if (SubType.Data1 == WAVE_FORMAT_DRM)
            return ("DRM");
        if (SubType.Data1 == WAVE_FORMAT_WMAVOICE9)
            return ("WMA Voice 9");
        if (SubType.Data1 == WAVE_FORMAT_WMAVOICE10)
            return ("WMA Voice 10");
        if (SubType.Data1 == WAVE_FORMAT_OKI_ADPCM)
            return ("OKI ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_DVI_ADPCM)
            return ("Intel DVI ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_IMA_ADPCM)
            return ("Intel IMA ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_MEDIASPACE_ADPCM)
            return ("Videologic ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_SIERRA_ADPCM)
            return ("Sierra ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_G723_ADPCM)
            return ("G723 ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_DIGISTD)
            return ("DIGISTD");
        if (SubType.Data1 == WAVE_FORMAT_DIGIFIX)
            return ("DIGIFIX");
        if (SubType.Data1 == WAVE_FORMAT_DIALOGIC_OKI_ADPCM)
            return ("Dialogic ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_MEDIAVISION_ADPCM)
            return ("Media Vision ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_CU_CODEC)
            return ("HP CU Codec");
        if (SubType.Data1 == WAVE_FORMAT_HP_DYN_VOICE)
            return ("HP DynVoice");
        if (SubType.Data1 == WAVE_FORMAT_YAMAHA_ADPCM)
            return ("Yamaha ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_SONARC)
            return ("Sonarc");
        if (SubType.Data1 == WAVE_FORMAT_DSPGROUP_TRUESPEECH)
            return ("DPS Group TrueSpeech");
        if (SubType.Data1 == WAVE_FORMAT_ECHOSC1)
            return ("Echo Speech 1");
        if (SubType.Data1 == WAVE_FORMAT_AUDIOFILE_AF36)
            return ("AF36");
        if (SubType.Data1 == WAVE_FORMAT_APTX)
            return ("APTX");
        if (SubType.Data1 == WAVE_FORMAT_AUDIOFILE_AF10)
            return ("AF10");
        if (SubType.Data1 == WAVE_FORMAT_PROSODY_1612)
            return ("Prosody 1622");
        if (SubType.Data1 == WAVE_FORMAT_LRC)
            return ("LRC");
        if (SubType.Data1 == WAVE_FORMAT_DOLBY_AC2)
            return ("Dolby AC2");
        if (SubType.Data1 == WAVE_FORMAT_GSM610)
            return ("GSM 610");
        if (SubType.Data1 == WAVE_FORMAT_MSNAUDIO)
            return ("MSN Audio");
        if (SubType.Data1 == WAVE_FORMAT_ANTEX_ADPCME)
            return ("Antex ADPCME");
        if (SubType.Data1 == WAVE_FORMAT_CONTROL_RES_VQLPC)
            return ("Control Resources VQLPC");
        if (SubType.Data1 == WAVE_FORMAT_DIGIREAL)
            return ("DigiReal");
        if (SubType.Data1 == WAVE_FORMAT_DIGIADPCM)
            return ("DigiADPCM");
        if (SubType.Data1 == WAVE_FORMAT_CONTROL_RES_CR10)
            return ("Control Resources CR10");
        if (SubType.Data1 == WAVE_FORMAT_NMS_VBXADPCM)
            return ("VBX ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_CS_IMAADPCM)
            return ("Crystal IMA ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_ECHOSC3)
            return ("Echo Speech 3");
        if (SubType.Data1 == WAVE_FORMAT_ROCKWELL_ADPCM)
            return ("Rockwell ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_ROCKWELL_DIGITALK)
            return ("Rockwell DigiTalk");
        if (SubType.Data1 == WAVE_FORMAT_XEBEC)
            return ("Xebec");
        if (SubType.Data1 == WAVE_FORMAT_G721_ADPCM)
            return ("G721 ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_G728_CELP)
            return ("G728 CELP");
        if (SubType.Data1 == WAVE_FORMAT_MSG723)
            return ("MSG723");
        if (SubType.Data1 == WAVE_FORMAT_INTEL_G723_1)
            return ("Intel G723.1");
        if (SubType.Data1 == WAVE_FORMAT_INTEL_G729)
            return ("Intel G729");
        if (SubType.Data1 == WAVE_FORMAT_SHARP_G726)
            return ("Sharp G726");
        if (SubType.Data1 == WAVE_FORMAT_MPEG)
            return ("MPEG");
        if (SubType.Data1 == WAVE_FORMAT_RT24)
            return ("InSoft RT24");
        if (SubType.Data1 == WAVE_FORMAT_PAC)
            return ("InSoft PAC");
        if (SubType.Data1 == WAVE_FORMAT_MPEGLAYER3)
            return ("MPEG Layer 3");
        if (SubType.Data1 == WAVE_FORMAT_LUCENT_G723)
            return ("Lucent G723");
        if (SubType.Data1 == WAVE_FORMAT_CIRRUS)
            return ("Cirrus Logic");
        if (SubType.Data1 == WAVE_FORMAT_ESPCM)
            return ("ESS PCM");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE)
            return ("Voxware");
        if (SubType.Data1 == WAVE_FORMAT_CANOPUS_ATRAC)
            return ("Canopus ATRAC");
        if (SubType.Data1 == WAVE_FORMAT_G726_ADPCM)
            return ("APICOM G726");
        if (SubType.Data1 == WAVE_FORMAT_G722_ADPCM)
            return ("APICOM G722");
        if (SubType.Data1 == WAVE_FORMAT_DSAT)
            return ("DSAT");
        if (SubType.Data1 == WAVE_FORMAT_DSAT_DISPLAY)
            return ("DSAT Display");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_BYTE_ALIGNED)
            return ("Voxware Byte Aligned");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_AC8)
            return ("Voxware AC8");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_AC10)
            return ("Voxware AC10");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_AC16)
            return ("Voxware AC16");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_AC20)
            return ("Voxware AC20");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_RT24)
            return ("Voxware RT24");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_RT29)
            return ("Voxware RT29");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_RT29HW)
            return ("Voxware RT29HW");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_VR12)
            return ("Voxware VR12");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_VR18)
            return ("Voxware VR18");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_TQ40)
            return ("Voxware TQ40");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_SC3)
            return ("Voxware SC3");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_SC3_1)
            return ("Voxware SC3.1");
        if (SubType.Data1 == WAVE_FORMAT_SOFTSOUND)
            return ("Softsound");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_TQ60)
            return ("Voxware TQ60");
        if (SubType.Data1 == WAVE_FORMAT_MSRT24)
            return ("MSRT24");
        if (SubType.Data1 == WAVE_FORMAT_G729A)
            return ("AT&T G729A");
        if (SubType.Data1 == WAVE_FORMAT_MVI_MVI2)
            return ("NVI2");
        if (SubType.Data1 == WAVE_FORMAT_DF_G726)
            return ("DataFusion G726");
        if (SubType.Data1 == WAVE_FORMAT_DF_GSM610)
            return ("DataFusion GSM610");
        if (SubType.Data1 == WAVE_FORMAT_ISIAUDIO)
            return ("Iterated Systems");
        if (SubType.Data1 == WAVE_FORMAT_ONLIVE)
            return ("OnLive!");
        if (SubType.Data1 == WAVE_FORMAT_MULTITUDE_FT_SX20)
            return ("Multitude FT SX20");
        if (SubType.Data1 == WAVE_FORMAT_INFOCOM_ITS_G721_ADPCM)
            return ("Infocom ITS G721 ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_CONVEDIA_G729)
            return ("Convedia G729");
        if (SubType.Data1 == WAVE_FORMAT_CONGRUENCY)
            return ("Congruency");
        if (SubType.Data1 == WAVE_FORMAT_SBC24)
            return ("SBC24");
        if (SubType.Data1 == WAVE_FORMAT_DOLBY_AC3_SPDIF)
            return ("Dolby AC3 SPDIF");
        if (SubType.Data1 == WAVE_FORMAT_MEDIASONIC_G723)
            return ("MediaSonic G723");
        if (SubType.Data1 == WAVE_FORMAT_PROSODY_8KBPS)
            return ("Prosody 8kps");
        if (SubType.Data1 == WAVE_FORMAT_ZYXEL_ADPCM)
            return ("ZyXEL ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_PHILIPS_LPCBB)
            return ("Philips LPCBB");
        if (SubType.Data1 == WAVE_FORMAT_PACKED)
            return ("Studer Packed");
        if (SubType.Data1 == WAVE_FORMAT_MALDEN_PHONYTALK)
            return ("Malden PhonyTalk");
        if (SubType.Data1 == WAVE_FORMAT_RACAL_RECORDER_GSM)
            return ("Racal GSM");
        if (SubType.Data1 == WAVE_FORMAT_RACAL_RECORDER_G720_A)
            return ("Racal G720.A");
        if (SubType.Data1 == WAVE_FORMAT_RACAL_RECORDER_G723_1)
            return ("Racal G723.1");
        if (SubType.Data1 == WAVE_FORMAT_RACAL_RECORDER_TETRA_ACELP)
            return ("Racal Tetra ACELP");
        if (SubType.Data1 == WAVE_FORMAT_NEC_AAC)
            return ("NEC AAC");
        if (SubType.Data1 == WAVE_FORMAT_RAW_AAC1)
            return ("Raw AAC-1");
        if (SubType.Data1 == WAVE_FORMAT_RHETOREX_ADPCM)
            return ("Rhetorex ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_IRAT)
            return ("BeCubed IRAT");
        if (SubType.Data1 == WAVE_FORMAT_VIVO_G723)
            return ("Vivo G723");
        if (SubType.Data1 == WAVE_FORMAT_VIVO_SIREN)
            return ("vivo Siren");
        if (SubType.Data1 == WAVE_FORMAT_PHILIPS_CELP)
            return ("Philips Celp");
        if (SubType.Data1 == WAVE_FORMAT_PHILIPS_GRUNDIG)
            return ("Philips Grundig");
        if (SubType.Data1 == WAVE_FORMAT_DIGITAL_G723)
            return ("DEC G723");
        if (SubType.Data1 == WAVE_FORMAT_SANYO_LD_ADPCM)
            return ("Sanyo ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_ACEPLNET)
            return ("Sipro Lab ACEPLNET");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_ACELP4800)
            return ("Sipro Lab ACELP4800");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_ACELP8V3)
            return ("Sipro Lab ACELP8v3");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_G729)
            return ("Spiro Lab G729");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_G729A)
            return ("Spiro Lab G729A");
        if (SubType.Data1 == WAVE_FORMAT_SIPROLAB_KELVIN)
            return ("Spiro Lab Kelvin");
        if (SubType.Data1 == WAVE_FORMAT_VOICEAGE_AMR)
            return ("VoiceAge AMR");
        if (SubType.Data1 == WAVE_FORMAT_G726ADPCM)
            return ("Dictaphone G726 ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_DICTAPHONE_CELP68)
            return ("Dictaphone CELP68");
        if (SubType.Data1 == WAVE_FORMAT_DICTAPHONE_CELP54)
            return ("Dictaphone CELP54");
        if (SubType.Data1 == WAVE_FORMAT_QUALCOMM_PUREVOICE)
            return ("Qualcomm PureVoice");
        if (SubType.Data1 == WAVE_FORMAT_QUALCOMM_HALFRATE)
            return ("Qualcomm Half-Rate");
        if (SubType.Data1 == WAVE_FORMAT_TUBGSM)
            return ("Ring Zero Systems TUBGSM");
        if (SubType.Data1 == WAVE_FORMAT_MSAUDIO1)
            return ("Microsoft Audio 1");
        if (SubType.Data1 == WAVE_FORMAT_WMAUDIO2)
            return ("Windows Media Audio 2");
        if (SubType.Data1 == WAVE_FORMAT_WMAUDIO3)
            return ("Windows Media Audio 3");
        if (SubType.Data1 == WAVE_FORMAT_WMAUDIO_LOSSLESS)
            return ("Window Media Audio Lossless");
        if (SubType.Data1 == WAVE_FORMAT_WMASPDIF)
            return ("Windows Media Audio SPDIF");
        if (SubType.Data1 == WAVE_FORMAT_UNISYS_NAP_ADPCM)
            return ("Unisys ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_UNISYS_NAP_ULAW)
            return ("Unisys uLaw");
        if (SubType.Data1 == WAVE_FORMAT_UNISYS_NAP_ALAW)
            return ("Unisys aLaw");
        if (SubType.Data1 == WAVE_FORMAT_UNISYS_NAP_16K)
            return ("Unisys 16k");
        if (SubType.Data1 == WAVE_FORMAT_SYCOM_ACM_SYC008)
            return ("SyCom ACM SYC008");
        if (SubType.Data1 == WAVE_FORMAT_SYCOM_ACM_SYC701_G726L)
            return ("SyCom ACM SYC701 G726L");
        if (SubType.Data1 == WAVE_FORMAT_SYCOM_ACM_SYC701_CELP54)
            return ("SyCom ACM SYC701 CELP54");
        if (SubType.Data1 == WAVE_FORMAT_SYCOM_ACM_SYC701_CELP68)
            return ("SyCom ACM SYC701 CELP68");
        if (SubType.Data1 == WAVE_FORMAT_KNOWLEDGE_ADVENTURE_ADPCM)
            return ("Knowledge Adventure ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_FRAUNHOFER_IIS_MPEG2_AAC)
            return ("Fraunhofer MPEG-2 AAC");
        if (SubType.Data1 == WAVE_FORMAT_DTS_DS)
            return ("DTS DS");
        if (SubType.Data1 == WAVE_FORMAT_CREATIVE_ADPCM)
            return ("Creative Labs ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_CREATIVE_FASTSPEECH8)
            return ("Creative Labs FastSpeech 8");
        if (SubType.Data1 == WAVE_FORMAT_CREATIVE_FASTSPEECH10)
            return ("Creative Labs FastSpeech 10");
        if (SubType.Data1 == WAVE_FORMAT_UHER_ADPCM)
            return ("UHER ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_ULEAD_DV_AUDIO)
            return ("Ulead DV Audio");
        if (SubType.Data1 == WAVE_FORMAT_ULEAD_DV_AUDIO_1)
            return ("Ulead DV Audio.1");
        if (SubType.Data1 == WAVE_FORMAT_QUARTERDECK)
            return ("Quarterdeck");
        if (SubType.Data1 == WAVE_FORMAT_ILINK_VC)
            return ("I-link VC");
        if (SubType.Data1 == WAVE_FORMAT_RAW_SPORT)
            return ("RAW SPORT");
        if (SubType.Data1 == WAVE_FORMAT_ESST_AC3)
            return ("ESS Technology AC3");
        if (SubType.Data1 == WAVE_FORMAT_GENERIC_PASSTHRU)
            return ("Generic Passthrough");
        if (SubType.Data1 == WAVE_FORMAT_IPI_HSX)
            return ("IPI HSX");
        if (SubType.Data1 == WAVE_FORMAT_IPI_RPELP)
            return ("IPI RPELP");
        if (SubType.Data1 == WAVE_FORMAT_CS2)
            return ("Consistent Software 2");
        if (SubType.Data1 == WAVE_FORMAT_SONY_SCX)
            return ("Sony SCX");
        if (SubType.Data1 == WAVE_FORMAT_SONY_SCY)
            return ("Sony SCY");
        if (SubType.Data1 == WAVE_FORMAT_SONY_ATRAC3)
            return ("Sony ATRAC3");
        if (SubType.Data1 == WAVE_FORMAT_SONY_SPC)
            return ("Sony SPC");
        if (SubType.Data1 == WAVE_FORMAT_TELUM_AUDIO)
            return ("Telum Audio");
        if (SubType.Data1 == WAVE_FORMAT_TELUM_IA_AUDIO)
            return ("Telum IA Audio");
        if (SubType.Data1 == WAVE_FORMAT_NORCOM_VOICE_SYSTEMS_ADPCM)
            return ("Norcom ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_FM_TOWNS_SND)
            return ("Fujitsu Towns Sound");
        if (SubType.Data1 == WAVE_FORMAT_MICRONAS)
            return ("Micronas");
        if (SubType.Data1 == WAVE_FORMAT_MICRONAS_CELP833)
            return ("Micronas CELP833");
        if (SubType.Data1 == WAVE_FORMAT_BTV_DIGITAL)
            return ("Brooktree Digital");
        if (SubType.Data1 == WAVE_FORMAT_INTEL_MUSIC_CODER)
            return ("Intel Music Coder");
        if (SubType.Data1 == WAVE_FORMAT_INDEO_AUDIO)
            return ("Indeo Audio");
        if (SubType.Data1 == WAVE_FORMAT_QDESIGN_MUSIC)
            return ("QDesign Music");
        if (SubType.Data1 == WAVE_FORMAT_ON2_VP7_AUDIO)
            return ("On2 VP7");
        if (SubType.Data1 == WAVE_FORMAT_ON2_VP6_AUDIO)
            return ("On2 VP6");
        if (SubType.Data1 == WAVE_FORMAT_VME_VMPCM)
            return ("AT&T VME VMPCM");
        if (SubType.Data1 == WAVE_FORMAT_TPC)
            return ("AT&T TPC");
        if (SubType.Data1 == WAVE_FORMAT_LIGHTWAVE_LOSSLESS)
            return ("Lightwave Lossless");
        if (SubType.Data1 == WAVE_FORMAT_OLIGSM)
            return ("Olivetti GSM");
        if (SubType.Data1 == WAVE_FORMAT_OLIADPCM)
            return ("Olivetti ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_OLICELP)
            return ("Olivetti CELP");
        if (SubType.Data1 == WAVE_FORMAT_OLISBC)
            return ("Olivetti SBC");
        if (SubType.Data1 == WAVE_FORMAT_OLIOPR)
            return ("Olivetti OPR");
        if (SubType.Data1 == WAVE_FORMAT_LH_CODEC)
            return ("Lernout & Hauspie");
        if (SubType.Data1 == WAVE_FORMAT_LH_CODEC_CELP)
            return ("Lernout & Hauspie CELP");
        if (SubType.Data1 == WAVE_FORMAT_LH_CODEC_SBC8)
            return ("Lernout & Hauspie SBC8");
        if (SubType.Data1 == WAVE_FORMAT_LH_CODEC_SBC12)
            return ("Lernout & Hauspie SBC12");
        if (SubType.Data1 == WAVE_FORMAT_LH_CODEC_SBC16)
            return ("Lernout & Hauspie SBC16");
        if (SubType.Data1 == WAVE_FORMAT_NORRIS)
            return ("Norris");
        if (SubType.Data1 == WAVE_FORMAT_ISIAUDIO_2)
            return ("ISIAudio 2");
        if (SubType.Data1 == WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS)
            return ("AT&T SoundSpace Musicompress");
        if (SubType.Data1 == WAVE_FORMAT_MPEG_ADTS_AAC)
            return ("MPEG ADT5 AAC");
        if (SubType.Data1 == WAVE_FORMAT_MPEG_RAW_AAC)
            return ("MPEG RAW AAC");
        if (SubType.Data1 == WAVE_FORMAT_MPEG_LOAS)
            return ("MPEG LOAS");
        if (SubType.Data1 == WAVE_FORMAT_NOKIA_MPEG_ADTS_AAC)
            return ("Nokia MPEG ADT5 AAC");
        if (SubType.Data1 == WAVE_FORMAT_NOKIA_MPEG_RAW_AAC)
            return ("Nokia MPEG RAW AAC");
        if (SubType.Data1 == WAVE_FORMAT_VODAFONE_MPEG_ADTS_AAC)
            return ("Vodafone MPEG ADTS AAC");
        if (SubType.Data1 == WAVE_FORMAT_VODAFONE_MPEG_RAW_AAC)
            return ("Vodafone MPEG RAW AAC");
        if (SubType.Data1 == WAVE_FORMAT_MPEG_HEAAC)
            return ("MPEG HEAAC");
        if (SubType.Data1 == WAVE_FORMAT_VOXWARE_RT24_SPEECH)
            return ("voxware RT24 Speech");
        if (SubType.Data1 == WAVE_FORMAT_SONICFOUNDRY_LOSSLESS)
            return ("Sonic Foundry Lossless");
        if (SubType.Data1 == WAVE_FORMAT_INNINGS_TELECOM_ADPCM)
            return ("Innings ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_LUCENT_SX8300P)
            return ("Lucent SX8300P");
        if (SubType.Data1 == WAVE_FORMAT_LUCENT_SX5363S)
            return ("Lucent SX5363S");
        if (SubType.Data1 == WAVE_FORMAT_CUSEEME)
            return ("CUSeeMe");
        if (SubType.Data1 == WAVE_FORMAT_NTCSOFT_ALF2CM_ACM)
            return ("NTCSoft ALF2CM ACM");
        if (SubType.Data1 == WAVE_FORMAT_DVM)
            return ("FAST Multimedia DVM");
        if (SubType.Data1 == WAVE_FORMAT_DTS2)
            return ("DTS2");
        if (SubType.Data1 == WAVE_FORMAT_MAKEAVIS)
            return ("MAKEAVIS");
        if (SubType.Data1 == WAVE_FORMAT_DIVIO_MPEG4_AAC)
            return ("Divio MPEG-4 AAC");
        if (SubType.Data1 == WAVE_FORMAT_NOKIA_ADAPTIVE_MULTIRATE)
            return ("Nokia Adaptive Multirate");
        if (SubType.Data1 == WAVE_FORMAT_DIVIO_G726)
            return ("Divio G726");
        if (SubType.Data1 == WAVE_FORMAT_LEAD_SPEECH)
            return ("LEAD Speech");
        if (SubType.Data1 == WAVE_FORMAT_LEAD_VORBIS)
            return ("LEAD Vorbis");
        if (SubType.Data1 == WAVE_FORMAT_WAVPACK_AUDIO)
            return ("xiph.org WavPack");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_1)
            return ("Ogg Vorbis Mode 1");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_2)
            return ("Ogg Vorbis Mode 2");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_3)
            return ("Ogg Vorbis Mode 3");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_1_PLUS)
            return ("Ogg Vorbis Mode 1 Plus");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_2_PLUS)
            return ("Ogg Vorbis Mode 2 Plus");
        if (SubType.Data1 == WAVE_FORMAT_OGG_VORBIS_MODE_3_PLUS)
            return ("Ogg Vorbis Mode 3 Plus");
        if (SubType.Data1 == WAVE_FORMAT_3COM_NBX)
            return ("3COM NBX");
        if (SubType.Data1 == WAVE_FORMAT_FAAD_AAC)
            return ("FAAD AAC");
        if (SubType.Data1 == WAVE_FORMAT_AMR_NB)
            return ("AMR Narrowband");
        if (SubType.Data1 == WAVE_FORMAT_AMR_WB)
            return ("AMR Wideband");
        if (SubType.Data1 == WAVE_FORMAT_AMR_WP)
            return ("AMR Wideband Plus");
        if (SubType.Data1 == WAVE_FORMAT_GSM_AMR_CBR)
            return ("GSMA/3GPP CBR");
        if (SubType.Data1 == WAVE_FORMAT_GSM_AMR_VBR_SID)
            return ("GSMA/3GPP VBR SID");
        if (SubType.Data1 == WAVE_FORMAT_COMVERSE_INFOSYS_G723_1)
            return ("Converse Infosys G723.1");
        if (SubType.Data1 == WAVE_FORMAT_COMVERSE_INFOSYS_AVQSBC)
            return ("Converse Infosys AVQSBC");
        if (SubType.Data1 == WAVE_FORMAT_COMVERSE_INFOSYS_SBC)
            return ("Converse Infosys SBC");
        if (SubType.Data1 == WAVE_FORMAT_SYMBOL_G729_A)
            return ("Symbol Technologies G729.A");
        if (SubType.Data1 == WAVE_FORMAT_VOICEAGE_AMR_WB)
            return ("VoiceAge AMR Wideband");
        if (SubType.Data1 == WAVE_FORMAT_INGENIENT_G726)
            return ("Ingenient G726");
        if (SubType.Data1 == WAVE_FORMAT_MPEG4_AAC)
            return ("MPEG-4 AAC");
        if (SubType.Data1 == WAVE_FORMAT_ENCORE_G726)
            return ("Encore G726");
        if (SubType.Data1 == WAVE_FORMAT_ZOLL_ASAO)
            return ("ZOLL Medical ASAO");
        if (SubType.Data1 == WAVE_FORMAT_SPEEX_VOICE)
            return ("xiph.org Speex Voice");
        if (SubType.Data1 == WAVE_FORMAT_VIANIX_MASC)
            return ("Vianix MASC");
        if (SubType.Data1 == WAVE_FORMAT_WM9_SPECTRUM_ANALYZER)
            return ("Windows Media 9 Spectrum Analyzer");
        if (SubType.Data1 == WAVE_FORMAT_WMF_SPECTRUM_ANAYZER)
            return ("Windows Media Foundation Spectrum Analyzer");
        if (SubType.Data1 == WAVE_FORMAT_GSM_610)
            return ("GSM 610");
        if (SubType.Data1 == WAVE_FORMAT_GSM_620)
            return ("GSM 620");
        if (SubType.Data1 == WAVE_FORMAT_GSM_660)
            return ("GSM 660");
        if (SubType.Data1 == WAVE_FORMAT_GSM_690)
            return ("GSM 690");
        if (SubType.Data1 == WAVE_FORMAT_GSM_ADAPTIVE_MULTIRATE_WB)
            return ("GSM Adaptive Multirate Wideband");
        if (SubType.Data1 == WAVE_FORMAT_POLYCOM_G722)
            return ("Polycom G722");
        if (SubType.Data1 == WAVE_FORMAT_POLYCOM_G728)
            return ("Polycom G728");
        if (SubType.Data1 == WAVE_FORMAT_POLYCOM_G729_A)
            return ("Polycom G729.A");
        if (SubType.Data1 == WAVE_FORMAT_POLYCOM_SIREN)
            return ("Polycom Siren");
        if (SubType.Data1 == WAVE_FORMAT_GLOBAL_IP_ILBC)
            return ("Global IP ILBC");
        if (SubType.Data1 == WAVE_FORMAT_RADIOTIME_TIME_SHIFT_RADIO)
            return ("RadioTime");
        if (SubType.Data1 == WAVE_FORMAT_NICE_ACA)
            return ("Nice Systems ACA");
        if (SubType.Data1 == WAVE_FORMAT_NICE_ADPCM)
            return ("Nice Systems ADPCM");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G721)
            return ("Vocord G721");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G726)
            return ("Vocord G726");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G722_1)
            return ("Vocord G722.1");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G728)
            return ("Vocord G728");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G729)
            return ("Vocord G729");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G729_A)
            return ("Vocord G729.A");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_G723_1)
            return ("Vocord G723.1");
        if (SubType.Data1 == WAVE_FORMAT_VOCORD_LBC)
            return ("Vocord LBC");
        if (SubType.Data1 == WAVE_FORMAT_NICE_G728)
            return ("Nice Systems G728");
        if (SubType.Data1 == WAVE_FORMAT_FRACE_TELECOM_G729)
            return ("France Telecom G729");
        if (SubType.Data1 == WAVE_FORMAT_CODIAN)
            return ("CODIAN");
        if (SubType.Data1 == WAVE_FORMAT_FLAC)
            return ("flac.sourceforge.net");
    }

    // unknown type
    return "Unknown SubType";
}
