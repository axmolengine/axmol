cmake_minimum_required(VERSION 3.6)

set(lib_name ogg)
set(target_name ${lib_name})

project(${target_name})

include(../cmake/CocosExternalConfig.cmake)

set(${target_name}_src
  src/ogg/bitwise.c
  src/ogg/framing.c
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

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name}
  PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include"
  PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/vorbis"
)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)
