/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "LAppPal.hpp"
#include <Model/CubismMoc.hpp>
#include "cocos2d.h"

using namespace Csm;
USING_NS_AX;

csmByte* LAppPal::LoadFileAsBytes(const csmChar* filePath, csmSizeInt* outSize)
{
    ssize_t size = 0;
    csmByte* buf = FileUtils::getInstance()->getDataFromFile(filePath).takeBuffer(&size);
    *outSize = static_cast<csmSizeInt>(size);
    return buf;
}

void LAppPal::ReleaseBytes(csmByte* byteData)
{
    free(byteData);
}

csmFloat32  LAppPal::GetDeltaTime()
{
    if(Director::getInstance() != NULL)
    {
        return Director::getInstance()->getDeltaTime();
    }
    return 0.0f;
}

void LAppPal::PrintLog(const csmChar* format, ...)
{
    va_list args;
    csmChar buf[256];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args); // 標準出力でレンダリング
    AXLOGD("{}", buf); // Axmolのログ関数で出力
    va_end(args);
}

void LAppPal::PrintMessage(const csmChar* message)
{
    PrintLog("%s", message);
}
