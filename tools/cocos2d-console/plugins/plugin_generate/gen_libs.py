#!/usr/bin/python
#-*- coding: UTF-8 -*-

import os
import sys
import shutil
import json
import utils
import gen_prebuilt_mk

import cocos
from MultiLanguage import MultiLanguage

from cocos import CCPluginError
from cocos import Logging
from argparse import ArgumentParser

class LibsCompiler(cocos.CCPlugin):
    CFG_FILE = 'configs/gen_libs_config.json'

    KEY_LIBS_OUTPUT = 'libs_output_dir'
    KEY_XCODE_PROJS_INFO = 'xcode_projs_info'
    KEY_VS_PROJS_INFO = 'vs_projs_info'
    KEY_SUPPORT_VS_VERSIONS = 'support_vs_versions'
    KEY_ANDROID_MKS = "android_mks"
    CHECK_KEYS = [
        KEY_LIBS_OUTPUT,
        KEY_XCODE_PROJS_INFO,
        KEY_VS_PROJS_INFO,
        KEY_SUPPORT_VS_VERSIONS,
        KEY_ANDROID_MKS
    ]

    KEY_XCODE_TARGETS = 'targets'
    KEY_VS_BUILD_TARGETS = 'build_targets'

    @staticmethod
    def plugin_name():
      return "gen-libs"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('GEN_LIBS_BRIEF')

    def parse_args(self, argv):
        """Custom and check param list.
        """
        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())
        parser.add_argument('-c', dest='clean', action="store_true",
                            help=MultiLanguage.get_string('GEN_LIBS_ARG_CLEAN'))
        parser.add_argument('-e', dest='engine_path', help=MultiLanguage.get_string('GEN_LIBS_ARG_ENGINE'))
        parser.add_argument('-p', dest='platform', action="append", choices=['ios', 'mac', 'android', 'win32'],
                            help=MultiLanguage.get_string('GEN_LIBS_ARG_PLATFORM'))
        parser.add_argument('-m', "--mode", dest='compile_mode', default='debug', choices=['debug', 'release'],
                            help=MultiLanguage.get_string('GEN_LIBS_ARG_MODE'))
        parser.add_argument('--dis-strip', dest='disable_strip', action="store_true",
                            help=MultiLanguage.get_string('GEN_LIBS_ARG_DISABLE_STRIP'))
        group = parser.add_argument_group(MultiLanguage.get_string('GEN_LIBS_GROUP_WIN'))
        group.add_argument('--vs', dest='vs_version', type=int, default=None,
                           help=MultiLanguage.get_string('GEN_LIBS_ARG_VS'))
        group = parser.add_argument_group(MultiLanguage.get_string('GEN_LIBS_GROUP_ANDROID'))
        group.add_argument("--app-abi", dest="app_abi",
                            help=MultiLanguage.get_string('GEN_LIBS_ARG_ABI'))
        group.add_argument("--ap", dest="android_platform",
                            help=MultiLanguage.get_string('COMPILE_ARG_AP'))
        group.add_argument('-l', dest='language', 
                            choices=['cpp', 'lua', 'js'],
                            default='cpp',
                            help='set project type to build')

        (args, unknown) = parser.parse_known_args(argv)
        self.init(args)

        return args

    def run(self, argv, dependencies):
        self.parse_args(argv)
        self.compile()

    def init(self, args):
        if getattr(sys, 'frozen', None):
            self.cur_dir = os.path.realpath(os.path.dirname(sys.executable))
            self.default_engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir)
        else:
            self.cur_dir = os.path.realpath(os.path.dirname(__file__))
            self.default_engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir, os.pardir)
        self.default_engine_path = os.path.normpath(self.default_engine_path)

        if args.engine_path is None:
            self.repo_x = self.default_engine_path
        else:
            engine_path = os.path.expanduser(args.engine_path)
            if os.path.isabs(engine_path):
                self.repo_x = os.path.normpath(engine_path)
            else:
                self.repo_x = os.path.normpath(os.path.abspath(engine_path))

        if not os.path.isdir(self.repo_x):
            raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_WRONG_PATH_FMT', self.repo_x),
                                CCPluginError.ERROR_WRONG_ARGS)

        self.cfg_file_path = os.path.join(self.cur_dir, LibsCompiler.CFG_FILE)
        self.parse_config()

        # arguments check and set
        self.clean = args.clean
        self.mode = args.compile_mode
        self._verbose = True
        self.language = args.language

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

        self.disable_strip = args.disable_strip
        self.vs_version = args.vs_version
        self.use_incredibuild = False
        if args.app_abi is None:
            self.app_abi = 'armeabi-v7a'
        else:
            self.app_abi = args.app_abi
        self.app_abi_list = self.app_abi.split(":")
        self.android_platform = args.android_platform

        self.lib_dir = os.path.normpath(os.path.join(self.repo_x, self.cfg_info[LibsCompiler.KEY_LIBS_OUTPUT]))

    def parse_config(self):
        if not os.path.isfile(self.cfg_file_path):
            raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_WRONG_FILE_FMT', self.cfg_file_path),
                                CCPluginError.ERROR_PATH_NOT_FOUND)

        try:
            f = open(self.cfg_file_path)
            self.cfg_info = json.load(f)
            f.close()
        except:
            raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_PARSE_FILE_FMT', self.cfg_file_path),
                                CCPluginError.ERROR_PARSE_FILE)

        for k in LibsCompiler.CHECK_KEYS:
            if k not in self.cfg_info.keys():
                raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_KEY_NOT_FOUND_FMT', (k, self.cfg_file_path)),
                                    CCPluginError.ERROR_WRONG_CONFIG)

    def get_cfg_info(self):
        return self.cfg_info

    def compile(self):
        if self.clean:
            self.clean_libs()

        if cocos.os_is_mac():
            if self.build_mac or self.build_ios:
                self.compile_mac_ios()

        if cocos.os_is_win32():
            if self.build_win:
                self.compile_win()

        if self.build_android:
            self.compile_android()
            # generate prebuilt mk files
            # self.modify_binary_mk()

    def build_win32_proj(self, cmd_path, sln_path, proj_name, mode):
        build_cmd = " ".join([
            "\"%s\"" % cmd_path,
            "\"%s\"" % sln_path,
            "/t:%s" % proj_name,
            "/property:Configuration=%s" % mode,
            "/m"
        ])
        self._run_cmd(build_cmd)

    def compile_win(self):
        if self.mode == 'debug':
            mode_str = 'Debug'
        else:
            mode_str = 'Release'

        # get the VS versions will be used for compiling
        support_vs_versions = self.cfg_info[LibsCompiler.KEY_SUPPORT_VS_VERSIONS]
        compile_vs_versions = support_vs_versions
        if self.vs_version is not None:
            if self.vs_version not in support_vs_versions:
                raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_NOT_SUPPORT_VS_FMT', self.vs_version),
                                    CCPluginError.ERROR_WRONG_ARGS)
            else:
                compile_vs_versions = [ self.vs_version ]

        vs_cmd_info = {}
        for vs_version in compile_vs_versions:
            # get the vs command with specified version
            vs_command = utils.get_msbuild_path(vs_version)
            if vs_command is None:
                Logging.warning(MultiLanguage.get_string('GEN_LIBS_WARNING_VS_NOT_FOUND_FMT', vs_version))
            else:
                vs_cmd_info[vs_version] = vs_command

        if len(vs_cmd_info) == 0:
            raise CCPluginError(MultiLanguage.get_string('GEN_LIBS_ERROR_VS_NOT_FOUND'),
                                CCPluginError.ERROR_TOOLS_NOT_FOUND)

        cocos2d_proj_file = os.path.join(self.repo_x, 'cocos/2d/libcocos2d.vcxproj')

        # get the VS projects info
        win32_proj_info = self.cfg_info[LibsCompiler.KEY_VS_PROJS_INFO]
        proj_path = win32_proj_info['proj_path']
        for vs_version in compile_vs_versions:
            if not vs_version in vs_cmd_info.keys():
                continue

            try:
                vs_command = vs_cmd_info[vs_version]
                # clean solutions
                full_proj_path = os.path.join(self.repo_x, proj_path)
                clean_cmd = " ".join([
                    "\"%s\"" % vs_command,
                    "\"%s\"" % full_proj_path,
                    "/t:Clean /p:Configuration=%s" % mode_str
                ])
                self._run_cmd(clean_cmd)

                output_dir = os.path.join(self.lib_dir, "win32")

                # get the build folder & win32 output folder
                build_folder_path = os.path.join(os.path.dirname(proj_path), "%s.win32" % mode_str)
                win32_output_dir = os.path.join(self.repo_x, output_dir)
                if not os.path.exists(win32_output_dir):
                    os.makedirs(win32_output_dir)

                # build project
                if self.use_incredibuild:
                    # use incredibuild, build whole sln
                    build_cmd = " ".join([
                        "BuildConsole",
                        "%s" % proj_path,
                        "/build",
                        "/cfg=\"%s|Win32\"" % mode_str
                    ])
                    self._run_cmd(build_cmd)
                else:
                    for proj_name in win32_proj_info[self.language][LibsCompiler.KEY_VS_BUILD_TARGETS]:
                        # build the projects
                        self.build_win32_proj(vs_command, proj_path, proj_name, mode_str)

                # copy the libs into prebuilt dir
                for file_name in os.listdir(build_folder_path):
                    name, ext = os.path.splitext(file_name)
                    if ext != ".lib" and ext != ".dll":
                        continue

                    file_path = os.path.join(build_folder_path, file_name)
                    shutil.copy(file_path, win32_output_dir)

            except Exception as e:
                raise e

    def compile_mac_ios(self):
        xcode_proj_info = self.cfg_info[LibsCompiler.KEY_XCODE_PROJS_INFO]
        if self.mode == 'debug':
            mode_str = 'Debug'
        else:
            mode_str = 'Release'

        XCODE_CMD_FMT = "xcodebuild -project \"%s\" -configuration %s -target \"%s\" %s CONFIGURATION_BUILD_DIR=%s"
        ios_out_dir = os.path.join(self.lib_dir, "ios")
        mac_out_dir = os.path.join(self.lib_dir, "mac")
        ios_sim_libs_dir = os.path.join(ios_out_dir, "simulator")
        ios_dev_libs_dir = os.path.join(ios_out_dir, "device")
        cocos_cmd = self._get_cocos_cmd_path()

        if self.language == 'cpp':
            build_types = ['cpp']
        if self.language == 'lua':
            build_types = ['cpp', 'lua']
        if self.language == 'js':
            build_types = ['cpp', 'js']

        for key in build_types:
            proj_info = xcode_proj_info[key]
            proj_path = os.path.join(self.repo_x, proj_info['proj_path'])
            target = proj_info['targets']

            if self.build_mac:
                # compile mac
                build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, "%s Mac" % target, "", mac_out_dir)
                self._run_cmd(build_cmd)

            if self.build_ios:
                # compile ios simulator
                build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, "%s iOS" % target, "-sdk iphonesimulator ARCHS=\"i386 x86_64\" VALID_ARCHS=\"i386 x86_64\"", ios_sim_libs_dir)
                self._run_cmd(build_cmd)

                # compile ios device
                build_cmd = XCODE_CMD_FMT % (proj_path, mode_str, "%s iOS" % target, "-sdk iphoneos", ios_dev_libs_dir)
                self._run_cmd(build_cmd)

            if self.build_ios:
                # generate fat libs for iOS
                for lib in os.listdir(ios_sim_libs_dir):
                    sim_lib = os.path.join(ios_sim_libs_dir, lib)
                    dev_lib = os.path.join(ios_dev_libs_dir, lib)
                    output_lib = os.path.join(ios_out_dir, lib)
                    lipo_cmd = "lipo -create -output \"%s\" \"%s\" \"%s\"" % (output_lib, sim_lib, dev_lib)

                    self._run_cmd(lipo_cmd)

            # remove the simulator & device libs in iOS
            utils.rmdir(ios_sim_libs_dir)
            utils.rmdir(ios_dev_libs_dir)

        if not self.disable_strip:
            # strip the libs
            if self.build_ios:
                ios_strip_cmd = "xcrun -sdk iphoneos strip -S %s/*.a" % ios_out_dir
                self._run_cmd(ios_strip_cmd)
            if self.build_mac:
                mac_strip_cmd = "xcrun strip -S %s/*.a" % mac_out_dir
                self._run_cmd(mac_strip_cmd)

    def compile_android(self):
        # build .so for android
        cmd_path = self._get_cocos_cmd_path()
        engine_dir = self.repo_x

        # build the simulator project
        proj_path = os.path.join(engine_dir, 'tests/cpp-empty-test')
        if self.language == 'lua':
            proj_path = os.path.join(engine_dir, 'tests/lua-empty-test')
        elif self.language == 'js':
            proj_path = os.path.join(engine_dir, 'tests/js-tests')

        for app_abi_item in self.app_abi_list:
            build_cmd = "%s compile -s %s -p android --no-sign --mode %s --app-abi %s" % (cmd_path, proj_path, self.mode, app_abi_item)
            if self.android_platform is not None:
                build_cmd += ' --ap %s' % self.android_platform
            self._run_cmd(build_cmd)

            # copy .a to prebuilt dir
            ANDROID_A_PATH = "proj.android/app/build/intermediates/ndkBuild/%s/obj/local/%s" % (self.mode, app_abi_item)
            if self.language != 'cpp':
                ANDROID_A_PATH = 'project/' + ANDROID_A_PATH

            android_out_dir = os.path.join(self.lib_dir, "android", app_abi_item)
            obj_dir = os.path.join(proj_path, ANDROID_A_PATH)
            copy_cfg = {
                "from": obj_dir,
                "to": android_out_dir,
                "include": [
                    "*.a$"
                ]
            }
            cocos.copy_files_with_config(copy_cfg, obj_dir, android_out_dir)

        if not self.disable_strip:
            # strip the android libs
            ndk_root = os.environ["NDK_ROOT"]
            if cocos.os_is_win32():
                if cocos.os_is_32bit_windows():
                    check_bits = [ "", "-x86_64" ]
                else:
                    check_bits = [ "-x86_64", "" ]

                sys_folder_name = "windows"
                for bit_str in check_bits:
                    check_folder_name = "windows%s" % bit_str
                    check_path = os.path.join(ndk_root, "toolchains/arm-linux-androideabi-4.9/prebuilt/%s" % check_folder_name)
                    if os.path.isdir(check_path):
                        sys_folder_name = check_folder_name
                        break
            elif cocos.os_is_mac():
                sys_folder_name = "darwin-x86_64"
            else:
                sys_folder_name = "linux-x86_64"

            # set strip execute file name
            if cocos.os_is_win32():
                strip_execute_name = "strip.exe"
            else:
                strip_execute_name = "strip"

            # strip arm libs
            strip_cmd_path = os.path.join(ndk_root, "toolchains/arm-linux-androideabi-4.9/prebuilt/%s/arm-linux-androideabi/bin/%s"
                % (sys_folder_name, strip_execute_name))
            if os.path.exists(strip_cmd_path):
                self.trip_libs(strip_cmd_path, os.path.join(android_out_dir, "armeabi-v7a"))
                    

            # strip arm64-v8a libs
            strip_cmd_path = os.path.join(ndk_root, "toolchains/aarch64-linux-android-4.9/prebuilt/%s/aarch64-linux-android/bin/%s" % (sys_folder_name, strip_execute_name))
            if os.path.exists(strip_cmd_path) and os.path.exists(os.path.join(android_out_dir, "arm64-v8a")):
                self.trip_libs(strip_cmd_path, os.path.join(android_out_dir, 'arm64-v8a'))

            # strip x86 libs
            strip_cmd_path = os.path.join(ndk_root, "toolchains/x86-4.8/prebuilt/%s/i686-linux-android/bin/%s" % (sys_folder_name, strip_execute_name))
            if os.path.exists(strip_cmd_path) and os.path.exists(os.path.join(android_out_dir, "x86")):
                self.trip_libs(strip_cmd_path, os.path.join(android_out_dir, 'x86'))

    def _get_cocos_cmd_path(self):
        CONSOLE_PATH = "tools/cocos2d-console/bin"

        engine_dir = self.repo_x
        console_dir = os.path.join(engine_dir, CONSOLE_PATH)
        if cocos.os_is_win32():
            cmd_path = os.path.join(console_dir, "cocos.bat")
        else:
            cmd_path = os.path.join(console_dir, "cocos")

        return cmd_path

    def trip_libs(self, strip_cmd, folder):
        if not os.path.isdir(folder):
            return

        if cocos.os_is_win32():
            for name in os.listdir(folder):
                basename, ext = os.path.splitext(name)
                if ext == ".a":
                    full_name = os.path.join(folder, name)
                    command = "%s -S %s" % (strip_cmd, full_name)
                    self._run_cmd(command)
        else:
            strip_cmd = "%s -S %s/*.a" % (strip_cmd, folder)
            self._run_cmd(strip_cmd)

    def modify_binary_mk(self):
        android_libs = os.path.join(self.lib_dir, "android")
        android_mks = self.cfg_info[LibsCompiler.KEY_ANDROID_MKS]
        for mk_file in android_mks:
            mk_file_path = os.path.normpath(os.path.join(self.repo_x, mk_file))
            if not os.path.isfile(mk_file_path):
                Logging.warning(MultiLanguage.get_string('COMPILE_ERROR_GRALEW_NOT_EXIST_FMT', mk_file_path))
                continue

            dst_file_path = os.path.join(os.path.dirname(mk_file_path), "prebuilt-mk", os.path.basename(mk_file_path))
            tmp_obj = gen_prebuilt_mk.MKGenerator(mk_file_path, android_libs, dst_file_path)
            tmp_obj.do_generate()

    def clean_libs(self):
        utils.rmdir(self.lib_dir)
