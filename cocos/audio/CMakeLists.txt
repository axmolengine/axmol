
set(COCOS_AUDIO_SRC
    audio/AudioEngine.cpp
    )

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
        audio/apple/AudioPlayer.h
		audio/apple/AudioDecoderEXT.h
        audio/apple/AudioEngineImpl.h
        )
    set(COCOS_AUDIO_PLATFORM_SRC
        audio/AudioDecoderManager.cpp
        audio/AudioDecoder.cpp
        audio/AudioDecoderOgg.cpp
        audio/AudioDecoderWav.cpp
	    audio/apple/AudioDecoderEXT.mm
        audio/apple/AudioCache.mm
        audio/apple/AudioEngineImpl.mm
        audio/apple/AudioPlayer.mm
        )
endif()

set(COCOS_AUDIO_HEADER
    audio/include/AudioEngine.h
    audio/include/Export.h
    audio/include/AudioMacros.h
    ${COCOS_AUDIO_PLATFORM_HEADER}
    )
list(APPEND COCOS_AUDIO_SRC ${COCOS_AUDIO_PLATFORM_SRC})
