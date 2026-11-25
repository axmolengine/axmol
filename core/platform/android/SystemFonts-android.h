/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <string_view>
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
    ~SystemFonts();

    SystemFontResult findFont(const SystemFontRequest&);

  private:
    struct CacheEntry;

  private:
    std::vector<CacheEntry> _cache;
  };
}
