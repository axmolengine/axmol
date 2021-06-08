
set(lib_name openssl)

project(${lib_name})

include(../cmake/CocosExternalConfig.cmake)

set(OPENSSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" CACHE STRING "OpenSSL include dir" FORCE)

# -----macro: set openssl sub target-----
macro(set_openssl_sub_target sub_target_name sub_lib_name)
  
  if(WINDOWS)
    add_library(${sub_target_name} SHARED IMPORTED GLOBAL)
  else()
    add_library(${sub_target_name} STATIC IMPORTED GLOBAL)
  endif()

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )

  set(imp_lib_name "")
  if(WINDOWS)
    set(imp_lib_name "${platform_spec_path}/lib${sub_lib_name}.lib")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${platform_spec_path}/lib${sub_lib_name}-1_1.dll"
      IMPORTED_IMPLIB "${platform_spec_path}/lib${sub_lib_name}.lib"
    )
  else()
    set(imp_lib_name "${platform_spec_path}/lib${sub_lib_name}.a")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${platform_spec_path}/lib${sub_lib_name}.a"
    )
  endif()
  if(${sub_lib_name} STREQUAL "ssl")
    set(OPENSSL_SSL_LIBRARY "${imp_lib_name}" CACHE STRING "" FORCE)
  else()
    set(OPENSSL_CRYPTO_LIBRARY "${imp_lib_name}" CACHE STRING "" FORCE)
  endif()
endmacro()

# -----sub target 1: ssl-----
set(target_name OpenSSL::SSL ssl)
set_openssl_sub_target(${target_name})

# -----sub target 2: crypto-----
set(target_name OpenSSL::Crypto crypto)
set_openssl_sub_target(${target_name})

