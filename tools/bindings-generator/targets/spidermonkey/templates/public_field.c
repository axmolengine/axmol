## ===== member implementation template
bool ${signature_name}_get_${name}(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject jsthis(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(jsthis);
    ${namespaced_class_name}* cobj = (${namespaced_class_name} *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "${signature_name}_get_${name} : Invalid Native Object");

    JS::RootedValue jsret(cx);
    #if $ntype.is_object and not $ntype.object_can_convert($generator, False)
    ${ntype.from_native({"generator": $generator,
                         "type_name": $ntype.namespaced_name.replace("*", ""),
                         "ntype": $ntype.get_whole_name($generator),
                         "level": 2,
                         "in_value": "cobj->" + $pretty_name,
                         "out_value": "jsret"
                        })};
    #else
    ${ntype.from_native({"generator": $generator,
                         "type_name": $ntype.namespaced_name.replace("*", ""),
                         "ntype": $ntype.get_whole_name($generator),
                         "level": 2,
                         "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
                         "in_value":"cobj->" + $pretty_name,
                         "out_value": "jsret"
                         })};
    #end if
    args.rval().set(jsret);
    return true;
}
bool ${signature_name}_set_${name}(JSContext *cx, uint32_t argc, jsval *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject jsthis(cx, args.thisv().toObjectOrNull());
    js_proxy_t *proxy = jsb_get_js_proxy(jsthis);
    ${namespaced_class_name}* cobj = (${namespaced_class_name} *)(proxy ? proxy->ptr : NULL);
    JSB_PRECONDITION2( cobj, cx, false, "${signature_name}_set_${name} : Invalid Native Object");

    bool ok = true;
#if $ntype.is_numeric
    ${ntype.to_string($generator)} arg0 = 0;
#elif $ntype.is_pointer
    ${ntype.to_string($generator)} arg0 = nullptr;
#else
    ${ntype.to_string($generator)} arg0;
#end if
#if $ntype.is_object and not $ntype.object_can_convert($generator)
    ${ntype.to_native({"generator": $generator,
                        "arg_idx": 2,
                        "ntype": $ntype.get_whole_name($generator),
                        "in_value": "args.get(0)",
                        "out_value": "arg0",
                        "func_name": $name,
                        "level": 2,
                        "arg":$ntype,
                    })};
#else
    ${ntype.to_native({"generator": $generator,
                        "arg_idx": 2,
                        "in_value": "args.get(0)",
                        "out_value": "arg0",
                        "func_name": $name,
                        "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
                        "level": 2,
                        "arg":$ntype,
                    })};
#end if
    JSB_PRECONDITION2(ok, cx, false, "${signature_name}_set_${name} : Error processing new value");
    cobj->$pretty_name = arg0;
    return true;
}
