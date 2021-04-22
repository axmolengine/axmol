
set(lib_name png)
set(target_name ${lib_name})

project(${lib_name})
include(../cmake/CocosExternalConfig.cmake)

set(${target_name}_src
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

if(WIN32)
    set(${target_name}_src ${${target_name}_src}
        intel/intel_init.c
        intel/filter_sse2_intrinsics.c
    )
elseif(ANDROID OR IOS)
        set(${target_name}_src ${${target_name}_src}
            arm/arm_init.c
            arm/filter_neon.S
            arm/filter_neon_intrinsics.c
            arm/palette_neon_intrinsics.c
        )
endif()

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

# platform optimizing flags
if(WIN32)
    target_compile_definitions(${target_name} PRIVATE PNG_INTEL_SSE_OPT=1)
elseif(ANDROID)
    target_compile_definitions(${target_name}
        PRIVATE PNG_ARM_NEON_CHECK_SUPPORTED=1
    )
endif()

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)
