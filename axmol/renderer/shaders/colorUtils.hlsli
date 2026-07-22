#ifndef M_PI
#define M_PI 3.141592654
#endif

float3 transformHSV(float3 inColor, float3 hsv)
{
    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float vsu = v * s * cos(h * M_PI / 180.0);
    float vsw = v * s * sin(h * M_PI / 180.0);
    float3 outColor = float3(
        (.299 * v + .701 * vsu + .168 * vsw) * inColor.r + (.587 * v - .587 * vsu + .330 * vsw) * inColor.g +
            (.114 * v - .114 * vsu - .497 * vsw) * inColor.b,
        (.299 * v - .299 * vsu - .328 * vsw) * inColor.r + (.587 * v + .413 * vsu + .035 * vsw) * inColor.g +
            (.114 * v - .114 * vsu + .292 * vsw) * inColor.b,
        (.299 * v - .300 * vsu + 1.25 * vsw) * inColor.r + (.587 * v - .588 * vsu - 1.05 * vsw) * inColor.g +
            (.114 * v + .886 * vsu - .203 * vsw) * inColor.b
    );
    return outColor;
}

float3 trasnformYUV(inout float3 YUV, float4x4 colorTransform)
{
    // The legacy buffer stores offsets in c0.w, c1.w and c2.w.
    // With column-major interpretation, these form row 3.
    YUV -= colorTransform[3u].xyz;

    float3x3 m = float3x3(
        colorTransform[0u].xyz,
        colorTransform[1u].xyz,
        colorTransform[2u].xyz);

    return mul(m, YUV);
}
