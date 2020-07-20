#!/usr/bin/python
#-*- coding: UTF-8 -*-
# ----------------------------------------------------------------------------
# Generate Cocos Simulator
#
# Copyright 2015 (C) zhangbin
#
# License: MIT
# ----------------------------------------------------------------------------

import os
import re
import shutil
import string
import sys

import cocos
from MultiLanguage import MultiLanguage
import utils

from datetime import date
from cocos import CCPluginError, Logging
from argparse import ArgumentParser

class SimulatorCompiler(cocos.CCPlugin):
    SIMULATOR_PROJ_PATH = 'tools/simulator'
    SIMULATOR_SLN_PATH = "frameworks/runtime-src/proj.win32/simulator.sln"
    SIMULATOR_XCODE_PATH = "frameworks/runtime-src/proj.ios_mac/simulator.xcodeproj"
    COCOS_CMD_PATH = 'tools/cocos2d-console/bin/cocos'

    DEFAULT_OUTPUT_FOLDER_NAME = 'simulator'

    @staticmethod
    def plugin_name():
      return "gen-simulator"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('GEN_SIM_BRIEF')

    def parse_args(self, argv):
        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())
        parser.add_argument('-c', '--clean', dest='do_clean', action='store_true',
                            help=MultiLanguage.get_string('GEN_SIM_ARG_CLEAN'))
        parser.add_argument('-e', dest='engine_path', help=MultiLanguage.get_string('GEN_SIM_ARG_ENGINE'))
        parser.add_argument('-m', "--mode", dest='compile_mode', type=str, choices=['debug', 'release'],
                            help=MultiLanguage.get_string('GEN_SIM_ARG_MODE'))
        parser.add_argument('-o', '--output', dest='out_dir',
                            help=MultiLanguage.get_string('GEN_SIM_ARG_OUTPUT'))
        parser.add_argument('-p', dest='platform', action="append", choices=['ios', 'mac', 'android', 'win32'],
                            help=MultiLanguage.get_string('GEN_SIM_ARG_PLATFORM'))
        parser.add_argument('--vs', dest='vs_version', type=int,
                            help=MultiLanguage.get_string('GEN_SIM_ARG_VS'))

        (args, unknown) = parser.parse_known_args()
        self.init(args)

        return args

    def init(self, args):
        if getattr(sys, 'frozen', None):
            self.cur_dir = os.path.realpath(os.path.dirname(sys.executable))
            self.default_engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir)
        else:
            self.cur_dir = os.path.realpath(os.path.dirname(__file__))
            self.default_engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir, os.pardir)
        self.default_engine_path = os.path.normpath(self.default_engine_path)

        if args.engine_path is None:
            self.engine_root = self.default_engine_path
        else:
            engine_path = os.path.expanduser(args.engine_path)
            if os.path.isabs(engine_path):
                self.engine_root = os.path.normpath(engine_path)
            else:
                self.engine_root = os.path.normpath(os.path.abspath(engine_path))

        if not os.path.isdir(self.engine_root):
            raise CCPluginError(MultiLanguage.get_string('GEN_SIM_ERROR_WRONG_PATH_FMT', self.engine_root),
                                CCPluginError.ERROR_WRONG_ARGS)

        self.simulator_abs_path = os.path.join(self.engine_root, SimulatorCompiler.SIMULATOR_PROJ_PATH)
        self.cocos_bin = os.path.join(self.engine_root, SimulatorCompiler.COCOS_CMD_PATH)
        engine_version = utils.get_engine_version(self.engine_root)
        # get the short version after "cocos2d-x-"
        self.engine_version = engine_version[10:]

        # get the full path of output dir.
        if args.out_dir is None:
            self.simulator_output_dir = os.path.join(self.engine_root, SimulatorCompiler.DEFAULT_OUTPUT_FOLDER_NAME)
        else:
            out_dir = os.path.expanduser(args.out_dir)
            if os.path.isabs(out_dir):
                self.simulator_output_dir = os.path.normpath(out_dir)
            else:
                self.simulator_output_dir = os.path.normpath(os.path.abspath(out_dir))

        # get arguments
        self.is_clean_before_build = args.do_clean
        if args.compile_mode is None:
            self.mode = 'debug'
        else:
            self.mode = args.compile_mode

        if args.platform is None:
            self.build_ios = True
            self.build_mac = True
            self.build_win = True
            self.build_android = True
        else:
            self.build_ios = False
            self.build_mac = False
            self.build_win = False
            self.build_android = False
            if 'win32' in args.platform:
                self.build_win = True
            if 'ios' in args.platform:
                self.build_ios = True
            if 'mac' in args.platform:
                self.build_mac = True
            if 'android' in args.platform:
                self.build_android = True

        self.build_log = ""
        self.vs_version = args.vs_version
        self._verbose = True

    def replace_keyword_with_content(self,content,old,new):
        found_string = string.find(content,old) >= 0
        return found_string, string.replace(content,old,new)

    def get_content_from_file(self,file_path):
        if not os.path.isfile(file_path):
            raise CCPluginError(MultiLanguage.get_string('GEN_SIM_ERROR_FILE_NOT_FOUND_FMT', file_path),
                                CCPluginError.ERROR_PATH_NOT_FOUND)

        with open(file_path) as f:
            return f.read()

    def write_content_to_file(self,content, file_path):
        if not os.path.isfile(file_path):
            raise CCPluginError(MultiLanguage.get_string('GEN_SIM_ERROR_FILE_NOT_FOUND_FMT', file_path),
                                CCPluginError.ERROR_PATH_NOT_FOUND)

        with open(file_path, 'w') as f:
            f.write(content)

    def replace_keyword_with_file(self,file_path,keyword_map):
        content = self.get_content_from_file(file_path)

        found = False
        for k in keyword_map:
            old, new = k, keyword_map[k]
            has_found, content = self.replace_keyword_with_content(content, old, new)
            if has_found :
                found = has_found

        if found == True:
            self.write_content_to_file(content,file_path)

    def get_keywords(self):
        osx_keyword = {
            "CC_TARGET_OS_IPHONE,":"CC_TARGET_OS_IPHONE,\n\"COCOS2D_DEBUG=1\",",
            "CC_TARGET_OS_MAC,":"CC_TARGET_OS_MAC,\n\"COCOS2D_DEBUG=1\",",
            "COCOS2D_DEBUG=0":"COCOS2D_DEBUG=1",
        }

        win_keyword = {
            "_WINDOWS":"_WINDOWS;COCOS2D_DEBUG=1",
        }

        if cocos.os_is_mac():
            return osx_keyword
        if cocos.os_is_win32():
            return win_keyword

        return {}

    def convert_path_to_win32(self,path):
        return path.replace("/","\\")

    def get_depend_project_file_list(self):
        file_list = []

        if cocos.os_is_mac() and (self.build_mac or self.build_ios):
            IOS_MAC_PROJECT_SUFFIX = "project.pbxproj"
            IOS_MAC_PROJECT_REFERENCES_TAG = 'ProjectRef ='
            IOS_MAC_PROJECT_NAME_RE = r'\w+.xcodeproj'
            IOS_MAC_PROJECT_PATH_RE = r'name = %s; path = (.)*.xcodeproj'

            project_file_path = os.path.join(self.simulator_abs_path,
                                             SimulatorCompiler.SIMULATOR_XCODE_PATH,
                                             IOS_MAC_PROJECT_SUFFIX)
            contents_str = self.get_content_from_file(project_file_path)
            lines = re.split(r'\n', contents_str)

            simulator_mac_project_path = os.path.dirname(os.path.dirname(project_file_path))
            project_references = []
            for l in lines:
                if IOS_MAC_PROJECT_REFERENCES_TAG in l:
                    ret = re.search(IOS_MAC_PROJECT_NAME_RE, l)
                    if ret: project_references.append(ret.group(0))

            for references in project_references:
                re_str = IOS_MAC_PROJECT_PATH_RE % references
                ret = re.search(re_str, contents_str)
                if ret:
                    match_str = ret.group(0)
                    match_str = match_str.replace("name = %s; path = " % references, "")
                    match_str = match_str.replace('"', "")
                    file_list.append(os.path.join(simulator_mac_project_path, match_str, IOS_MAC_PROJECT_SUFFIX))

        if cocos.os_is_win32() and self.build_android:
            WIN32_PROJECT_TAG = "Project(\""
            project_file_path = os.path.join(self.simulator_abs_path, SimulatorCompiler.SIMULATOR_SLN_PATH)
            simulator_win32_project_path = os.path.dirname(project_file_path)

            content_str = self.get_content_from_file(project_file_path)
            lines = content_str.split('\n')
            for l in lines:
                if l.startswith(WIN32_PROJECT_TAG):
                    ret = re.compile('"(.*?)"').findall(l.split(',')[1])
                    if ret:
                        path = self.convert_path_to_win32(os.path.join(simulator_win32_project_path, ret[0]))
                        file_list.append(path)

        return file_list

    def compile_for_osx(self):
        if self.is_clean_before_build:
            project_directory = os.path.join(self.simulator_abs_path, "frameworks/runtime-src/proj.ios_mac/")

            command = "xcodebuild -alltargets -configuration %s clean" % ("Debug" if self.mode == 'debug' else 'Release')
            self._run_cmd(command, project_directory)

        command = ' '.join([
            "mkdir -p %s" % (os.path.join(self.simulator_abs_path, "src")),
            " && %s compile -p mac -m %s -o \"%s\" --no-res --compile-script 0" % (self.cocos_bin
                , "debug" if self.mode == 'debug' else "release"
                , os.path.join(self.simulator_output_dir,"mac")),
            " && strip %s" % (os.path.join(self.simulator_output_dir,"mac/Simulator.app/Contents/MacOS/Simulator")),
            ])

        self._run_cmd(command, self.simulator_abs_path)
        self.build_log += MultiLanguage.get_string('GEN_SIM_BUILD_SUCCESS_FMT', ('Mac', self.mode))

    def compile_for_ios(self):
        if self.is_clean_before_build:
            project_directory = os.path.join(self.simulator_abs_path, "frameworks/runtime-src/proj.ios_mac/")

            command = "xcodebuild -alltargets -configuration %s clean" % ("Debug" if self.mode =='debug' else 'Release')
            self._run_cmd(command, project_directory)

        command = ' '.join([
            " %s compile -p ios -m %s -o \"%s\" --no-res --compile-script 0" % (self.cocos_bin
                , "debug" if self.mode == 'debug' else "release"
                , os.path.join(self.simulator_output_dir,"ios")),
            " && strip %s" % (os.path.join(self.simulator_output_dir,"ios","Simulator.app/Simulator")),
            " && rm -fr %s" % (os.path.join(self.simulator_output_dir,"ios","Simulator.app.dSYM")),
            ])

        self._run_cmd(command, self.simulator_abs_path)
        self.build_log += MultiLanguage.get_string('GEN_SIM_BUILD_SUCCESS_FMT', ('iOS', self.mode))

    def compile_for_win32(self):
        win32_output_dir = os.path.join(self.simulator_output_dir, "win32")
        win32_output_dir = self.convert_path_to_win32(win32_output_dir)
        if not os.path.isdir(win32_output_dir):
            os.makedirs(win32_output_dir)

        # get the vs version should be used
        if self.vs_version is None:
            ver_param = ''
        else:
            ver_param = '--vs %d' % self.vs_version

        if self.mode == 'debug':
            win32_src_dir = os.path.join(self.simulator_abs_path,"runtime/win32/")
            win32_src_dir = self.convert_path_to_win32(win32_src_dir)
            win32_dll_dir = self.convert_path_to_win32(os.path.join(os.path.dirname(self.cur_dir),"dll/"))
            command = ' '.join([
                " %s compile -p win32 -m debug --no-res --compile-script 0 %s" % (self.cocos_bin, ver_param),
                " && xcopy /Y %s*.dll %s" % (win32_src_dir, win32_output_dir),
                " && xcopy /Y %s*.exe %s" % (win32_src_dir, win32_output_dir),
                " && if exist %s*.dll xcopy /Y %s*.dll %s" % (win32_dll_dir,win32_dll_dir,win32_output_dir)
            ])
        else:
            command = " %s compile -p win32 -m release --no-res --compile-script 0 -o %s %s" % (self.cocos_bin,win32_output_dir,ver_param)

        self._run_cmd(command, self.simulator_abs_path)
        self.build_log += MultiLanguage.get_string('GEN_SIM_BUILD_SUCCESS_FMT', ('Win32', self.mode))

    def compile_for_android(self):
        rename_command = ' '.join([
                "mv %s %s" % (os.path.join(self.simulator_output_dir,"android","simulator-debug.apk"),
                              os.path.join(self.simulator_output_dir,"android","Simulator.apk"))
            ])

        command = ' '.join([
            " %s compile -p android --mode %s -o \"%s\" --no-res --compile-script 0" % (self.cocos_bin
                 , "debug" if self.mode == 'debug' else "release"
                 , os.path.join(self.simulator_output_dir,"android")),
            "&& %s" % (rename_command),
            ])

        self._run_cmd(command, self.simulator_abs_path)
        self.build_log += MultiLanguage.get_string('GEN_SIM_BUILD_SUCCESS_FMT', ('Android', self.mode))

    def do_compile(self):
        if cocos.os_is_mac():
            if self.build_mac:
                self.compile_for_osx()
            if self.build_ios:
                self.compile_for_ios()

        if cocos.os_is_win32():
            if self.build_win:
                self.compile_for_win32()

        if self.build_android:
            self.compile_for_android()

    def change_cocos2d_debug_macro_to_1(self, file_list):
        keyword = self.get_keywords()
        for file_path in file_list:
            self.replace_keyword_with_file(file_path, keyword)

    def update_bundle_version(self):
        build_date = date.today().strftime("%Y%m%d")

        if cocos.os_is_mac() and self.build_mac:
            # mac
            info_plist_path = os.path.join(self.simulator_abs_path, "frameworks/runtime-src/proj.ios_mac/mac/Info.plist")
            info_plist_content = self.get_content_from_file(info_plist_path)

            match = re.compile('<key>CFBundleVersion</key>(\s)*<string>(.*?)</string>').findall(info_plist_content)
            if len(match):
                build_date_tag = "<string>%s</string>" % match[0][1]
                keyword_map = { build_date_tag : "<string>%s</string>" % build_date }
                self.replace_keyword_with_file(info_plist_path, keyword_map)

            match = re.compile('<key>CFBundleShortVersionString</key>(\s)*<string>(.*?)</string>').findall(info_plist_content)
            if len(match):
                build_date_tag = "<string>%s</string>" % match[0][1]
                keyword_map = { build_date_tag : "<string>%s</string>" % self.engine_version }
                self.replace_keyword_with_file(info_plist_path, keyword_map)

        if cocos.os_is_win32() and self.build_win:
            # win32
            game_rc_path = os.path.join(self.simulator_abs_path,"frameworks/runtime-src/proj.win32/game.rc")
            game_rc_content = self.get_content_from_file(game_rc_path)
            match = re.compile('"Version[^\(]*\(.*\)"').findall(game_rc_content)
            if len(match):
                build_info_str = match[0]
                target_str = '"Version %s (%s)"' % (self.engine_version, build_date)
                keyword_map = { build_info_str : target_str}
                self.replace_keyword_with_file(game_rc_path,keyword_map)

    def backup_files(self, files):
        for f in files:
            full_path = os.path.abspath(f)
            if not os.path.isfile(full_path):
                continue

            backup_file_path = '%s.bak' % full_path
            shutil.copyfile(full_path, backup_file_path)

    def rollback_files(self, files):
        for f in files:
            full_path = os.path.abspath(f)
            backup_file_path = '%s.bak' % full_path
            if not os.path.isfile(backup_file_path):
                continue

            try:
                shutil.copyfile(backup_file_path, full_path)
                os.remove(backup_file_path)
            except:
                Logging.warning(MultiLanguage.get_string('GEN_SIM_ROLL_BACK_FAIL_FMT',
                                                         (full_path, backup_file_path, full_path)))
                pass

    def run(self, argv, dependencies):
        self.parse_args(argv)
        if self.is_clean_before_build:
            utils.rmdir(self.simulator_output_dir)

        # backup some files
        modify_files = self.get_depend_project_file_list()
        if cocos.os_is_mac() and self.build_mac:
            modify_files.append(os.path.join(self.simulator_abs_path, 'frameworks/runtime-src/proj.ios_mac/mac/Info.plist'))

        if cocos.os_is_win32() and self.build_win:
            modify_files.append(os.path.join(self.simulator_abs_path, 'frameworks/runtime-src/proj.win32/game.rc'))

        self.backup_files(modify_files)

        try:
            # modify bundle version
            self.update_bundle_version()

            # modify project config files
            self.change_cocos2d_debug_macro_to_1(modify_files)

            # compile simulator
            self.do_compile()
        except Exception as e:
            raise e
        finally:
            # roll back modified files
            self.rollback_files(modify_files)
            Logging.info("")
            Logging.info(self.build_log)
            Logging.info("")

        return 0
