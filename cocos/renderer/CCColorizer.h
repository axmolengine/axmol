/**
 * CCColorizer, for color translate matrix
 * reference: http://graficaobscura.com/matrix/index.html
 * @autohr HALX99 2020
 */
#pragma once
#ifndef _CCCOLORIZER_H_
#define _CCCOLORIZER_H_
#include "math/CCMath.h"
#include "base/ccTypes.h"
#include <string>

NS_CC_BEGIN
class CC_DLL Colorizer
{
public:
    static bool enableNodeIntelliShading(Node* node,
        const Vec3& hsv,
        const Vec3& filter = Vec3(1.0f, 0.45f, 0.3109f));

    static void updateNodeHsv(Node* node,
        const Vec3& hsv,
        const Vec3& filter = Vec3(1.0f, 0.45f, 0.3109f));
};
NS_CC_END

#endif
