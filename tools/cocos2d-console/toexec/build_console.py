#!/usr/bin/python
# ----------------------------------------------------------------------------
# build_console: Build cocos2d-console into executable binary file with PyInstaller
#
# Author: Bin Zhang
#
# License: MIT
# ----------------------------------------------------------------------------
'''
Build cocos2d-console into executable binary file with PyInstaller
'''

import os
import json
import subprocess
import excopy
import ConfigParser
import sys
import shutil

from argparse import ArgumentParser

def run_shell(cmd, cwd=None):
    p = subprocess.Popen(cmd, shell=True, cwd=cwd)
    p.wait()

    if p.returncode:
        raise subprocess.CalledProcessError(returncode=p.returncode, cmd=cmd)

    return p.returncode

def os_is_win32():
    return sys.platform == 'win32'

def os_is_linux():
    return 'linux' in sys.platform

class Builder(object):
    CONFIG_FILE = "config.json"

    KEY_COPY_CONFIG = "copy_config"
    KEY_MODIFY_CONFIG = "modify_config"
    KEY_HIDDEN_IMPORT = "hidden_import"

    ENTRANCE_FILE = "bin/cocos.py"

    CMD_FORMAT = 'pyinstaller -F %s %s --distpath "%s" --specpath "%s" --workpath "%s" --clean -y "%s"'

    def __init__(self, args):
        self.my_path = os.path.realpath(os.path.dirname(__file__))

        # get the source path
        if args.src_path is None:
            src_path = os.path.abspath(os.path.join(self.my_path, os.path.pardir))
        else:
            src_path = os.path.expanduser(args.src_path)

        if os.path.isabs(src_path):
            self.src_path = src_path
        else:
            self.src_path = os.path.abspath(src_path)

        if not os.path.isdir(self.src_path):
            raise Exception("%s is not a available path." % self.src_path)

        self.entrance_file = os.path.join(self.src_path, Builder.ENTRANCE_FILE)

        # get the dst path
        if args.dst_path is None:
            self.dst_path = os.path.abspath("output")
        else:
            dst_path = os.path.expanduser(args.dst_path)
            if os.path.isabs(dst_path):
                self.dst_path = dst_path
            else:
                self.dst_path = os.path.abspath(dst_path)
        if os_is_linux():
            self.dst_path = os.path.join(self.dst_path, "linux")
        else:
            self.dst_path = os.path.join(self.dst_path, sys.platform)

        # parse config file
        cfg_file = os.path.join(self.my_path, Builder.CONFIG_FILE)
        f = open(cfg_file)
        self.cfg_info = json.load(f)
        f.close()

    def _get_dirs(self, path, dir_list=None):
        if dir_list is None:
            dir_list = []

        if not os.path.isdir(path):
            return dir_list

        for name in os.listdir(path):
            full_path = os.path.join(path, name)
            if os.path.isdir(full_path):
                dir_list.append(full_path)
                self._get_dirs(full_path, dir_list)

        return dir_list

    def modify_files(self, modify_info):
        import re
        modify_file = modify_info["file_path"]
        if not os.path.isabs(modify_file):
            modify_file = os.path.abspath(os.path.join(self.dst_path, modify_file))

        if not os.path.isfile(modify_file):
            return

        pattern = modify_info["pattern"]
        replace_str = modify_info["replace_string"]

        f = open(modify_file)
        lines = f.readlines()
        f.close()

        new_lines = []
        for line in lines:
            new_line = re.sub(pattern, replace_str, line)
            new_lines.append(new_line)

        f = open(modify_file, "w")
        f.writelines(new_lines)
        f.close()

    def do_build(self):
        print("Source Path : %s" % self.src_path)
        print("Output Path : %s" % self.dst_path)
        print("Start building")

        if os.path.exists(self.dst_path):
            shutil.rmtree(self.dst_path)

        # copy files
        copy_config = self.cfg_info[Builder.KEY_COPY_CONFIG]
        copy_cfgs = copy_config["common"]
        if sys.platform in copy_config:
            copy_cfgs += copy_config[sys.platform]
        elif os_is_linux():
            copy_cfgs += copy_config["linux"]

        for element in copy_cfgs:
            excopy.copy_files_with_config(element, self.src_path, self.dst_path)

        # modify files
        modify_config = self.cfg_info[Builder.KEY_MODIFY_CONFIG]
        for element in modify_config:
            self.modify_files(element)

        # get the path parameter
        plugins_path = os.path.join(self.src_path, "plugins")
        bin_path = os.path.join(self.src_path, "bin")
        dir_list = self._get_dirs(plugins_path)
        dir_list.append(plugins_path)
        dir_list.append(bin_path)
        dir_list.append(self.src_path)

        if os_is_win32():
            sep = ";"
        else:
            sep = ":"
        path_param = "-p %s" % sep.join(dir_list)

        # get the runtime-hook parameter
        _cp = ConfigParser.ConfigParser(allow_no_value=True)
        _cp.optionxform = str
        _cp.read(os.path.join(self.src_path, "bin/cocos2d.ini"))

        runtime_hook_param = ""
        hidden_import_param = ""

        # add hidden import params for config.json
        if self.cfg_info.has_key(Builder.KEY_HIDDEN_IMPORT):
            hidden_import_cfg = self.cfg_info[Builder.KEY_HIDDEN_IMPORT]
        else:
            hidden_import_cfg = {}

        if len(hidden_import_cfg) > 0:
            for key in hidden_import_cfg:
                hidden_import_param += "--hidden-import %s " % key
                runtime_hook_param += '--runtime-hook "%s" ' % os.path.join(self.src_path, hidden_import_cfg[key])

        for s in _cp.sections():
            if s == 'plugins':
                for classname in _cp.options(s):
                    parts = classname.split(".")
                    module_name = parts[0]
                    hidden_import_param += "--hidden-import %s " % module_name

                    module_path = os.path.join(plugins_path, module_name)
                    if os.path.isdir(module_path):
                        runtime_hook_param += '--runtime-hook "%s" ' % ("%s/__init__.py" % module_path)
                    else:
                        module_file = "%s.py" % module_path
                        if os.path.isfile(module_file):
                            runtime_hook_param += '--runtime-hook "%s" ' % module_file

        # additional hooks path
        add_hook_dir_param = '--additional-hooks-dir "%s" ' % plugins_path
        add_hook_dir_param += '--additional-hooks-dir "%s" ' % bin_path
        add_hook_dir_param += '--additional-hooks-dir "%s"' % self.src_path

        # build *.py
        if os_is_linux():
            spec_path = os.path.join(self.my_path, "build", "linux")
        else:
            spec_path = os.path.join(self.my_path, "build", sys.platform)
        work_path = spec_path
        if os.path.exists(spec_path):
            shutil.rmtree(spec_path)
        build_cmd = Builder.CMD_FORMAT % (path_param, '%s %s %s' % (hidden_import_param, add_hook_dir_param, runtime_hook_param), self.dst_path, spec_path, work_path, self.entrance_file)
        run_shell(build_cmd)

        print("Building succeed.")


if __name__ == "__main__":
    parser = ArgumentParser(description="Generate executable file for cocos2d-console by PyInstaller.")
    parser.add_argument('-s', '--src-path', dest='src_path', help='Specify the path of cocos2d-console.')
    parser.add_argument('-d', '--dst-path', dest='dst_path', help='Specify the path of output.')
    (args, unknown) = parser.parse_known_args()

    if len(unknown) > 0:
        print("unknown arguments: %s" % unknown)

    builder = Builder(args)
    builder.do_build()
