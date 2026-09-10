/****************************************************************************

Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#import <Foundation/NSString.h>

namespace ax
{
static NSString* svtons(std::string_view str)
{
    return !str.empty() ? [[NSString alloc] initWithBytes:str.data() length:str.length() encoding:NSUTF8StringEncoding]
                        : nil;
}
}  // namespace ax
