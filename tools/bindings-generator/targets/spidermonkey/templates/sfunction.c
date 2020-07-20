## ===== static function implementation template
JSBool ${signature_name}(JSContext *cx, uint32_t argc, jsval *vp)
{
#if len($arguments) > 0
	jsval *argv = JS_ARGV(cx, vp);
	JSBool ok = JS_TRUE;
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
		${arg.to_string($generator)} arg${count};
			#set $count = $count + 1
		#end while
		#set $count = 0
		#while $count < $arg_idx
			#set $arg = $arguments[$count]
		${arg.to_native({"generator": $generator,
			"in_value": "argv[" + str(count) + "]",
			"out_value": "arg" + str(count),
			"class_name": $class_name,
			"level": 2,
			"ntype": str($arg)})};
	        #set $arg_array += ["arg"+str($count)]
	        #set $count = $count + 1
		#end while
		#if $arg_idx > 0
		JSB_PRECONDITION2(ok, cx, JS_FALSE, "Error processing arguments");
		#end if
		#set $arg_list = ", ".join($arg_array)
	#if str($ret_type) != "void"
		${ret_type} ret = ${namespaced_class_name}::${func_name}($arg_list);
		jsval jsret;
		${ret_type.from_native({"generator": $generator,
								"in_value": "ret",
								"out_value": "jsret",
								"ntype": str($ret_type),
								"level": 1})};
		JS_SET_RVAL(cx, vp, jsret);
	#else
		${namespaced_class_name}::${func_name}($arg_list);
		JS_SET_RVAL(cx, vp, JSVAL_VOID);
	#end if
		return JS_TRUE;
	}
		#set $arg_idx = $arg_idx + 1
	#end while
#end if
	JS_ReportError(cx, "wrong number of arguments");
	return JS_FALSE;
}

