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

#if defined(__ANDROID__)
#    include "AndroidMediaEngine.h"
#    include "platform/android/jni/JniHelper.h"

extern "C" {
JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaEngine_nativeFireEvent(JNIEnv* env, jclass, jlong pME, int arg1)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_fireMediaEvent((ax::MEMediaEventType)arg1);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaEngine_nativeStoreVideoMeta(JNIEnv* env,
                                                                                jclass,
                                                                                jlong pME,
                                                                                int outputX,
                                                                                int outputY,
                                                                                int videoX,
                                                                                int videoY,
                                                                                int cbcrOffset,
                                                                                int rotation,
                                                                                int videoPF)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_storeVideoMeta(outputX, outputY, videoX, videoY, cbcrOffset, rotation, videoPF);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaEngine_nativeStoreLastVideoSample(JNIEnv* env,
                                                                                      jclass,
                                                                                      jlong pME,
                                                                                      jobject sampleBuffer,
                                                                                      int sampleLen)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    auto sampleData = static_cast<uint8_t*>(env->GetDirectBufferAddress(sampleBuffer));

    mediaEngine->_storeLastVideoSample(sampleData, sampleLen);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaEngine_nativeStoreDuration(JNIEnv* env,
                                                                               jclass,
                                                                               jlong pME,
                                                                               double duration)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_storeDuration(duration);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaEngine_nativeStoreCurrentTime(JNIEnv* env,
                                                                                  jclass,
                                                                                  jlong pME,
                                                                                  double currentTime)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_storeCurrentTime(currentTime);
}
}

namespace ax
{

static const char* className = "dev.axmol.lib.AxmolMediaEngine";

AndroidMediaEngine::AndroidMediaEngine()
{
    // create java object
    context = JniHelper::callStaticObjectMethod(className, "createMediaEngine");
    if (context)
        JniHelper::callVoidMethod(className, "bindNativeObject", context, (jlong)(uintptr_t)this);
}
AndroidMediaEngine::~AndroidMediaEngine()
{
    if (context)
    {
        // clear callback
        JniHelper::callVoidMethod(className, "bindNativeObject", context, (jlong)(uintptr_t)0);
        JniHelper::getEnv()->DeleteGlobalRef(static_cast<jobject>(context));
        context = nullptr;
    }
}

void AndroidMediaEngine::setAutoPlay(bool bAutoPlay)
{
    if (context)
        JniHelper::callVoidMethod(className, "setAutoPlay", context, bAutoPlay);
}
bool AndroidMediaEngine::open(std::string_view sourceUri)
{
    return context && JniHelper::callBooleanMethod(className, "open", context, sourceUri);
}
bool AndroidMediaEngine::close()
{
    return context && JniHelper::callBooleanMethod(className, "close", context);
}
bool AndroidMediaEngine::setLoop(bool bLooping)
{
    return context && JniHelper::callBooleanMethod(className, "setLoop", context, bLooping);
}
bool AndroidMediaEngine::setRate(double fRate)
{
    return context && JniHelper::callBooleanMethod(className, "setRate", context, fRate);
}
bool AndroidMediaEngine::setCurrentTime(double fSeekTimeInSec)
{
    return context && JniHelper::callBooleanMethod(className, "setCurrentTime", context, fSeekTimeInSec);
}
bool AndroidMediaEngine::play()
{
    return context && JniHelper::callBooleanMethod(className, "play", context);
}
bool AndroidMediaEngine::pause()
{
    return context && JniHelper::callBooleanMethod(className, "pause", context);
}
bool AndroidMediaEngine::stop()
{
    return context && JniHelper::callBooleanMethod(className, "stop", context);
}
bool AndroidMediaEngine::isPlaybackEnded() const
{
    return context && JniHelper::callBooleanMethod(className, "isPlaybackEnded", context);
}
MEMediaState AndroidMediaEngine::getState() const
{
    return context ? (MEMediaState)JniHelper::callIntMethod(className, "getState", context) : MEMediaState::Closed;
}

bool AndroidMediaEngine::transferVideoFrame()
{
    if (context)
    {
        std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
        if (!_frameBuffer1.empty())
        {
            _frameBuffer2.swap(_frameBuffer1);
            lck.unlock();  // unlock immidiately before invoke user callback (maybe upload buffer to GPU)

            auto& buffer = _frameBuffer2;

            ax::MEVideoFrame frame{buffer.data(), buffer.data() + _cbcrOffset, buffer.size(),
                                   ax::MEVideoPixelDesc{static_cast<ax::MEVideoPixelFormat>(_videoPF), _outputDim},
                                   _videoDim};
            frame._vpd._rotation = _videoRotation;
            assert(static_cast<int>(frame._dataLen) >= frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2);
            _onVideoFrame(frame);
            _frameBuffer2.clear();
            return true;
        }
    }
    return false;
}

void AndroidMediaEngine::_storeVideoMeta(int outputX,
                                         int outputY,
                                         int videoX,
                                         int videoY,
                                         int cbcrOffset,
                                         int rotation,
                                         int videoPF)
{
    _outputDim.set(outputX, outputY);
    _videoDim.set(videoX, videoY);
    _cbcrOffset    = cbcrOffset;
    _videoRotation = rotation;
    _videoPF       = videoPF;
}

void AndroidMediaEngine::_storeLastVideoSample(const uint8_t* buf, size_t len)
{
    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    _frameBuffer1.assign(buf, buf + len);
}

}  // namespace ax

#endif
