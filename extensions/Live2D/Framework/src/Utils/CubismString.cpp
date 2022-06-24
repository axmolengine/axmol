/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismString.hpp"
#include "Type/csmVector.hpp"
#include <stdio.h>
#include <stdarg.h>

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Utils {

//標準出力の戻り値が複製されるのでオーバーヘッドは大きい。
csmString CubismString::GetFormatedString(const csmChar* format, ...)
{
    csmInt32 bufferSize = 256;
    csmChar* buffer = static_cast<csmChar*>(CSM_MALLOC(sizeof(csmChar)* bufferSize));

    va_list args;
    va_start(args, format);

    for (;;) {
#ifdef _WINDOWS
        if (vsnprintf_s(buffer, bufferSize, _TRUNCATE, format, args) < bufferSize) {
#else
        if (vsnprintf(buffer, bufferSize, format, args) < bufferSize) {
#endif
            break;
        } else {
            // メモリが足りない為、拡張して確保しなおす。
            CSM_FREE(buffer);
            bufferSize *= 2;
            buffer = static_cast<csmChar*>(CSM_MALLOC(sizeof(csmChar)* bufferSize));
        }
    }
    va_end(args);

    csmString ret = buffer;
    CSM_FREE(buffer);

    return ret; // CubismString型にされて返されるためアドレスを返すので良い。
}

csmBool CubismString::IsStartsWith(const csmChar* text, const csmChar* startWord)
{
    while (*startWord != '\0')
    {
        if (*text == '\0' || *(text++) != *(startWord++))
        {
            return false;
        }
    }
    return true;
}

csmFloat32 CubismString::StringToFloat(const csmChar* string, csmInt32 length, csmInt32 position, csmInt32* outEndPos)
{
    csmInt32 i = position;
    csmBool minus = false; //マイナスフラグ
    csmBool period = false;
    csmFloat32 v1 = 0;

    //負号の確認
    csmInt32 c = string[i];
    if (c == '-')
    {
        minus = true;
        i++;
    }

    //整数部の確認
    for (; i < length; i++)
    {
        c = string[i];
        if ('0' <= c && c <= '9')
        {
            v1 = v1 * 10 + (c - '0');
        }
        else if (c == '.')
        {
            period = true;
            i++;
            break;
        }
        else
        {
            break;
        }
    }

    //小数部の確認
    if (period)
    {
        csmFloat32 mul = 0.1f;
        for (; i < length; i++)
        {
            c = string[i] & 0xFF;
            if ('0' <= c && c <= '9')
            {
                v1 += mul * (c - '0');
            }
            else
            {
                break;
            }
            mul *= 0.1f; //一桁下げる
            if (!c) break;
        }
    }

    if (i == position)
    {
        //一文字も読み込まなかった場合
        *outEndPos = -1; //エラー値が入るので呼び出し元で適切な処理を行う
        return 0;
    }

    if (minus) v1 = -v1;

    *outEndPos = i;
    return v1;
}

}}}}

//------------------------- LIVE2D NAMESPACE ------------
