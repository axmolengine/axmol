\#include "scripting/js-bindings/auto/${out_file}.hpp"
#if $macro_judgement
$macro_judgement
#end if
\#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"
#for header in $headers
    #set include_header = os.path.basename(header)
    #if $replace_headers.has_key(include_header)
\#include "${replace_headers[include_header]}"
    #else
        #set relative = os.path.relpath(header, $search_path)
        #if not '..' in relative
\#include "${relative.replace(os.path.sep, '/')}"
        #else
\#include "${include_header}"
        #end if
    #end if
#end for
#if $cpp_headers
#for header in $cpp_headers
\#include "${header}"
#end for
#end if

template<class T>
static bool dummy_constructor(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS_ReportError(cx, "Constructor for the requested class is not available, please refer to the API reference.");
    return false;
}

static bool empty_constructor(JSContext *cx, uint32_t argc, jsval *vp) {
    return false;
}

static bool js_is_native_obj(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    args.rval().setBoolean(true);
    return true;
}
