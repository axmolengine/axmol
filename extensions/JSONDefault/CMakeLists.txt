set(target_name JSONDefault)

FILE(GLOB JSON_DEFAULT_HEADER src/JSONDefault/*.h)
FILE(GLOB JSON_DEFAULT_SOURCE src/JSONDefault/*.cpp)

add_library(${target_name} STATIC
  ${JSON_DEFAULT_HEADER}
  ${JSON_DEFAULT_SOURCE})

target_include_directories(${target_name} PUBLIC src)

ax_setup_ext_config(${target_name})
