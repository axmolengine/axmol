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

// The axslcc sampler presets
struct SamplerPreset
{
    enum enum_type : uint32_t
    {
        // --- Linear sampling ---
        LinearClamp,   // Linear, clamp to edge
        LinearWrap,    // Linear, repeat
        LinearMirror,  // Linear, mirror repeat
        LinearBorder,  // Linear, border color

        // --- Point sampling ---
        PointClamp,   // Nearest, clamp to edge
        PointWrap,    // Nearest, repeat
        PointMirror,  // Nearest, mirror repeat
        PointBorder,  // Nearest, border color

        // --- Linear + Mipmap ---
        LinearMipClamp,   // Linear min/mag, mip linear, clamp
        LinearMipWrap,    // Linear min/mag, mip linear, wrap
        LinearMipMirror,  // Linear min/mag, mip linear, mirror
        LinearMipBorder,  // Linear min/mag, mip linear, border

        // --- Anisotropic filtering ---
        AnisoClamp,   // Anisotropic, clamp to edge
        AnisoWrap,    // Anisotropic, repeat
        AnisoMirror,  // Anisotropic, mirror repeat
        AnisoBorder,  // Anisotropic, border color

        // --- Depth comparison samplers (shadow maps) ---
        ShadowCmpClamp,   // Compare sampler, clamp to edge
        ShadowCmpWrap,    // Compare sampler, repeat
        ShadowCmpMirror,  // Compare sampler, mirror repeat
        ShadowCmpBorder,  // Compare sampler, border color

        // --- Special cases ---
        LinearNoMipClamp,  // Linear min/mag, no mip, clamp (UI, 2D sprites)
        PointNoMipClamp,   // Point min/mag, no mip, clamp (pixel art)

        //
        Count
    };
};
inline constexpr uint16_t kPresetSamplerDescriptorSet = 1;
inline constexpr uint16_t kCustomSamplerDescriptorSet = 2;

inline constexpr int16_t kInvalidSamplerPreset = -1;

inline constexpr int32_t kVulkanSamplerBindingShift = 1024;

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
    uint32_t num_uniform_buffers;
    uint32_t num_storage_images;
    uint32_t num_storage_buffers;
    uint16_t flatten_ubo;
    uint16_t debug_info;

    // inputs: sc_refl_input[num_inputs]
    // uniform-buffers: sc_refl_uniformbuffer[num_uniform_buffers]
    // textures: sc_refl_texture[num_textures]
    // samplers: sc_refl_sampler[num_samplers]
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
    uint16_t reserved2;
};

enum SCSamplerFlags : uint8_t
{
    SC_SAMPLER_FLAG_NONE       = 0,
    SC_SAMPLER_FLAG_COMPARISON = 1 << 0,
};

struct sc_refl_sampler
{
    // Sampler variable name as declared in the source shader.
    char name[SC_NAME_LEN];

    // Logical sampler register index inside descriptor_set.
    //
    // Built-in sampler:
    //   descriptor_set == kPresetSamplerDescriptorSet
    //   binding == preset_index
    //   valid range: [0, SamplerPreset::Count)
    //
    // Custom sampler:
    //   descriptor_set == kCustomSamplerDescriptorSet
    //   binding is the Program-local custom sampler index
    //   valid range: [0, custom_sampler_count)
    //
    // This value is not:
    //   - a SamplerRegistry SamplerId;
    //   - a D3D12 sampler heap slot;
    //   - a Vulkan implementation-specific descriptor index;
    //   - a Metal or D3D11 backend sampler slot.
    int32_t binding;

    // HLSL register space (logical shader namespace).
    //
    // This is NOT a Vulkan descriptor set nor any backend binding slot.
    // Backends translate logical (space, binding) to backend-specific
    // bindings independently.
    //
    // kPresetSamplerDescriptorSet for Axmol built-in presets.
    // kCustomSamplerDescriptorSet for Program-local custom samplers.
    uint16_t space;

    // Number of sampler array elements.
    // This is normally 1 for a non-array SamplerState declaration.
    uint16_t count;

    // SamplerPreset enum value for an Axmol built-in sampler.
    //
    // Built-in sampler:
    //   preset_index >= 0
    //   binding must equal preset_index.
    //
    // Custom sampler:
    //   preset_index == kInvalidSamplerPreset.
    int16_t preset_index;

    // Bitmask of SCSamplerFlags.
    //
    // SC_SAMPLER_FLAG_COMPARISON may be set only when axslcc can
    // reliably determine that the declaration is a comparison sampler.
    // Leave this field as SC_SAMPLER_FLAG_NONE when that information
    // cannot be determined reliably.
    uint8_t flags;

    // Reserved for future serialized sampler reflection fields.
    // Must be initialized to zero.
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

constexpr bool sc_sampler_is_preset(const sc_refl_sampler& sampler)
{
    return sampler.preset_index >= 0;
}

constexpr bool sc_sampler_is_custom(const sc_refl_sampler& sampler)
{
    return sampler.preset_index == kInvalidSamplerPreset;
}

}  // namespace axslc
