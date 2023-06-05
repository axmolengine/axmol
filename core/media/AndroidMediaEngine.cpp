#if defined(__ANDROID__)
#    include "AndroidMediaEngine.h"
#    include "platform/android/jni/JniHelper.h"

extern "C" {
JNIEXPORT void JNICALL Java_org_axmol_lib_MediaEngine_nativeHandleEvent(JNIEnv* env, jclass, jlong pME, int arg1)
{
    auto mediaEngine = (ax::AndroidMediaEngine*)((uintptr_t)pME);
    if (!mediaEngine)
        return;

    mediaEngine->handleEvent(arg1);
}
JNIEXPORT void JNICALL Java_org_axmol_lib_MediaEngine_nativeHandleVideoSample(JNIEnv* env,
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

    auto sampleData = (uint8_t*)env->GetDirectBufferAddress(sampleBuffer);

    mediaEngine->handleVideoSample(sampleData, sampleLen, outputX, outputY, videoX, videoY);
}
}

NS_AX_BEGIN

static const char* className                    = "org.axmol.lib.MediaEngine";
static const std::string_view assets_uri_prefix = "file://assets/";

struct ExoPlayer
{
    static const int STATE_IDLE      = 1;
    static const int STATE_BUFFERING = 2;
    static const int STATE_READY     = 3;
    static const int STATE_ENDED     = 4;
};

AndroidMediaEngine::AndroidMediaEngine()
{
    // create java object
    context = JniHelper::callStaticObjectMethod(className, "CreateMediaEngine");
    if (context)
        JniHelper::callVoidMethod(className, "BindNativeObject", context, (jlong)(uintptr_t)this);
}
AndroidMediaEngine::~AndroidMediaEngine()
{

    if (context)
    {
        // clear callback
        JniHelper::callVoidMethod(className, "BindNativeObject", context, (jlong)(uintptr_t)0);
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
        JniHelper::callVoidMethod(className, "SetAutoPlay", context, bAutoPlay);
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
    JniHelper::callBooleanMethod(className, "Open", context, sourceUri);
    return true;
}
bool AndroidMediaEngine::Close()
{
    if (context)
        JniHelper::callBooleanMethod(className, "Close", context);
    return true;
}
bool AndroidMediaEngine::SetLoop(bool bLooping)
{
    if (context)
        JniHelper::callBooleanMethod(className, "SetLoop", context, bLooping);
    return true;
}
bool AndroidMediaEngine::SetRate(double fRate)
{
    if (context)
        JniHelper::callBooleanMethod(className, "SetRate", context, fRate);
    return true;
}
bool AndroidMediaEngine::SetCurrentTime(double fSeekTimeInSec)
{
    if (context)
        JniHelper::callBooleanMethod(className, "SetCurrentTime", context, fSeekTimeInSec);
    return true;
}
bool AndroidMediaEngine::Play()
{
    if (context)
        JniHelper::callBooleanMethod(className, "Play", context);
    return true;
}
bool AndroidMediaEngine::Pause()
{
    if (context)
        JniHelper::callBooleanMethod(className, "Pause", context);
    return true;
}
bool AndroidMediaEngine::Stop()
{
    if (context)
        JniHelper::callBooleanMethod(className, "Stop", context);
    return true;
}
MEMediaState AndroidMediaEngine::GetState() const
{
    if (context)
    {
        int state = JniHelper::callIntMethod(className, "GetState", context);
        return (MEMediaState)state;
    }
    return MEMediaState::Closed;
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
