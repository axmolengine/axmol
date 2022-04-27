/******************************************************************************

 @File         PVRTDecompress.h

 @Title

 @Copyright    Copyright (C) 2000 - 2008 by Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  PVRTC Texture Decompression.

 @Compression  https://developer.imaginationtech.com/pvrtextool/
               https://cdn.imgtec.com/sdk/OFFLINE/PVRTexToolSetup-2021_R2.exe

 ******************************************************************************/

#ifndef __PVR_H__
#define __PVR_H__

int PVRTDecompressPVRTC(const void* const pCompressedData,
                        const int XDim,
                        const int YDim,
                        void* pDestData,
                        const bool Do2bitMode);

#endif  //__PVR_H__
