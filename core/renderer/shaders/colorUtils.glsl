#ifndef M_PI
#define M_PI 3.141592654
#endif

/*
*  transform color with HSV
* @params
*   inColor: input color in rgb
*   h: hue shift (in degrees)
*   s: saturation multiplier (scalar)
*   v: value multiplier (scalar)
* @refers
*   https://beesbuzz.biz/code/16-hsv-color-transforms
*   https://www.shadertoy.com/view/3syyW1
*/
vec3 transformHSV(vec3 inColor, vec3 hsv)
{
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float vsu = v * s * cos(h * M_PI / 180.0);
    float vsw = v * s * sin(h * M_PI / 180.0);
    vec3 outColor = vec3(
        (.299 * v + .701 * vsu + .168 * vsw) * inColor.r + (.587 * v - .587 * vsu + .330 * vsw) * inColor.g +
            (.114 * v - .114 * vsu - .497 * vsw) * inColor.b,
        (.299 * v - .299 * vsu - .328 * vsw) * inColor.r + (.587 * v + .413 * vsu + .035 * vsw) * inColor.g +
            (.114 * v - .114 * vsu + .292 * vsw) * inColor.b,
        (.299 * v - .300 * vsu + 1.25 * vsw) * inColor.r + (.587 * v - .588 * vsu - 1.05 * vsw) * inColor.g +
            (.114 * v + .886 * vsu - .203 * vsw) * inColor.b
    );
    return outColor;
}

vec3 trasnformYUV(vec3 YUV, mat4 colorTransform)
{
    YUV -= vec3(colorTransform[0].w, colorTransform[1].w, colorTransform[2].w);
    return mat3(
        colorTransform[0].xyz,
        colorTransform[1].xyz,
        colorTransform[2].xyz
    ) * YUV;
}
