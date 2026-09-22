/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <string_view>

namespace ax
{
  struct SystemFontRequest
  {
    std::u32string_view chars;
    std::string_view family;
    bool bold;
    bool italic;
    bool color;
  };
}
