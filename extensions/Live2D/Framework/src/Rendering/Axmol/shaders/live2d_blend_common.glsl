vec4 ConvertPremultipliedToStraight(vec4 source)
{
    if (abs(source.a) < 0.00001)
    {
        source.rgb = vec3(0.0);
    }
    else
    {
        source.rgb /= source.a;
    }

    return source;
}

float CubismColorBurn(float colorSource, float colorDestination)
{
    if (abs(colorDestination - 1.0) < 0.000001)
    {
        return 1.0;
    }
    if (abs(colorSource) < 0.000001)
    {
        return 0.0;
    }

    return 1.0 - min(1.0, (1.0 - colorDestination) / colorSource);
}

float CubismColorDodge(float colorSource, float colorDestination)
{
    if (colorDestination <= 0.0)
    {
        return 0.0;
    }
    if (colorSource >= 1.0)
    {
        return 1.0;
    }

    return min(1.0, colorDestination / (1.0 - colorSource));
}

float CubismOverlay(float colorSource, float colorDestination)
{
    float mul = 2.0 * colorSource * colorDestination;
    float scr = 1.0 - 2.0 * (1.0 - colorSource) * (1.0 - colorDestination);
    return colorDestination < 0.5 ? mul : scr;
}

float CubismSoftLight(float colorSource, float colorDestination)
{
    float val1 = colorDestination - (1.0 - 2.0 * colorSource) * colorDestination * (1.0 - colorDestination);
    float val2 = colorDestination + (2.0 * colorSource - 1.0) * colorDestination * ((16.0 * colorDestination - 12.0) * colorDestination + 3.0);
    float val3 = colorDestination + (2.0 * colorSource - 1.0) * (sqrt(colorDestination) - colorDestination);

    if (colorSource <= 0.5)
    {
        return val1;
    }
    if (colorDestination <= 0.25)
    {
        return val2;
    }

    return val3;
}

float CubismHardLight(float colorSource, float colorDestination)
{
    float mul = 2.0 * colorSource * colorDestination;
    float scr = 1.0 - 2.0 * (1.0 - colorSource) * (1.0 - colorDestination);
    return colorSource < 0.5 ? mul : scr;
}

float CubismLinearLight(float colorSource, float colorDestination)
{
    float burn = max(0.0, 2.0 * colorSource + colorDestination - 1.0);
    float dodge = min(1.0, 2.0 * (colorSource - 0.5) + colorDestination);
    return colorSource < 0.5 ? burn : dodge;
}

float CubismGetMax(vec3 rgbC)
{
    return max(rgbC.r, max(rgbC.g, rgbC.b));
}

float CubismGetMin(vec3 rgbC)
{
    return min(rgbC.r, min(rgbC.g, rgbC.b));
}

float CubismSaturation(vec3 rgbC)
{
    return CubismGetMax(rgbC) - CubismGetMin(rgbC);
}

float CubismLuma(vec3 rgbC)
{
    return 0.30 * rgbC.r + 0.59 * rgbC.g + 0.11 * rgbC.b;
}

vec3 CubismClipColor(vec3 rgbC)
{
    float luma = CubismLuma(rgbC);
    float maxv = CubismGetMax(rgbC);
    float minv = CubismGetMin(rgbC);
    vec3 outputColor = rgbC;

    outputColor = minv < 0.0 ? luma + (outputColor - luma) * luma / (luma - minv) : outputColor;
    outputColor = maxv > 1.0 ? luma + (outputColor - luma) * (1.0 - luma) / (maxv - luma) : outputColor;

    return outputColor;
}

vec3 CubismSetLuma(vec3 rgbC, float luma)
{
    return CubismClipColor(rgbC + (luma - CubismLuma(rgbC)));
}

vec3 CubismSetSaturation(vec3 rgbC, float saturation)
{
    float maxv = CubismGetMax(rgbC);
    float minv = CubismGetMin(rgbC);
    float medv = rgbC.r + rgbC.g + rgbC.b - maxv - minv;
    float outputMax = minv < maxv ? saturation : 0.0;
    float outputMed = minv < maxv ? (medv - minv) * saturation / (maxv - minv) : 0.0;
    float outputMin = 0.0;

    if (rgbC.r == maxv)
    {
        return rgbC.b < rgbC.g ? vec3(outputMax, outputMed, outputMin) : vec3(outputMax, outputMin, outputMed);
    }
    if (rgbC.g == maxv)
    {
        return rgbC.r < rgbC.b ? vec3(outputMin, outputMax, outputMed) : vec3(outputMed, outputMax, outputMin);
    }

    return rgbC.g < rgbC.r ? vec3(outputMed, outputMin, outputMax) : vec3(outputMin, outputMed, outputMax);
}

vec3 ColorBlend(vec3 colorSource, vec3 colorDestination, int colorBlendMode)
{
    if (colorBlendMode == 1)
    {
        return min(colorSource + colorDestination, 1.0);
    }
    if (colorBlendMode == 2)
    {
        return colorSource + colorDestination;
    }
    if (colorBlendMode == 3)
    {
        return min(colorSource, colorDestination);
    }
    if (colorBlendMode == 4)
    {
        return colorSource * colorDestination;
    }
    if (colorBlendMode == 5)
    {
        return vec3(
            CubismColorBurn(colorSource.r, colorDestination.r),
            CubismColorBurn(colorSource.g, colorDestination.g),
            CubismColorBurn(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 6)
    {
        return max(vec3(0.0), colorSource + colorDestination - 1.0);
    }
    if (colorBlendMode == 7)
    {
        return max(colorSource, colorDestination);
    }
    if (colorBlendMode == 8)
    {
        return colorSource + colorDestination - colorSource * colorDestination;
    }
    if (colorBlendMode == 9)
    {
        return vec3(
            CubismColorDodge(colorSource.r, colorDestination.r),
            CubismColorDodge(colorSource.g, colorDestination.g),
            CubismColorDodge(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 10)
    {
        return vec3(
            CubismOverlay(colorSource.r, colorDestination.r),
            CubismOverlay(colorSource.g, colorDestination.g),
            CubismOverlay(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 11)
    {
        return vec3(
            CubismSoftLight(colorSource.r, colorDestination.r),
            CubismSoftLight(colorSource.g, colorDestination.g),
            CubismSoftLight(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 12)
    {
        return vec3(
            CubismHardLight(colorSource.r, colorDestination.r),
            CubismHardLight(colorSource.g, colorDestination.g),
            CubismHardLight(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 13)
    {
        return vec3(
            CubismLinearLight(colorSource.r, colorDestination.r),
            CubismLinearLight(colorSource.g, colorDestination.g),
            CubismLinearLight(colorSource.b, colorDestination.b));
    }
    if (colorBlendMode == 14)
    {
        return CubismSetLuma(CubismSetSaturation(colorSource, CubismSaturation(colorDestination)), CubismLuma(colorDestination));
    }
    if (colorBlendMode == 15)
    {
        return CubismSetLuma(colorSource, CubismLuma(colorDestination));
    }

    return colorSource;
}

vec4 OverlapRgba(vec3 color, vec3 colorSource, vec3 colorDestination, vec3 parameter)
{
    vec3 rgb = color * parameter.x + colorSource * parameter.y + colorDestination * parameter.z;
    float alpha = parameter.x + parameter.y + parameter.z;
    return vec4(rgb, alpha);
}

vec4 AlphaBlend(vec3 color, vec4 colorSource, vec4 colorDestination, int alphaBlendMode)
{
    vec3 parameter;
    if (alphaBlendMode == 1)
    {
        parameter = vec3(colorSource.a * colorDestination.a, 0.0, colorDestination.a * (1.0 - colorSource.a));
    }
    else if (alphaBlendMode == 2)
    {
        parameter = vec3(0.0, 0.0, colorDestination.a * (1.0 - colorSource.a));
    }
    else if (alphaBlendMode == 3)
    {
        parameter = vec3(min(colorSource.a, colorDestination.a), max(colorSource.a - colorDestination.a, 0.0), max(colorDestination.a - colorSource.a, 0.0));
    }
    else if (alphaBlendMode == 4)
    {
        parameter = vec3(max(colorSource.a + colorDestination.a - 1.0, 0.0), min(colorSource.a, 1.0 - colorDestination.a), min(colorDestination.a, 1.0 - colorSource.a));
    }
    else
    {
        parameter = vec3(colorSource.a * colorDestination.a, colorSource.a * (1.0 - colorDestination.a), colorDestination.a * (1.0 - colorSource.a));
    }

    return OverlapRgba(color, colorSource.rgb, colorDestination.rgb, parameter);
}
