
project(thirdparty)

if (NOT DEFINED AX_USE_ALSOFT)
    if(NOT APPLE)
        set(AX_USE_ALSOFT ON)
    else()
        set(AX_USE_ALSOFT OFF) 
    endif()
endif()

option(OPT_BOX2D_OPTIMIZED "" OFF)
option(AX_WITH_BOX2D "Build with internal Box2D support" ON)
option(AX_WITH_CHIPMUNK "Build with internal chipmunk support" ON)
option(AX_WITH_FREETYPE "Build with internal freetype support" ON)
option(AX_WITH_RECAST "Build with internal recast support" ON)
option(AX_WITH_BULLET "Build with internal bullet support" ON)
option(AX_WITH_JPEG "Build with internal jpeg support" ON)
option(AX_WITH_OPENSSL "Build with internal openssl support" ON)
option(AX_WITH_WEBP "Build with internal webp support" ON)
option(AX_WITH_PUGIXML "Build with internal pugixml support" ON)
option(AX_WITH_CLIPPER "Build with internal clipper support" ON)
option(AX_WITH_CONVERTUTF "Build with internal ConvertUTF support" ON)
option(AX_WITH_POLY2TRI "Build with internal poly2tri support" ON)
option(AX_WITH_ZLIB "Build with internal zlib support" ON)
option(AX_WITH_CURL "Build with internal curl support" ON)
option(AX_WITH_UNZIP "Build with internal unzip support" ON)
option(AX_WITH_ASTC "Build with internal ASTC support" ON)
option(AX_WITH_ALSOFT "Build with internal openal-soft support" ${AX_USE_ALSOFT})
option(AX_WITH_CARES "Build with internal c-ares support" OFF)
option(AX_WITH_LLHTTP "Build with lhttp support" ON)
option(AX_WITH_YAML_CPP "Build with yaml-cpp support" OFF)
option(AX_WITH_KCP "Build with internal kcp support" OFF)

# by default, enable ios,macOS openal-soft framework for legal license LGPL-2.1
option(ALSOFT_OSX_FRAMEWORK "" ON)

set(ANDROID_SHARED_LOADS "" CACHE INTERNAL "The android shared libraries load source code" )
set(ANDROID_SHARED_LOAD_FILE_NAME "SharedLoader.java" CACHE INTERNAL "The android shared load java file name" )
set(ANDROID_SHARED_LOAD_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_SHARED_LOAD_FILE_NAME}.in" CACHE INTERNAL "The android shared libraries load config code file" )

# cmake_policy(SET CMP0079 NEW)

function(configure_target_outdir target)
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "ThirdParty"
    )
endfunction()

if(NOT DEFINED AX_USE_LUAJIT)
    set(AX_USE_LUAJIT FALSE CACHE INTERNAL "Use luajit instead plainlua")
endif()

if(NOT AX_USE_LUAJIT)
    message(STATUS "Using plainlua https://lua.org")
    set (AX_LUA_ENGINE "plainlua"  CACHE INTERNAL "")
else()
    message(STATUS "Using luajit https://luajit.org")
    set (AX_LUA_ENGINE "luajit"  CACHE INTERNAL "")
endif()

add_library(thirdparty STATIC empty.cpp)

# bellow are header only libs
target_include_directories(thirdparty
    INTERFACE "robin-map/include"
)

if (ANDROID)
    target_include_directories(thirdparty
        INTERFACE "jni.hpp/include"
    )
endif()

add_subdirectory(fmt EXCLUDE_FROM_ALL)
target_link_libraries(thirdparty fmt-header-only)

# bellow are non header only libs
add_subdirectory(zlib)
target_link_libraries(thirdparty 
    dep_zlib
)

if(AX_WITH_BOX2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
    if(OPT_BOX2D_OPTIMIZED)
        add_subdirectory(box2d-optimized)
        configure_target_outdir(box2d-optimized)
        target_link_libraries(thirdparty box2d-optimized)
    else()
        add_subdirectory(box2d)
        configure_target_outdir(box2d)
        target_link_libraries(thirdparty box2d)
    endif()
endif(AX_WITH_BOX2D)

if(AX_WITH_CHIPMUNK)
    set(CP_BUILD_SHARED OFF CACHE BOOL "Build chipmunk as shared library" FORCE)
    set(CP_BUILD_STATIC ON CACHE BOOL "Build chipmunk as static library" FORCE)
    set(CP_BUILD_DEMOS OFF CACHE BOOL "Build chipmunk demos" FORCE)
    set(CP_INSTALL_STATIC OFF CACHE BOOL "Install chipmunk static" FORCE)
    add_subdirectory(chipmunk)
    set_target_properties(chipmunk PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include"
    )
    
    # !important adxe not use double precision
    target_compile_definitions(chipmunk PUBLIC CP_USE_CGTYPES=0)
    target_compile_definitions(chipmunk PUBLIC CP_USE_DOUBLES=0)

    #~ include_directories("${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include")
    target_link_libraries(thirdparty chipmunk)
    configure_target_outdir(chipmunk)
endif(AX_WITH_CHIPMUNK)
if(AX_WITH_FREETYPE)
    if (NOT LINUX)
        set(FT_WITH_ZLIB ON CACHE BOOL "Use system zlib instead of internal library." FORCE)
        set(DISABLE_FORCE_DEBUG_POSTFIX ON CACHE BOOL "" FORCE)
        set(SKIP_INSTALL_ALL TRUE CACHE BOOL "Skip freetype instal." FORCE)
        add_subdirectory(freetype)
        target_link_libraries(thirdparty freetype)
        configure_target_outdir(freetype)
    endif()
    target_include_directories(thirdparty PUBLIC "freetype/include")
endif(AX_WITH_FREETYPE)
if(AX_WITH_RECAST)
    add_subdirectory(recast)
    target_link_libraries(thirdparty recast)
    configure_target_outdir(recast)
endif(AX_WITH_RECAST)
if(AX_WITH_BULLET)
    add_subdirectory(bullet)
    target_link_libraries(thirdparty bullet)
    configure_target_outdir(bullet)
endif(AX_WITH_BULLET)
if(AX_WITH_JPEG)
    # set(WITH_SIMD ${CMAKE_ASM_NASM_COMPILER_LOADED} CACHE BOOL "Build TurboJPEG with simd speed up" FORCE)
    # set(WITH_JPEG8 TRUE CACHE BOOL "Build libjpeg-turbo with JEPG8 API" FORCE)
    # set(ENABLE_SHARED FALSE CACHE BOOL "Build shared libraries" FORCE)
    # set(WITH_TURBOJPEG FALSE CACHE BOOL "Include the TurboJPEG API library and associated test programs" FORCE)
    # set(JTURBO_ENABLE_INSTALL FALSE CACHE BOOL "Enable libjpeg-turbo install" FORCE)
    # set(JTURBO_BUILD_TESTS FALSE CACHE BOOL "Build TurboJPEG tests and examples" FORCE)
    # set(JTURBO_BUILD_BINARIES FALSE CACHE BOOL "Build TurboJPEG binaries" FORCE)
    # set(WITH_CRT_DLL TRUE CACHE BOOL "Build TurboJPEG with crt dll" FORCE)
    add_subdirectory(jpeg)
    # configure_target_outdir(jpeg)
    # configure_target_outdir(simd)
    # if(TARGET simd-objs)
    #     configure_target_outdir(simd-objs)
    # endif()
    # if(TARGET jsimdcfg)
    #     configure_target_outdir(jsimdcfg)
    # endif()
    target_link_libraries(thirdparty dep_jpeg)
    # target_include_directories(thirdparty INTERFACE 
    #   "${CMAKE_CURRENT_LIST_DIR}/jpeg" 
    #   "${JPEG_CONFIG_OUT_DIR}")
endif(AX_WITH_JPEG)
if(AX_WITH_OPENSSL)
    add_subdirectory(openssl)
    if(ANDROID OR LINUX)
        target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)
        target_link_libraries(thirdparty OpenSSL::SSL)
    else()
        target_link_libraries(thirdparty OpenSSL::Crypto OpenSSL::SSL)
    endif()
    target_compile_definitions(thirdparty PUBLIC OPENSSL_SUPPRESS_DEPRECATED=1)
endif(AX_WITH_OPENSSL)
if(AX_WITH_WEBP)
    add_subdirectory(webp)
    target_link_libraries(thirdparty webp)
    configure_target_outdir(webp)
endif(AX_WITH_WEBP)
if(AX_WITH_PUGIXML)
    add_subdirectory(pugixml)
    target_link_libraries(thirdparty pugixml)
    configure_target_outdir(pugixml)
endif(AX_WITH_PUGIXML)

add_subdirectory(xxhash)
target_link_libraries(thirdparty xxhash)
configure_target_outdir(xxhash)

add_subdirectory(lz4)
target_link_libraries(thirdparty lz4)
configure_target_outdir(lz4)

if(AX_WITH_CLIPPER)
    add_subdirectory(clipper)
    target_link_libraries(thirdparty clipper)
    configure_target_outdir(clipper)
endif(AX_WITH_CLIPPER)
if(AX_WITH_CONVERTUTF)
    add_subdirectory(ConvertUTF)
    target_link_libraries(thirdparty ConvertUTF)
    configure_target_outdir(ConvertUTF)
endif(AX_WITH_CONVERTUTF)
if(AX_WITH_POLY2TRI)
    add_subdirectory(poly2tri)
    target_link_libraries(thirdparty poly2tri)
    configure_target_outdir(poly2tri)
endif(AX_WITH_POLY2TRI)

if(AX_WITH_ASTC)
    add_subdirectory(astc)
    target_link_libraries(thirdparty astc)
    configure_target_outdir(astc)
endif(AX_WITH_ASTC)

# use lua/js specific libs by property to prevent conflict
if(AX_ENABLE_EXT_LUA)
    add_subdirectory(lua/${AX_LUA_ENGINE})
    add_subdirectory(lua/tolua)
    add_subdirectory(lua/lua-cjson)
    set_property(TARGET thirdparty APPEND PROPERTY 
        CC_LUA_DEPEND ${AX_LUA_ENGINE} tolua lua-cjson
    )
endif()

if(AX_WITH_CURL)
    add_subdirectory(curl)
    if(ANDROID OR LINUX)
        target_link_libraries(libcurl INTERFACE OpenSSL::SSL)
    endif()
    target_link_libraries(thirdparty libcurl)
endif(AX_WITH_CURL)

if(ANDROID)
    add_subdirectory(android-specific/cpufeatures)
    # target_link_libraries(thirdparty 
    #     cpufeatures 
    # )
endif()

# The openal-soft(LGPL 2.1)
if(AX_WITH_ALSOFT)
    set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "Build utility program" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library" FORCE)
    add_subdirectory(openal EXCLUDE_FROM_ALL)
    configure_target_outdir(common)
    configure_target_outdir(OpenAL)
    target_link_libraries(thirdparty
        OpenAL
    )
    target_compile_definitions(thirdparty PUBLIC AX_USE_ALSOFT=1)
    
    if(ANDROID)
        set(ANDROID_SHARED_LOADS "${ANDROID_SHARED_LOADS}System.loadLibrary(\"openal\");" CACHE INTERNAL "Android Shared Loads" )
    endif()
endif()

# The ogg decoder
add_subdirectory(ogg)
target_link_libraries(thirdparty ogg)
configure_target_outdir(ogg)

if(WINDOWS OR LINUX)
    add_subdirectory(glad)
    target_link_libraries(thirdparty glad)
    configure_target_outdir(glad)
endif()

if(WINDOWS OR MACOSX OR LINUX)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    add_subdirectory(glfw)
    configure_target_outdir(glfw)
    set_target_properties(glfw PROPERTIES
                      OUTPUT_NAME glfw)
    target_link_libraries(thirdparty glfw)
    if(NOT WINDOWS)
        target_link_libraries(thirdparty pthread)
    endif()
    target_include_directories(thirdparty PUBLIC "glfw/include/GLFW")
endif()

# libpng
add_subdirectory(png)
target_include_directories(png PRIVATE "zlib/include")
if(ANDROID)
    target_include_directories(png PRIVATE ${cpufeatures_include_dir})
    target_link_libraries(png INTERFACE cpufeatures)
endif()
target_link_libraries(thirdparty png)
configure_target_outdir(png)

# unzip depend on zlib
if(AX_WITH_UNZIP)
    add_subdirectory(unzip)
    target_link_libraries(thirdparty unzip)
    configure_target_outdir(unzip)
endif(AX_WITH_UNZIP)

if(AX_WITH_CARES)
    set(CARES_STATIC ON CACHE BOOL "Build c-ares as static library" FORCE)
    set(CARES_SHARED OFF CACHE BOOL "Build c-ares as shared library" FORCE)
    set(CARES_BUILD_TOOLS OFF CACHE BOOL "Build c-ares tools" FORCE)
    set(CARES_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)" FORCE)
    add_subdirectory(c-ares)
    target_link_libraries(thirdparty c-ares)
    include_directories($<TARGET_FILE_DIR:c-ares>)
    include_directories("c-ares/include")
    configure_target_outdir(c-ares)
    target_compile_definitions(thirdparty PUBLIC YASIO_HAVE_CARES=1)
endif(AX_WITH_CARES)

if(AX_WITH_LLHTTP)
    add_subdirectory(llhttp)
    configure_target_outdir(llhttp)
    target_link_libraries(thirdparty llhttp)
endif()

# yaml
if(AX_WITH_YAML_CPP)
    set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
    set(YAML_CPP_CLANG_FORMAT_EXE OFF CACHE BOOL "" FORCE)
    # set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    add_subdirectory(yaml-cpp)
    configure_target_outdir(yaml-cpp)
    add_dependencies(thirdparty yaml-cpp)
endif()

# kcp
if(AX_WITH_KCP)
    add_subdirectory(kcp)
    configure_target_outdir(kcp)
    target_link_libraries(thirdparty kcp)
    target_compile_definitions(thirdparty PUBLIC YASIO_HAVE_KCP=1)
endif()

# put "thirdparty" into thirdparty folder, too
configure_target_outdir(thirdparty)

if(MACOSX OR IOS)
    add_subdirectory(glsl-optimizer)
    target_link_libraries(thirdparty 
    dep_glcpp_library
    dep_glsl_optimizer
    dep_libmesa
    )
endif()

if (ANDROID)
    function(config_android_shared_libs package_name target_folder)
        string(REPLACE "." "/" package_path ${package_name})
        set(ANDROID_PACKAGE_NAME ${package_name})
        configure_file(${ANDROID_SHARED_LOAD_FILE_PATH} ${target_folder}/${package_path}/${ANDROID_SHARED_LOAD_FILE_NAME} @ONLY)
    endfunction()
endif()
