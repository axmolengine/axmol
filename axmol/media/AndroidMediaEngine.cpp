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
#    include "axmol/media/AndroidMediaEngine.h"
#    include "axmol/platform/android/jni/JniHelper.h"

extern "C" {
JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaPlayer_nativeFireEvent(JNIEnv* env, jclass, jlong pME, int arg1)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_fireMediaEvent((ax::MEMediaEventType)arg1);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaPlayer_nativeSetDuration(JNIEnv* env,
                                                                             jclass,
                                                                             jlong pME,
                                                                             double duration)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->_setDuration(duration);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaPlayer_nativeSetVideoMeta(JNIEnv* env,
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

    mediaEngine->_setVideoMeta(outputX, outputY, videoX, videoY, cbcrOffset, rotation, videoPF);
}

JNIEXPORT void JNICALL Java_dev_axmol_lib_AxmolMediaPlayer_nativeProcessVideoFrame(JNIEnv* env,
                                                                                   jclass,
                                                                                   jlong pME,
                                                                                   jobject sampleBuffer,
                                                                                   int sampleLen,
                                                                                   jlong presentationTimeUs)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    auto sampleData = static_cast<uint8_t*>(env->GetDirectBufferAddress(sampleBuffer));
    if (sampleData)
        mediaEngine->_processVideoFrame(sampleData, sampleLen, presentationTimeUs);
}
}

namespace ax
{

static const char* className = "dev.axmol.lib.AxmolMediaPlayer";

AndroidMediaEngine::AndroidMediaEngine()
{
    // Create Java MediaPlayer and take ownership of the global reference.
    // Note: JniHelper::callStaticObjectMethod returns a jobject that has been
    // promoted to a GlobalRef (via NewGlobalRef). Do NOT call NewGlobalRef again;
    // ensure DeleteGlobalRef(_mediaPlayer) is called during native teardown.
    _mediaPlayer = JniHelper::callStaticObjectMethod(className, "createMediaPlayer", (jlong)(uintptr_t)this);
}
AndroidMediaEngine::~AndroidMediaEngine()
{
    if (_mediaPlayer)
    {
        // clear callback
        JniHelper::callVoidMethod(className, "dispose", _mediaPlayer);
        JniHelper::getEnv()->DeleteGlobalRef(_mediaPlayer);
        _mediaPlayer = nullptr;
    }
}

void AndroidMediaEngine::setAutoPlay(bool bAutoPlay)
{
    if (_mediaPlayer)
        JniHelper::callVoidMethod(className, "setAutoPlay", _mediaPlayer, bAutoPlay);
}
bool AndroidMediaEngine::open(std::string_view sourceUri)
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "open", _mediaPlayer, sourceUri);
}
bool AndroidMediaEngine::close()
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "close", _mediaPlayer);
}
bool AndroidMediaEngine::setLoop(bool bLooping)
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "setLoop", _mediaPlayer, bLooping);
}
bool AndroidMediaEngine::setRate(double fRate)
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "setRate", _mediaPlayer, fRate);
}
bool AndroidMediaEngine::setCurrentTime(double fSeekTimeInSec)
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "setCurrentTime", _mediaPlayer, fSeekTimeInSec);
}
bool AndroidMediaEngine::play()
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "play", _mediaPlayer);
}
bool AndroidMediaEngine::pause()
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "pause", _mediaPlayer);
}
bool AndroidMediaEngine::stop()
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "stop", _mediaPlayer);
}
bool AndroidMediaEngine::isPlaybackEnded() const
{
    return _mediaPlayer && JniHelper::callBooleanMethod(className, "isPlaybackEnded", _mediaPlayer);
}
MEMediaState AndroidMediaEngine::getState() const
{
    return _mediaPlayer ? (MEMediaState)JniHelper::callIntMethod(className, "getState", _mediaPlayer)
                        : MEMediaState::Closed;
}

bool AndroidMediaEngine::transferVideoFrame()
{
    if (_mediaPlayer)
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

void AndroidMediaEngine::_setVideoMeta(int outputX,
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

void AndroidMediaEngine::_processVideoFrame(const uint8_t* sampleData, size_t sampleLen, int64_t presentationTimeUs)
{
    _currentTime = presentationTimeUs / 1000000.0;

    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    _frameBuffer1.assign(sampleData, sampleData + sampleLen);
}

}  // namespace ax

#endif
