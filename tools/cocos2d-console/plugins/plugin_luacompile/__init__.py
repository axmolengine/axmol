#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos "luacompile" plugin
#
# Copyright 2013 (C) Intel
#
# License: MIT
# ----------------------------------------------------------------------------

'''
"luacompile" plugin for cocos command line tool
'''

__docformat__ = 'restructuredtext'

import sys
import subprocess
import os
import json
import inspect
import shutil

import cocos
from MultiLanguage import MultiLanguage

############################################################ 
#http://www.coolcode.org/archives/?article-307.html
############################################################ 

import struct 

_DELTA = 0x9E3779B9  

def _long2str(v, w):  
    n = (len(v) - 1) << 2  
    if w:  
        m = v[-1]  
        if (m < n - 3) or (m > n): return ''  
        n = m  
    s = struct.pack('<%iL' % len(v), *v)  
    return s[0:n] if w else s  
  
def _str2long(s, w):  
    n = len(s)  
    m = (4 - (n & 3) & 3) + n  
    s = s.ljust(m, "\0")  
    v = list(struct.unpack('<%iL' % (m >> 2), s))  
    if w: v.append(n)  
    return v  
  
def encrypt(str, key):  
    if str == '': return str  
    v = _str2long(str, True)  
    k = _str2long(key.ljust(16, "\0"), False)  
    n = len(v) - 1  
    z = v[n]  
    y = v[0]  
    sum = 0  
    q = 6 + 52 // (n + 1)  
    while q > 0:  
        sum = (sum + _DELTA) & 0xffffffff  
        e = sum >> 2 & 3  
        for p in xrange(n):  
            y = v[p + 1]  
            v[p] = (v[p] + ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z))) & 0xffffffff  
            z = v[p]  
        y = v[0]  
        v[n] = (v[n] + ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[n & 3 ^ e] ^ z))) & 0xffffffff  
        z = v[n]  
        q -= 1  
    return _long2str(v, False)  
  
def decrypt(str, key):  
    if str == '': return str  
    v = _str2long(str, False)  
    k = _str2long(key.ljust(16, "\0"), False)  
    n = len(v) - 1  
    z = v[n]  
    y = v[0]  
    q = 6 + 52 // (n + 1)  
    sum = (q * _DELTA) & 0xffffffff  
    while (sum != 0):  
        e = sum >> 2 & 3  
        for p in xrange(n, 0, -1):  
            z = v[p - 1]  
            v[p] = (v[p] - ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z))) & 0xffffffff  
            y = v[p]  
        z = v[n]  
        v[0] = (v[0] - ((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[0 & 3 ^ e] ^ z))) & 0xffffffff  
        y = v[0]  
        sum = (sum - _DELTA) & 0xffffffff  
    return _long2str(v, True)  



#import cocos
class CCPluginLuaCompile(cocos.CCPlugin):
    """
    compiles (encodes) and minifies Lua files
    """
    @staticmethod
    def plugin_name():
        return "luacompile"

    @staticmethod
    def brief_description():
        # returns a short description of this module
        return MultiLanguage.get_string('LUACOMPILE_BRIEF')

    # This is not the constructor, just an initializator
    def init(self, options, workingdir):
        """
        Arguments:
        - `options`:
        """
        self._current_src_dir = None
        self._src_dir_arr = self.normalize_path_in_list(options.src_dir_arr)
        self._dst_dir = options.dst_dir
        if not os.path.isabs(self._dst_dir):
            self._dst_dir = os.path.abspath(self._dst_dir)
        self._verbose = options.verbose
        self._workingdir = workingdir
        self._lua_files = {}
        self._isEncrypt = options.encrypt
        self._encryptkey = options.encryptkey
        self._encryptsign = options.encryptsign
        self._bytecode_64bit = options.bytecode_64bit

        self._luajit_exe_path = self.get_luajit_path()
        self._disable_compile = options.disable_compile

        if self._luajit_exe_path is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_TOOL_NOT_FOUND'),
                                      cocos.CCPluginError.ERROR_TOOLS_NOT_FOUND)

        self._luajit_dir = os.path.dirname(self._luajit_exe_path)

    def normalize_path_in_list(self, list):
        for i in list:
            tmp = os.path.normpath(i)
            if not os.path.isabs(tmp):
                tmp = os.path.abspath(tmp)
            list[list.index(i)] = tmp
        return list

    def get_relative_path(self, luafile):
        try:
            pos = luafile.index(self._current_src_dir)
            if pos != 0:
                raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_SRCDIR_NAME_NOT_FOUND'),
                                          cocos.CCPluginError.ERROR_WRONG_ARGS)

            return luafile[len(self._current_src_dir)+1:]
        except ValueError:
            raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_SRCDIR_NAME_NOT_FOUND'),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)

    def get_output_file_path(self, luafile):
        """
        Gets output file path by source lua file
        """
        # create folder for generated file
        luac_filepath = ""
        # Unknow to remove 'c' 
        relative_path = self.get_relative_path(luafile)+"c"
        luac_filepath = os.path.join(self._dst_dir, relative_path)

        dst_rootpath = os.path.split(luac_filepath)[0]
        try:
            # print "creating dir (%s)" % (dst_rootpath)
            os.makedirs(dst_rootpath)
        except OSError:
            if os.path.exists(dst_rootpath) == False:
                # There was an error on creation, so make sure we know about it
                raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_MKDIR_FAILED_FMT',
                                                                   dst_rootpath),
                                          cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        # print "return luac path: "+luac_filepath
        return luac_filepath

    def get_luajit_path(self):
        ret = None

        bit_prefix = "64bit" if self._bytecode_64bit else "32bit"
        if cocos.os_is_win32():
            ret = os.path.join(self._workingdir, "bin", bit_prefix, "luajit-win.exe")
        elif cocos.os_is_mac():
            ret = os.path.join(self._workingdir, "bin", bit_prefix, "luajit-mac")
        elif cocos.os_is_linux():
            ret = os.path.join(self._workingdir, "bin", bit_prefix, "luajit-linux")

        print("luajit bin path: " + ret)
        return ret

    def compile_lua(self, lua_file, output_file):
        """
        Compiles lua file
        """
        cocos.Logging.debug(MultiLanguage.get_string('LUACOMPILE_DEBUG_COMPILE_FILE_FMT', lua_file))

        with cocos.pushd(self._luajit_dir):
            cmd_str = "\"%s\" -b \"%s\" \"%s\"" % (self._luajit_exe_path, lua_file, output_file)
            self._run_cmd(cmd_str)

    # TODO
    # def compress_js(self):
    def deep_iterate_dir(self, rootDir):
        for lists in os.listdir(rootDir):
            path = os.path.join(rootDir, lists)
            if os.path.isdir(path):
                self.deep_iterate_dir(path)
            elif os.path.isfile(path):
                if os.path.splitext(path)[1] == ".lua":
                    self._lua_files[self._current_src_dir].append(path)

    # UNDO
    # def index_in_list(self, lua_file, l):
    # def lua_filename_pre_order_compare(self, a, b):
    # def lua_filename_post_order_compare(self, a, b):
    # def _lua_filename_compare(self, a, b, files, delta):
    # def reorder_lua_files(self):

    def handle_all_lua_files(self):
        """
        Arguments:
        - `self`:
        """

        cocos.Logging.info(MultiLanguage.get_string('LUACOMPILE_INFO_PROCESS_FILE'))
        index = 0
        for src_dir in self._src_dir_arr:
            for lua_file in self._lua_files[src_dir]:
                self._current_src_dir = src_dir
                dst_lua_file = self.get_output_file_path(lua_file)
                if self._disable_compile:
                    shutil.copy(lua_file, dst_lua_file)
                else:
                    self.compile_lua(lua_file, dst_lua_file)                   

                if self._isEncrypt == True:
                    bytesFile = open(dst_lua_file, "rb+")
                    encryBytes = encrypt(bytesFile.read(), self._encryptkey)
                    encryBytes = self._encryptsign + encryBytes
                    bytesFile.seek(0)
                    bytesFile.write(encryBytes)
                    bytesFile.close()
                    index = index + 1



    def run(self, argv, dependencies):
        """
        """
        self.parse_args(argv)

        # tips
        cocos.Logging.warning(MultiLanguage.get_string('LUACOMPILE_WARNING_TIP_MSG'))
        # create output directory
        try:
            os.makedirs(self._dst_dir)
        except OSError:
            if os.path.exists(self._dst_dir) == False:
                raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_MKDIR_FAILED_FMT',
                                                                   self._dst_dir),
                                          cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        # deep iterate the src directory
        for src_dir in self._src_dir_arr:
            self._current_src_dir = src_dir
            self._lua_files[self._current_src_dir] = []
            self.deep_iterate_dir(src_dir)

        self.handle_all_lua_files()

        cocos.Logging.info(MultiLanguage.get_string('LUACOMPILE_INFO_FINISHED'))

    def parse_args(self, argv):
        """
        """

        from argparse import ArgumentParser

        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())

        parser.add_argument("-v", "--verbose",
                          action="store_true",
                          dest="verbose",
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_VERBOSE'))
        parser.add_argument("-s", "--src", dest="src_dir_arr",
                          action="append", help=MultiLanguage.get_string('LUACOMPILE_ARG_SRC'))
        parser.add_argument("-d", "--dst", dest="dst_dir",
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_DST'))
        parser.add_argument("-e", "--encrypt",
                          action="store_true", dest="encrypt",default=False,
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_ENCRYPT'))
        parser.add_argument("-k", "--encryptkey",
                          dest="encryptkey",default="2dxLua",
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_ENCRYPT_KEY'))
        parser.add_argument("-b", "--encryptsign",
                          dest="encryptsign",default="XXTEA",
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_ENCRYPT_SIGN'))
        parser.add_argument("--disable-compile",
                          action="store_true", dest="disable_compile", default=False,
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_DISABLE_COMPILE'))
        parser.add_argument("--bytecode-64bit",
                          action="store_true", dest="bytecode_64bit", default=False,
                          help=MultiLanguage.get_string('LUACOMPILE_ARG_BYTECODE_64BIT'))

        options = parser.parse_args(argv)

        if options.src_dir_arr == None:
            raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_SRC_NOT_SPECIFIED'),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)
        elif options.dst_dir == None:
            raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_DST_NOT_SPECIFIED'),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)
        else:
            for src_dir in options.src_dir_arr:
                if os.path.exists(src_dir) == False:
                    raise cocos.CCPluginError(MultiLanguage.get_string('LUACOMPILE_ERROR_DIR_NOT_EXISTED_FMT')
                                              % (src_dir), cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        # script directory
        if getattr(sys, 'frozen', None):
            workingdir = os.path.realpath(os.path.dirname(sys.executable))
        else:
            workingdir = os.path.realpath(os.path.dirname(__file__))

        self.init(options, workingdir)








