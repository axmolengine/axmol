set(target_name assets-manager)

FILE(GLOB_RECURSE ASSETS_MANAGER_HEADER *.h;./**/*.h)
FILE(GLOB_RECURSE ASSETS_MANAGER_SOURCE *.cpp;./**/*.cpp)

add_library(${target_name}
  ${ASSETS_MANAGER_HEADER}
  ${ASSETS_MANAGER_SOURCE})

target_include_directories(${target_name} PUBLIC src)

ax_setup_ext_config(${target_name})
