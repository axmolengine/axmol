
set(COCOS_AUDIO_SRC
    audio/AudioEngine.cpp
    )
    
message(STATUS "[AudioEngine] CC_USE_ALSOFT_ON_APPLE=${CC_USE_ALSOFT_ON_APPLE}") 

if(WINDOWS)

    set(COCOS_AUDIO_PLATFORM_HEADER
        audio/include/AudioDecoderManager.h
        audio/include/AudioDecoder.h
        audio/include/AudioPlayer.h
        audio/include/AudioDecoderOgg.h        
        audio/include/AudioEngineImpl.h
        audio/include/AudioDecoderMp3.h
        audio/include/AudioDecoderWav.h
        audio/include/AudioCache.h
        )

    set(COCOS_AUDIO_PLATFORM_SRC
        audio/AudioEngineImpl.cpp
        audio/AudioCache.cpp
        audio/AudioPlayer.cpp
        audio/AudioDecoder.cpp
        audio/AudioDecoderManager.cpp
        audio/AudioDecoderMp3.cpp
        audio/AudioDecoderOgg.cpp
        audio/AudioDecoderWav.cpp
        )

elseif(ANDROID)

    set(COCOS_AUDIO_PLATFORM_HEADER
        audio/include/AudioEngineImpl.h
        audio/include/AudioCache.h
        audio/include/AudioPlayer.h
        audio/include/AudioDecoder.h
        audio/include/AudioDecoderManager.h
        audio/include/AudioDecoderMp3.h
        audio/include/AudioDecoderOgg.h
        audio/include/AudioDecoderWav.h
        )

    set(COCOS_AUDIO_PLATFORM_SRC
        audio/AudioEngineImpl.cpp
        audio/AudioCache.cpp
        audio/AudioPlayer.cpp
        audio/AudioDecoder.cpp
        audio/AudioDecoderManager.cpp
        audio/AudioDecoderMp3.cpp
        audio/AudioDecoderOgg.cpp
        audio/AudioDecoderWav.cpp
        )

elseif(LINUX)
    set(COCOS_AUDIO_PLATFORM_HEADER
        audio/include/AudioDecoderManager.h
        audio/include/AudioDecoder.h
        audio/include/AudioPlayer.h
        audio/include/AudioDecoderOgg.h        
        audio/include/AudioEngineImpl.h
        audio/include/AudioDecoderMp3.h
        audio/include/AudioDecoderWav.h
        audio/include/AudioCache.h
        )

    set(COCOS_AUDIO_PLATFORM_SRC
        audio/AudioEngineImpl.cpp
        audio/AudioCache.cpp
        audio/AudioPlayer.cpp
        audio/AudioDecoder.cpp
        audio/AudioDecoderManager.cpp
        audio/AudioDecoderMp3.cpp
        audio/AudioDecoderOgg.cpp
        audio/AudioDecoderWav.cpp
        )

elseif(APPLE)
    # common
    set(COCOS_AUDIO_PLATFORM_HEADER
        audio/include/AudioDecoderManager.h
        audio/include/AudioDecoder.h
        audio/include/AudioDecoderOgg.h
        audio/include/AudioDecoderWav.h
        audio/include/AudioCache.h
        audio/apple/AudioDecoderEXT.h
        )
    
    set(COCOS_AUDIO_PLATFORM_SRC
        audio/AudioDecoderManager.cpp
        audio/AudioDecoder.cpp
        audio/AudioDecoderOgg.cpp
        audio/AudioDecoderWav.cpp
        audio/apple/AudioDecoderEXT.mm
        )

    if(CC_USE_ALSOFT_ON_APPLE)
        set(COCOS_AUDIO_PLATFORM_HEADER ${COCOS_AUDIO_PLATFORM_HEADER}
          audio/include/AudioPlayer.h
          audio/include/AudioEngineImpl.h
        )
        set(COCOS_AUDIO_PLATFORM_SRC ${COCOS_AUDIO_PLATFORM_SRC}
          audio/AudioCache.cpp
          audio/AudioPlayer.cpp
          audio/AudioEngineImpl.cpp
        )
    else()
        set(COCOS_AUDIO_PLATFORM_HEADER ${COCOS_AUDIO_PLATFORM_HEADER}
          audio/apple/AudioPlayer.h
          audio/apple/AudioEngineImpl.h
        )
        set(COCOS_AUDIO_PLATFORM_SRC ${COCOS_AUDIO_PLATFORM_SRC}
          audio/apple/AudioCache.mm
          audio/apple/AudioPlayer.mm
          audio/apple/AudioEngineImpl.mm
        )
    endif()
endif()

set(COCOS_AUDIO_HEADER
    audio/include/AudioEngine.h
    audio/include/Export.h
    audio/include/AudioMacros.h
    ${COCOS_AUDIO_PLATFORM_HEADER}
    )
list(APPEND COCOS_AUDIO_SRC ${COCOS_AUDIO_PLATFORM_SRC})
