void register_all_${prefix}(JSContext* cx, JS::HandleObject obj) {
    #if $target_ns
    // Get the ns
    JS::RootedObject ns(cx);
    get_or_create_js_obj(cx, obj, "${target_ns}", &ns);
    #else 
    // Get the global ns
    JS::RootedObject ns(cx, ScriptingCore::getInstance()->getGlobalObject());
    #end if

    #for jsclass in $sorted_classes
    #if $in_listed_classes(jsclass)
    js_register_${prefix}_${jsclass}(cx, ns);
    #end if
    #end for
}

#if $macro_judgement
\#endif //$macro_judgement
#end if