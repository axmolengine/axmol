
#define DIRLIGHT TEXCOORD1
#define POINTLIGHT TEXCOORD2
#define SPOTLIGHT TEXCOORD4
#define SPOTLIGHT_NORM TEXCOORD5

/* follow macro in glslcc too large
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

#if !defined(GLES2)

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

#else

#  define vfloat_def(x, y) float x[y]
#  define vfloat_at(x, y) x[y]

#  define vvec2_def(x, y) vec2 x[y]
#  define vvec2_at1(x, y, z) x[y][z]
#  define vvec2_at(x, y) x[y]

#  define vvec3_def(x, y) vec3 x[y]
#  define vvec3_at1(x, y, z) x[y][z]
#  define vvec3_at(x, y) x[y]

#  define RG8_CHANNEL xw // emulate RG8 with LA8

#  define FWIDTH(dist) fwidth(dist)

#endif
