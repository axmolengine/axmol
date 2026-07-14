// The Axmol Shader Compiler spec, define macros and structs
// match with axslcc-3.99.0+

#pragma once

#include <stdint.h>

namespace axslc
{

#define sc_makefourcc(_a, _b, _c, _d) \
    (((uint32_t)(_a) | ((uint32_t)(_b) << 8) | ((uint32_t)(_c) << 16) | ((uint32_t)(_d) << 24)))

#pragma pack(push, 1)

#define SC_CHUNK          sc_makefourcc('A', 'X', 'S', 'C')
#define SC_CHUNK_STAG     sc_makefourcc('S', 'T', 'A', 'G')
#define SC_CHUNK_REFL     sc_makefourcc('R', 'E', 'F', 'L')
#define SC_CHUNK_CODE     sc_makefourcc('C', 'O', 'D', 'E')

#define SC_BYTECODE_FLAG  0x80000000U  // bit 31 of profile_ver: code is bytecode (DXBC/DXIL)

#define SC_LANG_GLES      sc_makefourcc('G', 'L', 'E', 'S')
#define SC_LANG_HLSL      sc_makefourcc('H', 'L', 'S', 'L')
#define SC_LANG_GLSL      sc_makefourcc('G', 'L', 'S', 'L')
#define SC_LANG_MSL       sc_makefourcc('M', 'S', 'L', ' ')
#define SC_LANG_SPIRV     sc_makefourcc('S', 'P', 'I', 'R')

#define SC_STAGE_VERTEX   sc_makefourcc('V', 'E', 'R', 'T')
#define SC_STAGE_FRAGMENT sc_makefourcc('F', 'R', 'A', 'G')
#define SC_STAGE_COMPUTE  sc_makefourcc('C', 'O', 'M', 'P')

#define SC_NAME_LEN       32

enum Dim : uint16_t
{
    Dim1D          = 0,
    Dim2D          = 1,
    Dim3D          = 2,
    DimCube        = 3,
    DimRect        = 4,
    DimBuffer      = 5,
    DimSubpassData = 6
};

enum ShaderLang
{
    SHADER_LANG_NONE = -1,
    SHADER_LANG_ESSL = 0,
    SHADER_LANG_HLSL,
    SHADER_LANG_MSL,
    SHADER_LANG_GLSL,
    SHADER_LANG_SPIRV,
    SHADER_LANG_COUNT
};

enum SamplerSource : uint8_t
{
    SC_SAMPLER_SOURCE_SHADER_PRESET = 0,
    SC_SAMPLER_SOURCE_TEXTURE_OWNED = 1,
    SC_SAMPLER_SOURCE_CUSTOM        = 2,
};

enum SCType : uint16_t
{
    // Float
    SC_TYPE_FLOAT4,
    SC_TYPE_FLOAT3,
    SC_TYPE_FLOAT2,
    SC_TYPE_FLOAT,
    // Int
    SC_TYPE_INT4,
    SC_TYPE_INT3,
    SC_TYPE_INT2,
    SC_TYPE_INT,
    // Unsigned / Packed (vertex inputs)
    SC_TYPE_USHORT4,
    SC_TYPE_USHORT2,
    SC_TYPE_UBYTE4,
    // Matrix
    SC_TYPE_MAT4,
    SC_TYPE_MAT3,
    // Half precision (uniforms)
    SC_TYPE_HALF4,
    SC_TYPE_HALF3,
    SC_TYPE_HALF2,
    SC_TYPE_HALF,
};

// sc chunk aka file header
struct sc_chunk
{
    uint16_t major;  // SC_VERSION_MAJOR
    uint16_t minor;  // SC_VERSION_MINOR
    uint16_t num_targets;
    uint16_t reserved;
};

struct sc_target_entry
{
    uint32_t lang;  // SHADER_LANG_ESSL / HLSL / MSL / SPIRV ...
    uint32_t profile_ver;
    uint32_t offset;
};

// REFL
struct sc_chunk_refl
{
    char name[SC_NAME_LEN];
    uint32_t num_inputs;
    uint32_t num_textures;
    uint32_t num_samplers;
    uint32_t num_sampling_pairs;
    uint32_t num_uniform_buffers;
    uint32_t num_storage_images;
    uint32_t num_storage_buffers;
    uint16_t flatten_ubo;
    uint16_t debug_info;

    // inputs: sc_refl_input[num_inputs]
    // uniform-buffers: sc_refl_uniformbuffer[num_uniform_buffers]
    // textures: sc_refl_texture[num_textures]
    // samplers: sc_refl_sampler[num_samplers]
    // sampling-pairs: sc_refl_sampling_pair[num_sampling_pairs]
    // storage_images: sc_refl_texture[num_storage_images]
    // storage_buffers: sc_refl_buffer[num_storage_buffers]
};

struct sc_refl_input
{
    char name[SC_NAME_LEN];
    char semantic[SC_NAME_LEN];
    int32_t location;
    uint16_t semantic_index;
    uint16_t var_type;
};

// @since 3.0.0, modified
struct sc_refl_texture
{
    char name[SC_NAME_LEN];
    int32_t binding;
    uint16_t descriptor_set;
    uint8_t image_dim;        // @see enum Dim: Dim1D, Dim2D, Dim3D, DimCube ...
    uint8_t multisample : 1;  // whether sampler2DMS
    uint8_t arrayed : 1;      // whether samplerXXArray
    uint8_t reserved : 6;     // reserved field
    uint16_t count;
    uint8_t sampler_source;  // SamplerSource
    uint8_t reserved2;
};

struct sc_refl_sampler
{
    char name[SC_NAME_LEN];
    int32_t binding;
    uint16_t descriptor_set;
    uint16_t count;
    int16_t preset_index;  // -1 when not a base.hlsli preset
    uint8_t comparison;
    uint8_t reserved;
};

struct sc_refl_sampling_pair
{
    int32_t texture_binding;
    int32_t sampler_binding;
    uint16_t texture_set;
    uint16_t sampler_set;
    int16_t preset_index;
    uint8_t sampler_source;
    uint8_t reserved;
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
    char name[SC_NAME_LEN];
    int32_t binding;
    uint32_t size_bytes;
    uint16_t array_size;
    uint16_t num_members;
} sc_refl_ub;

typedef struct sc_refl_uniformbuffer_member
{
    char name[SC_NAME_LEN];
    int32_t offset;
    uint32_t size_bytes;
    uint16_t array_size;
    uint16_t var_type;
} sc_refl_ub_member;
#pragma pack(pop)

}  // namespace axslc
