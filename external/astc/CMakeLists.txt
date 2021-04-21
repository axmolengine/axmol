
set(lib_name astc)
set(target_name ${lib_name})

project(${lib_name})

set(${target_name}_src
    astc_averages_and_directions.cpp 
    astc_block_sizes2.cpp 
    astc_color_quantize.cpp 
    astc_color_unquantize.cpp 
    astc_compress_symbolic.cpp 
    astc_compute_variance.cpp 
    astc_decompress_symbolic.cpp 
    astc_encoding_choice_error.cpp 
    astc_find_best_partitioning.cpp 
    astc_ideal_endpoints_and_weights.cpp 
    astc_image_load_store.cpp 
    astc_integer_sequence.cpp 
    astc_kmeans_partitioning.cpp 
    # astc_ktx_dds.cpp 
    # astc_main.cpp 
    astc_partition_tables.cpp 
    astc_percentile_tables.cpp 
    astc_pick_best_endpoint_format.cpp 
    astc_quantization.cpp 
    # astc_stb_tga.cpp 
    astc_symbolic_physical.cpp 
    astc_toplevel.cpp 
    astc_weight_align.cpp 
    astc_weight_quant_xfer_tables.cpp 
    mathlib.cpp 
    softfloat.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC ..)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

