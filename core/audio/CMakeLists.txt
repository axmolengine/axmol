
# common headers and sources
set(_AX_AUDIO_HEADER
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

set(_AX_AUDIO_SRC
    audio/AudioEngine.cpp
    audio/AudioDecoderManager.cpp
    audio/AudioDecoder.cpp
    audio/AudioDecoderOgg.cpp
    audio/AudioPlayer.cpp
    audio/AudioCache.cpp
    audio/AudioEngineImpl.cpp
    )

if(APPLE)
    set_source_files_properties(audio/AudioEngineImpl.cpp PROPERTIES LANGUAGE OBJCXX)

    set(_AX_AUDIO_HEADER ${_AX_AUDIO_HEADER}
                        audio/AudioDecoderEXT.h)
    set(_AX_AUDIO_SRC ${_AX_AUDIO_SRC}
                        audio/AudioDecoderEXT.mm)
else()
    set(_AX_AUDIO_HEADER ${_AX_AUDIO_HEADER}
                        audio/AudioDecoderMp3.h
                        audio/AudioDecoderWav.h
    )
    set(_AX_AUDIO_SRC ${_AX_AUDIO_SRC}
                        audio/AudioDecoderMp3.cpp
                        audio/AudioDecoderWav.cpp
    )
endif()
