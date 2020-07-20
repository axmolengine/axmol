/**
 * @method ${func_name}
#if $is_constructor
 * @constructor
#end if
#if str($ret_type) != "void"
 * @return A value converted from C/C++ "${ret_type}"
#end if
#if $min_args > 0
	#for $arg in $arguments
 * @param {$arg}
	#end for
#end if
 */
${func_name} : function () {},

