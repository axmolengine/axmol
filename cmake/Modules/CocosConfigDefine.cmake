If(APPLE)
    if(${CMAKE_VERSION} VERSION_LESS "3.14")
        message(FATAL_ERROR "Please use CMake 3.14 or newer for Apple platform (macOS, iOS, tvOS or watchOS)")
    endif()
endif()

 #Please use them everywhere
 #WINDOWS   =   Windows Desktop
 #ANDROID    =  Android
 #IOS    =  iOS
 #MACOSX    =  MacOS X
 #LINUX      =   Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
    set(PLATFORM_FOLDER win32)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(PLATFORM_FOLDER android)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    if(ANDROID)
        set(PLATFORM_FOLDER android)
    else()
        set(LINUX TRUE)
        set(PLATFORM_FOLDER linux)
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(APPLE TRUE)
    set(MACOSX TRUE)
    set(PLATFORM_FOLDER mac)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(APPLE TRUE)
    set(IOS TRUE)
    set(PLATFORM_FOLDER ios)
else()
    message(FATAL_ERROR "Unsupported platform, CMake will exit")
    return()
endif()

# generators that are capable of organizing into a hierarchy of folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# simplify generator condition, please use them everywhere
if(CMAKE_GENERATOR STREQUAL Xcode)
    set(XCODE TRUE)
elseif(CMAKE_GENERATOR MATCHES Visual)
    set(VS TRUE)
endif()
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# custom target property for lua/js link
define_property(TARGET
    PROPERTY CC_JS_DEPEND
    BRIEF_DOCS "cocos2d js depend libs"
    FULL_DOCS "use to save depend libs of cocos2d js project"
) 
define_property(TARGET
    PROPERTY CC_LUA_DEPEND
    BRIEF_DOCS "cocos2d lua depend libs"
    FULL_DOCS "use to save depend libs of cocos2d lua project"
) 

# config c standard
if (NOT WINDOWS)
    if(NOT DEFINED C_STD)
        set(C_STD 11)
    endif()
    message(STATUS "C_STD=${C_STD}")
    set(CMAKE_C_STANDARD ${C_STD})
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif()

# config c++ standard
if(NOT DEFINED CXX_STD)
    set(CXX_STD 14)
endif()
message(STATUS "CXX_STD=${CXX_STD}")
set(CMAKE_CXX_STANDARD ${CXX_STD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# check visual studio version
if(WINDOWS)
    # not support other compile tools except MSVC for now
    if(MSVC)
        # Visual Studio 2015, MSVC_VERSION 1900      (v140 toolset)
        # Visual Studio 2017, MSVC_VERSION 1910-1919 (v141 toolset)
        if(${MSVC_VERSION} EQUAL 1900 OR ${MSVC_VERSION} GREATER 1900)
            message(STATUS "using Windows MSVC generate cocos2d-x project, MSVC_VERSION:${MSVC_VERSION}")
        else()
            message(FATAL_ERROR "using Windows MSVC generate cocos2d-x project, MSVC_VERSION:${MSVC_VERSION} lower than needed")
        endif()
    else()
        message(FATAL_ERROR "please using Windows MSVC compile cocos2d-x project, support other compile tools not yet")
    endif()
endif()

# Set macro definitions for special platforms
function(use_cocos2dx_compile_define target)
    target_compile_definitions(${target} PUBLIC $<$<CONFIG:Debug>:COCOS2D_DEBUG=1>)
    
    # !important engine-x not use double precision
    # target_compile_definitions(${target} PUBLIC CP_USE_CGTYPES=0)
    # target_compile_definitions(${target} PUBLIC CP_USE_DOUBLES=0)
    
    if(APPLE)
        target_compile_definitions(${target} PUBLIC __APPLE__)
        target_compile_definitions(${target} PUBLIC USE_FILE32API)
        if(CC_COMPAT_GL)
            target_compile_definitions(${target} 
                PUBLIC CC_COMPAT_GL=1 
                PUBLIC GL_SILENCE_DEPRECATION=1
            )
        endif()
        
    elseif(LINUX)
        target_compile_definitions(${target} PUBLIC LINUX)
        target_compile_definitions(${target} PUBLIC _GNU_SOURCE)
    elseif(ANDROID)
        target_compile_definitions(${target} PUBLIC ANDROID)
        target_compile_definitions(${target} PUBLIC USE_FILE32API)
    elseif(WINDOWS)
        if(CC_COMPAT_GL)
            target_compile_definitions(${target} PUBLIC CC_COMPAT_GL=1)
        endif()
        target_compile_definitions(${target} 
            PUBLIC WIN32
            PUBLIC _WIN32
            PUBLIC _WINDOWS
            PUBLIC UNICODE
            PUBLIC _UNICODE
            PUBLIC _CRT_SECURE_NO_WARNINGS
            PUBLIC _SCL_SECURE_NO_WARNINGS
            # PUBLIC GLAD_GLAPI_EXPORT
        )
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(${target} 
                PRIVATE _USRDLL
                PRIVATE _USEGUIDLL # ui
            )
        else()
            target_compile_definitions(${target} PUBLIC CC_STATIC)
        endif()
    endif()
endfunction()

# Set compiler options
function(use_cocos2dx_compile_options target)
    if(MSVC)
        target_compile_options(${target}
            PUBLIC /MP
        )
    endif()
endfunction()

# softfp for android armv7a?
# if(ANDROID)
# 	if(${ANDROID_ABI} STREQUAL "armeabi-v7a")
#         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp")
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfloat-abi=softfp")
# 	endif()
# endif()

# Try enable asm & nasm compiler support
set(can_use_assembler TRUE)
enable_language(ASM)
enable_language(ASM_NASM OPTIONAL)
message(STATUS "The nasm compiler speed up libraries: jpeg(libjpeg-turbo)")

if(NOT EXISTS "${CMAKE_ASM_NASM_COMPILER}")
   set(CMAKE_ASM_NASM_COMPILER_LOADED FALSE CACHE BOOL "Does cmake asm nasm compiler loaded" FORCE)
   message(WARNING "The nasm compiler doesn't present on your system PATH, please download from: https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/")
endif()
