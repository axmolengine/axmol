
#set fields = $current_class.fields

int lua_register_${generator.prefix}_${current_class.class_name}(lua_State* tolua_S)
{
    tolua_module(tolua_S, "${current_class.class_name}", 0);
    tolua_beginmodule(tolua_S,"${current_class.class_name}");
    #for m in fields
        tolua_constant(tolua_S, "${m['name']}", ${m['value']});
    #end for
    tolua_endmodule(tolua_S);

    auto typeName = typeid(${current_class.namespaced_class_name}).name(); // rtti is literal storage
    g_luaType[reinterpret_cast<uintptr_t>(typeName)] = "${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}";
    g_typeCast[typeName] = "${generator.scriptname_from_native($current_class.namespaced_class_name, $current_class.namespace_name)}";
    return 1;
}
