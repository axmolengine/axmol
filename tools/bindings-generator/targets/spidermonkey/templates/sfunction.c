## ===== static function implementation template
bool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
#if len($arguments) > 0
    bool ok = true;
#end if
#if len($arguments) >= $min_args
    #set arg_count = len($arguments)
    #set arg_idx = $min_args
    #while $arg_idx <= $arg_count
    if (argc == ${arg_idx}) {
        #set arg_list = ""
        #set arg_array = []
        #set $count = 0
        #while $count < $arg_idx
            #set $arg = $arguments[$count]
            #if $arg.is_numeric
        ${arg.to_string($generator)} arg${count} = 0;
            #elif $arg.is_pointer
        ${arg.to_string($generator)} arg${count} = nullptr;
            #else
        ${arg.to_string($generator)} arg${count};
            #end if
            #set $count = $count + 1
        #end while
        #set $count = 0
        #while $count < $arg_idx
            #set $arg = $arguments[$count]
        ${arg.to_native({"generator": $generator,
            "in_value": "args.get(" + str(count) + ")",
            "out_value": "arg" + str(count),
            "class_name": $class_name,
            "level": 2,
            "ntype": str($arg)})};
            #set $arg_array += ["arg"+str($count)]
            #set $count = $count + 1
        #end while
        #if $arg_idx > 0
        JSB_PRECONDITION2(ok, cx, false, "${signature_name} : Error processing arguments");
        #end if
        #set $arg_list = ", ".join($arg_array)
    #if str($ret_type) != "void"

        #if $func_name.startswith("create") and $is_ref_class
        auto ret = ${namespaced_class_name}::${func_name}($arg_list);
        js_type_class_t *typeClass = js_get_type_from_native<${namespaced_class_name}>(ret);
        JS::RootedObject jsret(cx, jsb_ref_autoreleased_create_jsobject(cx, ret, typeClass, "${namespaced_class_name}"));
        args.rval().set(OBJECT_TO_JSVAL(jsret));
        #elif $func_name.startswith("getInstance") and $is_ref_class
        auto ret = ${namespaced_class_name}::${func_name}($arg_list);
        js_type_class_t *typeClass = js_get_type_from_native<${namespaced_class_name}>(ret);
        JS::RootedObject jsret(cx, jsb_ref_get_or_create_jsobject(cx, ret, typeClass, "${namespaced_class_name}"));
        args.rval().set(OBJECT_TO_JSVAL(jsret));
        #else
          #if $ret_type.is_enum
        int ret = (int)${namespaced_class_name}::${func_name}($arg_list);
          #else
        ${ret_type.get_whole_name($generator)} ret = ${namespaced_class_name}::${func_name}($arg_list);
          #end if
        jsval jsret = JSVAL_NULL;
        ${ret_type.from_native({"generator": $generator,
                                "in_value": "ret",
                                "out_value": "jsret",
                                "ntype": str($ret_type),
                                "level": 1})};
        args.rval().set(jsret);
        #end if
    #else
        ${namespaced_class_name}::${func_name}($arg_list);
        args.rval().setUndefined();
    #end if
        return true;
    }
        #set $arg_idx = $arg_idx + 1
    #end while
#end if
    JS_ReportError(cx, "${signature_name} : wrong number of arguments");
    return false;
}

