#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos "install" plugin
#
# Authr: Luis Parravicini
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"run" plugin for cocos command line tool
'''

__docformat__ = 'restructuredtext'

import sys
import os
import cocos
from MultiLanguage import MultiLanguage
import BaseHTTPServer
import webbrowser
import threading
import subprocess
import re

class CCPluginRun(cocos.CCPlugin):
    """
    Compiles a project and runs it on the target
    """

    @staticmethod
    def depends_on():
        return ('deploy',)

    @staticmethod
    def plugin_name():
      return "run"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('RUN_BRIEF')

    def _add_custom_options(self, parser):
        parser.add_argument("-m", "--mode", dest="mode", default='debug',
                          help=MultiLanguage.get_string('RUN_ARG_MODE'))

        group = parser.add_argument_group(MultiLanguage.get_string('RUN_ARG_GROUP_WEB'))
        group.add_argument("-b", "--browser", dest="browser",
                          help=MultiLanguage.get_string('RUN_ARG_BROWSER'))
        group.add_argument("--param", dest="param",
                          help=MultiLanguage.get_string('RUN_ARG_PARAM'))
        group.add_argument("--port", dest="port", metavar="SERVER_PORT", nargs='?',
                          help=MultiLanguage.get_string('RUN_ARG_PORT'))
        group.add_argument("--host", dest="host", metavar="SERVER_HOST", nargs='?', default='127.0.0.1',
                          help=MultiLanguage.get_string('RUN_ARG_HOST'))
        group.add_argument("--no-console", action="store_true", dest="no_console", default=False,
                          help=MultiLanguage.get_string('RUN_ARG_NO_CONSOLE'))
        group.add_argument("--working-dir", dest="working_dir", default='',
                          help=MultiLanguage.get_string('RUN_ARG_WORKING_DIR'))

        group = parser.add_argument_group(MultiLanguage.get_string('RUN_ARG_GROUP_IOS'))
        group.add_argument("-sdk", dest="use_sdk", metavar="USE_SDK", nargs='?', default='iphonesimulator',
                          help=MultiLanguage.get_string('RUN_ARG_IOS_SDK'))

    def _check_custom_options(self, args):
        self._port = args.port
        self._mode = args.mode
        self._host = args.host
        self._browser = args.browser
        self._param = args.param
        self._no_console = args.no_console
        self._working_dir = args.working_dir

    def get_ios_sim_name(self):
        # get the version of xcodebuild
        ver = cocos.get_xcode_version()
        match = re.match(r'(\d+).*', ver)
        ret = None
        if match:
            ver_num = int(match.group(1))
            if ver_num <= 5:
                ret = "ios-sim-xcode5"
            elif ver_num < 8:
                ret = "ios-sim-xcode6"

        return ret

    def _get_cmd_output(self, cmds):
        child = subprocess.Popen(cmds, stdout=subprocess.PIPE)
        out = child.stdout.read()
        child.wait()
        errCode = child.returncode

        return (errCode, out)

    def _get_simulator_id(self):
        (errCode, out) = self._get_cmd_output([ "xcrun", "instruments", "-s" ])
        names = []
        if errCode == 0:
            pattern = r'(^iPhone[^\[]+)\[(.*)\]\s*\(Simulator\)'
            lines = out.split('\n')
            for line in lines:
                match = re.match(pattern, line)
                if match:
                    info = {
                        "name" : match.group(1),
                        'id' : match.group(2)
                    }
                    names.append(info)

        ret = None
        retName = None
        phoneTypeNum = 0
        phoneType = ''
        iosVer = 0
        if len(names) > 0:
            name_pattern = r'iPhone\s+((\d+)[^\(]+)\((.*)\)'
            for info in names:
                name = info["name"]
                id = info["id"]
                if name.find('Apple Watch') > 0:
                    continue

                match = re.match(name_pattern, name)
                if match:
                    # get the matched data
                    typeNum = int(match.group(2))
                    tmpType = match.group(1)
                    tmpIOSVer = match.group(3)

                    if ((typeNum > phoneTypeNum) or
                        (typeNum == phoneTypeNum and tmpType > phoneType) or
                        (typeNum == phoneTypeNum and tmpType == phoneType and cocos.version_compare(tmpIOSVer, '>', iosVer))):
                        # find the max phone type number first
                        ret = id
                        retName = name.strip()
                        phoneTypeNum = typeNum
                        phoneType = tmpType
                        iosVer = tmpIOSVer

        if ret is None:
            raise cocos.CCPluginError('Get simulator failed!')

        print('Using simulator: %s' % retName)
        return ret

    def _get_bundle_id(self, app_path):
        plistFile = os.path.join(app_path, 'Info.plist')
        (errCode, out) = self._get_cmd_output([ 'plutil', '-convert', 'json', '-o', '-', plistFile ])
        ret = None
        if errCode == 0:
            import json
            jsonObj = json.loads(out)
            if jsonObj is not None and jsonObj.has_key('CFBundleIdentifier'):
                ret = jsonObj['CFBundleIdentifier']

        if ret is None:
            raise cocos.CCPluginError('Get the bundle ID of app %s failed' % app_path)

        return ret

    def _run_ios_app(self, ios_app_path):
        if not cocos.os_is_mac():
            raise cocos.CCPluginError('Now only support run iOS simulator on Mac OS')

        # get bundle id
        bundle_id = self._get_bundle_id(ios_app_path)

        # find simulator
        simulator_id = self._get_simulator_id()

        try:
            # run the simulator
            xcode_version = cocos.get_xcode_version()
            xcode9_and_upper = cocos.version_compare(xcode_version,">=",9)
            if xcode9_and_upper:
                self._run_cmd('xcrun simctl boot "%s"' % simulator_id)
                self._run_cmd('open `xcode-select -p`/Applications/Simulator.app')
            else:
                self._run_cmd('xcrun instruments -w "%s"' % simulator_id)
        except Exception as e:
            pass

        # install app
        self._run_cmd('xcrun simctl install "%s" "%s"' % (simulator_id, ios_app_path))

        # run app
        self._run_cmd('xcrun simctl launch "%s" "%s"' % (simulator_id, bundle_id))

    def run_ios_sim(self, dependencies):
        if not self._platforms.is_ios_active():
            return

        deploy_dep = dependencies['deploy']
        if deploy_dep._use_sdk == 'iphoneos':
            cocos.Logging.warning(MultiLanguage.get_string('RUN_WARNING_IOS_FOR_DEVICE_FMT',
                                                           os.path.dirname(deploy_dep._iosapp_path)))
        else:
            ios_sim_name = self.get_ios_sim_name()
            if ios_sim_name is None:
                # there is not a ios-sim for current installed xcode
                # try to use xcrun commands
                self._run_ios_app(deploy_dep._iosapp_path)
            else:
                if getattr(sys, 'frozen', None):
                    cur_dir = os.path.realpath(os.path.dirname(sys.executable))
                else:
                    cur_dir = os.path.realpath(os.path.dirname(__file__))
                iossim_exe_path = os.path.join(cur_dir, 'bin', ios_sim_name)
                launch_sim = "%s launch \"%s\" &" % (iossim_exe_path, deploy_dep._iosapp_path)
                self._run_cmd(launch_sim)

    def run_ios_device(self):
        if not self._platforms.is_ios_active():
            return

        cocos.Logging.warning('Do not support running on iOS devices.')

    def _run_with_desktop_options(self, cmd):
        if self._no_console:
            cmd += ' -console no'
        if self._working_dir:
            cmd += ' -workdir "%s"' % self._working_dir
        self._run_cmd(cmd)

    def run_mac(self, dependencies):
        if not self._platforms.is_mac_active():
            return

        deploy_dep = dependencies['deploy']
        launch_macapp = '\"%s/Contents/MacOS/%s\"' % (deploy_dep._macapp_path, deploy_dep.target_name)
        self._run_with_desktop_options(launch_macapp)

    def run_android_device(self, dependencies):
        if not self._platforms.is_android_active():
            return

        sdk_root = cocos.check_environment_variable('ANDROID_SDK_ROOT')
        adb_path = cocos.CMDRunner.convert_path_to_cmd(os.path.join(sdk_root, 'platform-tools', 'adb'))
        deploy_dep = dependencies['deploy']
        startapp = "%s shell am start -n \"%s/%s\"" % (adb_path, deploy_dep.package, deploy_dep.activity)
        self._run_cmd(startapp)
        pass

    def open_webbrowser(self, url):
        if self._browser is None:
            threading.Event().wait(1)
            webbrowser.open_new(url)
        else:
            if cocos.os_is_mac():
                if self._param is None:
                    url_cmd = "open -a \"%s\" \"%s\"" % (self._browser, url)
                else:
                    url_cmd = "\"%s\" \"%s\" %s" % (self._browser, url, self._param)
            else:
                if self._param is None:
                    url_cmd = "\"%s\" %s" % (self._browser, url)
                else:
                    url_cmd = "\"%s\" \"%s\" %s" % (self._browser, url, self._param)
            self._run_cmd(url_cmd)

    def run_web(self, dependencies):
        if not self._platforms.is_web_active():
            return

        from SimpleHTTPServer import SimpleHTTPRequestHandler
        HandlerClass = SimpleHTTPRequestHandler
        ServerClass  = BaseHTTPServer.HTTPServer
        Protocol     = "HTTP/1.0"
        HandlerClass.protocol_version = Protocol

        host = self._host
        if self._port is None:
            port = 8000
            port_max_add = 2000
        else:
            port = int(self._port)
            port_max_add = 0

        deploy_dep = dependencies['deploy']
        run_root = deploy_dep.run_root

        i = 0
        httpd = None
        while (i <= port_max_add):
            port += i
            i += 1
            server_address = (host, port)
            try:
                cocos.Logging.info(MultiLanguage.get_string('RUN_INFO_HOST_PORT_FMT', (host, port)))
                httpd = ServerClass(server_address, HandlerClass)
            except Exception as e:
                httpd = None
                cocos.Logging.warning(MultiLanguage.get_string('RUN_WARNING_SERVER_FAILED_FMT', (host, port, e)))

            if httpd is not None:
                break

        if httpd is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('RUN_ERROR_START_SERVER_FAILED'),
                                      cocos.CCPluginError.ERROR_OTHERS)

        from threading import Thread
        sub_url = deploy_dep.sub_url
        url = 'http://%s:%s%s' % (host, port, sub_url)
        thread = Thread(target = self.open_webbrowser, args = (url,))
        thread.start()

        sa = httpd.socket.getsockname()
        with cocos.pushd(run_root):
            cocos.Logging.info(MultiLanguage.get_string('RUN_INFO_SERVING_FMT', (sa[0], sa[1])))
            httpd.serve_forever()

    def run_win32(self, dependencies):
        if not self._platforms.is_win32_active():
            return

        deploy_dep = dependencies['deploy']
        run_root = deploy_dep.run_root
        exe = deploy_dep.project_name
        with cocos.pushd(run_root):
            self._run_with_desktop_options(os.path.join(run_root, exe))

    def run_linux(self, dependencies):
        if not self._platforms.is_linux_active():
            return

        deploy_dep = dependencies['deploy']
        run_root = deploy_dep.run_root
        exe = deploy_dep.project_name
        with cocos.pushd(run_root):
            self._run_with_desktop_options(os.path.join(run_root, exe))

    def run_tizen(self, dependencies):
        if not self._platforms.is_tizen_active():
            return

        deploy_dep = dependencies['deploy']
        tizen_packageid = deploy_dep.tizen_packageid
        tizen_studio_path = cocos.check_environment_variable("TIZEN_STUDIO_HOME")
        tizen_cmd_path = cocos.CMDRunner.convert_path_to_cmd(os.path.join(tizen_studio_path, "tools", "ide", "bin", "tizen"))

        startapp = "%s run -p %s" % (tizen_cmd_path, tizen_packageid)
        self._run_cmd(startapp)


    def run(self, argv, dependencies):
        self.parse_args(argv)
        cocos.Logging.info(MultiLanguage.get_string('RUN_INFO_START_APP'))
        self.run_android_device(dependencies)
        self.run_ios_sim(dependencies)
        # self.run_ios_device()
        self.run_mac(dependencies)
        self.run_web(dependencies)
        self.run_win32(dependencies)
        self.run_linux(dependencies)
