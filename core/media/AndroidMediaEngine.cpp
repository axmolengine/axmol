#if defined(__ANDROID__)
#    include "AndroidMediaEngine.h"
#    include "platform/android/jni/JniHelper.h"

extern "C" {
JNIEXPORT void JNICALL Java_org_axmol_lib_AxmolMediaEngine_nativeHandleEvent(JNIEnv* env, jclass, jlong pME, int arg1)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->handleEvent(arg1);
}
JNIEXPORT void JNICALL Java_org_axmol_lib_AxmolMediaEngine_nativeHandleVideoSample(JNIEnv* env,
                                                                              jclass,
                                                                              jlong pME,
                                                                              jobject sampleBuffer,
                                                                              int sampleLen,
                                                                              int outputX,
                                                                              int outputY,
                                                                              int videoX,
                                                                              int videoY)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    auto sampleData = static_cast<uint8_t*>(env->GetDirectBufferAddress(sampleBuffer));

    mediaEngine->handleVideoSample(sampleData, sampleLen, outputX, outputY, videoX, videoY);
}
}

NS_AX_BEGIN

static const char* className                    = "org.axmol.lib.AxmolMediaEngine";
static const std::string_view assets_uri_prefix = "file://assets/";

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
void AndroidMediaEngine::SetMediaEventCallback(MEMediaEventCallback cb1)
{
    _eventCallback = std::move(cb1);
}

void AndroidMediaEngine::SetAutoPlay(bool bAutoPlay)
{
    if (context)
        JniHelper::callVoidMethod(className, "setAutoPlay", context, bAutoPlay);
}
bool AndroidMediaEngine::Open(std::string_view sourceUri)
{
    if (!context)
        return false;

    std::string android_asset_uri;
    if (cxx20::starts_with(sourceUri, FILE_URL_SCHEME))
    {
        if (cxx20::starts_with(sourceUri, assets_uri_prefix))
        {  // not abs path, must be assets/, insert:
            android_asset_uri = "file:///android_asset/"sv;
            android_asset_uri += sourceUri.substr(assets_uri_prefix.length());
            sourceUri = android_asset_uri;
        }
    }

    // call java object method
    return JniHelper::callBooleanMethod(className, "open", context, sourceUri);
}
bool AndroidMediaEngine::Close()
{
    return context && JniHelper::callBooleanMethod(className, "close", context);
}
bool AndroidMediaEngine::SetLoop(bool bLooping)
{
    return context && JniHelper::callBooleanMethod(className, "setLoop", context, bLooping);
}
bool AndroidMediaEngine::SetRate(double fRate)
{
    return context && JniHelper::callBooleanMethod(className, "setRate", context, fRate);
}
bool AndroidMediaEngine::SetCurrentTime(double fSeekTimeInSec)
{
    return context && JniHelper::callBooleanMethod(className, "setCurrentTime", context, fSeekTimeInSec);
}
bool AndroidMediaEngine::Play()
{
    return context && JniHelper::callBooleanMethod(className, "play", context);
}
bool AndroidMediaEngine::Pause()
{
    return context && JniHelper::callBooleanMethod(className, "pause", context);
}
bool AndroidMediaEngine::Stop()
{
    return context && JniHelper::callBooleanMethod(className, "stop", context);
}
MEMediaState AndroidMediaEngine::GetState() const
{
    return context ? (MEMediaState)JniHelper::callIntMethod(className, "getState", context) : MEMediaState::Closed;
}

bool AndroidMediaEngine::TransferVideoFrame(std::function<void(const MEVideoFrame&)> callback)
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
            assert(static_cast<int>(frame._dataLen) >= frame._vpd._dim.x * frame._vpd._dim.y * 3 / 2);
            callback(frame);
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
                                           int videoY)
{
    std::unique_lock<std::mutex> lck(_frameBuffer1Mtx);
    _frameBuffer1.assign(buf, buf + len, std::true_type{});
    _outputDim.set(outputX, outputY);
    _videoDim.set(videoX, videoY);
}

void AndroidMediaEngine::handleEvent(int event)
{
    if (_eventCallback)
        _eventCallback((MEMediaEventType)event);
}

NS_AX_END

#endif
