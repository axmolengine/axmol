
# common headers and sources
set(COCOS_AUDIO_HEADER
    audio/alconfig.h
    audio/AudioEngine.h
    audio/AudioMacros.h
    audio/AudioDecoderManager.h
    audio/AudioDecoder.h
    audio/AudioDecoderOgg.h
    audio/AudioPlayer.h
    audio/AudioCache.h
    audio/AudioEngineImpl.h
    )
    
set(COCOS_AUDIO_SRC
    audio/AudioEngine.cpp
    audio/AudioDecoderManager.cpp
    audio/AudioDecoder.cpp
    audio/AudioDecoderOgg.cpp
    audio/AudioPlayer.cpp
    audio/AudioCache.cpp
    )
    
# stupid, clang always compie .mm as objc/cpp mix obj
if(ANDROID OR LINUX)
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
        audio/linux-link.cpp
    )
else()
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
        audio/AudioEngineImpl.mm
    )
endif()

# Apple, we use system audio decoder
if(APPLE)
    set(COCOS_AUDIO_HEADER ${COCOS_AUDIO_HEADER}
                        audio/AudioDecoderEXT.h)
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
                        audio/AudioDecoderEXT.mm)
else()
    set(COCOS_AUDIO_HEADER ${COCOS_AUDIO_HEADER}
                        audio/AudioDecoderMp3.h
                        audio/AudioDecoderWav.h
    )
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
                        audio/AudioDecoderMp3.cpp
                        audio/AudioDecoderWav.cpp
    )
endif()
