set(lib_name yasio)
set(target_name ${lib_name})

project(${lib_name})

FILE(GLOB_RECURSE YASIO_ALL_SOURCES "yasio/*.hpp")

list(APPEND YASIO_ALL_SOURCES
  "yasio/io_service.cpp"
  "yasio/xxsocket.cpp"
)

if(CARES_INCLUDE_DIR AND ANDROID)
  list(APPEND YASIO_ALL_SOURCES "yasio/platform/yasio_jni.cpp")
endif()

if(MSVC_IDE)
  list(APPEND YASIO_ALL_SOURCES "yasio/yasio.natvis")
endif()

source_group_by_dir("yasio" YASIO_ALL_SOURCES)

add_library(${target_name} STATIC
  ${YASIO_ALL_SOURCES}
)

target_include_directories(${target_name} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(CARES_INCLUDE_DIR)
  target_compile_definitions(${target_name} PUBLIC YASIO_USE_CARES=1)
  target_include_directories(${target_name} PUBLIC ${CARES_INCLUDE_DIR})
  target_link_libraries(${target_name} PUBLIC c-ares)
endif()

target_include_directories(${target_name} PUBLIC ${OPENSSL_INCLUDE_DIR})
target_compile_definitions(${target_name} PUBLIC YASIO_SSL_BACKEND=1)

if(kcp_SOURCE_DIR)
  target_compile_definitions(${target_name} PUBLIC YASIO_ENABLE_KCP=1)
  target_include_directories(${target_name} PUBLIC ${kcp_SOURCE_DIR})
endif()

if(ANDROID OR LINUX)
    target_link_libraries(yasio INTERFACE OpenSSL::SSL)
endif()
