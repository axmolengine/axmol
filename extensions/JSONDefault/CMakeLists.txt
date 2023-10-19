set(target_name JSONDefault)

FILE(GLOB JSON_DEFAULT_HEADER *.h)
FILE(GLOB JSON_DEFAULT_SOURCE *.cpp)

add_library(${target_name} STATIC
        ${JSON_DEFAULT_HEADER} 
        ${JSON_DEFAULT_SOURCE})

setup_ax_extension_config(${target_name})
