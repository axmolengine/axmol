#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos-console: command line tool manager for cocos2d-x
#
# Author: Ricardo Quesada
# Copyright 2013 (C) Zynga, Inc
#
# License: MIT
# ----------------------------------------------------------------------------
'''
Command line tool manager for cocos
'''

__docformat__ = 'restructuredtext'


# python
import sys
import os
import subprocess
from contextlib import contextmanager
import cocos_project
import shutil
import string
import locale
import gettext
import json
import utils
import re


# FIXME: MultiLanguage should be deprecated in favor of gettext
from MultiLanguage import MultiLanguage

COCOS2D_CONSOLE_VERSION = '2.3'


class Cocos2dIniParser:
    def __init__(self):
        import ConfigParser
        self._cp = ConfigParser.ConfigParser(allow_no_value=True)
        self._cp.optionxform = str

        # read global config file
        self.cocos2d_path = os.path.dirname(os.path.abspath(sys.argv[0]))
        self._cp.read(os.path.join(self.cocos2d_path, "cocos2d.ini"))

        # XXX: override with local config ??? why ???
        self._cp.read("~/.cocos2d-js/cocos2d.ini")

    def parse_plugins(self):
        classes = {}

        for s in self._cp.sections():
            if s == 'plugins':
                for classname in self._cp.options(s):
                    plugin_class = get_class(classname)
                    category = plugin_class.plugin_category()
                    name = plugin_class.plugin_name()
                    if name is None:
                        print(MultiLanguage.get_string('COCOS_PARSE_PLUGIN_WARNING_FMT', classname))
                    if len(category) == 0:
                        key = name
                    else:
                        # combine category & name as key
                        # eg. 'project_new'
                        key = category + '_' + name
                    classes[key] = plugin_class
        _check_dependencies(classes)
        return classes

    def _sanitize_path(self, path):
        if len(path) == 0:
            return None
        path = os.path.expanduser(path)
        path = os.path.abspath(os.path.join(self.cocos2d_path, path))
        if not os.path.isdir(path):
            Logging.warning(MultiLanguage.get_string('COCOS_WARNING_INVALID_DIR_IN_INI_FMT', path))
            return None
        return path

    def get_plugins_path(self):
        path = self._cp.get('paths', 'plugins')

        if not os.path.isabs(path):
            path = os.path.join(get_current_path(), path)

        path = self._sanitize_path(path)
        return path

    def get_cocos2dx_path(self):
        cocos2d_x = self._cp.get('paths', 'cocos2d_x')
        cocos2d_x = self._sanitize_path(cocos2d_x)
        return cocos2d_x

    def get_templates_path(self):
        templates = self._cp.get('paths', 'templates')
        templates = self._sanitize_path(templates)
        return templates

    def get_cocos2dx_mode(self):
        mode = self._cp.get('global', 'cocos2d_x_mode')
        if mode is None or len(mode) == 0:
            mode = 'source'

        if mode not in ('source', 'precompiled', 'distro'):
            Logging.warning(MultiLanguage.get_string('COCOS_WARNING_INVALID_MODE_FMT', mode))
            mode = 'source'

        return mode

    def is_statistic_enabled(self):
        try:
            ret = self._cp.getboolean('global', 'enable_stat')
        except:
            ret = True

        return ret


class Logging:
    # TODO maybe the right way to do this is to use something like colorama?
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    MAGENTA = '\033[35m'
    RESET = '\033[0m'

    @staticmethod
    def _print(s, color=None):
        if color and sys.stdout.isatty() and sys.platform != 'win32':
            print(color + s + Logging.RESET)
        else:
            print(s)

    @staticmethod
    def debug(s):
        Logging._print(s, Logging.MAGENTA)

    @staticmethod
    def info(s):
        Logging._print(s, Logging.GREEN)

    @staticmethod
    def warning(s):
        Logging._print(s, Logging.YELLOW)

    @staticmethod
    def error(s):
        Logging._print(s, Logging.RED)


class CCPluginError(Exception):
    ERROR_WRONG_ARGS = 11           # wrong arguments
    ERROR_PATH_NOT_FOUND = 12       # path not found
    ERROR_BUILD_FAILED = 13         # build failed
    ERROR_RUNNING_CMD = 14          # error when running command
    ERROR_CMD_NOT_FOUND = 15        # command not found
    ERROR_ENV_VAR_NOT_FOUND = 16    # environment variable not found
    ERROR_TOOLS_NOT_FOUND = 17      # depend on tools not found
    ERROR_PARSE_FILE = 18           # error when parse files
    ERROR_WRONG_CONFIG = 19         # configuration is wrong

    ERROR_OTHERS = 101              # other errors

    def __init__(self, err_args, err_no=1):
        super(CCPluginError, self).__init__(err_args)
        self.error_no = err_no

    def get_error_no(self):
        return self.error_no


class CMDRunner(object):

    @staticmethod
    def run_cmd(command, verbose, cwd=None):
        if verbose:
            Logging.debug(MultiLanguage.get_string('COCOS_DEBUG_RUNNING_CMD_FMT', ''.join(command)))
        else:
            log_path = CCPlugin._log_path()
            command += ' >"%s" 2>&1' % log_path
        sys.stdout.flush()
        ret = subprocess.call(command, shell=True, cwd=cwd)
        if ret != 0:
            message = MultiLanguage.get_string('COCOS_ERROR_RUNNING_CMD_RET_FMT', str(ret))
            if not verbose:
                message += (MultiLanguage.get_string('COCOS_ERROR_CHECK_LOG_FMT', log_path))
            raise CCPluginError(message, CCPluginError.ERROR_RUNNING_CMD)

    @staticmethod
    def output_for(command, verbose):
        if verbose:
            Logging.debug(MultiLanguage.get_string('COCOS_DEBUG_RUNNING_CMD_FMT', command))
        else:
            log_path = CCPlugin._log_path()

        try:
            return subprocess.check_output(command, stderr=subprocess.STDOUT, shell=True)
        except subprocess.CalledProcessError as e:
            output = e.output
            message = MultiLanguage.get_string('COCOS_ERROR_RUNNING_CMD')

            if not verbose:
                with open(log_path, 'w') as f:
                    f.write(output)
                message += MultiLanguage.get_string('COCOS_ERROR_CHECK_LOG_FMT', log_path)
            else:
                Logging.error(output)

            raise CCPluginError(message, CCPluginError.ERROR_RUNNING_CMD)

    @staticmethod
    def convert_path_to_cmd(path):
        """ Escape paths which include spaces to correct style which bash(mac) and cmd(windows) can treat correctly.

            eg: on mac: convert '/usr/xxx/apache-ant 1.9.3' to '/usr/xxx/apache-ant\ 1.9.3'
            eg: on windows: convert '"c:\apache-ant 1.9.3"\bin' to '"c:\apache-ant 1.9.3\bin"'
        """
        ret = path
        if os_is_mac():
            ret = path.replace("\ ", " ").replace(" ", "\ ")

        if os_is_win32():
            ret = "\"%s\"" % (path.replace("\"", ""))

        # print("!!!!! Convert %s to %s\n" % (path, ret))
        return ret

    @staticmethod
    def convert_path_to_python(path):
        """ Escape paths which include spaces to correct style which python can treat correctly.

            eg: on mac: convert '/usr/xxx/apache-ant\ 1.9.3' to '/usr/xxx/apache-ant 1.9.3'
            eg: on windows: convert '"c:\apache-ant 1.9.3"\bin' to 'c:\apache-ant 1.9.3\bin'
        """
        ret = path
        if os_is_mac():
            ret = path.replace("\ ", " ")

        if os_is_win32():
            ret = ret.replace("\"", "")

        # print("!!!!! Convert %s to %s\n" % (path, ret))
        return ret


class DataStatistic(object):
    '''
    In order to improve cocos, we periodically send anonymous data about how you use cocos.
    You can turn off this function by change the value of "enable_stat" in cocos2d.ini.

    Information collected will be used to develop new features and improve cocos.

    Since no personally identifiable information is collected,
    the anonymous data will not be meaningful to anyone outside of Chukong Inc.
    '''
    inited = False
    stat_obj = None
    key_last_state = 'last_stat_enabled'
    key_agreement_shown = 'agreement_shown'

    @classmethod
    def get_cfg_file_path(cls):
        return os.path.join(os.path.expanduser('~/.cocos'), 'local_cfg.json')

    @classmethod
    def get_cfg_value(cls, key, default_value):
        local_cfg_file = cls.get_cfg_file_path()
        if not os.path.isfile(local_cfg_file):
            cur_info = None
        else:
            try:
                f = open(local_cfg_file)
                cur_info = json.load(f)
                f.close()
            except:
                cur_info = None

        ret = default_value
        if cur_info is not None:
            if key in cur_info:
                ret = cur_info[key]

        return ret

    @classmethod
    def set_cfg_value(cls, key, value):
        # get current local config info
        cfg_file = cls.get_cfg_file_path()
        if not os.path.isfile(cfg_file):
            cur_info = {}
        else:
            try:
                f = open(cfg_file)
                cur_info = json.load(f)
                f.close()
            except:
                cur_info = {}

        # set the value in config
        cur_info[key] = value

        # make config directory if it's not already there
        cfg_dir = os.path.dirname(cfg_file)
        if not os.path.exists(cfg_dir):
            os.makedirs(cfg_dir)

        # write the config
        f = open(cfg_file, 'w')
        json.dump(cur_info, f, sort_keys=True, indent=4)
        f.close()

    # get the stat agreed or not
    @classmethod
    def is_agreement_shown(cls):
        return cls.get_cfg_value(cls.key_agreement_shown, False)

    @classmethod
    def change_agree_stat(cls, agreed):
        cls.set_cfg_value(cls.key_agreement_shown, True)

        # write the config to ini
        ini_file = os.path.join(get_current_path(), "cocos2d.ini")
        f = open(ini_file)
        old_lines = f.readlines()
        f.close()

        new_str = 'enable_stat=%s' % ('true' if agreed else 'false')
        new_lines = []
        for line in old_lines:
            new_line = re.sub('enable_stat[ \t]*=(.*)$', new_str, line)
            new_lines.append(new_line)

        f = open(ini_file, 'w')
        f.writelines(new_lines)
        f.close()

    @classmethod
    def show_stat_agreement(cls, skip_agree_value=None):
        if cls.is_agreement_shown():
            return

        if skip_agree_value is None:
            # show the agreement
            input_value = raw_input(MultiLanguage.get_string('COCOS_AGREEMENT'))
            agreed = (input_value.lower() != 'n' and input_value.lower() != 'no')
        else:
            # --agreement is used to skip the input
            agreed = skip_agree_value
        cls.change_agree_stat(agreed)

    # change the last time statistics status in local config file.
    @classmethod
    def change_last_state(cls, enabled):
        cls.set_cfg_value(cls.key_last_state, enabled)

    # get the last time statistics status in local config file.
    @classmethod
    def get_last_state(cls):
        return cls.get_cfg_value(cls.key_last_state, True)

    @classmethod
    def init_stat_obj(cls):
        if cls.inited == False:
            # get the cocos_stat module
            m = None
            try:
                m = __import__("cocos_stat")
            except:
                pass

            if m is not None:
                stat_cls = getattr(m, "Statistic")
                cls.stat_obj = stat_cls(STAT_VERSION)

            # cocos_stat is found
            if cls.stat_obj is not None:
                # check config in cocos2d.ini
                parser = Cocos2dIniParser()
                cur_enabled = parser.is_statistic_enabled()

                # get last time is enabled or not
                last_enabled = cls.get_last_state()

                if not cur_enabled:
                    # statistics is disabled
                    if last_enabled:
                        cls.stat_obj.send_event('switch', 'off', 'stat_closed')
                    cls.stat_obj = None

                # update last time status
                if cur_enabled != last_enabled:
                    cls.change_last_state(cur_enabled)

            # try to send the cached events
            if cls.stat_obj is not None:
                cls.stat_obj.send_cached_events()

            cls.inited = True

        return cls.stat_obj

    @classmethod
    def stat_event(cls, category, action, label):
        try:
            cls.init_stat_obj()
            if cls.stat_obj is None:
                return

            cls.stat_obj.send_event(category, action, label)
        except:
            pass

    @classmethod
    def terminate_stat(cls):
        try:
            if cls.stat_obj is None:
                return

            cls.stat_obj.terminate_stat()
        except:
            pass


#
# Plugins should be a sublass of CCPlugin
#
class CCPlugin(object):

    def _run_cmd(self, command, cwd=None):
        CMDRunner.run_cmd(command, self._verbose, cwd)

    def _output_for(self, command):
        return CMDRunner.output_for(command, self._verbose)

    @classmethod
    def get_cocos2d_path(cls):
        """returns the path where Cocos2d-x is installed"""

        #
        # 1: Check for config.ini
        #
        parser = Cocos2dIniParser()
        cocos2dx_path = parser.get_cocos2dx_path()

        if cocos2dx_path is not None:
            return cocos2dx_path

        #
        # 2: default engine path
        #
        # possible path of console
        # /Users/myself/cocos2d-x/tools/cocos2d-console/bin
        # if so, we have to remove the last 3 segments
        path = cls.get_console_path()
        path = os.path.abspath(path)
        cocos2dx_path = os.path.abspath(os.path.join(
            path, os.path.pardir, os.path.pardir, os.path.pardir))
        if os.path.isdir(cocos2dx_path):
            return cocos2dx_path

        if cls.get_cocos2d_mode() is not "distro":
            # In 'distro' mode this is not a warning since
            # the source code is not expected to be installed
            Logging.warning(MultiLanguage.get_string('COCOS_WARNING_ENGINE_NOT_FOUND'))
        return None

    @classmethod
    def get_console_path(cls):
        """returns the path where cocos console is installed"""
        run_path = unicode(get_current_path(), "utf-8")
        return run_path

    @classmethod
    def get_templates_paths(cls):
        """returns a set of paths where templates are installed"""

        parser = Cocos2dIniParser()
        templates_path = parser.get_templates_path()

        paths = []

        #
        # 1: Check for config.ini
        #
        if templates_path is not None:
            paths.append(templates_path)

        #
        # 2: Path defined by walking the cocos2d path
        #
        path = cls.get_cocos2d_path()

        if path is not None:
            # Try one: cocos2d-x/templates (assuming it is using cocos2d-x's setup.py)
            # Try two: cocos2d-x/../../templates
            possible_paths = [['templates'], ['..', '..', 'templates']]
            for p in possible_paths:
                p = string.join(p, os.sep)
                template_path = os.path.abspath(os.path.join(path, p))
                try:
                    if os.path.isdir(template_path):
                        paths.append(template_path)
                except Exception as e:
                    Logging.info(MultiLanguage.get_string('COCOS_INFO_CHECK_TEMPLATE_PATH_FAILED_FMT', template_path))
                    Logging.info("%s" % e)
                    pass

        #
        # 3: Templates can be in ~/.cocos2d/templates as well
        #
        user_path = os.path.expanduser("~/.cocos/templates")
        if os.path.isdir(user_path):
            paths.append(user_path)

        if len(paths) == 0:
            raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_TEMPLATE_NOT_FOUND'),
                                CCPluginError.ERROR_PATH_NOT_FOUND)

        # remove duplicates
        from collections import OrderedDict
        ordered = OrderedDict.fromkeys(paths)
        paths = ordered.keys()
        return paths

    @classmethod
    def get_cocos2d_mode(cls):
        parser = Cocos2dIniParser()
        return parser.get_cocos2dx_mode()

    @staticmethod
    def _log_path():
        log_dir = os.path.expanduser("~/.cocos")
        if not os.path.exists(log_dir):
            os.mkdir(log_dir)
        return os.path.join(log_dir, "cocos.log")

    # the list of plugins this plugin needs to run before itself.
    # ie: if it returns ('a', 'b'), the plugin 'a' will run first, then 'b'
    # and after that, the plugin itself.
    # they all share the same command line arguments
    @staticmethod
    def depends_on():
        return None

    # returns the plugin category,
    # default is empty string.
    @staticmethod
    def plugin_category():
        return ""

    # returns the plugin name
    @staticmethod
    def plugin_name():
        pass

    # returns help
    @staticmethod
    def brief_description():
        pass

    # Constructor
    def __init__(self):
        pass

    # Setup common options. If a subclass needs custom options,
    # override this method and call super.
    def init(self, args):
        self._verbose = (not args.quiet)
        self._platforms = cocos_project.Platforms(self._project, args.platform, args.proj_dir)
        if self._platforms.none_active():
            self._platforms.select_one()

    # Run it
    def run(self, argv, dependencies):
        pass

    # If a plugin needs to add custom parameters, override this method.
    # There's no need to call super
    def _add_custom_options(self, parser):
        pass

    # If a plugin needs to check custom parameters values after parsing them,
    # override this method.
    # There's no need to call super
    def _check_custom_options(self, args):
        pass

    def parse_args(self, argv):
        from argparse import ArgumentParser

        # FIXME:
        # CCPlugin should not parse any argument. Plugins are responsoble for doing it
        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())
        parser.add_argument("-s", "--src",
                            dest="src_dir",
                            help=MultiLanguage.get_string('COCOS_HELP_ARG_SRC'))
        parser.add_argument("-q", "--quiet",
                            action="store_true",
                            dest="quiet",
                            help=MultiLanguage.get_string('COCOS_HELP_ARG_QUIET'))
        platform_list = cocos_project.Platforms.list_for_display()
        parser.add_argument("-p", "--platform",
                            dest="platform",
                            help=MultiLanguage.get_string('COCOS_HELP_ARG_PLATFORM'))
        parser.add_argument("--list-platforms",
                            action="store_true",
                            dest="listplatforms",
                            help=_("List available platforms"))
        parser.add_argument("--proj-dir",
                            dest="proj_dir",
                            help=MultiLanguage.get_string('COCOS_HELP_ARG_PROJ_DIR'))
        self._add_custom_options(parser)

        (args, unkonw) = parser.parse_known_args(argv)

        if args.src_dir is None:
            self._project = cocos_project.Project(os.path.abspath(os.getcwd()))
        else:
            self._project = cocos_project.Project(
                os.path.abspath(args.src_dir))

        args.src_dir = self._project.get_project_dir()
        if args.src_dir is None:
            raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_PROJECT_NOT_FOUND'),
                                CCPluginError.ERROR_WRONG_ARGS)

        if args.platform:
            args.platform = args.platform.lower()
            if args.platform not in platform_list:
                raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_UNKNOWN_PLATFORM_FMT', args.platform),
                                    CCPluginError.ERROR_WRONG_ARGS)

        if args.listplatforms and self._project is not None:
            platforms = cocos_project.Platforms(self._project, args.platform, args.proj_dir)
            p = platforms.get_available_platforms().keys()
            print('{"platforms":' + json.dumps(p) + '}')
            sys.exit(0)

        self.init(args)
        self._check_custom_options(args)


def get_current_path():
    if getattr(sys, 'frozen', None):
        ret = os.path.realpath(os.path.dirname(sys.executable))
    else:
        ret = os.path.realpath(os.path.dirname(__file__))

    return ret


# get_class from: http://stackoverflow.com/a/452981
def get_class(kls):
    parts = kls.split('.')
    module = ".".join(parts[:-1])
    if len(parts) == 1:
        m = sys.modules[__name__]
        m = getattr(m, parts[0])
    else:
        m = __import__(module)
        for comp in parts[1:]:
            m = getattr(m, comp)
    return m


def _check_dependencies_exist(dependencies, classes, plugin_name):
    for dep in dependencies:
        if dep not in classes:
            raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_INVALID_DEPENDENCY_FMT',
                                (plugin_name, dep)),
                                CCPluginError.ERROR_CMD_NOT_FOUND)


def _check_dependencies(classes):
    for k in classes:
        plugin = classes[k]
        dependencies = plugin.depends_on()
        if dependencies is not None:
            _check_dependencies_exist(dependencies, classes, k)


# common functions

def check_environment_variable(var):
    ''' Checking the environment variable, if found then return it's value, else raise error
    '''
    try:
        value = os.environ[var]
    except Exception:
        raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_ENV_NOT_DEFINED_FMT', var),
                            CCPluginError.ERROR_ENV_VAR_NOT_FOUND)

    return value


def get_xcode_version():
    commands = [
        "xcodebuild",
        "-version"
    ]
    child = subprocess.Popen(commands, stdout=subprocess.PIPE)

    xcode = None
    version = None
    for line in child.stdout:
        if 'Xcode' in line:
            xcode, version = str.split(line, ' ')

    child.wait()

    if xcode is None:
        raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_XCODE_NOT_INSTALLED'),
                            CCPluginError.ERROR_TOOLS_NOT_FOUND)

    return version

def app_is_installed(adb_cmd, pack_name):
    list_pack_cmd = "%s shell 'pm list packages'" % (adb_cmd)
    desired_name = "package:%s" % (pack_name)
    child = subprocess.Popen(list_pack_cmd, stdout=subprocess.PIPE, shell=True)
    for line in child.stdout:
        if desired_name == line.strip():
            return True
    return False

def version_compare(a, op, b):
    '''Compares two version numbers to see if a op b is true

    op is operator
    op can be ">", "<", "==", "!=", ">=", "<="
    a and b are version numbers (dot separated)
    a and b can be string, float or int

    Please note that: 3 == 3.0 == 3.0.0 ... ("==" is not a simple string cmp)
    '''
    allowed = [">", "<", "==", "!=", ">=", "<="]
    if op not in allowed:
        raise ValueError("op must be one of {}".format(allowed))

    # Use recursion to simplify operators:
    if op[0] == "<": # Reverse args and inequality sign:
        return version_compare(b, op.replace("<",">"), a)
    if op == ">=":
        return version_compare(a,"==",b) or version_compare(a,">",b)
    if op == "!=":
        return not version_compare(a,"==",b)

    # We now have 1 of 2 base cases, "==" or ">":
    assert op in ["==", ">"]

    a = [int(x) for x in str(a).split(".")]
    b = [int(x) for x in str(b).split(".")]

    for i in range(max(len(a), len(b))):
        ai, bi = 0, 0   # digits
        if len(a) > i:
            ai = a[i]
        if len(b) > i:
            bi = b[i]
        if ai > bi:
            if op == ">":
                return True
            else: # op "=="
                return False
        if ai < bi:
            # Both "==" and ">" are False:
            return False
    if op == ">":
        return False    # op ">" and all digits were equal
    return True         # op "==" and all digits were equal

def copy_files_in_dir(src, dst):

    for item in os.listdir(src):
        path = os.path.join(src, item)
        if os.path.isfile(path):
            path = add_path_prefix(path)
            copy_dst = add_path_prefix(dst)
            shutil.copy(path, copy_dst)
        if os.path.isdir(path):
            new_dst = os.path.join(dst, item)
            if not os.path.isdir(new_dst):
                os.makedirs(add_path_prefix(new_dst))
            copy_files_in_dir(path, new_dst)


def copy_files_with_config(config, src_root, dst_root):
    src_dir = config["from"]
    dst_dir = config["to"]

    src_dir = os.path.join(src_root, src_dir)
    dst_dir = os.path.join(dst_root, dst_dir)

    include_rules = None
    if "include" in config:
        include_rules = config["include"]
        include_rules = convert_rules(include_rules)

    exclude_rules = None
    if "exclude" in config:
        exclude_rules = config["exclude"]
        exclude_rules = convert_rules(exclude_rules)

    copy_files_with_rules(
        src_dir, src_dir, dst_dir, include_rules, exclude_rules)


def copy_files_with_rules(src_rootDir, src, dst, include=None, exclude=None):
    if os.path.isfile(src):
        if not os.path.exists(dst):
            os.makedirs(add_path_prefix(dst))

        copy_src = add_path_prefix(src)
        copy_dst = add_path_prefix(dst)
        shutil.copy(copy_src, copy_dst)
        return

    if (include is None) and (exclude is None):
        if not os.path.exists(dst):
            os.makedirs(add_path_prefix(dst))
        copy_files_in_dir(src, dst)
    elif (include is not None):
        # have include
        for name in os.listdir(src):
            abs_path = os.path.join(src, name)
            rel_path = os.path.relpath(abs_path, src_rootDir)
            if os.path.isdir(abs_path):
                sub_dst = os.path.join(dst, name)
                copy_files_with_rules(
                    src_rootDir, abs_path, sub_dst, include=include)
            elif os.path.isfile(abs_path):
                if _in_rules(rel_path, include):
                    if not os.path.exists(dst):
                        os.makedirs(add_path_prefix(dst))

                    abs_path = add_path_prefix(abs_path)
                    copy_dst = add_path_prefix(dst)
                    shutil.copy(abs_path, copy_dst)
    elif (exclude is not None):
        # have exclude
        for name in os.listdir(src):
            abs_path = os.path.join(src, name)
            rel_path = os.path.relpath(abs_path, src_rootDir)
            if os.path.isdir(abs_path):
                sub_dst = os.path.join(dst, name)
                copy_files_with_rules(
                    src_rootDir, abs_path, sub_dst, exclude=exclude)
            elif os.path.isfile(abs_path):
                if not _in_rules(rel_path, exclude):
                    if not os.path.exists(dst):
                        os.makedirs(add_path_prefix(dst))

                    abs_path = add_path_prefix(abs_path)
                    copy_dst = add_path_prefix(dst)
                    shutil.copy(abs_path, copy_dst)


def _in_rules(rel_path, rules):
    ret = False
    path_str = rel_path.replace("\\", "/")
    for rule in rules:
        if re.match(rule, path_str):
            ret = True

    return ret


def convert_rules(rules):
    ret_rules = []
    for rule in rules:
        ret = rule.replace('.', '\\.')
        ret = ret.replace('*', '.*')
        ret = "%s" % ret
        ret_rules.append(ret)

    return ret_rules


def os_is_win32():
    return sys.platform == 'win32'


def os_is_32bit_windows():
    if not os_is_win32():
        return False

    arch = os.environ['PROCESSOR_ARCHITECTURE'].lower()
    archw = "PROCESSOR_ARCHITEW6432" in os.environ
    return (arch == "x86" and not archw)


def os_is_mac():
    return sys.platform == 'darwin'


def os_is_linux():
    return 'linux' in sys.platform


def add_path_prefix(path_str):
    if not os_is_win32():
        return path_str

    if path_str.startswith("\\\\?\\"):
        return path_str

    ret = "\\\\?\\" + os.path.abspath(path_str)
    ret = ret.replace("/", "\\")
    return ret


# get from http://stackoverflow.com/questions/6194499/python-os-system-pushd
@contextmanager
def pushd(newDir):
    previousDir = os.getcwd()
    os.chdir(newDir)
    yield
    os.chdir(previousDir)


def help():
    print(MultiLanguage.get_string('COCOS_HELP_BRIEF_FMT',
          (sys.argv[0], COCOS2D_CONSOLE_VERSION)))
    print(MultiLanguage.get_string('COCOS_HELP_AVAILABLE_CMD'))
    parse = Cocos2dIniParser()
    classes = parse.parse_plugins()
    max_name = max(len(classes[key].plugin_name(
    ) + classes[key].plugin_category()) for key in classes.keys())
    max_name += 4
    for key in classes.keys():
        plugin_class = classes[key]
        category = plugin_class.plugin_category()
        category = (category + ' ') if len(category) > 0 else ''
        name = plugin_class.plugin_name()
        print("\t%s%s%s%s" % (category, name,
                              ' ' * (max_name - len(name + category)),
                              plugin_class.brief_description()))

    print(MultiLanguage.get_string('COCOS_HELP_AVAILABLE_ARGS_FMT',
                                   MultiLanguage.get_available_langs()))
    print(MultiLanguage.get_string('COCOS_HELP_EXAMPLE'))

def show_version():
    print(COCOS_ENGINE_VERSION)
    print("Cocos Console %s" % COCOS2D_CONSOLE_VERSION)

def run_plugin(command, argv, plugins):
    run_directly = False
    if len(argv) > 0:
        if argv[0] in ['--help', '-h']:
            run_directly = True

    plugin = plugins[command]()

    if run_directly:
        plugin.run(argv, None)
    else:
        dependencies = plugin.depends_on()
        dependencies_objects = {}
        if dependencies is not None:
            for dep_name in dependencies:
                # FIXME check there's not circular dependencies
                dependencies_objects[dep_name] = run_plugin(
                    dep_name, argv, plugins)
        # don't print this info. Not useful to users, and generates noise when parsing output
#        Logging.info(MultiLanguage.get_string('COCOS_INFO_RUNNING_PLUGIN_FMT', plugin.__class__.plugin_name()))
        plugin.run(argv, dependencies_objects)
        return plugin


def _check_python_version():
    major_ver = sys.version_info[0]
    minor_ver = sys.version_info[1]
    ret = True
    if major_ver != 2:
        ret = False
    elif minor_ver < 7:
        ret = False

    if not ret:
        print(MultiLanguage.get_string('COCOS_PYTHON_VERSION_TIP_FMT') % (major_ver, minor_ver))

    return ret

# gettext
language = None
encoding = None
try:
    locale.setlocale(locale.LC_ALL, '')  # use user's preferred locale
    language, encoding = locale.getlocale()
except:
    pass

if language is not None:
    filename = "language_%s.mo" % language[0:2]
    try:
        trans = gettext.GNUTranslations(open(filename, "rb"))
    except IOError:
        trans = gettext.NullTranslations()
    trans.install()
    _ = trans.gettext
else:
    _ = MultiLanguage.get_string

if __name__ == "__main__":
    # Parse the arguments, specify the language
    language_arg = '--ol'
    if language_arg in sys.argv:
        idx = sys.argv.index(language_arg)
        if idx == (len(sys.argv) - 1):
            Logging.error(MultiLanguage.get_string('COCOS_ERROR_OL_NO_VALUE'))
            sys.exit(CCPluginError.ERROR_WRONG_ARGS)

        # set specified language
        MultiLanguage.set_language(sys.argv[idx+1])

        # remove the argument '--ol' & the value
        sys.argv.pop(idx)
        sys.argv.pop(idx)

    agreement_arg = '--agreement'
    skip_agree_value = None
    if agreement_arg in sys.argv:
        idx = sys.argv.index(agreement_arg)
        if idx == (len(sys.argv) - 1):
            Logging.error(MultiLanguage.get_string('COCOS_ERROR_AGREEMENT_NO_VALUE'))
            sys.exit(CCPluginError.ERROR_WRONG_ARGS)

        # get the argument value
        agree_value = sys.argv[idx+1]
        if agree_value.lower() == 'n':
            skip_agree_value = False
        else:
            skip_agree_value = True

        # remove the argument '--agreement' & the value
        sys.argv.pop(idx)
        sys.argv.pop(idx)

    # Get the engine version for the DataStat
    cur_path = get_current_path()
    engine_path = os.path.normpath(os.path.join(cur_path, '../../../'))
    COCOS_ENGINE_VERSION = utils.get_engine_version(engine_path)
    STAT_VERSION = COCOS_ENGINE_VERSION
    ver_pattern = r"cocos2d-x-(.*)"
    match = re.match(ver_pattern, COCOS_ENGINE_VERSION)
    if match:
        STAT_VERSION = match.group(1)

    DataStatistic.show_stat_agreement(skip_agree_value)
    DataStatistic.stat_event('cocos', 'start', 'invoked')

    if not _check_python_version():
        DataStatistic.terminate_stat()
        sys.exit(CCPluginError.ERROR_TOOLS_NOT_FOUND)

    parser = Cocos2dIniParser()
    plugins_path = parser.get_plugins_path()
    sys.path.append(plugins_path)

    if len(sys.argv) == 1 or sys.argv[1] in ('-h', '--help'):
        help()
        DataStatistic.terminate_stat()
        sys.exit(0)

    if len(sys.argv) > 1 and sys.argv[1] in ('-v', '--version'):
        show_version()
        DataStatistic.terminate_stat()
        sys.exit(0)

    try:
        plugins = parser.parse_plugins()
        command = sys.argv[1]
        argv = sys.argv[2:]
        # try to find plugin by name
        if command in plugins:
            DataStatistic.stat_event('cocos', 'running_command', command)
            run_plugin(command, argv, plugins)
        else:
            # try to find plugin by category_name, so the len(sys.argv) at
            # least 3.
            if len(sys.argv) > 2:
                # combine category & name as key
                # eg. 'project_new'
                command = sys.argv[1] + '_' + sys.argv[2]
                argv = sys.argv[3:]
                if command in plugins:
                    DataStatistic.stat_event('cocos', 'running_command', command)
                    run_plugin(command, argv, plugins)
                else:
                    raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_CMD_NOT_FOUND_FMT',
                                                                 ' '.join(sys.argv[1:])),
                                        CCPluginError.ERROR_CMD_NOT_FOUND)
            else:
                raise CCPluginError(MultiLanguage.get_string('COCOS_ERROR_CMD_NOT_FOUND_FMT', command),
                                    CCPluginError.ERROR_CMD_NOT_FOUND)

    except Exception as e:
        # FIXME don't know how to handle this. Can't catch cocos2d.CCPluginError
        # as it's not defined that way in this file, but the plugins raise it
        # with that name.
        if e.__class__.__name__ == 'CCPluginError':
            Logging.error(' '.join(e.args))
            # import traceback
            # print '-' * 60
            # traceback.print_exc(file=sys.stdout)
            # print '-' * 60
            err_no = e.get_error_no()
            sys.exit(err_no)
        else:
            raise
    finally:
        DataStatistic.terminate_stat()
