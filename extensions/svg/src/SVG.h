/****************************************************************************
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "extensions/ExtensionExport.h"
#include "extensions/ExtensionMacros.h"

#include <string_view>

namespace ax
{
class Sprite;
class Texture2D;

namespace ext
{

AX_EXT_API Texture2D* loadSVGTexture(std::string_view path, int width = -1, int height = -1);
AX_EXT_API Sprite* createSVGSprite(std::string_view path, int width = -1, int height = -1);

}  // namespace ext
}  // namespace ax
