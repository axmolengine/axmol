## ===== member implementation template
int ${signature_name}_get${name}(lua_State* tolua_S)
{
    ${namespaced_class_name}* cobj = nullptr;
\#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}",0,&tolua_err)) goto tolua_lerror;
\#endif
    
    cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S,1,0);

\#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function '${signature_name}_get${name}'", nullptr);
        return 0;
    }
\#endif

    #if $ntype.is_object and not $ntype.object_can_convert($generator, False)
    ${ntype.from_native({"generator": $generator,
                         "type_name": $ntype.namespaced_name.replace("*", ""),
                         "ntype": $ntype.get_whole_name($generator)+"*",
                         "level": 2,
                         "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
                         "in_value":"&cobj->" + $pretty_name,
                        })};
    #else
    ${ntype.from_native({"generator": $generator,
                         "type_name": $ntype.namespaced_name.replace("*", ""),
                         "ntype": $ntype.get_whole_name($generator),
                         "level": 2,
                         "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
                         "in_value":"cobj->" + $pretty_name,
                         })};
    #end if

    return 1;
\#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function '${signature_name}_get${name}'.",&tolua_err);
    return 0;
\#endif
}

int ${signature_name}_set${name}(lua_State* tolua_S)
{
    int argc = 0;
    ${namespaced_class_name}* cobj = nullptr;
    bool ok  = true;

\#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (!tolua_isusertype(tolua_S,1,"${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}",0,&tolua_err)) goto tolua_lerror;
\#endif

    cobj = (${namespaced_class_name}*)tolua_tousertype(tolua_S,1,0);

\#if COCOS2D_DEBUG >= 1
    if (!cobj) 
    {
        tolua_error(tolua_S,"invalid 'cobj' in function '${signature_name}_set${name}'", nullptr);
        return 0;
    }
\#endif
    argc = lua_gettop(tolua_S) - 1;

    if (1 == argc)
    {
    #if $ntype.is_object and not $ntype.object_can_convert($generator)
        ${ntype.to_string($generator)}* arg0 = nullptr;
    #else
        ${ntype.to_string($generator)} arg0;
    #end if
        ${ntype.to_native({"generator": $generator,
                            "arg_idx": 2,
                            "out_value": "arg0",
                            "lua_namespaced_class_name": $generator.scriptname_from_native($namespaced_class_name, $namespace_name),
                            "func_name": $name,
                            "scriptname": $generator.scriptname_from_native($ntype.namespaced_name, $ntype.namespace_name),
                            "level": 2,
                            "arg":$ntype,
                      })};
    #if $ntype.is_object and not $ntype.object_can_convert($generator)
        cobj->$pretty_name = *arg0;
    #else
        cobj->$pretty_name = arg0;
    #end if
        return 0;
    }

    CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "${generator.scriptname_from_native($namespaced_class_name, $namespace_name)}:${name}",argc, 1);
    return 0;

\#if COCOS2D_DEBUG >= 1
tolua_lerror:
    tolua_error(tolua_S,"#ferror in function '${signature_name}_get${name}'.",&tolua_err);
    return 0;
\#endif
}