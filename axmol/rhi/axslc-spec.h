// The Axmol Shader Compiler spec, define macros and structs, can be include anywhere
// match with axslcc-3.0.0+

#pragma once

#include <stdint.h>

namespace axslc
{

#define sc_makefourcc(_a, _b, _c, _d) \
    (((uint32_t)(_a) | ((uint32_t)(_b) << 8) | ((uint32_t)(_c) << 16) | ((uint32_t)(_d) << 24)))

#pragma pack(push, 1)

#define SC_CHUNK               sc_makefourcc('S', 'G', 'S', '3')
#define SC_CHUNK_STAG          sc_makefourcc('S', 'T', 'A', 'G')
#define SC_CHUNK_REFL          sc_makefourcc('R', 'E', 'F', 'L')
#define SC_CHUNK_CODE          sc_makefourcc('C', 'O', 'D', 'E')
#define SC_CHUNK_DATA          sc_makefourcc('D', 'A', 'T', 'A')

#define SC_LANG_GLES           sc_makefourcc('G', 'L', 'E', 'S')
#define SC_LANG_HLSL           sc_makefourcc('H', 'L', 'S', 'L')
#define SC_LANG_GLSL           sc_makefourcc('G', 'L', 'S', 'L')
#define SC_LANG_MSL            sc_makefourcc('M', 'S', 'L', ' ')
#define SC_LANG_SPIRV          sc_makefourcc('S', 'P', 'I', 'R')

#define SC_VERTEXFORMAT_FLOAT  sc_makefourcc('F', 'L', 'T', '1')
#define SC_VERTEXFORMAT_FLOAT2 sc_makefourcc('F', 'L', 'T', '2')
#define SC_VERTEXFORMAT_FLOAT3 sc_makefourcc('F', 'L', 'T', '3')
#define SC_VERTEXFORMAT_FLOAT4 sc_makefourcc('F', 'L', 'T', '4')
#define SC_VERTEXFORMAT_INT    sc_makefourcc('I', 'N', 'T', '1')
#define SC_VERTEXFORMAT_INT2   sc_makefourcc('I', 'N', 'T', '2')
#define SC_VERTEXFORMAT_INT3   sc_makefourcc('I', 'N', 'T', '3')
#define SC_VERTEXFORMAT_INT4   sc_makefourcc('I', 'N', 'T', '4')
#define SC_VERTEXFORMAT_MAT3   sc_makefourcc('M', 'A', 'T', '3')
#define SC_VERTEXFORMAT_MAT4   sc_makefourcc('M', 'A', 'T', '4')

#define SC_STAGE_VERTEX        sc_makefourcc('V', 'E', 'R', 'T')
#define SC_STAGE_FRAGMENT      sc_makefourcc('F', 'R', 'A', 'G')
#define SC_STAGE_COMPUTE       sc_makefourcc('C', 'O', 'M', 'P')

#define SC_IMAGEDIM_1D         sc_makefourcc('1', 'D', ' ', ' ')
#define SC_IMAGEDIM_2D         sc_makefourcc('2', 'D', ' ', ' ')
#define SC_IMAGEDIM_3D         sc_makefourcc('3', 'D', ' ', ' ')
#define SC_IMAGEDIM_CUBE       sc_makefourcc('C', 'U', 'B', 'E')
#define SC_IMAGEDIM_RECT       sc_makefourcc('R', 'E', 'C', 'T')
#define SC_IMAGEDIM_BUFFER     sc_makefourcc('B', 'U', 'F', 'F')
#define SC_IMAGEDIM_SUBPASS    sc_makefourcc('S', 'U', 'B', 'P')

enum Dim
{
    Dim1D          = 0,
    Dim2D          = 1,
    Dim3D          = 2,
    DimCube        = 3,
    DimRect        = 4,
    DimBuffer      = 5,
    DimSubpassData = 6
};

// SGS chunk
struct sc_chunk
{
    uint32_t lang;  // sc_shader_lang
    uint32_t profile_ver;
};

// REFL
struct sc_chunk_refl
{
    char name[32];
    uint32_t num_inputs;
    uint32_t num_textures;
    uint32_t num_uniform_buffers;
    uint32_t num_storage_images;
    uint32_t num_storage_buffers;
    uint16_t flatten_ubo;
    uint16_t debug_info;

    // inputs: sc_refl_input[num_inputs]
    // uniform-buffers: sc_refl_uniformbuffer[num_uniform_buffers]
    // textures: sc_refl_texture[num_textures]
    // storage_images: sc_refl_texture[num_storage_images]
    // storage_buffers: sc_refl_buffer[num_storage_buffers]
};

struct sc_refl_input
{
    char name[32];
    int32_t loc;
    char semantic[32];
    uint32_t semantic_index;
    uint32_t format;
};

// @since 3.0.0, modified
struct sc_refl_texture
{
    char name[32];
    int32_t binding;
    uint8_t image_dim;        // @see enum Dim: Dim1D, Dim2D, Dim3D, DimCube ...
    uint8_t multisample : 1;  // whether sampler2DMS
    uint8_t arrayed : 1;      // whether samplerXXArray
    uint8_t reserved : 6;     // reserved field
    uint16_t count;
};

struct sc_refl_buffer
{
    char name[32];
    int32_t binding;
    uint32_t size_bytes;
    uint32_t array_stride;
};

typedef struct sc_refl_uniformbuffer
{
    char name[32];
    int32_t binding;
    uint32_t size_bytes;
    uint16_t array_size;
    uint16_t num_members;
} sc_refl_ub;

typedef struct sc_refl_uniformbuffer_member
{
    char name[32];
    int32_t offset;
    uint32_t format;
    uint32_t size_bytes;
    uint16_t array_size;
} sc_refl_ub_member;
#pragma pack(pop)

}  // namespace axslc
