#if defined(__ANDROID__)
#    include "AndroidMediaEngine.h"
#    include "platform/android/jni/JniHelper.h"

extern "C" {
JNIEXPORT void JNICALL Java_org_axmol_lib_AxmolMediaEngine_nativeHandleEvent(JNIEnv* env, jclass, jlong pME, int arg1)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->fireMediaEvent((ax::MEMediaEventType)arg1);
}
JNIEXPORT void JNICALL Java_org_axmol_lib_AxmolMediaEngine_nativeHandleVideoSample(JNIEnv* env,
                                                                              jclass,
                                                                              jlong pME,
                                                                              jobject sampleBuffer,
                                                                              int sampleLen,
                                                                              int outputX,
                                                                              int outputY,
                                                                              int videoX,
                                                                              int videoY,
                                                                              int rotation)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    auto sampleData = static_cast<uint8_t*>(env->GetDirectBufferAddress(sampleBuffer));

    mediaEngine->handleVideoSample(sampleData, sampleLen, outputX, outputY, videoX, videoY, rotation);
}
}

NS_AX_BEGIN

static const char* className                    = "org.axmol.lib.AxmolMediaEngine";

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

            ax::MEVideoFrame frame{buffer.data(), buffer.data() + _outputDim.x * _outputDim.y, buffer.size(),
                                   ax::MEVideoPixelDesc{ax::MEVideoPixelFormat::NV12, _outputDim}, _videoDim};
            frame._vpd._rotation = _videoRotation;
            assert(static_cast<int>(frame._dataLen) >= frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2);
            _onVideoFrame(frame);
            _frameBuffer2.clear();
            return true;
        }
    }
    return false;
}

void AndroidMediaEngine::handleVideoSample(const uint8_t* buf,
                                           size_t len,
                                           int outputX,
                                           int outputY,
                                           int videoX,
                                           int videoY,
                                           int rotation)
{
    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    _frameBuffer1.assign(buf, buf + len);
    _outputDim.set(outputX, outputY);
    _videoDim.set(videoX, videoY);
    _videoRotation = rotation;
}

NS_AX_END

#endif
