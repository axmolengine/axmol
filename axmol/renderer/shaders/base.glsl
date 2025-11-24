
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


// ============================================================================
// Axmol Shader Builtin Sampler
// ============================================================================
// ============================================================================
// --- Linear sampling (default first) ---
#define LinearClamp       0  // Linear, clamp to edge (default)
#define LinearWrap        1  // Linear, repeat
#define LinearMirror      2  // Linear, mirror repeat
#define LinearBorder      3  // Linear, border color

// --- Point sampling ---
#define PointClamp        4  // Nearest, clamp to edge
#define PointWrap         5  // Nearest, repeat
#define PointMirror       6 // Nearest, mirror repeat
#define PointBorder       7 // Nearest, border color

// --- Linear + Mipmap ---
#define LinearMipClamp    8  // Linear min/mag, mip linear, clamp
#define LinearMipWrap     9  // Linear min/mag, mip linear, wrap
#define LinearMipMirror   10  // Linear min/mag, mip linear, mirror
#define LinearMipBorder   11  // Linear min/mag, mip linear, border

// --- Anisotropic filtering ---
#define AnisoClamp        12 // Anisotropic, clamp to edge
#define AnisoWrap         13 // Anisotropic, repeat
#define AnisoMirror       14 // Anisotropic, mirror repeat
#define AnisoBorder       15 // Anisotropic, border color

// --- Depth comparison samplers (shadow maps) ---
#define ShadowCmpClamp    16 // Compare sampler, clamp to edge
#define ShadowCmpWrap     17 // Compare sampler, repeat
#define ShadowCmpMirror   18 // Compare sampler, mirror repeat
#define ShadowCmpBorder   19 // Compare sampler, border color

// --- Special cases ---
#define LinearNoMipClamp  20 // Linear min/mag, no mip, clamp (UI, 2D sprites)
#define PointNoMipClamp   21 // Point min/mag, no mip, clamp (pixel art)

