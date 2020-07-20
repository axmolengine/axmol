#set has_constructor = False
#if $current_class.methods.has_key('constructor')
#set has_constructor = True
${current_class.methods.constructor.generate_code($current_class)}
#end if

#set generator = $current_class.generator
#set methods = $current_class.methods_clean()
#set st_methods = $current_class.static_methods_clean()

#if len($current_class.parents) > 0
extern JSObject *jsb_${current_class.parents[0].class_name}_prototype;
#end if

void js_${generator.prefix}_${current_class.class_name}_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOGINFO("jsbindings: finalizing JS object %p (${current_class.class_name})", obj);
#if $generator.script_control_cpp
    js_proxy_t* nproxy;
    js_proxy_t* jsproxy;
    jsproxy = jsb_get_js_proxy(obj);
    if (jsproxy) {
        nproxy = jsb_get_native_proxy(jsproxy->ptr);

        ${current_class.namespaced_class_name} *nobj = static_cast<${current_class.namespaced_class_name} *>(nproxy->ptr);
        if (nobj)
            delete nobj;
        
        jsb_remove_proxy(nproxy, jsproxy);
    }
#end if
}

#if not $current_class.is_abstract
static JSBool js_${generator.prefix}_${current_class.class_name}_ctor(JSContext *cx, uint32_t argc, jsval *vp)
{
	JSObject *obj = JS_THIS_OBJECT(cx, vp);
    ${current_class.namespaced_class_name} *nobj = new ${current_class.namespaced_class_name}();
    js_proxy_t* p = jsb_new_proxy(nobj, obj);
#if not $generator.script_control_cpp
    nobj->autorelease();
    JS_AddNamedObjectRoot(cx, &p->obj, "${current_class.namespaced_class_name}");
#end if 
    JS_SET_RVAL(cx, vp, JSVAL_VOID);
    return JS_TRUE;
}

#end if
void js_register_${generator.prefix}_${current_class.class_name}(JSContext *cx, JSObject *global) {
	jsb_${current_class.class_name}_class = (JSClass *)calloc(1, sizeof(JSClass));
	jsb_${current_class.class_name}_class->name = "${current_class.target_class_name}";
	jsb_${current_class.class_name}_class->addProperty = JS_PropertyStub;
	jsb_${current_class.class_name}_class->delProperty = JS_PropertyStub;
	jsb_${current_class.class_name}_class->getProperty = JS_PropertyStub;
	jsb_${current_class.class_name}_class->setProperty = JS_StrictPropertyStub;
	jsb_${current_class.class_name}_class->enumerate = JS_EnumerateStub;
	jsb_${current_class.class_name}_class->resolve = JS_ResolveStub;
	jsb_${current_class.class_name}_class->convert = JS_ConvertStub;
	jsb_${current_class.class_name}_class->finalize = js_${generator.prefix}_${current_class.class_name}_finalize;
	jsb_${current_class.class_name}_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

	#if len($current_class.fields) > 0
	static JSPropertySpec properties[] = {
		{0, 0, 0, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER}
	};
	#else
	JSPropertySpec *properties = NULL;
	#end if

	#if len(methods) > 0
	static JSFunctionSpec funcs[] = {
		#for m in methods
		#set fn = m['impl']
		JS_FN("${m['name']}", ${fn.signature_name}, ${fn.min_args}, JSPROP_PERMANENT | JSPROP_ENUMERATE),
		#end for
#if not $current_class.is_abstract
        JS_FN("ctor", js_${generator.prefix}_${current_class.class_name}_ctor, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
#end if
        JS_FS_END
	};
	#else
	JSFunctionSpec *funcs = NULL;
	#end if

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

	jsb_${current_class.class_name}_prototype = JS_InitClass(
		cx, global,
#if len($current_class.parents) > 0
		jsb_${current_class.parents[0].class_name}_prototype,
#else
		NULL, // parent proto
#end if
		jsb_${current_class.class_name}_class,
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
	// make the class enumerable in the registered namespace
	JSBool found;
	JS_SetPropertyAttributes(cx, global, "${current_class.target_class_name}", JSPROP_ENUMERATE | JSPROP_READONLY, &found);

	// add the proto and JSClass to the type->js info hash table
	TypeTest<${current_class.namespaced_class_name}> t;
	js_type_class_t *p;
	uint32_t typeId = t.s_id();
	HASH_FIND_INT(_js_global_type_ht, &typeId, p);
	if (!p) {
		p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
		p->type = typeId;
		p->jsclass = jsb_${current_class.class_name}_class;
		p->proto = jsb_${current_class.class_name}_prototype;
#if len($current_class.parents) > 0
		p->parentProto = jsb_${current_class.parents[0].class_name}_prototype;
#else
		p->parentProto = NULL;
#end if
		HASH_ADD_INT(_js_global_type_ht, type, p);
	}
}

