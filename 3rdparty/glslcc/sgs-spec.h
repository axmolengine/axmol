// sgs spec, define sgs macros and structs, can be include anywhere
// match with glslcc-1.9.0

#pragma once

#include <stdint.h>

#define sgs_makefourcc(_a, _b, _c, _d) \
    (((uint32_t)(_a) | ((uint32_t)(_b) << 8) | ((uint32_t)(_c) << 16) | ((uint32_t)(_d) << 24)))

#pragma pack(push, 1)

#define SGS_CHUNK           sgs_makefourcc('S', 'G', 'S', ' ')
#define SGS_CHUNK_STAG      sgs_makefourcc('S', 'T', 'A', 'G')
#define SGS_CHUNK_REFL      sgs_makefourcc('R', 'E', 'F', 'L')
#define SGS_CHUNK_CODE      sgs_makefourcc('C', 'O', 'D', 'E')
#define SGS_CHUNK_DATA      sgs_makefourcc('D', 'A', 'T', 'A')

#define SGS_LANG_GLES sgs_makefourcc('G', 'L', 'E', 'S')
#define SGS_LANG_HLSL sgs_makefourcc('H', 'L', 'S', 'L')
#define SGS_LANG_GLSL sgs_makefourcc('G', 'L', 'S', 'L')
#define SGS_LANG_MSL  sgs_makefourcc('M', 'S', 'L', ' ')
#define SGS_LANG_SPIRV sgs_makefourcc('S', 'P', 'I', 'R')

#define SGS_VERTEXFORMAT_FLOAT      sgs_makefourcc('F', 'L', 'T', '1')
#define SGS_VERTEXFORMAT_FLOAT2     sgs_makefourcc('F', 'L', 'T', '2')
#define SGS_VERTEXFORMAT_FLOAT3     sgs_makefourcc('F', 'L', 'T', '3')
#define SGS_VERTEXFORMAT_FLOAT4     sgs_makefourcc('F', 'L', 'T', '4')
#define SGS_VERTEXFORMAT_INT        sgs_makefourcc('I', 'N', 'T', '1')
#define SGS_VERTEXFORMAT_INT2       sgs_makefourcc('I', 'N', 'T', '2')
#define SGS_VERTEXFORMAT_INT3       sgs_makefourcc('I', 'N', 'T', '3')
#define SGS_VERTEXFORMAT_INT4       sgs_makefourcc('I', 'N', 'T', '4')
#define SGS_VERTEXFORMAT_MAT3       sgs_makefourcc('M', 'A', 'T', '3')
#define SGS_VERTEXFORMAT_MAT4       sgs_makefourcc('M', 'A', 'T', '4')

#define SGS_STAGE_VERTEX            sgs_makefourcc('V', 'E', 'R', 'T')
#define SGS_STAGE_FRAGMENT          sgs_makefourcc('F', 'R', 'A', 'G')
#define SGS_STAGE_COMPUTE           sgs_makefourcc('C', 'O', 'M', 'P')

#define SGS_IMAGEDIM_1D            sgs_makefourcc('1', 'D', ' ', ' ')
#define SGS_IMAGEDIM_2D            sgs_makefourcc('2', 'D', ' ', ' ')
#define SGS_IMAGEDIM_3D            sgs_makefourcc('3', 'D', ' ', ' ')
#define SGS_IMAGEDIM_CUBE          sgs_makefourcc('C', 'U', 'B', 'E')
#define SGS_IMAGEDIM_RECT          sgs_makefourcc('R', 'E', 'C', 'T')
#define SGS_IMAGEDIM_BUFFER        sgs_makefourcc('B', 'U', 'F', 'F')
#define SGS_IMAGEDIM_SUBPASS       sgs_makefourcc('S', 'U', 'B', 'P')

// SGS chunk
struct sgs_chunk {
    uint32_t lang;          // sgs_shader_lang
    uint32_t profile_ver;   // 
};

// REFL
struct sgs_chunk_refl {
    char     name[32];
    uint32_t num_inputs;
    uint32_t num_textures;
    uint32_t num_uniform_buffers;
    uint32_t num_storage_images;
    uint32_t num_storage_buffers;
    uint16_t flatten_ubos;
    uint16_t debug_info;

    // inputs: sgs_refl_input[num_inputs]
    // uniform-buffers: sgs_refl_uniformbuffer[num_uniform_buffers]
    // textures: sgs_refl_texture[num_textures]
    // storage_images: sgs_refl_texture[num_storage_images]
    // storage_buffers: sgs_refl_buffer[num_storage_buffers]
};

struct sgs_refl_input {
    char     name[32];
    int32_t  loc;
    char     semantic[32];
    uint32_t semantic_index;
    uint32_t format;
};

struct sgs_refl_texture {
    char     name[32];
    int32_t  binding;
    uint32_t image_dim;
    uint8_t  multisample;
    uint8_t  is_array;
};

struct sgs_refl_buffer {
    char    name[32];
    int32_t binding;
    uint32_t size_bytes;
    uint32_t array_stride;
};

typedef struct sgs_refl_uniformbuffer {
    char     name[32];
    int32_t  binding;
    uint32_t size_bytes;
    uint16_t array_size;
    uint16_t num_members;
} sgs_refl_ub;

typedef struct sgs_refl_uniformbuffer_member {
    char     name[32];
    int32_t  offset;
    uint32_t format;
    uint32_t size_bytes;
    uint16_t array_size;
} sgs_refl_ub_member;
#pragma pack(pop)
