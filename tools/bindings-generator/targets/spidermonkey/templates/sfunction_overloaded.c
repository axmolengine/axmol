## ===== static function implementation template - for overloaded functions
bool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    #for func in $implementations
    
    #if len($func.arguments) >= $func.min_args
    #set arg_count = len($func.arguments)
    #set arg_idx = $func.min_args
    #while $arg_idx <= $arg_count
    do {
        if (argc == ${arg_idx}) {
            #set arg_list = ""
            #set arg_array = []
            #set count = 0
            #while $count < $arg_idx
                #set $arg = $func.arguments[$count]
                #if $arg.is_numeric
            ${arg.to_string($generator)} arg${count} = 0;
                #elif $arg.is_pointer
            ${arg.to_string($generator)} arg${count} = nullptr;
                #else
            ${arg.to_string($generator)} arg${count};
                #end if
            ${arg.to_native({"generator": $generator,
                             "in_value": "args.get(" + str(count) + ")",
                             "out_value": "arg" + str(count),
                             "class_name": $class_name,
                             "level": 3,
                             "ntype": str($arg)})};
            #set $arg_array += ["arg"+str(count)]
            #set $count = $count + 1
            #if $arg_idx > 0
            if (!ok) { ok = true; break; }
            #end if
            #end while
            #set $arg_list = ", ".join($arg_array)
            #if str($func.ret_type) != "void"
                #if $func.ret_type.is_enum
            int ret = (int)${namespaced_class_name}::${func.func_name}($arg_list);
                #else
            ${func.ret_type.get_whole_name($generator)} ret = ${namespaced_class_name}::${func.func_name}($arg_list);
                #end if
            jsval jsret = JSVAL_NULL;
            ${func.ret_type.from_native({"generator": $generator,
                                         "in_value": "ret",
                                         "out_value": "jsret",
                                         "ntype": str($func.ret_type),
                                         "level": 3})};
            args.rval().set(jsret);
            #else
            ${namespaced_class_name}::${func.func_name}($arg_list);
            #end if
            return true;
        }
        #set $arg_idx = $arg_idx + 1
    } while (0);
    #end while
    #end if
    #end for
    JS_ReportError(cx, "${signature_name} : wrong number of arguments");
    return false;
}
