## ===== static function implementation template
int ${signature_name}(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;

\#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
\#endif

\#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}",0,&tolua_err)) goto tolua_lerror;
\#endif

    argc = lua_gettop(tolua_S) - 1;

#if len($arguments) >= $min_args
    #set arg_count = len($arguments)
    #set arg_idx = $min_args
    #while $arg_idx <= $arg_count
    if (argc == ${arg_idx})
    {
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
                         "arg_idx": $count+2,
                         "class_name": $class_name,
                         "lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
                         "func_name": $func_name,
                         "level": 2,
                         "arg":$arg,
                         "ntype": $arg.namespaced_name.replace("*", ""),
                         "scriptname": $generator.scriptname_from_native($arg.namespaced_name, $arg.namespace_name)})};
                #set $arg_array += ["arg"+str($count)]
                #set $count = $count + 1
        #end while
        #if $arg_idx >= 0
        if(!ok)
        {
            tolua_error(tolua_S,"invalid arguments in function '${signature_name}'", nullptr);
            return 0;
        }
        #end if
            #set $arg_list = ", ".join($arg_array)
        #if $ret_type.name != "void"
            #if $ret_type.is_enum
        int ret = (int)${namespaced_class_name}::${func_name}($arg_list);
            #else
        ${ret_type.get_whole_name($generator)} ret = ${namespaced_class_name}::${func_name}($arg_list);
        #end if
        ${ret_type.from_native({"generator": $generator,
                                "in_value": "ret",
                                "out_value": "ret",
                                "type_name": $ret_type.namespaced_name.replace("*", ""),
                                "ntype": $ret_type.get_whole_name($generator),
                                "class_name": $class_name,
                                "level": 2,
                                "scriptname": $generator.scriptname_from_native($ret_type.namespaced_name, $ret_type.namespace_name)})};
        return 1;
        #else
        ${namespaced_class_name}::${func_name}($arg_list);
        lua_settop(tolua_S, 1);
        return 1;
        #end if
    }
        #set $arg_idx = $arg_idx + 1
    #end while
#end if
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d\n ", "${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}:${func_name}",argc, ${min_args});
    return 0;
\#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function '${signature_name}'.",&tolua_err);
\#endif
    return 0;
}
