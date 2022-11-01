
set(lib_name clipper2)
set(target_name ${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
	clipper.core.h
	clipper.engine.cpp
	clipper.engine.h
	clipper.export.h
	clipper.h
	clipper.minkowski.h
	clipper.offset.cpp
	clipper.offset.h
	clipper.rectclip.cpp
	clipper.rectclip.h
)

target_include_directories(${target_name} PUBLIC .)
