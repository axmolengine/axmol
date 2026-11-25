/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <string>

namespace ax
{
  struct SystemFontRequest;
  struct SystemFontResult;

  class SystemFonts
  {
  public:
    static SystemFonts& getInstance();

    SystemFonts();
    SystemFonts(const SystemFonts&) = delete;
    ~SystemFonts();

    SystemFonts& operator=(const SystemFonts&) = delete;

    SystemFontResult findFont(const SystemFontRequest&);
    std::string fontForFamily(std::string_view family);
  };
}
