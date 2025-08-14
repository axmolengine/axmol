\#pragma once
#if $macro_judgement
$macro_judgement
#end if

\#include "axmol/base/Config.h"

#if $hpp_headers
#for header in $hpp_headers
\#include "${header}"
#end for
#end if

\#include "tolua++.h"

int register_all_${prefix}(lua_State* tolua_S);
