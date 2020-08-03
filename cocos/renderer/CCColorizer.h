/**
 * CCColorizer, for color translate matrix
 * reference: http://graficaobscura.com/matrix/index.html
 * @autohr HALX99 2020
 */
#pragma once
#ifndef _CCCOLORIZER_H_
#define _CCCOLORIZER_H_
#include "math/CCMath.h"
#include <string>

NS_CC_BEGIN
class CC_DLL Colorizer
{
public:
    static bool enableNodeIntelliShading(Node* node,
        bool noMVP,
        const Vec3& hsv,
        const Vec3& filter = Vec3(1.0f, 0.45f, 0.3109f),
        bool forceShading = false,
        const Vec3& hsvShading = Vec3::ZERO);

    static void updateNodeHsv(Node* node,
        const Vec3& hsv,
        const Vec3& filter = Vec3(1.0f, 0.45f, 0.3109f),
        bool forceShading = false,
        const Vec3& hsvShading = Vec3::ZERO);
};
NS_CC_END

#endif
