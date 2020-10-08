\#include "base/ccConfig.h"
#if $macro_judgement
$macro_judgement
#end if 
\#ifndef __${prefix}_h__
\#define __${prefix}_h__
#if $hpp_headers
#for header in $hpp_headers
\#include "${header}"
#end for
#end if 

\#ifdef __cplusplus
extern "C" {
\#endif
\#include "tolua++.h"
\#ifdef __cplusplus
}
\#endif

int register_all_${prefix}(lua_State* tolua_S);
