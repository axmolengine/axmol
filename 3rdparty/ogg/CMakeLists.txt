
set(lib_name ogg)
set(target_name ${lib_name})

project(${target_name})

set(${target_name}_src
  src/ogg/bitwise.c
  src/ogg/framing.c
)

list(APPEND ${target_name}_src
  src/vorbis/analysis.c
  src/vorbis/bitrate.c
  src/vorbis/block.c
  src/vorbis/codebook.c
  src/vorbis/envelope.c
  src/vorbis/floor0.c
  src/vorbis/floor1.c
  src/vorbis/info.c
  src/vorbis/lookup.c
  src/vorbis/lpc.c
  src/vorbis/lsp.c
  src/vorbis/mapping0.c
  src/vorbis/mdct.c
  src/vorbis/psy.c
  src/vorbis/registry.c
  src/vorbis/res0.c
  src/vorbis/sharedbook.c
  src/vorbis/smallft.c
  src/vorbis/synthesis.c
  src/vorbis/vorbisenc.c
  src/vorbis/vorbisfile.c
  src/vorbis/window.c
)

if(AX_ENABLE_OPUS)
  list(APPEND ${target_name}_src
    src/opus/info.c
    src/opus/internal.c
    src/opus/opusfile.c
    src/opus/stream.c
  )
endif()

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name}
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include/opus"
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/vorbis"
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/opus"
)

if(AX_ENABLE_OPUS)
  set_target_properties(${target_name} PROPERTIES INTERFACE_COMPILE_DEFINITIONS AX_ENABLE_OPUS=1)
endif()
