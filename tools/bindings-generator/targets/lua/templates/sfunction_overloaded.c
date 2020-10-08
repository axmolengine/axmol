## ===== static function implementation template - for overloaded functions
int ${signature_name}(lua_State* tolua_S)
{
    int argc = 0;
    bool ok  = true;
\#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
\#endif

\#if COCOS2D_DEBUG >= 1
    if (!tolua_isusertable(tolua_S,1,"${generator.scriptname_from_native($namespaced_class_name,$namespace_name)}",0,&tolua_err)) goto tolua_lerror;
\#endif

    argc = lua_gettop(tolua_S)-1;

    #for func in $implementations   
    #if len($func.arguments) >= $func.min_args
    #set arg_count = len($func.arguments)
    #set arg_idx = $func.min_args
    #while $arg_idx <= $arg_count
    do 
    {
        if (argc == ${arg_idx})
        {
            #set arg_list = ""
            #set arg_array = []
            #set count = 0
            #while $count < $arg_idx
            #set $arg = $func.arguments[$count]
            ${arg.to_string($generator)} arg${count};
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
            #set $arg_array += ["arg"+str(count)]
            #set $count = $count + 1
            #if $arg_idx >= 0
            if (!ok) { break; }
            #end if
            #end while
            #set $arg_list = ", ".join($arg_array)
            #if str($func.ret_type) != "void"
                #if $func.ret_type.is_enum
            int ret = (int)${namespaced_class_name}::${func.func_name}($arg_list);
                #else
            ${func.ret_type.get_whole_name($generator)} ret = ${namespaced_class_name}::${func.func_name}($arg_list);
                #end if
            ${func.ret_type.from_native({"generator": $generator,
                                         "in_value": "ret",
                                         "out_value": "jsret",
                                         "type_name": $func.ret_type.namespaced_name.replace("*", ""),
                                         "ntype": $func.ret_type.get_whole_name($generator),
                                         "class_name": $class_name,
                                         "level": 2,
                                         "scriptname": $generator.scriptname_from_native($func.ret_type.namespaced_name, $func.ret_type.namespace_name)})};
            return 1;
            #else
            ${namespaced_class_name}::${func.func_name}($arg_list);
            lua_settop(tolua_S, 1);
            return 1;
            #end if
        }
    } while (0);
    #set $arg_idx = $arg_idx + 1
    ok  = true;
    #end while
    #end if
    #end for
    luaL_error(tolua_S, "%s has wrong number of arguments: %d, was expecting %d", "${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}:${func.func_name}",argc, ${func.min_args});
    return 0;
\#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function '${signature_name}'.",&tolua_err);
\#endif
    return 0;
}
