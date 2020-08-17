
# common headers and sources
set(COCOS_AUDIO_HEADER
    audio/include/alconfig.h
    audio/include/AudioEngine.h
    audio/include/Export.h
    audio/include/AudioMacros.h
    audio/include/AudioDecoderManager.h
    audio/include/AudioDecoder.h
    audio/include/AudioDecoderOgg.h
    audio/include/AudioPlayer.h
    audio/include/AudioCache.h
    audio/include/AudioEngineImpl.h
    )
    
set(COCOS_AUDIO_SRC
    audio/src/AudioEngine.cpp
    audio/src/AudioDecoderManager.cpp
    audio/src/AudioDecoder.cpp
    audio/src/AudioDecoderOgg.cpp
    audio/src/AudioPlayer.cpp
    audio/src/AudioCache.cpp
    )
    
# stupid, clang always compie .mm as objc/cpp mix obj
if(ANDROID)
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
        audio/src/android-link.cpp
    )
else()
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
        audio/src/AudioEngineImpl.mm
    )
endif()

# Apple, we use system audio decoder
if(APPLE)
    set(COCOS_AUDIO_HEADER ${COCOS_AUDIO_HEADER}
                        audio/include/AudioDecoderEXT.h)
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
                        audio/src/AudioDecoderEXT.mm)
else()
    set(COCOS_AUDIO_HEADER ${COCOS_AUDIO_HEADER}
                        audio/include/AudioDecoderMp3.h
                        audio/include/AudioDecoderWav.h
    )
    set(COCOS_AUDIO_SRC ${COCOS_AUDIO_SRC}
                        audio/src/AudioDecoderMp3.cpp
                        audio/src/AudioDecoderWav.cpp
    )
endif()
