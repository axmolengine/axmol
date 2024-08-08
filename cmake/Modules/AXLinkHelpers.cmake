include(${_AX_ROOT}/1k/platforms.cmake)
_1k_deprecated_32bit(axmol 2.2.0)

if(NOT CMAKE_GENERATOR MATCHES "Ninja")
    set(BUILD_CONFIG_DIR "\$\(Configuration\)/")
else()
    set(BUILD_CONFIG_DIR "")
endif()


macro(ax_link_ext EXTENSION_ENABLED LINKLIB)
    if(${EXTENSION_ENABLED})
        list(APPEND LIBS ${LINKLIB})
        foreach(INCLUDEDIR ${ARGN})
            target_include_directories(${APP_NAME}
                PRIVATE ${INCLUDEDIR}
            )
        endforeach()
    endif()
endmacro()

function(ax_link_cxx_prebuilt APP_NAME AX_ROOT_DIR AX_PREBUILT_DIR)
    # stupid: exclude CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG to avoid cmake generate
    # .vcxproj with incorrect debug msvc runtime, should be /MDd but got /MD
    set(GLSLCC_OUT_DIR_PROJ "${GLSLCC_OUT_DIR}")
    load_cache("${AX_ROOT_DIR}/${AX_PREBUILT_DIR}" EXCLUDE thirdparty_LIB_DEPENDS CMAKE_CXX_FLAGS_DEBUG CMAKE_C_FLAGS_DEBUG)
    set(GLSLCC_OUT_DIR_ENGINE ${GLSLCC_OUT_DIR})
    set(GLSLCC_OUT_DIR "${GLSLCC_OUT_DIR_PROJ}" CACHE STRING "" FORCE)
    unset(GLSLCC_OUT_DIR_PROJ)

    message(STATUS "AX_ENABLE_MSEDGE_WEBVIEW2=${AX_ENABLE_MSEDGE_WEBVIEW2}")
    message(STATUS "AX_ENABLE_MFMEDIA=${AX_ENABLE_MFMEDIA}")

    message(STATUS "AX_ENABLE_EXT_IMGUI=${AX_ENABLE_EXT_IMGUI}")
    message(STATUS "AX_ENABLE_EXT_INSPECTOR=${AX_ENABLE_EXT_INSPECTOR}")
    message(STATUS "AX_ENABLE_EXT_FAIRYGUI=${AX_ENABLE_EXT_FAIRYGUI}")
    message(STATUS "AX_ENABLE_EXT_LIVE2D=${AX_ENABLE_EXT_LIVE2D}")
    message(STATUS "AX_ENABLE_EXT_GUI=${AX_ENABLE_EXT_GUI}")
    message(STATUS "AX_ENABLE_EXT_ASSETMANAGER=${AX_ENABLE_EXT_ASSETMANAGER}")
    message(STATUS "AX_ENABLE_EXT_PARTICLE3D=${AX_ENABLE_EXT_PARTICLE3D}")
    message(STATUS "AX_ENABLE_EXT_PHYSICS_NODE=${AX_ENABLE_EXT_PHYSICS_NODE}")
    message(STATUS "AX_ENABLE_EXT_SPINE=${AX_ENABLE_EXT_SPINE}")
    message(STATUS "AX_ENABLE_EXT_EFFEKSEER=${AX_ENABLE_EXT_EFFEKSEER}")
    message(STATUS "AX_ENABLE_EXT_LUA=${AX_ENABLE_EXT_LUA}")
	message(STATUS "AX_ENABLE_EXT_DRAWNODEEX=${AX_ENABLE_EXT_DRAWNODEEX}")

    # compile defines can't inherit when link prebuits, so need add manually
    target_compile_definitions(${APP_NAME}
        PRIVATE AX_GLES_PROFILE=${AX_GLES_PROFILE}
        PRIVATE OPENSSL_SUPPRESS_DEPRECATED=1
        PRIVATE NOUNCRYPT=1
        PRIVATE P2T_STATIC_EXPORTS=1
        PRIVATE BT_USE_SSE_IN_API=1
        PRIVATE CP_USE_DOUBLES=0
        PRIVATE CP_USE_CGTYPES=0
    )

    ax_config_pred(${APP_NAME} AX_USE_ALSOFT)
    ax_config_pred(${APP_NAME} AX_ENABLE_MSEDGE_WEBVIEW2)
    ax_config_pred(${APP_NAME} AX_ENABLE_PHYSICS)
    ax_config_pred(${APP_NAME} AX_ENABLE_3D)
    ax_config_pred(${APP_NAME} AX_ENABLE_3D_PHYSICS)
    ax_config_pred(${APP_NAME} AX_ENABLE_NAVMESH)
    ax_config_pred(${APP_NAME} AX_ENABLE_MEDIA)
    ax_config_pred(${APP_NAME} AX_ENABLE_AUDIO)
    ax_config_pred(${APP_NAME} AX_ENABLE_CONSOLE)

    if (AX_ISA_SIMD MATCHES "sse")
        target_compile_definitions(${APP_NAME} PRIVATE AX_USE_SSE=1)
    endif()

    if (BUILD_SHARED_LIBS)
        target_compile_definitions(${APP_NAME} PRIVATE AX_DLLIMPORT=1)
    endif()

    target_include_directories(${APP_NAME}
        PRIVATE ${AX_ROOT_DIR}/3rdparty/lua
        PRIVATE ${AX_ROOT_DIR}/extensions/scripting
        PRIVATE ${AX_ROOT_DIR}/extensions/scripting/lua-bindings/manual
        PRIVATE ${AX_ROOT_DIR}
        PRIVATE ${AX_ROOT_DIR}/3rdparty
        PRIVATE ${AX_ROOT_DIR}/extensions
        PRIVATE ${AX_ROOT_DIR}/core
        PRIVATE ${AX_ROOT_DIR}/core/platform
        PRIVATE ${AX_ROOT_DIR}/core/base
        PRIVATE ${AX_ROOT_DIR}/core/audio
        PRIVATE ${AX_ROOT_DIR}/core/platform/win32
        PRIVATE ${AX_ROOT_DIR}/3rdparty/fmt/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/robin-map/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/freetype/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/glfw/include/GLFW
        PRIVATE ${AX_ROOT_DIR}/3rdparty/box2d/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/chipmunk/include
        PRIVATE ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/engine/3rdparty/freetype/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/webp/src/webp
        PRIVATE ${AX_ROOT_DIR}/3rdparty/pugixml
        PRIVATE ${AX_ROOT_DIR}/3rdparty/xxhash
        PRIVATE ${AX_ROOT_DIR}/3rdparty/ConvertUTF
        PRIVATE ${AX_ROOT_DIR}/3rdparty/openal/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/ogg/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/glad/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/glfw/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/png
        PRIVATE ${AX_ROOT_DIR}/3rdparty/unzip/.
        PRIVATE ${AX_ROOT_DIR}/3rdparty/llhttp/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/lua/plainlua
        PRIVATE ${AX_ROOT_DIR}/3rdparty/lua/tolua/.
        PRIVATE ${AX_ROOT_DIR}/3rdparty/lua/lua-cjson/.
        PRIVATE ${AX_ROOT_DIR}/3rdparty/zlib/_x/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/jpeg-turbo/_x/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/openssl/_x/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/curl/_x/include
        PRIVATE ${AX_ROOT_DIR}/3rdparty/yasio
    )



    SET (CONFIGURATION_SUBFOLDER "")
    target_link_directories(${APP_NAME}
        PRIVATE ${AX_ROOT_DIR}/3rdparty/openssl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/3rdparty/zlib/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/3rdparty/jpeg-turbo/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/3rdparty/curl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}
        PRIVATE ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/lib  # cmake will auto add suffix '/$(Configuration)', refer to https://github.com/Kitware/CMake/blob/master/Source/cmVisualStudio10TargetGenerator.cxx#L4145
    )

    # Linking platform libs
    if (WINDOWS)
        target_link_libraries(${APP_NAME} winmm Version)
    else()
        target_link_libraries(${APP_NAME} X11 fontconfig)
    endif()

    # Linking engine and 3rdparty libs
    set(LIBS
        axmol
        box2d
        chipmunk
        freetype
        webp
        pugixml
        xxhash
        fastlz
        clipper2
        ConvertUTF
        poly2tri
        astcenc
        ogg
        glad
        glfw
        png
        unzip
        llhttp
        simdjson
        yasio
        websocket-parser
    )

    ax_link_ext(AX_ENABLE_EXT_DRAGONBONES "DragonBones" "${AX_ROOT_DIR}/extensions/DragonBones/src")
    ax_link_ext(AX_ENABLE_EXT_COCOSTUDIO "cocostudio" "${AX_ROOT_DIR}/extensions/cocostudio/src")
    ax_link_ext(AX_ENABLE_EXT_ASSETMANAGER "assets-manager" "${AX_ROOT_DIR}/extensions/assets-manager/src")
    ax_link_ext(AX_ENABLE_EXT_PARTICLE3D "particle3d" "${AX_ROOT_DIR}/extensions/Particle3D/src")
    ax_link_ext(AX_ENABLE_EXT_INSPECTOR "Inspector" "${AX_ROOT_DIR}/extensions/Inspector/src")
    ax_link_ext(AX_ENABLE_EXT_SDFGEN "SDFGen" "${AX_ROOT_DIR}/extensions/SDFGen/src")
    ax_link_ext(AX_ENABLE_EXT_DRAWNODEEX "DrawNodeEx" "${AX_ROOT_DIR}/extensions/DrawNodeEx/src")
    ax_link_ext(AX_ENABLE_EXT_GUI "GUI" "${AX_ROOT_DIR}/extensions/GUI/src")
    ax_link_ext(AX_ENABLE_EXT_FAIRYGUI "fairygui" "${AX_ROOT_DIR}/extensions/fairygui/src")
    ax_link_ext(AX_ENABLE_EXT_LIVE2D "Live2D" "${AX_ROOT_DIR}/extensions/Live2D/Framework/src")
    ax_link_ext(AX_ENABLE_EXT_EFFEKSEER "EffekseerForCocos2d-x" "${AX_ROOT_DIR}/extensions/Effekseer")
    ax_link_ext(AX_ENABLE_EXT_PHYSICS_NODE "physics-nodes" "${AX_ROOT_DIR}/extensions/physics-nodes/src")
    ax_link_ext(AX_ENABLE_NAVMESH "recast" "${AX_ROOT_DIR}/3rdparty/recast")
    ax_link_ext(AX_ENABLE_3D_PHYSICS "bullet" "${AX_ROOT_DIR}/3rdparty/bullet")

    ax_link_ext(AX_ENABLE_EXT_IMGUI "ImGui"
        "${AX_ROOT_DIR}/extensions/ImGui/src" "${AX_ROOT_DIR}/extensions/ImGui/src/ImGui/imgui"
    )

    ax_link_ext(AX_ENABLE_EXT_SPINE "spine"
        "${AX_ROOT_DIR}/extensions/spine/runtime/include" "${AX_ROOT_DIR}/extensions/spine/src"
    )

    if (WINDOWS)
        target_link_libraries(${APP_NAME}
            ${LIBS}
            zlib
            jpeg-static
            libcrypto
            libssl
            libcurl_imp
        )

        if (AX_ENABLE_AUDIO)
            target_link_libraries(${APP_NAME}
                OpenAL32
            )
        endif()
    else()
        target_link_libraries(${APP_NAME}
            ${LIBS}
            z
            jpeg
            curl
            ssl
            crypto
        )

        if (AX_ENABLE_AUDIO)
            target_link_libraries(${APP_NAME}
                openal
            )
        endif()
    endif()

    target_link_libraries(${APP_NAME} debug fmtd optimized fmt)

    # Copy dlls to app bin dir
    if(WINDOWS)
        set(ssl_dll_suffix "")
        if(WIN64)
            set(ssl_dll_suffix "-${ARCH_ALIAS}")
        endif()
        add_custom_command(TARGET ${APP_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${AX_ROOT_DIR}/3rdparty/openssl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libcrypto-3${ssl_dll_suffix}.dll"
            "${AX_ROOT_DIR}/3rdparty/openssl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libssl-3${ssl_dll_suffix}.dll"
            "${AX_ROOT_DIR}/3rdparty/curl/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libcurl.dll"
            "${AX_ROOT_DIR}/3rdparty/zlib/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/zlib1.dll"
            $<TARGET_FILE_DIR:${APP_NAME}>)

        if (AX_ENABLE_AUDIO)
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}OpenAL32.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>)
        endif()

        if (BUILD_SHARED_LIBS)
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}glad.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}glfw.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}pugixml.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}freetype.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}axmol.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}cocostudio.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}GUI.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}particle3d.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}physics-nodes.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}spine.dll"
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}assets-manager.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>)
        endif()

        # Copy windows angle binaries
        if (AX_GLES_PROFILE)
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${AX_ROOT_DIR}/3rdparty/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libGLESv2.dll
                ${AX_ROOT_DIR}/3rdparty/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/libEGL.dll
                ${AX_ROOT_DIR}/3rdparty/angle/_x/lib/${PLATFORM_NAME}/${ARCH_ALIAS}/d3dcompiler_47.dll
                $<TARGET_FILE_DIR:${APP_NAME}>
            )
        endif()

        if (AX_ENABLE_MSEDGE_WEBVIEW2)
            if(CMAKE_GENERATOR MATCHES "Ninja")
                target_link_libraries(${APP_NAME} ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll.lib)
                target_include_directories(${APP_NAME} PRIVATE ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
                add_custom_command(TARGET ${APP_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/packages/Microsoft.Web.WebView2/build/native/${ARCH_ALIAS}/WebView2Loader.dll"
                        $<TARGET_FILE_DIR:${APP_NAME}>
                    )
            else()
                target_link_libraries(${APP_NAME} ${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/packages/Microsoft.Web.WebView2/build/native/Microsoft.Web.WebView2.targets)
            endif()
        endif()
    endif()

    # prebuilt, need copy axslc folder to target output directory
    get_target_property(rt_output ${APP_NAME} RUNTIME_OUTPUT_DIRECTORY)
    ax_sync_target_res(${APP_NAME} LINK_TO "${rt_output}/${CMAKE_CFG_INTDIR}/axslc" FOLDERS ${GLSLCC_OUT_DIR_ENGINE} SYNC_TARGET_ID axslc-builtin)
endfunction(ax_link_cxx_prebuilt)

function(ax_link_lua_prebuilt APP_NAME AX_ROOT_DIR AX_PREBUILT_DIR)
    if (NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${APP_NAME}
	        PRIVATE _USRLUASTATIC=1
        )
    endif()
    target_link_libraries(${APP_NAME} axlua lua-cjson tolua plainlua)

    ax_link_cxx_prebuilt(${APP_NAME} ${AX_ROOT_DIR} ${AX_PREBUILT_DIR})

    if (WINDOWS)
        if(MSVC)
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/${BUILD_CONFIG_DIR}plainlua.dll"
                    $<TARGET_FILE_DIR:${APP_NAME}>)
        else()
            add_custom_command(TARGET ${APP_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${AX_ROOT_DIR}/${AX_PREBUILT_DIR}/bin/$<CONFIG>/plainlua.dll"
                $<TARGET_FILE_DIR:${APP_NAME}>)
        endif()
    endif()
endfunction(ax_link_lua_prebuilt)
