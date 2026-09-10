/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "axmol/platform/msw/DWriteTextRenderer.h"
namespace ax
{

Data Device::getTextureDataForText(std::string_view text,
                                   const FontDefinition& textDefinition,
                                   TextAlign align,
                                   int& width,
                                   int& height,
                                   bool& hasPremultipliedAlpha)
{
    Data ret;
    do
    {
        auto& textRenderer = DWriteTextRenderer::sharedTextRenderer();

        SIZE size = {(LONG)textDefinition._dimensions.width, (LONG)textDefinition._dimensions.height};
        if (!textRenderer.drawText(text, textDefinition, static_cast<unsigned int>(align), hasPremultipliedAlpha, size,
                                   ret))
            break;

        width  = size.cx;
        height = size.cy;

    } while (0);

    return ret;
}

}  // namespace ax
