option(AX_ENABLE_MSEDGE_WEBVIEW2 "Enable msedge webview2" TRUE)

if(NOT ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32"))
    set(WIN64 TRUE)
    set(ARCH_ALIAS "x64")
else()
    set(WIN32 TRUE)
    set(ARCH_ALIAS "x86")
endif()

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
    set(THIRD_PARTY_ARCH "\$\(Configuration\)/")
endif()

message(STATUS "AX_ENABLE_MSEDGE_WEBVIEW2=${AX_ENABLE_MSEDGE_WEBVIEW2}")

function(adxe_link_cxx_prebuilt APP_NAME AX_ROOT_DIR AX_PREBUILT_DIR)
    if (NOT AX_USE_SHARED_PREBUILT)
        target_compile_definitions(${APP_NAME}
            PRIVATE CC_STATIC=1
        )
    endif()

    target_include_directories(${APP_NAME}
        PRIVATE ${AX_ROOT_DIR}/thirdparty/lua
        PRIVATE ${AX_ROOT_DIR}/extensions/scripting/lua-bindings/manual
        PRIVATE ${AX_ROOT_DIR}
        PRIVATE ${AX_ROOT_DIR}/thirdparty
        PRIVATE ${AX_ROOT_DIR}/extensions
        PRIVATE ${AX_ROOT_DIR}/core
        PRIVATE ${AX_ROOT_DIR}/core/platform
        PRIVATE ${AX_ROOT_DIR}/core/base
        PRIVATE ${AX_ROOT_DIR}/core/audio
        PRIVATE ${AX_ROOT_DIR}/core/platform/win32
        PRIVATE ${AX_ROOT_DIR}/thirdparty/fmt/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/robin-map/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/freetype/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/glfw/include/GLFW
        PRIVATE ${AX_ROOT_DIR}/thirdparty/box2d/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/chipmunk/include
        PRIVATE ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/engine/thirdparty/freetype/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/recast/..
        PRIVATE ${AX_ROOT_DIR}/thirdparty/bullet/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/webp/src/webp
        PRIVATE ${AX_ROOT_DIR}/thirdparty/pugixml
        PRIVATE ${AX_ROOT_DIR}/thirdparty/xxhash/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/lz4/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/clipper/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/ConvertUTF/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/poly2tri/..
        PRIVATE ${AX_ROOT_DIR}/thirdparty/astc/..
        PRIVATE ${AX_ROOT_DIR}/thirdparty/openal/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/ogg/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/glad/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/glfw/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/png
        PRIVATE ${AX_ROOT_DIR}/thirdparty/unzip/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/llhttp/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/lua/plainlua
        PRIVATE ${AX_ROOT_DIR}/thirdparty/lua/tolua/.
        PRIVATE ${AX_ROOT_DIR}/thirdparty/lua/lua-cjson/.
        PRIVATE ${AX_ROOT_DIR}/extensions/cocostudio
        PRIVATE ${AX_ROOT_DIR}/extensions/spine/runtime/include
        PRIVATE ${AX_ROOT_DIR}/extensions/fairygui
        PRIVATE ${AX_ROOT_DIR}/extensions/GUI
        PRIVATE ${AX_ROOT_DIR}/thirdparty/zlib/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/jpeg/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/openssl/include
        PRIVATE ${AX_ROOT_DIR}/thirdparty/curl/include
    )

    target_link_directories(${APP_NAME}
        PRIVATE ${AX_ROOT_DIR}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/thirdparty/jpeg/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/lib/Debug
    )

    # Linking OS libs
    target_link_libraries(${APP_NAME} winmm Version)

    # Linking engine and thirdparty libs
    target_link_libraries(${APP_NAME}
        spine.lib
        particle3d.lib
        assets-manager.lib
        cocostudio.lib
        DragonBones.lib
        adxe.lib
        box2d.lib
        chipmunk.lib
        freetype.lib
        zlib.lib
        recast.lib
        bullet.lib
        jpeg-static.lib
        libcrypto.lib
        libssl.lib
        webp.lib
        pugixml.lib
        xxhash.lib
        lz4.lib
        clipper.lib
        ConvertUTF.lib
        poly2tri.lib
        astc.lib
        libcurl_imp.lib
        OpenAL32.lib
        ogg.lib
        glad.lib
        glfw.lib
        png.lib
        unzip.lib
        llhttp.lib
        physics-nodes.lib
    )

    # Copy dlls to app bin dir
        # copy thirdparty dlls to target bin dir
    # copy_thirdparty_dlls(${APP_NAME} $<TARGET_FILE_DIR:${APP_NAME}>)
    set(ssl_dll_suffix "")
    if(WIN64)
        set(ssl_dll_suffix "-${ARCH_ALIAS}")
    endif()
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AX_ROOT_DIR}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libcrypto-3${ssl_dll_suffix}.dll"
        "${AX_ROOT_DIR}/thirdparty/openssl/prebuilt/windows/${ARCH_ALIAS}/libssl-3${ssl_dll_suffix}.dll"
        "${AX_ROOT_DIR}/thirdparty/curl/prebuilt/windows/${ARCH_ALIAS}/libcurl.dll"
        "${AX_ROOT_DIR}/thirdparty/zlib/prebuilt/windows/${ARCH_ALIAS}/zlib1.dll"
        "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${THIRD_PARTY_ARCH}OpenAL32.dll"
        $<TARGET_FILE_DIR:${APP_NAME}>)

    # Copy windows angle binaries
    if (AX_USE_COMPAT_GL)
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${AX_ROOT_DIR}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libGLESv2.dll
            ${AX_ROOT_DIR}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/libEGL.dll
            ${AX_ROOT_DIR}/thirdparty/angle/prebuilt/${ARCH_ALIAS}/d3dcompiler_47.dll
            $<TARGET_FILE_DIR:${APP_NAME}>
        )
    endif()

    if (AX_ENABLE_MSEDGE_WEBVIEW2)
        if(CMAKE_GENERATOR STREQUAL "Ninja")
            target_link_libraries(${APP_NAME} ${AX_ROOT_DIR}/build/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
            target_include_directories(${APP_NAME} ${AX_ROOT_DIR}/build/packages/Microsoft.Web.WebView2/build/native/include)
            add_custom_command(TARGET ${cocos_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${AX_ROOT_DIR}/build/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>
            )
        else()
            target_link_libraries(${APP_NAME} ${AX_ROOT_DIR}/build/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
        endif()
    endif()
endfunction(adxe_link_cxx_prebuilt)

function(adxe_link_lua_prebuilt APP_NAME AX_ROOT_DIR AX_PREBUILT_DIR)
    adxe_link_cxx_prebuilt(${APP_NAME} ${AX_ROOT_DIR} ${AX_PREBUILT_DIR})

    if (NOT AX_USE_SHARED_PREBUILT)
        target_compile_definitions(${APP_NAME}
	        PRIVATE _USRLUASTATIC=1
        )
    endif()
    target_link_libraries(${APP_NAME} adxelua lua-cjson tolua plainlua)
    add_custom_command(TARGET ${APP_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${THIRD_PARTY_ARCH}plainlua.dll"
         $<TARGET_FILE_DIR:${APP_NAME}>)
endfunction(adxe_link_lua_prebuilt)
