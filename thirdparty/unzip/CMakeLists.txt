
set(lib_name unzip)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
  crypt.c
  ioapi.c
  ioapi_mem.c
  unzip.c
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC .)

if(MACOSX OR ANDROID OR WINDOWS)
  get_target_property(zlib_header dep_zlib INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(${target_name} PRIVATE ${zlib_header}
)
endif()

