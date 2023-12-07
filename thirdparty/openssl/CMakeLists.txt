
set(lib_name openssl)

project(${lib_name})

_1kfetch_dist(openssl "${PARENT_DIRECTORY}/_deps")

set(OPENSSL_INCLUDE_DIR "${openssl_INC_DIR}" CACHE STRING "OpenSSL include dir" FORCE)

# -----macro: set openssl sub target-----
macro(set_openssl_sub_target sub_target_name sub_lib_name)
  
  if(WINDOWS)
    add_library(${sub_target_name} SHARED IMPORTED GLOBAL)
  else()
    add_library(${sub_target_name} STATIC IMPORTED GLOBAL)
  endif()

  set_target_properties(${sub_target_name} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${openssl_INC_DIR}"
  )

  set(imp_lib_name "")
  if(WINDOWS)
    set(impl_lib_suffix "")
    if(WIN64)
        set(impl_lib_suffix "-${ARCH_ALIAS}")
    endif()
    set(imp_lib_name "${openssl_LIB_DIR}/lib${sub_lib_name}.lib")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${openssl_LIB_DIR}/lib${sub_lib_name}-3${impl_lib_suffix}.dll"
      IMPORTED_IMPLIB "${openssl_LIB_DIR}/lib${sub_lib_name}.lib"
    )
  else()
    set(imp_lib_name "${openssl_LIB_DIR}/lib${sub_lib_name}.a")
    set_target_properties(${sub_target_name} PROPERTIES
      IMPORTED_LOCATION "${openssl_LIB_DIR}/lib${sub_lib_name}.a"
    )
  endif()
  if(${sub_lib_name} STREQUAL "ssl")
    set(OPENSSL_ROOT_DIR "${openssl_LIB_DIR}" CACHE STRING "" FORCE)
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
