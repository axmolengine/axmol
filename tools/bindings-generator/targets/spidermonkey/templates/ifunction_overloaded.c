## ===== instance function implementation template - for overloaded functions
JSBool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	JSBool ok = JS_TRUE;

	JSObject *obj = NULL;
	${namespaced_class_name}* cobj = NULL;
#if not $is_constructor
	obj = JS_THIS_OBJECT(cx, vp);
	js_proxy_t *proxy = jsb_get_js_proxy(obj);
	cobj = (${namespaced_class_name} *)(proxy ? proxy->ptr : NULL);
	JSB_PRECONDITION2( cobj, cx, JS_FALSE, "Invalid Native Object");
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
			${arg.to_string($generator)} arg${count};
			${arg.to_native({"generator": $generator,
							 "in_value": "argv[" + str(count) + "]",
							 "out_value": "arg" + str(count),
							 "class_name": $class_name,
							 "level": 3,
							 "ntype": str($arg)})};
				#set $arg_array += ["arg"+str(count)]
				#set $count = $count + 1
			#if $arg_idx > 0
			if (!ok) { ok = JS_TRUE; break; }
			#end if
			#end while
			#set $arg_list = ", ".join($arg_array)
		#end if
		#if $is_constructor
			cobj = new ${namespaced_class_name}(${arg_list});
#if not $generator.script_control_cpp
			cocos2d::CCObject *_ccobj = dynamic_cast<cocos2d::CCObject *>(cobj);
			if (_ccobj) {
				_ccobj->autorelease();
			}
#end if
			TypeTest<${namespaced_class_name}> t;
			js_type_class_t *typeClass;
			uint32_t typeId = t.s_id();
			HASH_FIND_INT(_js_global_type_ht, &typeId, typeClass);
			assert(typeClass);
			obj = JS_NewObject(cx, typeClass->jsclass, typeClass->proto, typeClass->parentProto);
			js_proxy_t* proxy = jsb_new_proxy(cobj, obj);
#if not $generator.script_control_cpp
			JS_AddNamedObjectRoot(cx, &proxy->obj, "${namespaced_class_name}");
#end if
		#else
			#if str($func.ret_type) != "void"
			${func.ret_type} ret = cobj->${func.func_name}($arg_list);
			jsval jsret;
			${func.ret_type.from_native({"generator": $generator,
													  "in_value": "ret",
													  "out_value": "jsret",
													  "ntype": str($func.ret_type),
													  "level": 2})};
			JS_SET_RVAL(cx, vp, jsret);
			#else
			cobj->${func.func_name}($arg_list);
			JS_SET_RVAL(cx, vp, JSVAL_VOID);
			#end if
			return JS_TRUE;
		#end if
		}
	} while(0);

	#set $arg_idx = $arg_idx + 1
	#end while
#end if
#end for
#if $is_constructor
	if (cobj) {
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
		return JS_TRUE;
	}
#end if
	JS_ReportError(cx, "wrong number of arguments");
	return JS_FALSE;
}
