# x-studio spec extensions
if(BUILD_EXTENSION_XS_IDE)
    ## libeditor
    add_subdirectory(libeditor)
    if(BUILD_EXTENSION_SPINE)
        target_compile_definitions(libeditor PUBLIC CC_BUILD_WITH_SPINE=1)
        if(BUILD_SHARED_LIBS)
          target_link_libraries(libeditor spine particle3d)
        else()
          target_include_directories(libeditor PUBLIC ${CMAKE_CURRENT_LIST_DIR}/spine/runtime/include)
        endif()
    endif()

    ## libccs30
    add_subdirectory(libccs30)
    if(BUILD_EXTENSION_DRAGONBONES)
        target_compile_definitions(libccs30 PRIVATE CC_BUILD_WITH_DRANGBONES=1)
        if(BUILD_SHARED_LIBS)
          target_link_libraries(libccs30 DragonBones)
        endif()
    endif()
    target_link_libraries(libccs30 libeditor)

    ## libccs19
    add_subdirectory(libccs19)
    target_link_libraries(libccs19 libeditor)

    ## libccs20
    add_subdirectory(libccs20)
    target_link_libraries(libccs20 libeditor)

    ## libccs21
    add_subdirectory(libccs21)
    target_link_libraries(libccs21 libeditor)
endif()
