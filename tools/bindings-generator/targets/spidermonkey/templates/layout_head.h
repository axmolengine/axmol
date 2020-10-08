\#include "base/ccConfig.h"
#if $macro_judgement
$macro_judgement
#end if 
\#ifndef __${prefix}_h__
\#define __${prefix}_h__

\#include "jsapi.h"
\#include "jsfriendapi.h"
#if $hpp_headers
#for header in $hpp_headers
\#include "${header}"
#end for
#end if 
