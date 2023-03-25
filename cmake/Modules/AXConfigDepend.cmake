macro(ax_depend)
    # confim the libs, prepare to link
    set(PLATFORM_SPECIFIC_LIBS)

    message(STATUS "AX_USE_ALSOFT=${AX_USE_ALSOFT}")

    if(WINDOWS)
        list(APPEND PLATFORM_SPECIFIC_LIBS ws2_32 userenv psapi winmm Version Iphlpapi opengl32)
        if(WINRT)
            list(APPEND PLATFORM_SPECIFIC_LIBS windowscodecs Advapi32 runtimeobject Dwrite)
        endif()
    elseif(LINUX)
        # need review those libs: X11 Xi Xrandr Xxf86vm Xinerama Xcursor rt m
        list(APPEND PLATFORM_SPECIFIC_LIBS dl X11 Xi Xrandr Xxf86vm Xinerama Xcursor rt m bsd)
        # use older cmake style on below linux libs
        ax_find_package(Fontconfig FONTCONFIG REQUIRED)	
        ax_find_package(GTK3 GTK3 REQUIRED)
        # ax_find_package(ZLIB ZLIB REQUIRED)
        # ax_find_package(PNG PNG REQUIRED)
        # ax_find_package(GLEW GLEW REQUIRED)
        ax_find_package(OpenGL OPENGL REQUIRED)
        # ax_find_package(CURL CURL REQUIRED)
        # ax_find_package(SQLite3 SQLITE3 REQUIRED)
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)	
        find_package(Threads REQUIRED)	
        set(THREADS_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
    elseif(ANDROID)
        list(APPEND PLATFORM_SPECIFIC_LIBS GLESv2 EGL log android OpenSLES)
    elseif(APPLE)

        include_directories(/System/Library/Frameworks)
        find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
        find_library(FOUNDATION_LIBRARY Foundation)
        find_library(QUARTZCORE_LIBRARY QuartzCore)
        find_library(GAMECONTROLLER_LIBRARY GameController)
        find_library(METAL_LIBRARY Metal)
        find_library(COREVIDEO_LIBRARY CoreVideo)
        find_library(AVFOUNDATION_LIBRARY AVFoundation)
        find_library(COREMEDIA_LIBRARY CoreMedia)
        set(_AX_APPLE_LIBS
            ${AUDIOTOOLBOX_LIBRARY}
            ${QUARTZCORE_LIBRARY}
            ${FOUNDATION_LIBRARY}
            ${GAMECONTROLLER_LIBRARY}
            ${METAL_LIBRARY}
            ${COREVIDEO_LIBRARY}
            ${AVFOUNDATION_LIBRARY}
            ${COREMEDIA_LIBRARY}
            )

        if(NOT AX_USE_ALSOFT)
            find_library(OPENAL_LIBRARY OpenAL)
            set(_AX_APPLE_LIBS 
            ${OPENAL_LIBRARY}
            ${_AX_APPLE_LIBS}
            )
        endif()
            
        if(MACOSX)
            list(APPEND PREBUILT_SPECIFIC_LIBS GLFW3)

            find_library(COCOA_LIBRARY Cocoa)
            find_library(OPENGL_LIBRARY OpenGL)
            find_library(APPLICATIONSERVICES_LIBRARY ApplicationServices)
            find_library(IOKIT_LIBRARY IOKit)
            find_library(APPKIT_LIBRARY AppKit)
            #find_library(ICONV_LIBRARY iconv)
            find_library(AUDIOUNIT_LIBRARY AudioUnit)
            find_library(COREAUDIO_LIBRARY CoreAudio)
            find_library(SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
            list(APPEND PLATFORM_SPECIFIC_LIBS
                 ${COCOA_LIBRARY}
                 ${OPENGL_LIBRARY}
                 ${APPLICATIONSERVICES_LIBRARY}
                 ${IOKIT_LIBRARY}
                 ${_AX_APPLE_LIBS}
                 ${APPKIT_LIBRARY}
                 #${ICONV_LIBRARY}
                 ${AUDIOUNIT_LIBRARY}
                 ${COREAUDIO_LIBRARY}
                 ${SYSTEMCONFIGURATION_LIBRARY}
                 )
        elseif(IOS)
            # Locate system libraries on iOS
            find_library(UIKIT_LIBRARY UIKit)
            find_library(OPENGLES_LIBRARY OpenGLES)            
            find_library(AVKIT_LIBRARY AVKit)
            # find_library(CORE_MEDIA_LIBRARY CoreMedia)
            find_library(CORE_TEXT_LIBRARY CoreText)
            find_library(SECURITY_LIBRARY Security)
            find_library(CORE_GRAPHICS_LIBRARY CoreGraphics)
            # find_library(AV_FOUNDATION_LIBRARY AVFoundation)
            find_library(ZLIB z)
            #find_library(ICONVLIB iconv)

            if(NOT TVOS)
                find_library(CORE_MOTION_LIBRARY CoreMotion)
                find_library(WEBKIT_LIBRARY WebKit)
            endif()
            
            list(APPEND PLATFORM_SPECIFIC_LIBS
                 ${UIKIT_LIBRARY}
                 ${OPENGLES_LIBRARY}
                 ${AVKIT_LIBRARY}
                 ${CORE_MEDIA_LIBRARY}
                 ${CORE_TEXT_LIBRARY}
                 ${SECURITY_LIBRARY}
                 ${CORE_GRAPHICS_LIBRARY}
                 ${AV_FOUNDATION_LIBRARY}
                 ${_AX_APPLE_LIBS}
                 ${ZLIB}
                 #${ICONVLIB}
                 )

            if(NOT TVOS)
                list(APPEND PLATFORM_SPECIFIC_LIBS
                    ${CORE_MOTION_LIBRARY}
                    ${WEBKIT_LIBRARY}
                )
            endif()
        endif()
    endif()
endmacro()

macro(use_ax_depend target)
    ax_depend()
    foreach(platform_lib ${PLATFORM_SPECIFIC_LIBS})
        target_link_libraries(${target} ${platform_lib})
    endforeach()

    if(LINUX)
        ax_use_pkg(${target} FONTCONFIG)
        ax_use_pkg(${target} GTK3)
        #ax_use_pkg(${target} ZLIB)
        #ax_use_pkg(${target} PNG)
        #ax_use_pkg(${target} GLEW)
        ax_use_pkg(${target} OPENGL)
        #ax_use_pkg(${target} CURL)
        ax_use_pkg(${target} THREADS)
        #ax_use_pkg(${target} SQLITE3)
    endif()
endmacro()

