## ===== instance function implementation template - for overloaded functions
bool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
    bool ok = true;
    ${namespaced_class_name}* cobj = nullptr;

#if not $is_ctor   
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx);
#end if
#if $is_ctor
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
#end if
#if not $is_constructor
    obj.set(args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(obj);
    cobj = (${namespaced_class_name} *)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2( cobj, cx, false, "${signature_name} : Invalid Native Object");
#end if
#for func in $implementations
#if len($func.arguments) >= $func.min_args
    #set arg_count = len($func.arguments)
    #set arg_idx = $func.min_args
    #while $arg_idx <= $arg_count
    #set arg_list = ""
    #set arg_array = []
    do {
        #if $func.min_args >= 0
        if (argc == $arg_idx) {
            #set $count = 0
            #while $count < $arg_idx
                #set $arg = $func.arguments[$count]
                #set $arg_type = arg.to_string($generator)
                #if $arg.is_numeric
            ${arg_type} arg${count} = 0;
                #elif $arg.is_pointer
            ${arg_type} arg${count} = nullptr;
                #else
            ${arg_type} arg${count};
                #end if
            ${arg.to_native({"generator": $generator,
                             "in_value": "args.get(" + str(count) + ")",
                             "out_value": "arg" + str(count),
                             "class_name": $class_name,
                             "level": 3,
                             "ntype": str($arg)})};
                #set $arg_array += ["arg"+str(count)]
                #set $count = $count + 1
            #if $arg_idx > 0 and arg_type != "bool"
            if (!ok) { ok = true; break; }
            #end if
            #end while
            #set $arg_list = ", ".join($arg_array)
        #end if
        #if $is_constructor
            cobj = new (std::nothrow) ${namespaced_class_name}(${arg_list});

            #if not $is_ctor
            js_type_class_t *typeClass = js_get_type_from_native<${namespaced_class_name}>(cobj);
            JS::RootedObject proto(cx, typeClass->proto.ref());
            JS::RootedObject parent(cx, typeClass->parentProto.ref());
            obj = JS_NewObject(cx, typeClass->jsclass, proto, parent);
            #end if
            js_proxy_t* p = jsb_new_proxy(cobj, obj);
            #if $is_ref_class
            jsb_ref_init(cx, &p->obj, cobj, "${namespaced_class_name}");
            #else
            jsb_non_ref_init(cx, &p->obj, cobj, "${namespaced_class_name}");
            #end if
        #else
            #if str($func.ret_type) != "void"
                #if $func.ret_type.is_enum
            int ret = (int)cobj->${func.func_name}($arg_list);
                #else
            ${func.ret_type.get_whole_name($generator)} ret = cobj->${func.func_name}($arg_list);
                #end if
            jsval jsret = JSVAL_NULL;
            ${func.ret_type.from_native({"generator": $generator,
                                                      "in_value": "ret",
                                                      "out_value": "jsret",
                                                      "ntype": str($func.ret_type),
                                                      "level": 2})};
            args.rval().set(jsret);
            #else
            cobj->${func.func_name}($arg_list);
            args.rval().setUndefined();
            #end if
            return true;
        #end if
        }
    } while(0);

    #set $arg_idx = $arg_idx + 1
    #end while
#end if
#end for
#if $is_constructor
    if (cobj) {
        if (JS_HasProperty(cx, obj, "_ctor", &ok) && ok)
                ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(obj), "_ctor", args);

        args.rval().set(OBJECT_TO_JSVAL(obj));
        return true;
    }
#end if
    JS_ReportError(cx, "${signature_name} : wrong number of arguments");
    return false;
}
