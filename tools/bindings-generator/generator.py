#!/usr/bin/env python
# generator.py
# simple C++ generator, originally targetted for Spidermonkey bindings
#
# Copyright (c) 2011 - Zynga Inc.

from clang import cindex
import sys
import pdb
import ConfigParser
import yaml
import re
import os
import inspect
from Cheetah.Template import Template

type_map = {
    cindex.TypeKind.VOID        : "void",
    cindex.TypeKind.BOOL        : "bool",
    cindex.TypeKind.CHAR_U      : "unsigned char",
    cindex.TypeKind.UCHAR       : "unsigned char",
    cindex.TypeKind.CHAR16      : "char",
    cindex.TypeKind.CHAR32      : "char",
    cindex.TypeKind.USHORT      : "unsigned short",
    cindex.TypeKind.UINT        : "unsigned int",
    cindex.TypeKind.ULONG       : "unsigned long",
    cindex.TypeKind.ULONGLONG   : "unsigned long long",
    cindex.TypeKind.CHAR_S      : "char",
    cindex.TypeKind.SCHAR       : "char",
    cindex.TypeKind.WCHAR       : "wchar_t",
    cindex.TypeKind.SHORT       : "short",
    cindex.TypeKind.INT         : "int",
    cindex.TypeKind.LONG        : "long",
    cindex.TypeKind.LONGLONG    : "long long",
    cindex.TypeKind.FLOAT       : "float",
    cindex.TypeKind.DOUBLE      : "double",
    cindex.TypeKind.LONGDOUBLE  : "long double",
    cindex.TypeKind.NULLPTR     : "NULL",
    cindex.TypeKind.OBJCID      : "id",
    cindex.TypeKind.OBJCCLASS   : "class",
    cindex.TypeKind.OBJCSEL     : "SEL",
    # cindex.TypeKind.ENUM        : "int"
}

INVALID_NATIVE_TYPE = "??"

default_arg_type_arr = [

# An integer literal.
cindex.CursorKind.INTEGER_LITERAL,

# A floating point number literal.
cindex.CursorKind.FLOATING_LITERAL,

# An imaginary number literal.
cindex.CursorKind.IMAGINARY_LITERAL,

# A string literal.
cindex.CursorKind.STRING_LITERAL,

# A character literal.
cindex.CursorKind.CHARACTER_LITERAL,

# [C++ 2.13.5] C++ Boolean Literal.
cindex.CursorKind.CXX_BOOL_LITERAL_EXPR,

# [C++0x 2.14.7] C++ Pointer Literal.
cindex.CursorKind.CXX_NULL_PTR_LITERAL_EXPR,

# An expression that refers to some value declaration, such as a function,
# varible, or enumerator.
cindex.CursorKind.DECL_REF_EXPR
]

def native_name_from_type(ntype, underlying=False):
    kind = ntype.get_canonical().kind
    const = "const " if ntype.is_const_qualified() else ""
    if not underlying and kind == cindex.TypeKind.ENUM:
        decl = ntype.get_declaration()
        return namespaced_name(decl)
    elif kind in type_map:
        return const + type_map[kind]
    elif kind == cindex.TypeKind.RECORD:
        # might be an std::string
        decl = ntype.get_declaration()
        parent = decl.semantic_parent
        if decl.spelling == "string" and parent and parent.spelling == "std":
            return "std::string"
        else:
            # print >> sys.stderr, "probably a function pointer: " + str(decl.spelling)
            return const + decl.spelling
    else:
        # name = ntype.get_declaration().spelling
        # print >> sys.stderr, "Unknown type: " + str(kind) + " " + str(name)
        return INVALID_NATIVE_TYPE
        # pdb.set_trace()


def build_namespace(cursor, namespaces=[]):
    '''
    build the full namespace for a specific cursor
    '''
    if cursor:
        parent = cursor.semantic_parent
        if parent:
            if parent.kind == cindex.CursorKind.NAMESPACE or parent.kind == cindex.CursorKind.CLASS_DECL:
                namespaces.append(parent.displayname)
                build_namespace(parent, namespaces)

    return namespaces


def namespaced_name(declaration_cursor):
    ns_list = build_namespace(declaration_cursor, [])
    ns_list.reverse()
    ns = "::".join(ns_list)
    if len(ns) > 0:
        return ns + "::" + declaration_cursor.displayname
    return declaration_cursor.displayname


class NativeType(object):
    def __init__(self):
        self.is_object = False
        self.is_function = False
        self.is_enum = False
        self.not_supported = False
        self.param_types = []
        self.ret_type = None
        self.namespaced_name = ""
        self.name = ""

    @staticmethod
    def from_type(ntype):
        if ntype.kind == cindex.TypeKind.POINTER:
            nt = NativeType.from_type(ntype.get_pointee())
            nt.name += "*"
            nt.namespaced_name += "*"
            nt.is_enum = False
        elif ntype.kind == cindex.TypeKind.LVALUEREFERENCE:
            nt = NativeType.from_type(ntype.get_pointee())
            nt.namespaced_name = namespaced_name(ntype.get_pointee().get_declaration())
        else:
            nt = NativeType()

            if ntype.kind == cindex.TypeKind.RECORD:
                decl = ntype.get_declaration()
                if decl.kind == cindex.CursorKind.CLASS_DECL:
                    nt.is_object = True
                nt.name = decl.displayname
                nt.namespaced_name = namespaced_name(decl)
            else:
                nt.name = native_name_from_type(ntype)
                if nt.name != INVALID_NATIVE_TYPE and nt.name != "std::string" and nt.name != "std::function" and ntype.kind == cindex.TypeKind.UNEXPOSED:
                    return NativeType.from_type(ntype.get_canonical())

                nt.namespaced_name = nt.name
                nt.is_enum = ntype.get_canonical().kind == cindex.TypeKind.ENUM
                if nt.name == "std::function":
                    decl = ntype.get_canonical().get_declaration()
                    nt.namespaced_name = namespaced_name(decl)

                    r = re.compile('function<(\S+) \((.*)\)>').search(decl.displayname)
                    (ret_type, params) = r.groups()
                    params = filter(None, params.split(", "))

                    nt.is_function = True
                    nt.ret_type = NativeType.from_string(ret_type)
                    nt.param_types = [NativeType.from_string(string) for string in params]

        # mark argument as not supported
        if nt.name == INVALID_NATIVE_TYPE:
            nt.not_supported = True

        return nt

    @staticmethod
    def from_string(displayname):
        displayname = displayname.replace("cocos2d::", "")
        displayname = displayname.replace(" *", "*")

        nt = NativeType()
        nt.name = displayname
        nt.namespaced_name = displayname
        nt.is_object = True
        return nt

    @property
    def lambda_parameters(self):
        params = ["%s larg%d" % (str(nt), i) for i, nt in enumerate(self.param_types)]
        return ", ".join(params)

    def from_native(self, convert_opts):
        assert(convert_opts.has_key('generator'))
        generator = convert_opts['generator']
        name = self.name
        if self.is_object:
            if not generator.config['conversions']['from_native'].has_key(name):
                name = "object"
        elif self.is_enum:
            name = "int"

        if generator.config['conversions']['from_native'].has_key(name):
            tpl = generator.config['conversions']['from_native'][name]
            tpl = Template(tpl, searchList=[convert_opts])
            return str(tpl).rstrip()

        return "#pragma warning NO CONVERSION FROM NATIVE FOR " + name

    def to_native(self, convert_opts):
        assert('generator' in convert_opts)
        generator = convert_opts['generator']
        name = self.name
        if self.is_object:
            if not name in generator.config['conversions']['to_native']:
                name = "object"
        elif self.is_enum:
            name = "int"

        if self.is_function:
            tpl = Template(file=os.path.join(generator.target, "templates", "lambda.c"),
                searchList=[convert_opts, self])
            indent = convert_opts['level'] * "\t"
            return str(tpl).replace("\n", "\n" + indent)

        if generator.config['conversions']['to_native'].has_key(name):
            tpl = generator.config['conversions']['to_native'][name]
            tpl = Template(tpl, searchList=[convert_opts])
            return str(tpl).rstrip()
        return "#pragma warning NO CONVERSION TO NATIVE FOR " + name

    def to_string(self, generator):
        conversions = generator.config['conversions']
        if conversions.has_key('native_types') and conversions['native_types'].has_key(self.namespaced_name):
            return conversions['native_types'][self.namespaced_name]
        return self.namespaced_name

    def __str__(self):
        return self.namespaced_name

class NativeField(object):
    def __init__(self, cursor):
        cursor = cursor.canonical
        self.cursor = cursor
        self.name = cursor.displayname
        self.kind = cursor.type.kind
        self.location = cursor.location
        member_field_re = re.compile('m_(\w+)')
        match = member_field_re.match(self.name)
        if match:
            self.pretty_name = match.group(1)
        else:
            self.pretty_name = self.name

# return True if found default argument.
def iterate_param_node(param_node, depth=1):
    for node in param_node.get_children():
        # print(">"*depth+" "+str(node.kind))
        if (node.kind in default_arg_type_arr):
            return True

        if (iterate_param_node(node, depth+1)):
            return True

    return False

class NativeFunction(object):
    def __init__(self, cursor):
        self.cursor = cursor
        self.func_name = cursor.spelling
        self.signature_name = self.func_name
        self.arguments = []
        self.static = cursor.kind == cindex.CursorKind.CXX_METHOD and cursor.is_method_static()
        self.implementations = []
        self.is_constructor = False
        self.not_supported = False
        result = cursor.result_type
        # get the result
        if result.kind == cindex.TypeKind.LVALUEREFERENCE:
            result = result.get_pointee()
        self.ret_type = NativeType.from_type(cursor.result_type)
        # parse the arguments
        # if self.func_name == "spriteWithFile":
        #   pdb.set_trace()
        for arg in cursor.type.argument_types():
            nt = NativeType.from_type(arg)
            self.arguments.append(nt)
            # mark the function as not supported if at least one argument is not supported
            if nt.not_supported:
                self.not_supported = True


        found_default_arg = False
        index = -1

        for arg_node in self.cursor.get_children():
            if arg_node.kind == cindex.CursorKind.PARM_DECL:
                index+=1
                if (iterate_param_node(arg_node)):
                    found_default_arg = True
                    break

        self.min_args = index if found_default_arg else len(self.arguments)

    def generate_code(self, current_class=None, generator=None):
        gen = current_class.generator if current_class else generator
        config = gen.config
        tpl = Template(file=os.path.join(gen.target, "templates", "function.h"),
                        searchList=[current_class, self])
        gen.head_file.write(str(tpl))
        if self.static:
            if config['definitions'].has_key('sfunction'):
                tpl = Template(config['definitions']['sfunction'],
                                    searchList=[current_class, self])
                self.signature_name = str(tpl)
            tpl = Template(file=os.path.join(gen.target, "templates", "sfunction.c"),
                            searchList=[current_class, self])
        else:
            if not self.is_constructor:
                if config['definitions'].has_key('ifunction'):
                    tpl = Template(config['definitions']['ifunction'],
                                    searchList=[current_class, self])
                    self.signature_name = str(tpl)
            else:
                if config['definitions'].has_key('constructor'):
                    tpl = Template(config['definitions']['constructor'],
                                    searchList=[current_class, self])
                    self.signature_name = str(tpl)
            tpl = Template(file=os.path.join(gen.target, "templates", "ifunction.c"),
                            searchList=[current_class, self])
        gen.impl_file.write(str(tpl))
        apidoc_function_js = Template(file=os.path.join(gen.target,
                                                        "templates",
                                                        "apidoc_function.js"),
                                      searchList=[current_class, self])
        gen.doc_file.write(str(apidoc_function_js))

class NativeOverloadedFunction(object):
    def __init__(self, func_array):
        self.implementations = func_array
        self.func_name = func_array[0].func_name
        self.signature_name = self.func_name
        self.min_args = 100
        self.is_constructor = False
        for m in func_array:
            self.min_args = min(self.min_args, m.min_args)

    def append(self, func):
        self.min_args = min(self.min_args, func.min_args)
        self.implementations.append(func)

    def generate_code(self, current_class=None):
        gen = current_class.generator
        config = gen.config
        static = self.implementations[0].static
        tpl = Template(file=os.path.join(gen.target, "templates", "function.h"),
                        searchList=[current_class, self])
        gen.head_file.write(str(tpl))
        if static:
            if config['definitions'].has_key('sfunction'):
                tpl = Template(config['definitions']['sfunction'],
                                searchList=[current_class, self])
                self.signature_name = str(tpl)
            tpl = Template(file=os.path.join(gen.target, "templates", "sfunction_overloaded.c"),
                            searchList=[current_class, self])
        else:
            if not self.is_constructor:
                if config['definitions'].has_key('ifunction'):
                    tpl = Template(config['definitions']['ifunction'],
                                    searchList=[current_class, self])
                    self.signature_name = str(tpl)
            else:
                if config['definitions'].has_key('constructor'):
                    tpl = Template(config['definitions']['constructor'],
                                    searchList=[current_class, self])
                    self.signature_name = str(tpl)
            tpl = Template(file=os.path.join(gen.target, "templates", "ifunction_overloaded.c"),
                            searchList=[current_class, self])
        gen.impl_file.write(str(tpl))


class NativeClass(object):
    def __init__(self, cursor, generator):
        # the cursor to the implementation
        self.cursor = cursor
        self.class_name = cursor.displayname
        self.namespaced_class_name = self.class_name
        self.parents = []
        self.fields = []
        self.methods = {}
        self.static_methods = {}
        self.generator = generator
        self.is_abstract = self.class_name in generator.abstract_classes
        self._current_visibility = cindex.AccessSpecifierKind.PRIVATE

        registration_name = generator.get_class_or_rename_class(self.class_name)
        if generator.remove_prefix:
            self.target_class_name = re.sub('^'+generator.remove_prefix, '', registration_name)
        else:
            self.target_class_name = registration_name
        self.namespaced_class_name = namespaced_name(cursor)
        self.parse()

    def parse(self):
        '''
        parse the current cursor, getting all the necesary information
        '''
        self._deep_iterate(self.cursor)

    def methods_clean(self):
        '''
        clean list of methods (without the ones that should be skipped)
        '''
        ret = []
        for name, impl in self.methods.iteritems():
            should_skip = False
            if name == 'constructor':
                should_skip = True
            else:
                if self.generator.should_skip(self.class_name, name):
                    should_skip = True
            if not should_skip:
                ret.append({"name": name, "impl": impl})
        return ret

    def static_methods_clean(self):
        '''
        clean list of static methods (without the ones that should be skipped)
        '''
        ret = []
        for name, impl in self.static_methods.iteritems():
            should_skip = self.generator.should_skip(self.class_name, name)
            if not should_skip:
                ret.append({"name": name, "impl": impl})
        return ret

    def generate_code(self):
        '''
        actually generate the code. it uses the current target templates/rules in order to
        generate the right code
        '''
        config = self.generator.config
        prelude_h = Template(file=os.path.join(self.generator.target, "templates", "prelude.h"),
                            searchList=[{"current_class": self}])
        prelude_c = Template(file=os.path.join(self.generator.target, "templates", "prelude.c"),
                            searchList=[{"current_class": self}])
        apidoc_classhead_js = Template(file=os.path.join(self.generator.target,
                                                         "templates",
                                                         "apidoc_classhead.js"),
                                       searchList=[{"current_class": self}])
        self.generator.head_file.write(str(prelude_h))
        self.generator.impl_file.write(str(prelude_c))
        self.generator.doc_file.write(str(apidoc_classhead_js))
        for m in self.methods_clean():
            m['impl'].generate_code(self)
        for m in self.static_methods_clean():
            m['impl'].generate_code(self)
        # generate register section
        register = Template(file=os.path.join(self.generator.target, "templates", "register.c"),
                            searchList=[{"current_class": self}])
        apidoc_classfoot_js = Template(file=os.path.join(self.generator.target,
                                                         "templates",
                                                         "apidoc_classfoot.js"),
                                       searchList=[{"current_class": self}])
        self.generator.impl_file.write(str(register))
        self.generator.doc_file.write(str(apidoc_classfoot_js))

    def _deep_iterate(self, cursor=None, depth=0):
        for node in cursor.get_children():
            # print("%s%s - %s" % ("> " * depth, node.displayname, node.kind))
            if self._process_node(node):
                self._deep_iterate(node, depth + 1)

    def _process_node(self, cursor):
        '''
        process the node, depending on the type. If returns true, then it will perform a deep
        iteration on its children. Otherwise it will continue with its siblings (if any)

        @param: cursor the cursor to analyze
        '''
        if cursor.kind == cindex.CursorKind.CXX_BASE_SPECIFIER and not self.class_name in self.generator.classes_have_no_parents:
            parent = cursor.get_definition()
            if parent.displayname not in self.generator.base_classes_to_skip:
                #if parent and self.generator.in_listed_classes(parent.displayname):
                if not self.generator.generated_classes.has_key(parent.displayname):
                    parent = NativeClass(parent, self.generator)
                    self.generator.generated_classes[parent.class_name] = parent
                else:
                    parent = self.generator.generated_classes[parent.displayname]
                self.parents.append(parent)
        elif cursor.kind == cindex.CursorKind.FIELD_DECL:
            self.fields.append(NativeField(cursor))
        elif cursor.kind == cindex.CursorKind.CXX_ACCESS_SPEC_DECL:
            self._current_visibility = cursor.get_access_specifier()
        elif cursor.kind == cindex.CursorKind.CXX_METHOD:
            # skip if variadic
            if self._current_visibility == cindex.AccessSpecifierKind.PUBLIC and not cursor.type.is_function_variadic():
                m = NativeFunction(cursor)
                registration_name = self.generator.should_rename_function(self.class_name, m.func_name) or m.func_name
                # bail if the function is not supported (at least one arg not supported)
                if m.not_supported:
                    return
                if m.static:
                    if not self.static_methods.has_key(registration_name):
                        self.static_methods[registration_name] = m
                    else:
                        previous_m = self.static_methods[registration_name]
                        if isinstance(previous_m, NativeOverloadedFunction):
                            previous_m.append(m)
                        else:
                            self.static_methods[registration_name] = NativeOverloadedFunction([m, previous_m])
                else:
                    if not self.methods.has_key(registration_name):
                        self.methods[registration_name] = m
                    else:
                        previous_m = self.methods[registration_name]
                        if isinstance(previous_m, NativeOverloadedFunction):
                            previous_m.append(m)
                        else:
                            self.methods[registration_name] = NativeOverloadedFunction([m, previous_m])
            return True

        elif self._current_visibility == cindex.AccessSpecifierKind.PUBLIC and cursor.kind == cindex.CursorKind.CONSTRUCTOR and not self.is_abstract:
            # Skip copy constructor
            if cursor.displayname == self.class_name + "(const " + self.namespaced_class_name + " &)":
                # print "Skip copy constructor: " + cursor.displayname
                return True

            m = NativeFunction(cursor)
            m.is_constructor = True
            if not self.methods.has_key('constructor'):
                self.methods['constructor'] = m
            else:
                previous_m = self.methods['constructor']
                if isinstance(previous_m, NativeOverloadedFunction):
                    previous_m.append(m)
                else:
                    m = NativeOverloadedFunction([m, previous_m])
                    m.is_constructor = True
                    self.methods['constructor'] = m
            return True
        # else:
            # print >> sys.stderr, "unknown cursor: %s - %s" % (cursor.kind, cursor.displayname)
        return False

class Generator(object):
    def __init__(self, opts):
        self.index = cindex.Index.create()
        self.outdir = opts['outdir']
        self.prefix = opts['prefix']
        self.headers = opts['headers'].split(' ')
        self.classes = opts['classes']
        self.classes_have_no_parents = opts['classes_have_no_parents'].split(' ')
        self.base_classes_to_skip = opts['base_classes_to_skip'].split(' ')
        self.abstract_classes = opts['abstract_classes'].split(' ')
        self.clang_args = opts['clang_args']
        self.target = opts['target']
        self.remove_prefix = opts['remove_prefix']
        self.target_ns = opts['target_ns']
        self.impl_file = None
        self.head_file = None
        self.skip_classes = {}
        self.generated_classes = {}
        self.rename_functions = {}
        self.rename_classes = {}
        self.out_file = opts['out_file']
        self.script_control_cpp = opts['script_control_cpp'] == "yes"

        if opts['skip']:
            list_of_skips = re.split(",\n?", opts['skip'])
            for skip in list_of_skips:
                class_name, methods = skip.split("::")
                self.skip_classes[class_name] = []
                match = re.match("\[([^]]+)\]", methods)
                if match:
                    self.skip_classes[class_name] = match.group(1).split(" ")
                else:
                    raise Exception("invalid list of skip methods")
        if opts['rename_functions']:
            list_of_function_renames = re.split(",\n?", opts['rename_functions'])
            for rename in list_of_function_renames:
                class_name, methods = rename.split("::")
                self.rename_functions[class_name] = {}
                match = re.match("\[([^]]+)\]", methods)
                if match:
                    list_of_methods = match.group(1).split(" ")
                    for pair in list_of_methods:
                        k, v = pair.split("=")
                        self.rename_functions[class_name][k] = v
                else:
                    raise Exception("invalid list of rename methods")

        if opts['rename_classes']:
            list_of_class_renames = re.split(",\n?", opts['rename_classes'])
            for rename in list_of_class_renames:
                class_name, renamed_class_name = rename.split("::")
                self.rename_classes[class_name] = renamed_class_name


    def should_rename_function(self, class_name, method_name):
        if self.rename_functions.has_key(class_name) and self.rename_functions[class_name].has_key(method_name):
            # print >> sys.stderr, "will rename %s to %s" % (method_name, self.rename_functions[class_name][method_name])
            return self.rename_functions[class_name][method_name]
        return None

    def get_class_or_rename_class(self, class_name):
        if self.rename_classes.has_key(class_name):
            # print >> sys.stderr, "will rename %s to %s" % (method_name, self.rename_functions[class_name][method_name])
            return self.rename_classes[class_name]
        return class_name

    def should_skip(self, class_name, method_name, verbose=False):
        if class_name == "*" and self.skip_classes.has_key("*"):
            for func in self.skip_classes["*"]:
                if re.match(func, method_name):
                    return True
        else:
            for key in self.skip_classes.iterkeys():
                if key == "*" or re.match("^" + key + "$", class_name):
                    if verbose:
                        print "%s in skip_classes" % (class_name)
                    if len(self.skip_classes[key]) == 1 and self.skip_classes[key][0] == "*":
                        if verbose:
                            print "%s will be skipped completely" % (class_name)
                        return True
                    if method_name != None:
                        for func in self.skip_classes[key]:
                            if re.match(func, method_name):
                                if verbose:
                                    print "%s will skip method %s" % (class_name, method_name)
                                return True
        if verbose:
            print "%s will be accepted (%s, %s)" % (class_name, key, self.skip_classes[key])
        return False

    def in_listed_classes(self, class_name):
        """
        returns True if the class is in the list of required classes and it's not in the skip list
        """
        for key in self.classes:
            md = re.match("^" + key + "$", class_name)
            if md and not self.should_skip(class_name, None):
                return True
        return False

    def sorted_classes(self):
        '''
        sorted classes in order of inheritance
        '''
        sorted_list = []
        for class_name in self.generated_classes.iterkeys():
            nclass = self.generated_classes[class_name]
            sorted_list += self._sorted_parents(nclass)
        # remove dupes from the list
        no_dupes = []
        [no_dupes.append(i) for i in sorted_list if not no_dupes.count(i)]
        return no_dupes

    def _sorted_parents(self, nclass):
        '''
        returns the sorted list of parents for a native class
        '''
        sorted_parents = []
        for p in nclass.parents:
            if p.class_name in self.generated_classes.keys():
                sorted_parents += self._sorted_parents(p)
        if nclass.class_name in self.generated_classes.keys():
            sorted_parents.append(nclass.class_name)
        return sorted_parents

    def generate_code(self):
        # must read the yaml file first
        stream = file(os.path.join(self.target, "conversions.yaml"), "r")
        data = yaml.load(stream)
        self.config = data
        implfilepath = os.path.join(self.outdir, self.out_file + ".cpp")
        headfilepath = os.path.join(self.outdir, self.out_file + ".hpp")
        docfilepath = os.path.join(self.outdir, self.out_file + "_api.js")
        self.impl_file = open(implfilepath, "w+")
        self.head_file = open(headfilepath, "w+")
        self.doc_file = open(docfilepath, "w+")

        layout_h = Template(file=os.path.join(self.target, "templates", "layout_head.h"),
                            searchList=[self])
        layout_c = Template(file=os.path.join(self.target, "templates", "layout_head.c"),
                            searchList=[self])
        apidoc_ns_js = Template(file=os.path.join(self.target, "templates", "apidoc_ns.js"),
                                searchList=[self])
        self.head_file.write(str(layout_h))
        self.impl_file.write(str(layout_c))
        self.doc_file.write(str(apidoc_ns_js))

        self._parse_headers()

        layout_h = Template(file=os.path.join(self.target, "templates", "layout_foot.h"),
                            searchList=[self])
        layout_c = Template(file=os.path.join(self.target, "templates", "layout_foot.c"),
                            searchList=[self])
        self.head_file.write(str(layout_h))
        self.impl_file.write(str(layout_c))

        self.impl_file.close()
        self.head_file.close()
        self.doc_file.close()

    def _pretty_print(self, diagnostics):
        print("====\nErrors in parsing headers:")
        severities=['Ignored', 'Note', 'Warning', 'Error', 'Fatal']
        for idx, d in enumerate(diagnostics):
            print "%s. <severity = %s,\n    location = %r,\n    details = %r>" % (
                idx+1, severities[d.severity], d.location, d.spelling)
        print("====\n")
        
    def _parse_headers(self):
        for header in self.headers:
            tu = self.index.parse(header, self.clang_args)
            if len(tu.diagnostics) > 0:
                self._pretty_print(tu.diagnostics)
                is_fatal = False
                for d in tu.diagnostics:
                    if d.severity >= cindex.Diagnostic.Error:
                        is_fatal = True
                if is_fatal:
                    print("*** Found errors - can not continue")
                    raise Exception("Fatal error in parsing headers")
            self._deep_iterate(tu.cursor)

    def _deep_iterate(self, cursor, depth=0):
        # get the canonical type
        if cursor.kind == cindex.CursorKind.CLASS_DECL:
            if cursor == cursor.type.get_declaration() and len(cursor.get_children_array()) > 0 and self.in_listed_classes(cursor.displayname):
                if not self.generated_classes.has_key(cursor.displayname):
                    nclass = NativeClass(cursor, self)
                    nclass.generate_code()
                    self.generated_classes[cursor.displayname] = nclass
                return

        for node in cursor.get_children():
            # print("%s %s - %s" % (">" * depth, node.displayname, node.kind))
            self._deep_iterate(node, depth + 1)


def main():
    from optparse import OptionParser

    parser = OptionParser("usage: %prog [options] {configfile}")
    parser.add_option("-s", action="store", type="string", dest="section",
                        help="sets a specific section to be converted")
    parser.add_option("-t", action="store", type="string", dest="target",
                        help="specifies the target vm. Will search for TARGET.yaml")
    parser.add_option("-o", action="store", type="string", dest="outdir",
                        help="specifies the output directory for generated C++ code")
    parser.add_option("-n", action="store", type="string", dest="out_file",
                        help="specifcies the name of the output file, defaults to the prefix in the .ini file")

    (opts, args) = parser.parse_args()

    # script directory
    workingdir = os.path.dirname(inspect.getfile(inspect.currentframe()))

    if len(args) == 0:
        parser.error('invalid number of arguments')

    userconfig = ConfigParser.SafeConfigParser()
    userconfig.read('userconf.ini')
    print 'Using userconfig \n ', userconfig.items('DEFAULT')

    config = ConfigParser.SafeConfigParser()
    config.read(args[0])

    if (0 == len(config.sections())):
        raise Exception("No sections defined in config file")

    sections = []
    if opts.section:
        if (opts.section in config.sections()):
            sections = []
            sections.append(opts.section)
        else:
            raise Exception("Section not found in config file")
    else:
        print("processing all sections")
        sections = config.sections()

    # find available targets
    targetdir = os.path.join(workingdir, "targets")
    targets = []
    if (os.path.isdir(targetdir)):
        targets = [entry for entry in os.listdir(targetdir)
                    if (os.path.isdir(os.path.join(targetdir, entry)))]
    if 0 == len(targets):
        raise Exception("No targets defined")

    if opts.target:
        if (opts.target in targets):
            targets = []
            targets.append(opts.target)

    if opts.outdir:
        outdir = opts.outdir
    else:
        outdir = os.path.join(workingdir, "gen")
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    for t in targets:
        # Fix for hidden '.svn', '.cvs' and '.git' etc. folders - these must be ignored or otherwise they will be interpreted as a target.
        if t == ".svn" or t == ".cvs" or t == ".git" or t == ".gitignore":
            continue

        print "\n.... Generating bindings for target", t
        for s in sections:
            print "\n.... .... Processing section", s, "\n"
            gen_opts = {
                'prefix': config.get(s, 'prefix'),
                'headers':    (config.get(s, 'headers'        , 0, dict(userconfig.items('DEFAULT')))),
                'classes': config.get(s, 'classes').split(' '),
                'clang_args': (config.get(s, 'extra_arguments', 0, dict(userconfig.items('DEFAULT'))) or "").split(" "),
                'target': os.path.join(workingdir, "targets", t),
                'outdir': outdir,
                'remove_prefix': config.get(s, 'remove_prefix'),
                'target_ns': config.get(s, 'target_namespace'),
                'classes_have_no_parents': config.get(s, 'classes_have_no_parents'),
                'base_classes_to_skip': config.get(s, 'base_classes_to_skip'),
                'abstract_classes': config.get(s, 'abstract_classes'),
                'skip': config.get(s, 'skip'),
                'rename_functions': config.get(s, 'rename_functions'),
                'rename_classes': config.get(s, 'rename_classes'),
                'out_file': opts.out_file or config.get(s, 'prefix'),
                'script_control_cpp': config.get(s, 'script_control_cpp') if config.has_option(s, 'script_control_cpp') else 'no'
                }
            generator = Generator(gen_opts)
            generator.generate_code()

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print e
        sys.exit(1)