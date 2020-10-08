#set has_constructor = False
#if $current_class.methods.has_key('constructor')
#set has_constructor = True
#set constructor = $current_class.methods.constructor
${current_class.methods.constructor.generate_code($current_class)}
#end if

#set generator = $current_class.generator
#set methods = $current_class.methods_clean()
#set st_methods = $current_class.static_methods_clean()
#set public_fields = $current_class.public_fields
#if len($current_class.parents) > 0
extern JSObject *jsb_${current_class.parents[0].underlined_class_name}_prototype;

#end if
#if (not $current_class.is_ref_class and $has_constructor)
void js_${current_class.underlined_class_name}_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOGINFO("jsbindings: finalizing JS object %p (${current_class.class_name})", obj);
    js_proxy_t* nproxy;
    js_proxy_t* jsproxy;
    JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
    JS::RootedObject jsobj(cx, obj);
    jsproxy = jsb_get_js_proxy(jsobj);
    if (jsproxy) {
        ${current_class.namespaced_class_name} *nobj = static_cast<${current_class.namespaced_class_name} *>(jsproxy->ptr);
        nproxy = jsb_get_native_proxy(jsproxy->ptr);

        if (nobj) {
            jsb_remove_proxy(nproxy, jsproxy);
            JS::RootedValue flagValue(cx);
            JS_GetProperty(cx, jsobj, "__cppCreated", &flagValue);
            if (flagValue.isNullOrUndefined()){
                delete nobj;
            }
        }
        else
            jsb_remove_proxy(nullptr, jsproxy);
    }
}
#end if
#if $generator.in_listed_extend_classed($current_class.class_name) and $has_constructor
#if not $constructor.is_overloaded
    ${constructor.generate_code($current_class, None, False, True)}
#else
    ${constructor.generate_code($current_class, False, True)}
#end if
#end if
void js_register_${generator.prefix}_${current_class.class_name}(JSContext *cx, JS::HandleObject global) {
    jsb_${current_class.underlined_class_name}_class = (JSClass *)calloc(1, sizeof(JSClass));
    jsb_${current_class.underlined_class_name}_class->name = "${current_class.target_class_name}";
    jsb_${current_class.underlined_class_name}_class->addProperty = JS_PropertyStub;
    jsb_${current_class.underlined_class_name}_class->delProperty = JS_DeletePropertyStub;
    jsb_${current_class.underlined_class_name}_class->getProperty = JS_PropertyStub;
    jsb_${current_class.underlined_class_name}_class->setProperty = JS_StrictPropertyStub;
    jsb_${current_class.underlined_class_name}_class->enumerate = JS_EnumerateStub;
    jsb_${current_class.underlined_class_name}_class->resolve = JS_ResolveStub;
    jsb_${current_class.underlined_class_name}_class->convert = JS_ConvertStub;
#if (not $current_class.is_ref_class and $has_constructor)
    jsb_${current_class.underlined_class_name}_class->finalize = js_${current_class.underlined_class_name}_finalize;
#end if
    jsb_${current_class.underlined_class_name}_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    static JSPropertySpec properties[] = {
#for m in public_fields
    #if $generator.should_bind_field($current_class.class_name, m.name)
        JS_PSGS("${m.name}", ${m.signature_name}_get_${m.name}, ${m.signature_name}_set_${m.name}, JSPROP_PERMANENT | JSPROP_ENUMERATE),
    #end if
#end for
        JS_PS_END
    };

    static JSFunctionSpec funcs[] = {
        #for m in methods
        #set fn = m['impl']
        JS_FN("${m['name']}", ${fn.signature_name}, ${fn.min_args}, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        #end for
#if $generator.in_listed_extend_classed($current_class.class_name) and $has_constructor
        JS_FN("ctor", js_${generator.prefix}_${current_class.class_name}_ctor, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
#end if
        JS_FS_END
    };

    #if len(st_methods) > 0
    static JSFunctionSpec st_funcs[] = {
        #for m in st_methods
        #set fn = m['impl']
        JS_FN("${m['name']}", ${fn.signature_name}, ${fn.min_args}, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        #end for
        JS_FS_END
    };
    #else
    JSFunctionSpec *st_funcs = NULL;
    #end if

#if len($current_class.parents) > 0
    JS::RootedObject parent_proto(cx, jsb_${current_class.parents[0].underlined_class_name}_prototype);
#end if
    jsb_${current_class.underlined_class_name}_prototype = JS_InitClass(
        cx, global,
#if len($current_class.parents) > 0
        parent_proto,
#else
        JS::NullPtr(),
#end if
        jsb_${current_class.underlined_class_name}_class,
#if has_constructor
        js_${generator.prefix}_${current_class.class_name}_constructor, 0, // constructor
#else if $current_class.is_abstract
        empty_constructor, 0,
#else
        dummy_constructor<${current_class.namespaced_class_name}>, 0, // no constructor
#end if
        properties,
        funcs,
        NULL, // no static properties
        st_funcs);

    JS::RootedObject proto(cx, jsb_${current_class.underlined_class_name}_prototype);
    JS::RootedValue className(cx, std_string_to_jsval(cx, "${current_class.class_name}"));
    JS_SetProperty(cx, proto, "_className", className);
    JS_SetProperty(cx, proto, "__nativeObj", JS::TrueHandleValue);
#if $current_class.is_ref_class
    JS_SetProperty(cx, proto, "__is_ref", JS::TrueHandleValue);
#else
    JS_SetProperty(cx, proto, "__is_ref", JS::FalseHandleValue);
#end if
    // add the proto and JSClass to the type->js info hash table
#if len($current_class.parents) > 0
    jsb_register_class<${current_class.namespaced_class_name}>(cx, jsb_${current_class.underlined_class_name}_class, proto, parent_proto);
#else
    jsb_register_class<${current_class.namespaced_class_name}>(cx, jsb_${current_class.underlined_class_name}_class, proto, JS::NullPtr());
#end if
#if $generator.in_listed_extend_classed($current_class.class_name) and not $current_class.is_abstract
    anonEvaluate(cx, global, "(function () { ${generator.target_ns}.${current_class.target_class_name}.extend = cc.Class.extend; })()");
#end if
}

