#include "CCColorizer.h"
#include "math/Mat3.h"
#include "base/ccTypes.h"
#include "renderer/backend/ProgramCache.h"
#include "renderer/backend/Device.h"

NS_CC_BEGIN

static const char* s_smart_hsv_frag = R"(
#ifdef GL_ES
precision mediump float;
#endif 

varying vec2 v_texCoord;  
varying vec4 v_fragmentColor;

// HSV matrix
uniform mat3 u_mix_hsv;

// filter color RGB values
uniform vec3 u_filter_rgb;

// force shading HSV values
uniform vec3 u_shading_hsv;

// whether use force shading
uniform int u_force_shading;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() 
{ 
    vec4 pixColor = texture2D(CC_Texture0, v_texCoord);  // * v_fragmentColor; 
    vec3 rgbColor = u_mix_hsv * pixColor.rgb;

    float sum = pixColor.r + pixColor.g + pixColor.b;
    float rv = pixColor.r / sum;
    float gv = pixColor.g / sum;
    float bv = pixColor.b / sum;
    if(  (rv < u_filter_rgb.r && gv < u_filter_rgb.g && bv < u_filter_rgb.b) || pixColor.a < 0.1)
    { // color filters, resume to original color
         rgbColor = pixColor.rgb;

         rgbColor.r *= v_fragmentColor.a;
         rgbColor.g *= v_fragmentColor.a;
         rgbColor.b *= v_fragmentColor.a;
    } 
    else { // shading color
        if(pixColor.a > 0.0) {
            if( u_force_shading != 0 ) {  // force shading color
              if(gv > 0.115 && rgbColor.g > 0.115 && !(abs(rv - gv) < 0.10589 && abs(rv - bv) < 0.10589 && abs(gv - bv) < 0.0909) || ( rgbColor.b > 0.6280 && (bv - rv)  > 0.0599 ) ) { // currently is yellow, TODO: use hh,ss,vv
                  rgbColor.r = min(1.0, rgbColor.g * 1.2525);
                  rgbColor.b *= rgbColor.b;
                  rgbColor.g *= 0.9025;

                  rgbColor.r *= v_fragmentColor.a;
                  rgbColor.g *= v_fragmentColor.a;
                  rgbColor.b *= v_fragmentColor.a;
              }
           }
       }
    }
    rgbColor.rgb = rgbColor.rgb * v_fragmentColor.rgb;
    gl_FragColor = vec4(rgbColor, pixColor.a * v_fragmentColor.a); 
} 
)";

static const char* s_smart_hsv_frag_as = R"(#ifdef GL_ES
precision mediump float;
#endif 

varying vec2 v_texCoord;  
varying vec4 v_fragmentColor;

// HSV matrix
uniform mat3 u_mix_hsv;

// filter color RGB values
uniform vec3 u_filter_rgb;

// force shading HSV values
uniform vec3 u_shading_hsv;

// whether use force shading
uniform int u_force_shading;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() 
{ 
    vec4 texColor = vec4(texture2D(CC_Texture0, v_texCoord).rgb, texture2D(CC_Texture1, v_texCoord).r);
    texColor.rgb *= texColor.a; // Premultiply with Alpha channel
    
    vec3 rgbColor = u_mix_hsv * texColor.rgb;

    float sum = texColor.r + texColor.g + texColor.b;
    float rv = texColor.r / sum;
    float gv = texColor.g / sum;
    float bv = texColor.b / sum;
    if(  (rv < u_filter_rgb.r && gv < u_filter_rgb.g && bv < u_filter_rgb.b) || texColor.a < 0.1)
    { // color filters, resume to original color
         rgbColor = texColor.rgb;

         rgbColor.r *= v_fragmentColor.a;
         rgbColor.g *= v_fragmentColor.a;
         rgbColor.b *= v_fragmentColor.a;
    } 
    else { // shading color
        if(texColor.a > 0.0) {
            if( u_force_shading != 0 ) {  // force shading color
              if(gv > 0.115 && rgbColor.g > 0.115 && !(abs(rv - gv) < 0.10589 && abs(rv - bv) < 0.10589 && abs(gv - bv) < 0.0909) || ( rgbColor.b > 0.6280 && (bv - rv)  > 0.0599 ) ) { // currently is yellow, TODO: use hh,ss,vv
                  rgbColor.r = min(1.0, rgbColor.g * 1.2525);
                  rgbColor.b *= rgbColor.b;
                  rgbColor.g *= 0.9025;

                  rgbColor.r *= v_fragmentColor.a;
                  rgbColor.g *= v_fragmentColor.a;
                  rgbColor.b *= v_fragmentColor.a;
              }
           }
       }
    }
    rgbColor.rgb = rgbColor.rgb * v_fragmentColor.rgb;
    gl_FragColor = vec4(rgbColor, texColor.a * v_fragmentColor.a); 
} 
)";

#define FLT_SQRT_2 1.4142135623730950488016887242097
#define FLT_SQRT_3 1.7320508075688772935274463415059
#define FLT_XRS (1 / FLT_SQRT_2)
#define FLT_XRC FLT_XRS
#define FLT_YRS (-1 / FLT_SQRT_3)
#define FLT_YRC (FLT_SQRT_2 / FLT_SQRT_3)
#define RLUM    (0.3086)
#define GLUM    (0.6094)
#define BLUM    (0.0820)

typedef float _M3X3[3][3];
typedef float _M4X4[4][4];

static Vec3 fastest_rgb2hsv(const Color3B& rgb)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float h, s, v;

    float K = 0.f;

    if (g < b)
    {
        std::swap(g, b);
        K = -1.f;
    }

    if (r < g)
    {
        std::swap(r, g);
        K = -2.f / 6.f - K;
    }

    float chroma = r - std::min(g, b);
    h = fabs(K + (g - b) / (6.f * chroma + 1e-20f));
    s = chroma / (r + 1e-20f);
    v = r;

    return Vec3(h, s, v);
}

static Vec3 rgb2hsv(const Color3B& rgb)
{
    unsigned char dst_h, dst_s, dst_v;

    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

    float max = std::max({ r, g, b });
    float min = std::min({ r, g, b });

    v = max;

    if (max == 0.0f) {
        s = 0;
        h = 0;
    }
    else if (max - min == 0.0f) {
        s = 0;
        h = 0;
    }
    else {
        s = (max - min) / max;

        if (max == r) {
            h = 60 * ((g - b) / (max - min)) + 0;
        }
        else if (max == g) {
            h = 60 * ((b - r) / (max - min)) + 120;
        }
        else {
            h = 60 * ((r - g) / (max - min)) + 240;
        }
    }

    if (h < 0) h += 360.0f;

    dst_h = (unsigned char)(h / 2);   // dst_h : 0-180
    dst_s = (unsigned char)(s/* * 255*/); // dst_s : 0-255
    dst_v = (unsigned char)(v /** 255*/); // dst_v : 0-255

    return Vec3(dst_h / 180.f, dst_s, dst_v);
}

static void hsv2rgb(const unsigned char& src_h, const unsigned char& src_s, const unsigned char& src_v, unsigned char& dst_r, unsigned char& dst_g, unsigned char& dst_b)
{
    float h = src_h * 2.0f; // 0-360
    float s = src_s / 255.0f; // 0.0-1.0
    float v = src_v / 255.0f; // 0.0-1.0

    float r, g, b; // 0.0-1.0

    int   hi = (int)(h / 60.0f) % 6;
    float f = (h / 60.0f) - hi;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (hi) {
    case 0: r = v, g = t, b = p; break;
    case 1: r = q, g = v, b = p; break;
    case 2: r = p, g = v, b = t; break;
    case 3: r = p, g = q, b = v; break;
    case 4: r = t, g = p, b = v; break;
    case 5: r = v, g = p, b = q; break;
    }

    dst_r = (unsigned char)(r * 255); // dst_r : 0-255
    dst_g = (unsigned char)(g * 255); // dst_r : 0-255
    dst_b = (unsigned char)(b * 255); // dst_r : 0-255
}

static void printmat3(float mat[3][3])
{
    int x, y;
    char svalue[256];
    int n = 0;
    for (y = 0; y < 3; y++) {
        for (x = 0; x < 3; x++)
            n += sprintf(svalue + n, "%f ", mat[y][x]);
        n += sprintf(svalue + n, "\n");
    }
    n += sprintf(svalue + n, "\n");

    log("%s", svalue);
}

static void printmat4(float mat[4][4])
{
    int x, y;
    char svalue[256];
    int n = 0;
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++)
            n += sprintf(svalue + n, "%f ", mat[y][x]);
        n += sprintf(svalue + n, "\n");
    }
    n += sprintf(svalue + n, "\n");

    log("%s", svalue);
}

inline void printmat3(float(&mat)[9])
{
    printmat3((_M3X3&)mat);
}

inline void printmat4(float(&mat)[16])
{
    printmat4((_M4X4&)mat);
}

inline void createRotationX(Mat4* dst, float sv, float cv)
{
    dst->m[5] = cv;
    dst->m[6] = sv;
    dst->m[9] = -sv;
    dst->m[10] = cv;
}

inline void createRotationY(Mat4* dst, float sv, float cv)
{
    dst->m[0] = cv;
    dst->m[2] = -sv;
    dst->m[8] = sv;
    dst->m[10] = cv;
}

inline void createRotationZ(Mat4* dst, float sv, float cv)
{
    dst->m[0] = cv;
    dst->m[1] = sv;
    dst->m[4] = -sv;
    dst->m[5] = cv;
}

inline void rotateX(Mat4* dst, float sv, float cv)
{
    Mat4 temp;
    createRotationX(&temp, sv, cv);
    Mat4::multiply(temp, *dst, dst);
}

inline void rotateY(Mat4* dst, float sv, float cv)
{
    Mat4 temp;
    createRotationY(&temp, sv, cv);
    Mat4::multiply(temp, *dst, dst); // dst->multiply(temp); pitfall: matrix1 * matrix2 != matrix2 * matrix1
}

inline void rotateZ(Mat4* dst, float sv, float cv)
{
    Mat4 temp;
    createRotationZ(&temp, sv, cv);
    Mat4::multiply(temp, *dst, dst);
}

inline void rotateZ(Mat4* dst, float angle)
{
    Mat4 temp;
    Mat4::createRotationZ(angle, &temp);
    Mat4::multiply(temp, *dst, dst);
}

/*
*    matrixmult -
*        multiply two matricies
*/
static void matrixmult(float a[4][4], float b[4][4], float c[4][4])
{
    int x, y;
    float temp[4][4];

    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++) {
            temp[y][x] = b[y][0] * a[0][x]
                + b[y][1] * a[1][x]
                + b[y][2] * a[2][x]
                + b[y][3] * a[3][x];
        }
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
            c[y][x] = temp[y][x];
}

/*
*    identmat -
*        make an identity matrix
*/
inline void identmat(float* matrix)
{
    *matrix++ = 1.0;    /* row 1        */
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 2        */
    *matrix++ = 1.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 3        */
    *matrix++ = 0.0;
    *matrix++ = 1.0;
    *matrix++ = 0.0;
    *matrix++ = 0.0;    /* row 4        */
    *matrix++ = 0.0;
    *matrix++ = 0.0;
    *matrix++ = 1.0;
}

/*
*    xformpnt -
*        transform a 3D point using a matrix
*/
static void xformpnt(float matrix[4][4], float x, float y, float z, float* tx, float* ty, float* tz)
{
    *tx = x * matrix[0][0] + y * matrix[1][0] + z * matrix[2][0] + matrix[3][0];
    *ty = x * matrix[0][1] + y * matrix[1][1] + z * matrix[2][1] + matrix[3][1];
    *tz = x * matrix[0][2] + y * matrix[1][2] + z * matrix[2][2] + matrix[3][2];
}

/*
*    cscalemat -
*        make a color scale marix
*/
static void cscalemat(float mat[4][4], float rscale, float gscale, float bscale)
{
    float mmat[4][4];

    mmat[0][0] = rscale;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = gscale;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;


    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = bscale;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    lummat -
*        make a luminance marix
*/
static void lummat(float mat[4][4])
{
    float mmat[4][4];
    float rwgt, gwgt, bwgt;

    rwgt = RLUM;
    gwgt = GLUM;
    bwgt = BLUM;
    mmat[0][0] = rwgt;
    mmat[0][1] = rwgt;
    mmat[0][2] = rwgt;
    mmat[0][3] = 0.0;

    mmat[1][0] = gwgt;
    mmat[1][1] = gwgt;
    mmat[1][2] = gwgt;
    mmat[1][3] = 0.0;

    mmat[2][0] = bwgt;
    mmat[2][1] = bwgt;
    mmat[2][2] = bwgt;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    saturatemat -
*        make a saturation marix
*/
static void saturatemat(float mat[4][4], float sat)
{
    float mmat[4][4];
    float a, b, c, d, e, f, g, h, i;
    float rwgt, gwgt, bwgt;

    rwgt = RLUM;
    gwgt = GLUM;
    bwgt = BLUM;

    a = (1.0 - sat) * rwgt + sat;
    b = (1.0 - sat) * rwgt;
    c = (1.0 - sat) * rwgt;
    d = (1.0 - sat) * gwgt;
    e = (1.0 - sat) * gwgt + sat;
    f = (1.0 - sat) * gwgt;
    g = (1.0 - sat) * bwgt;
    h = (1.0 - sat) * bwgt;
    i = (1.0 - sat) * bwgt + sat;
    mmat[0][0] = a;
    mmat[0][1] = b;
    mmat[0][2] = c;
    mmat[0][3] = 0.0;

    mmat[1][0] = d;
    mmat[1][1] = e;
    mmat[1][2] = f;
    mmat[1][3] = 0.0;

    mmat[2][0] = g;
    mmat[2][1] = h;
    mmat[2][2] = i;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    offsetmat -
*        offset r, g, and b
*/
static void offsetmat(float mat[4][4], float roffset, float goffset, float boffset)
{
    float mmat[4][4];

    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;
    mmat[2][3] = 0.0;

    mmat[3][0] = roffset;
    mmat[3][1] = goffset;
    mmat[3][2] = boffset;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    xrotate -
*        rotate about the x (red) axis
*/
inline void xrotatemat(float mat[4][4], float rs, float rc)
{
    float mmat[4][4];

    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = rc;
    mmat[1][2] = rs;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = -rs;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    yrotate -
*        rotate about the y (green) axis
*/
static void yrotatemat(float mat[4][4], float rs, float rc)
{
    float mmat[4][4];

    mmat[0][0] = rc;
    mmat[0][1] = 0.0;
    mmat[0][2] = -rs;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;

    mmat[2][0] = rs;
    mmat[2][1] = 0.0;
    mmat[2][2] = rc;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    zrotate -
*        rotate about the z (blue) axis
*/
static void zrotatemat(float mat[4][4], float rs, float rc)
{
    float mmat[4][4];

    mmat[0][0] = rc;
    mmat[0][1] = rs;
    mmat[0][2] = 0.0;
    mmat[0][3] = 0.0;

    mmat[1][0] = -rs;
    mmat[1][1] = rc;
    mmat[1][2] = 0.0;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    zshear -
*        shear z using x and y.
*/
static void zshearmat(float mat[4][4], float dx, float dy)
{
    float mmat[4][4];

    mmat[0][0] = 1.0;
    mmat[0][1] = 0.0;
    mmat[0][2] = dx;
    mmat[0][3] = 0.0;

    mmat[1][0] = 0.0;
    mmat[1][1] = 1.0;
    mmat[1][2] = dy;
    mmat[1][3] = 0.0;

    mmat[2][0] = 0.0;
    mmat[2][1] = 0.0;
    mmat[2][2] = 1.0;
    mmat[2][3] = 0.0;

    mmat[3][0] = 0.0;
    mmat[3][1] = 0.0;
    mmat[3][2] = 0.0;
    mmat[3][3] = 1.0;
    matrixmult(mmat, mat, mat);
}

/*
*    simplehuerotatemat -
*        simple hue rotation. This changes luminance
*/
static void simplehuerotatemat(float mat[4][4], float rot)
{
    // log("************* simplehuerotatemat");

    float mag;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    identmat((float*)mat);

    /* rotate the grey vector into positive Z */
    mag = sqrt(2.0);
    xrs = 1.0 / mag;
    xrc = 1.0 / mag;
    xrotatemat(mat, xrs, xrc);
    //printmat(mat);

    mag = sqrt(3.0);
    yrs = -1.0 / mag;
    yrc = sqrt(2.0) / mag;
    yrotatemat(mat, yrs, yrc);
    //printmat(mat);

    /* rotate the hue */
    zrs = sin(rot);
    zrc = cos(rot);
    zrotatemat(mat, zrs, zrc);
    //printmat(mat);

    /* rotate the grey vector back into place */
    yrotatemat(mat, -yrs, yrc);
    // printmat(mat);
    xrotatemat(mat, -xrs, xrc);
    //printmat(mat);

    // log("############## simplehuerotatemat");
}

/*
*    huerotatemat -
*        rotate the hue, while maintaining luminance.
*/
static void huerotatemat(float mat[4][4], float rot)
{
    float mmat[4][4];
    float mag;
    float lx, ly, lz;
    float xrs, xrc;
    float yrs, yrc;
    float zrs, zrc;
    float zsx, zsy;

    identmat((float*)mat);
    identmat((float*)&mmat);

    /* rotate the grey vector into positive Z */
    mag = sqrt(2.0);
    xrs = 1.0 / mag;
    xrc = 1.0 / mag;
    xrotatemat(mmat, xrs, xrc);
    mag = sqrt(3.0);
    yrs = -1.0 / mag;
    yrc = sqrt(2.0) / mag;
    yrotatemat(mmat, yrs, yrc);

    /* shear the space to make the luminance plane horizontal */
    xformpnt(mmat, RLUM, GLUM, BLUM, &lx, &ly, &lz);
    zsx = lx / lz;
    zsy = ly / lz;
    zshearmat(mmat, zsx, zsy);

    /* rotate the hue */
    zrs = sin(rot);
    zrc = cos(rot);
    zrotatemat(mmat, zrs, zrc);

    /* unshear the space to put the luminance plane back */
    zshearmat(mmat, -zsx, -zsy);

    /* rotate the grey vector back into place */
    yrotatemat(mmat, -yrs, yrc);
    xrotatemat(mmat, -xrs, xrc);

    matrixmult(mmat, mat, mat);
}

/*
*    xformpnt -
*        transform a 3D point using a matrix
*/
static void xformpnt(Mat3* matrix, float x, float y, float z, float* tx, float* ty, float* tz)
{
    *tx = x * matrix->m[0] + y * matrix->m[3] + z * matrix->m[6];
    *ty = x * matrix->m[1] + y * matrix->m[4] + z * matrix->m[7];
    *tz = x * matrix->m[2] + y * matrix->m[5] + z * matrix->m[8];
}

/*
*    zshear -
*        shear z using x and y.
*/
static void zshearmat(Mat3* mat, float dx, float dy)
{
    // float mmat[4][4];
    Mat3 mmat;
    //mmat[0][0] = 1.0;
    //mmat[0][1] = 0.0;
    mmat.m[2] = dx;
    //mmat[0][3] = 0.0;

    //mmat[1][0] = 0.0;
    //mmat[1][1] = 1.0;
    mmat.m[5] = dy;
    //mmat[1][3] = 0.0;

    //mmat[2][0] = 0.0;
    //mmat[2][1] = 0.0;
    //mmat[2][2] = 1.0;
    //mmat[2][3] = 0.0;

    //mmat[3][0] = 0.0;
    //mmat[3][1] = 0.0;
    //mmat[3][2] = 0.0;
    //mmat[3][3] = 1.0;
    mat->mult(mmat);//matrixmult(mmat, mat, mat);
}

static void createHueMatrix(Mat4* dst, float angle)
{
    // log("************* createHueMatrix4f");

    memcpy(dst, &dst->IDENTITY, sizeof(*dst));

    /* rotate the grey vector into positive Z */
    createRotationX(dst, FLT_XRS, FLT_XRC);// rotateX(dst, FLT_XRS, FLT_XRC);
    rotateY(dst, FLT_YRS, FLT_YRC);

    /* rotate the hue */
    rotateZ(dst, angle);

    /* rotate the grey vector back into place */
    rotateY(dst, -FLT_YRS, FLT_YRC);
    rotateX(dst, -FLT_XRS, FLT_XRC);

    // log("############## createHueMatrix4f");
}

static void setMatrixHueOptimized(Mat3* dst, float angle)
{
    float lx, ly, lz;
    float zsx, zsy;
    // memcpy(dst, &dst->IDENTITY, sizeof(*dst));

    /* rotate the grey vector into positive Z */
    dst->createRotationX(FLT_XRS, FLT_XRC);// rotateX(dst, FLT_XRS, FLT_XRC);
    dst->rotateY(FLT_YRS, FLT_YRC);

    /* shear the space to make the luminance plane horizontal */
    xformpnt(dst, RLUM, GLUM, BLUM, &lx, &ly, &lz);
    zsx = lx / lz;
    zsy = ly / lz;
    zshearmat(dst, zsx, zsy);

    /* rotate the hue */
    dst->rotateZ(angle);

    /* unshear the space to put the luminance plane back */
    zshearmat(dst, -zsx, -zsy);

    /* rotate the grey vector back into place */
    dst->rotateY(-FLT_YRS, FLT_YRC);
    dst->rotateX(-FLT_XRS, FLT_XRC);
}

/*
*    Hue -
*        HSV - H
*/
static void setMatrixHue(Mat3* dst, float angle)
{
    log("************* createHueMat3");

    // memcpy(dst, &dst->IDENTITY, sizeof(*dst));

    /* rotate the grey vector into positive Z */
    dst->createRotationX(FLT_XRS, FLT_XRC);// rotateX(dst, FLT_XRS, FLT_XRC);
    dst->rotateY(FLT_YRS, FLT_YRC);

    /* rotate the hue */
    dst->rotateZ(angle);

    /* rotate the grey vector back into place */
    dst->rotateY(-FLT_YRS, FLT_YRC);
    dst->rotateX(-FLT_XRS, FLT_XRC);

    log("############## createHueMat3");
}

/*
*    saturatemat - 0~1, 0%~100% ?
*        HSV - S
*/
static void setMatrixSat(Mat3* dst, float sat)
{
    Mat3 temp;
    float a, b, c, d, e, f, g, h, i;
    float rwgt, gwgt, bwgt;

    rwgt = RLUM;
    gwgt = GLUM;
    bwgt = BLUM;

    a = (1.0 - sat) * rwgt + sat;
    b = (1.0 - sat) * rwgt;
    c = (1.0 - sat) * rwgt;
    d = (1.0 - sat) * gwgt;
    e = (1.0 - sat) * gwgt + sat;
    f = (1.0 - sat) * gwgt;
    g = (1.0 - sat) * bwgt;
    h = (1.0 - sat) * bwgt;
    i = (1.0 - sat) * bwgt + sat;
    temp.m[0] = a;
    temp.m[1] = b;
    temp.m[2] = c;
    // mmat[0][3] = 0.0;

    temp.m[3] = d;
    temp.m[4] = e;
    temp.m[5] = f;
    //mmat[1][3] = 0.0;

    temp.m[6] = g;
    temp.m[7] = h;
    temp.m[8] = i;
    // mmat[2][3] = 0.0;

    //mmat[3][0] = 0.0;
    //mmat[3][1] = 0.0;
    //mmat[3][2] = 0.0;
    //mmat[3][3] = 1.0;
    // matrixmult(mmat, mat, mat);
    dst->mult(temp);
}

/* Value
*    HSV - V: 0~1, 0%, 100%
*/
static void setMatrixVal(Mat3* dst, float value)
{
    // float mmat[4][4];
    Mat3 temp;
    temp.m[0] = value;
    //temp.m[1] = 0.0;
    //temp.m[2] = 0.0;
    //mmat[0][3] = 0.0;

    //mmat[1][0] = 0.0;
    temp.m[4] = value;
    // mmat[1][2] = 0.0;
    //mmat[1][3] = 0.0;


    //mmat[2][0] = 0.0;
    //mmat[2][1] = 0.0;
    temp.m[8] = value;
    //mmat[2][3] = 0.0;

    dst->mult(temp);
    // matrixmult(mmat, mat, mat);
}

static backend::Program* newHSVProgram()
{
    auto fragSource = s_smart_hsv_frag;
    return backend::Device::getInstance()->newProgram(positionTextureColor_vert, fragSource);
}

bool Colorizer::enableNodeIntelliShading(Node* node,
    bool noMVP,
    const Vec3& hsv,
    const Vec3& filter,
    bool forceShading,
    const Vec3& hsvShading)
{
    if (node == nullptr)
        return false;

    // TODO: cache it?
    auto program = newHSVProgram();
    if (program == nullptr) {
        return false;
    }

    auto programState = new backend::ProgramState(program);
    program->release();

    node->setProgramState(programState);
    programState->release();

    updateNodeHsv(node, hsv, filter, forceShading, hsvShading);

    return true;
}

void Colorizer::updateNodeHsv(Node* node,
    const Vec3& hsv,
    const Vec3& filter,
    bool forceShading,
    const Vec3& hsvShading)
{
    Mat3 hsvMatrix;
    setMatrixHueOptimized(&hsvMatrix, hsv.x);
    setMatrixSat(&hsvMatrix, hsv.y);
    setMatrixVal(&hsvMatrix, hsv.z);

    printmat3(hsvMatrix.m);

    auto programState = node->getProgramState();
    programState->setCallbackUniform(programState->getUniformLocation("u_mix_hsv"), [hsvMatrix](backend::ProgramState* programState, const backend::UniformLocation& location) {
        programState->setUniform(location, &hsvMatrix.m[0], sizeof(hsvMatrix));
    });

    programState->setUniform(programState->getUniformLocation("u_filter_rgb"), &filter, sizeof(filter));
    int32_t value = forceShading ? 1 : 0;
    programState->setUniform(programState->getUniformLocation("u_force_shading"), &value, sizeof(value));
}


NS_CC_END
