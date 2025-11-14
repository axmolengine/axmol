
#define DIRLIGHT TEXCOORD1
#define POINTLIGHT TEXCOORD2
#define SPOTLIGHT TEXCOORD4
#define SPOTLIGHT_NORM TEXCOORD5

/* follow macro in axslcc too large
    "TANGENT",
    "BINORMAL",
    "BLENDINDICES",
    "BLENDWEIGHT"
*/
#undef TANGENT
#undef BINORMAL
#undef BLENDINDICES
#undef BLENDWEIGHT

#define TANGENT TEXCOORD6
#define BINORMAL TEXCOORD7
#define BLENDINDICES COLOR1
#define BLENDWEIGHT COLOR2

#  define vfloat_def(x, y) vec4 x[(y + 3) / 4]
#  define vfloat_at(x, y) x[y / 4][y % 4]

#  define vvec2_def(x, y) vec4 x[(y * 2 + 3) / 4]
#  define vvec2_at1(x, y, z) x[(y / 2)][y % 2 * 2 + z]
#  define vvec2_at(x, y) vec2(vvec2_at1(x, y, 0), vvec2_at1(x, y, 1))

#  define vvec3_def(x, y) vec4 x[(y * 3 + 3) / 4]
#  define vvec3_at1(x, y, z) x[(y * 3 + z) / 4][((y * 3 + z) % 4)]
#  define vvec3_at(x, y) vec3(vvec3_at1(x, y, 0), vvec3_at1(x, y, 1), vvec3_at1(x, y, 2))

#  define RG8_CHANNEL xy

#  define FWIDTH(dist) 0.4


// ============================================================================
// Axmol Shader Binding Conventions
// ============================================================================
//
// To ensure consistent resource binding across all shaders in Axmol:
//
// - Uniform blocks are bound to set = 0
// - Samplers/textures are bound to set = 1
// - Vertex shader uniform block uses binding = 0
// - Fragment shader uniform block uses binding = 1
//
// These conventions minimize confusion and make pipeline layout predictable.
// If future changes are needed (e.g., adding more sets or bindings),
// update this header and notify contributors.
//
// ============================================================================

// Descriptor set indices
#define UNIFORM_SET   0
#define SAMPLER_SET   1

// Binding indices within uniform set
#define VS_UB_BINDING 0   // Vertex shader uniform block
#define FS_UB_BINDING 1   // Fragment shader uniform block
