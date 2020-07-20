#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos "install" plugin
#
# Copyright 2013 (C) Luis Parravicini
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"install" plugin for cocos command line tool
'''

__docformat__ = 'restructuredtext'

import os
import cocos
from MultiLanguage import MultiLanguage


class CCPluginDeploy(cocos.CCPlugin):
    """
    Install a project
    """

    @staticmethod
    def depends_on():
        return ('compile',)

    @staticmethod
    def plugin_name():
        return "deploy"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('DEPLOY_BRIEF')

    def _add_custom_options(self, parser):
        parser.add_argument("-m", "--mode", dest="mode", default='debug',
                          help=MultiLanguage.get_string('DEPLOY_ARG_MODE'))
        parser.add_argument("--no-uninstall", dest="no_uninstall", action="store_true",
                          help=MultiLanguage.get_string('DEPLOY_ARG_NO_UNINSTALL'))

    def _check_custom_options(self, args):

        if args.mode != 'release':
            args.mode = 'debug'

        self._mode = 'debug'
        if 'release' == args.mode:
            self._mode = args.mode

        self._no_uninstall = args.no_uninstall

    def _is_debug_mode(self):
        return self._mode == 'debug'

    def deploy_ios(self, dependencies):
        if not self._platforms.is_ios_active():
            return

        compile_dep = dependencies['compile']
        self._iosapp_path = compile_dep.app_path
        self._use_sdk = compile_dep._use_sdk

    def deploy_mac(self, dependencies):
        if not self._platforms.is_mac_active():
            return

        compile_dep = dependencies['compile']
        self._macapp_path = compile_dep.app_path
        self.target_name = compile_dep.app_name

    def deploy_web(self, dependencies):
        if not self._platforms.is_web_active():
            return

        compile_dep = dependencies['compile']
        self.sub_url = compile_dep.sub_url
        self.run_root = compile_dep.run_root

    def deploy_win32(self, dependencies):
        if not self._platforms.is_win32_active():
            return

        compile_dep = dependencies['compile']
        self.run_root = compile_dep.app_path
        self.project_name = compile_dep.project_name

    def find_xap_deploy_tool(self):
        import _winreg
        import re
        if cocos.os_is_32bit_windows():
            reg_flag_list = [ _winreg.KEY_WOW64_32KEY ]
        else:
            reg_flag_list = [ _winreg.KEY_WOW64_64KEY, _winreg.KEY_WOW64_32KEY ]

        pattern = re.compile(r"v(\d+).(\d+)")
        find_ret = None
        find_major = -1
        find_minor = -1
        for reg_flag in reg_flag_list:
            cocos.Logging.info(MultiLanguage.get_string('DEPLOY_INFO_FIND_XAP_FMT',
                                                        ("32bit" if reg_flag == _winreg.KEY_WOW64_32KEY else "64bit")))
            try:
                wp = _winreg.OpenKey(
                    _winreg.HKEY_LOCAL_MACHINE,
                    r"SOFTWARE\Microsoft\Microsoft SDKs\WindowsPhone",
                    0,
                    _winreg.KEY_READ | reg_flag
                )
            except:
                # windows phone not found, continue
                continue

            i = 0
            while True:
                try:
                    version = _winreg.EnumKey(wp, i)
                except:
                    break

                i += 1
                match = re.match(pattern, version)
                if match:
                    major = int(match.group(1))
                    minor = int(match.group(2))
                    if major > 7:
                        try:
                            key = _winreg.OpenKey(wp, "%s\Install Path" % version)
                            value, type = _winreg.QueryValueEx(key, "Install Path")
                            tool_path = os.path.join(value, "Tools", "XAP Deployment", "XapDeployCmd.exe")
                            if os.path.isfile(tool_path):
                                if (find_ret is None) or (major > find_major) or (major == find_major and minor > find_minor):
                                    find_ret = tool_path
                                    find_major = major
                                    find_minor = minor
                        except:
                            pass

        return find_ret

    def deploy_linux(self, dependencies):
        if not self._platforms.is_linux_active():
            return

        compile_dep = dependencies['compile']
        self.run_root = compile_dep.run_root
        self.project_name = compile_dep.project_name

    def deploy_android(self, dependencies):
        if not self._platforms.is_android_active():
            return

        cocos.Logging.info(MultiLanguage.get_string('DEPLOY_INFO_INSTALLING_APK'))

        compile_dep = dependencies['compile']
        self.package = compile_dep.android_package
        self.activity = compile_dep.android_activity
        apk_path = compile_dep.apk_path
        sdk_root = cocos.check_environment_variable('ANDROID_SDK_ROOT')
        adb_path = cocos.CMDRunner.convert_path_to_cmd(os.path.join(sdk_root, 'platform-tools', 'adb'))

        if not self._no_uninstall:
            # do uninstall only when that app is installed
            if cocos.app_is_installed(adb_path, self.package):
                adb_uninstall = "%s uninstall %s" % (adb_path, self.package)
                self._run_cmd(adb_uninstall)

        adb_install = "%s install -r \"%s\"" % (adb_path, apk_path)
        self._run_cmd(adb_install)

    def get_filename_by_extention(self, ext, path):
        filelist = os.listdir(path)

        for fname in filelist:
            name, extention = os.path.splitext(fname)
            if extention == ext:
                return  fname
        return None

    def run(self, argv, dependencies):
        self.parse_args(argv)
        cocos.Logging.info(MultiLanguage.get_string('DEPLOY_INFO_MODE_FMT', self._mode))
        self.deploy_ios(dependencies)
        self.deploy_mac(dependencies)
        self.deploy_android(dependencies)
        self.deploy_web(dependencies)
        self.deploy_win32(dependencies)
        self.deploy_linux(dependencies)
