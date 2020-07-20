## ===== static function implementation template - for overloaded functions
JSBool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
	jsval *argv = JS_ARGV(cx, vp);
	JSBool ok = JS_TRUE;
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
			#if str($func.ret_type) != "void"
			${func.ret_type} ret = ${namespaced_class_name}::${func.func_name}($arg_list);
			jsval jsret;
			${func.ret_type.from_native({"generator": $generator,
										 "in_value": "ret",
										 "out_value": "jsret",
										 "ntype": str($func.ret_type),
										 "level": 3})};
			JS_SET_RVAL(cx, vp, jsret);
			#else
			${namespaced_class_name}::${func.func_name}($arg_list);
			#end if
			return JS_TRUE;
		}
		#set $arg_idx = $arg_idx + 1
	} while (0);
	#end while
	#end if
	#end for
	JS_ReportError(cx, "wrong number of arguments");
	return JS_FALSE;
}
