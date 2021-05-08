
project(external)

if(NOT APPLE)
    set(OPT_ALSOFT_DEFAULT ON)
else()
    set(OPT_ALSOFT_DEFAULT OFF) 
endif()

option(BUILD_EXT_BOX2D "Build with internal Box2D support" ON)
option(BUILD_EXT_CHIPMUNK "Build with internal chipmunk support" ON)
option(BUILD_EXT_FREETYPE "Build with internal freetype support" ON)
option(BUILD_EXT_RECAST "Build with internal recast support" ON)
option(BUILD_EXT_BULLET "Build with internal bullet support" ON)
option(BUILD_EXT_JPEG "Build with internal jpeg support" ON)
option(BUILD_EXT_OPENSSL "Build with internal openssl support" ON)
option(BUILD_EXT_WEBP "Build with internal webp support" ON)
option(BUILD_EXT_PUGIXML "Build with internal pugixml support" ON)
option(BUILD_EXT_CLIPPER "Build with internal clipper support" ON)
option(BUILD_EXT_EDTAA3FUNC "Build with internal edtaa3func support" ON)
option(BUILD_EXT_CONVERTUTF "Build with internal ConvertUTF support" ON)
option(BUILD_EXT_POLY2TRI "Build with internal poly2tri support" ON)
option(BUILD_EXT_MD5 "Build with internal md5 support" ON)
option(BUILD_EXT_ZLIB "Build with internal zlib support" ON)
option(BUILD_EXT_CURL "Build with internal curl support" ON)
option(BUILD_EXT_UNZIP "Build with internal unzip support" ON)
option(BUILD_EXT_ASTC "Build with internal ASTC support" ON)
option(BUILD_EXT_ALSOFT "Build with internal openal-soft support" ${OPT_ALSOFT_DEFAULT})
option(BUILD_EXT_CARES "Build with internal c-ares support" OFF)
option(BUILD_EXT_KCP "Build with internal kcp support" OFF)

set(ANDROID_SHARED_LOADS "" CACHE INTERNAL "The android shared libraries load source code" )
set(ANDROID_SHARED_LOAD_FILE_NAME "SharedLoader.java" CACHE INTERNAL "The android shared load java file name" )
set(ANDROID_SHARED_LOAD_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${ANDROID_SHARED_LOAD_FILE_NAME}.in" CACHE INTERNAL "The android shared libraries load config code file" )

# cmake_policy(SET CMP0079 NEW)

function(configure_target_outdir target)
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "External"
    )
endfunction(configure_lib_outdir)

if(NOT DEFINED LUA_ENGINE)
    set(LUA_ENGINE "plainlua" CACHE INTERNAL "The lua core lib, luajit or plainlua")
endif()

if(LUA_ENGINE STREQUAL "plainlua")
    if(NOT DEFINED LUA_VERSION)
        set(LUA_VERSION "5.3.6" CACHE INTERNAL "The plainlua version")
    endif()
    message(STATUS "Using plainlua: ${LUA_VERSION}")
elseif(LUA_ENGINE STREQUAL "luajit")
    message(STATUS "Using luajit")
else()
    message(FATAL_ERROR "Invalid lua engine: ${LUA_ENGINE}, must be plainlua or luajit")
endif()

if(NOT BUILD_EXT_KCP)
    add_library(external STATIC empty.cpp)
else()
    add_library(external STATIC kcp/kcp.c)
endif()

if(MACOSX OR ANDROID OR WINDOWS)
    add_subdirectory(zlib)
    target_link_libraries(external 
        ext_zlib
    )
endif()

if(BUILD_EXT_BOX2D)
    set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "Build the Box2D unit tests" FORCE)
    set(BOX2D_BUILD_TESTBED OFF CACHE BOOL "Build the Box2D testbed" FORCE)
    add_subdirectory(box2d)
    configure_target_outdir(box2d)
    target_link_libraries(external box2d) 
endif(BUILD_EXT_BOX2D)

if(BUILD_EXT_CHIPMUNK)
    set(CP_BUILD_SHARED OFF CACHE BOOL "Build chipmunk as shared library" FORCE)
    set(CP_BUILD_STATIC ON CACHE BOOL "Build chipmunk as static library" FORCE)
    set(CP_BUILD_DEMOS OFF CACHE BOOL "Build chipmunk demos" FORCE)
    set(CP_INSTALL_STATIC OFF CACHE BOOL "Install chipmunk static" FORCE)
    add_subdirectory(chipmunk)
    set_target_properties(chipmunk PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include"
    )
    
    # !important engine-x not use double precision
    target_compile_definitions(chipmunk PUBLIC CP_USE_CGTYPES=0)
    target_compile_definitions(chipmunk PUBLIC CP_USE_DOUBLES=0)

    #~ include_directories("${CMAKE_CURRENT_SOURCE_DIR}/chipmunk/include")
    target_link_libraries(external chipmunk)
endif(BUILD_EXT_CHIPMUNK)
if(BUILD_EXT_FREETYPE)
    set(FT_WITH_ZLIB ON CACHE BOOL "Use system zlib instead of internal library." FORCE)
    add_subdirectory(freetype)
    target_include_directories(external PUBLIC "freetype/include")
    target_link_libraries(external freetype)
    configure_target_outdir(freetype)
endif(BUILD_EXT_FREETYPE)
if(BUILD_EXT_RECAST)
    add_subdirectory(recast)
    target_link_libraries(external recast)
endif(BUILD_EXT_RECAST)
if(BUILD_EXT_BULLET)
    add_subdirectory(bullet)
    target_link_libraries(external bullet)
endif(BUILD_EXT_BULLET)
if(BUILD_EXT_JPEG)
    set(WITH_SIMD ${CMAKE_ASM_NASM_COMPILER_LOADED} CACHE BOOL "Build TurboJPEG with simd speed up" FORCE)
    set(WITH_JPEG8 TRUE CACHE BOOL "Build libjpeg-turbo with JEPG8 API" FORCE)
    set(ENABLE_SHARED FALSE CACHE BOOL "Build shared libraries" FORCE)
    set(WITH_TURBOJPEG FALSE CACHE BOOL "Include the TurboJPEG API library and associated test programs" FORCE)
    set(JTURBO_ENABLE_INSTALL FALSE CACHE BOOL "Enable libjpeg-turbo install" FORCE)
    set(JTURBO_BUILD_TESTS FALSE CACHE BOOL "Build TurboJPEG tests and examples" FORCE)
    set(JTURBO_BUILD_BINARIES FALSE CACHE BOOL "Build TurboJPEG binaries" FORCE)
    set(WITH_CRT_DLL TRUE CACHE BOOL "Build TurboJPEG with crt dll" FORCE)
    add_subdirectory(jpeg)
    configure_target_outdir(jpeg)
    configure_target_outdir(simd)
    if(TARGET simd-objs)
        configure_target_outdir(simd-objs)
    endif()
    if(TARGET jsimdcfg)
        configure_target_outdir(jsimdcfg)
    endif()
    target_link_libraries(external jpeg)
    target_include_directories(external INTERFACE 
      "${CMAKE_CURRENT_LIST_DIR}/jpeg" 
      "${JPEG_CONFIG_OUT_DIR}")
endif(BUILD_EXT_JPEG)
if(BUILD_EXT_OPENSSL)
    add_subdirectory(openssl)
    if(ANDROID OR LINUX)
        target_link_libraries(OpenSSL::SSL INTERFACE OpenSSL::Crypto)
        target_link_libraries(external OpenSSL::SSL)
    else()
        target_link_libraries(external OpenSSL::Crypto OpenSSL::SSL)
    endif()
endif(BUILD_EXT_OPENSSL)
if(BUILD_EXT_WEBP)
    add_subdirectory(webp)
    target_link_libraries(external webp)
endif(BUILD_EXT_WEBP)
if(BUILD_EXT_PUGIXML)
    add_subdirectory(pugixml)
    target_link_libraries(external pugixml)
endif(BUILD_EXT_PUGIXML)

add_subdirectory(lz4)
target_link_libraries(external lz4)

if(BUILD_EXT_CLIPPER)
    add_subdirectory(clipper)
    target_link_libraries(external clipper)
endif(BUILD_EXT_CLIPPER)
if(BUILD_EXT_EDTAA3FUNC)
    add_subdirectory(edtaa3func)
    target_link_libraries(external edtaa3func)
endif(BUILD_EXT_EDTAA3FUNC)
if(BUILD_EXT_CONVERTUTF)
    add_subdirectory(ConvertUTF)
    target_link_libraries(external convertUTF)
endif(BUILD_EXT_CONVERTUTF)
if(BUILD_EXT_POLY2TRI)
    add_subdirectory(poly2tri)
    target_link_libraries(external poly2tri)
endif(BUILD_EXT_POLY2TRI)

if(BUILD_EXT_MD5)
    add_subdirectory(md5)
    target_link_libraries(external md5)
endif(BUILD_EXT_MD5)

if(BUILD_EXT_ASTC)
    add_subdirectory(astc)
    target_link_libraries(external astc)
endif(BUILD_EXT_ASTC)

# use lua/js specific libs by property to prevent conflict
if(BUILD_LUA_LIBS)
    add_subdirectory(lua/${LUA_ENGINE})
    add_subdirectory(lua/tolua)
    set_property(TARGET external APPEND PROPERTY 
        CC_LUA_DEPEND ${LUA_ENGINE} tolua
    )
endif()

if(NOT LINUX)
    if(BUILD_EXT_CURL)
        set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
        set(BUILD_CURL_EXE FALSE CACHE BOOL "Set to OFF to build curl executable." FORCE)
        set(CURL_DISABLE_TESTS TRUE CACHE BOOL "Disable curl tests building." FORCE)
        set(CMAKE_USE_OPENSSL TRUE CACHE BOOL "Use openssl for curl" FORCE)
        if(WIN32)
            set(CURL_BUILD_SHARED_LIBS TRUE CACHE BOOL "Set to ON to build curl executable." FORCE)
        endif()
        add_subdirectory(curl)
        configure_target_outdir(libcurl)
        # if(ANDROID OR LINUX)
        #     target_link_libraries(ext_curl INTERFACE ext_ssl)
        # endif()
        target_link_libraries(external libcurl)
    endif(BUILD_EXT_CURL)
endif(NOT LINUX)

if(ANDROID)
    add_subdirectory(android-specific/cpufeatures)
    # target_link_libraries(external 
    #     cpufeatures 
    # )
endif()

# The openal-soft(LGPL 2.1)
if(BUILD_EXT_ALSOFT)
    set(ALSOFT_DLOPEN OFF CACHE BOOL "Check for the dlopen API for loading optional libs" FORCE)
    set(ALSOFT_UTILS OFF CACHE BOOL "Build utility program" FORCE)
    set(ALSOFT_EXAMPLES OFF CACHE BOOL "Build example programs" FORCE)
    set(ALSOFT_INSTALL OFF CACHE BOOL "Install main library" FORCE)
    add_subdirectory(openal)
    configure_target_outdir(common)
    configure_target_outdir(OpenAL)
    target_link_libraries(external
        OpenAL
    )
    add_definitions(-DCC_USE_ALSOFT=1)
endif()

# The ogg decoder
add_subdirectory(ogg)
target_link_libraries(external ogg)

if(WINDOWS OR LINUX)
    add_subdirectory(glad)
    target_link_libraries(external glad)
endif()

if(WINDOWS)
    add_subdirectory(win32-specific/iconv)
    target_link_libraries(external ext_iconv)
endif()

if(WINDOWS OR MACOSX OR LINUX)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "Build the GLFW documentation" FORCE)
    add_subdirectory(glfw)
    configure_target_outdir(glfw)
    target_link_libraries(external glfw)
    if(NOT WINDOWS)
        target_link_libraries(external pthread)
    endif()
    target_include_directories(external PUBLIC "glfw/include/GLFW")
endif()

if(NOT LINUX)
    add_subdirectory(png)
    target_include_directories(png PRIVATE "zlib/include")
    if(ANDROID)
        target_include_directories(png PRIVATE ${cpufeatures_include_dir})
        target_link_libraries(png INTERFACE cpufeatures)
    endif()
    target_link_libraries(external png)
endif()

# unzip depend on zlib
if(BUILD_EXT_UNZIP)
    add_subdirectory(unzip)
    target_link_libraries(external unzip)
endif(BUILD_EXT_UNZIP)

# since 3.6/3.7 c runtime support build as DLL
# since 3.8 cpp runtime is preferred for cocos2d-x
# if(BUILD_EXT_SPINE_CPP)
#     add_subdirectory(spine-cpp)
#     target_link_libraries(external spine-cpp)
#     set_target_properties(spine-cpp
#     PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     FOLDER "External"
# )
# 
# endif(BUILD_EXT_SPINE_CPP)

if(BUILD_EXT_CARES)
    set(CARES_STATIC ON CACHE BOOL "Build c-ares as static library" FORCE)
    set(CARES_SHARED OFF CACHE BOOL "Build c-ares as shared library" FORCE)
    set(CARES_BUILD_TOOLS OFF CACHE BOOL "Build c-ares tools" FORCE)
    set(CARES_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)" FORCE)
    add_subdirectory(c-ares)
    target_link_libraries(external c-ares)
    include_directories($<TARGET_FILE_DIR:c-ares>)
    include_directories("c-ares/include")
    configure_target_outdir(c-ares)
endif(BUILD_EXT_CARES)

# put "external" into External folder, too
configure_target_outdir(external)

if(MACOSX OR IOS)
    add_subdirectory(glsl-optimizer)
    target_link_libraries(external 
    ext_glcpp_library
    ext_glsl_optimizer
    ext_libmesa
    )
endif()

if (ANDROID)
    function(config_android_shared_libs package_name target_folder)
        string(REPLACE "." "/" package_path ${package_name})
        set(ANDROID_PACKAGE_NAME ${package_name})
        configure_file(${ANDROID_SHARED_LOAD_FILE_PATH} ${target_folder}/${package_path}/${ANDROID_SHARED_LOAD_FILE_NAME} @ONLY)
    endfunction()
endif()
