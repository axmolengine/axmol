#set has_constructor = False
#if $current_class.methods.has_key('constructor')
#set has_constructor = True
${current_class.methods.constructor.generate_code($current_class)}
#end if
#
#set generator = $current_class.generator
#set methods = $current_class.methods_clean()
#set st_methods = $current_class.static_methods_clean()
#
static int lua_${generator.prefix}_${current_class.class_name}_finalize(lua_State* tolua_S)
{
    printf("luabindings: finalizing LUA object (${current_class.class_name})");
#if $generator.script_control_cpp
\#if COCOS2D_DEBUG >= 1
    tolua_Error tolua_err;
    if (
    !tolua_isusertype(tolua_S,1,"${current_class.class_name}",0,&tolua_err) ||
    !tolua_isnoobj(tolua_S,2,&tolua_err)
    )
        goto tolua_lerror;
    else
\#endif
    {
        ${current_class.namespaced_class_name}* self = (${current_class.namespaced_class_name}*)  tolua_tousertype(tolua_S,1,0);
\#if COCOS2D_DEBUG >= 1
        if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", nullptr);
\#endif
        delete self;
    }
    return 0;
\#if COCOS2D_DEBUG >= 1
    tolua_lerror:
    tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
    return 0;
\#endif
#end if
    return 0;
}

int lua_register_${generator.prefix}_${current_class.class_name}(lua_State* tolua_S)
{
    tolua_usertype(tolua_S,"${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}");
    #if len($current_class.parents) > 0
        #if $generator.script_control_cpp and $current_class.has_constructor
    tolua_cclass(tolua_S,"${current_class.class_name}","${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}","${generator.scriptname_from_native($current_class.parents[0].namespaced_class_name,$current_class.parents[0].namespace_name)}",lua_${generator.prefix}_${current_class.class_name}_finalize);
        #else
    tolua_cclass(tolua_S,"${current_class.class_name}","${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}","${generator.scriptname_from_native($current_class.parents[0].namespaced_class_name,$current_class.parents[0].namespace_name)}",nullptr);
        #end if
    #else
        #if $generator.script_control_cpp and $current_class.has_constructor
    tolua_cclass(tolua_S,"${current_class.class_name}","${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}","",lua_${generator.prefix}_${current_class.class_name}_finalize);
        #else
    tolua_cclass(tolua_S,"${current_class.class_name}","${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}","",nullptr);
        #end if
    #end if

    tolua_beginmodule(tolua_S,"${current_class.class_name}");
    #if has_constructor
        tolua_function(tolua_S,"new",lua_${generator.prefix}_${current_class.class_name}_constructor);
    #end if
    #for m in methods
        #set fn = m['impl']
        tolua_function(tolua_S,"${m['name']}",${fn.signature_name});
    #end for
    #for m in st_methods
        #set fn = m['impl']
        tolua_function(tolua_S,"${m['name']}", ${fn.signature_name});
    #end for
    tolua_endmodule(tolua_S);
    std::string typeName = typeid(${current_class.namespaced_class_name}).name();
    g_luaType[typeName] = "${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}";
    g_typeCast["${current_class.class_name}"] = "${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}";
    return 1;
}
