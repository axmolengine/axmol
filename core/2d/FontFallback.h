/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include "2d/IFontEngine.h"

#include "platform/PlatformDefine.h"

#include <memory>
#include <vector>

namespace ax
{
class FontFreeType;
class SystemFonts;

class AX_DLL FontFallback final : public IFontEngine
{
public:
    FontFallback(std::span<const std::string> fontFiles, bool enableSystemFonts);
    ~FontFallback();

private:
    FontFaceInfo* lookupFontFaceForCodepoint(char32_t charCode) override { return nullptr; }

  std::string lookupFontFaceForCodepoint(char32_t charCode, std::string_view family, bool bold, bool italic) override;

private:
    std::vector<std::string> _paths;
    std::vector<FontFreeType*> _fonts;
    std::vector<FontFreeType*> _cachedSystemFonts;
    SystemFonts* _systemFonts;
};
}  // namespace ax
