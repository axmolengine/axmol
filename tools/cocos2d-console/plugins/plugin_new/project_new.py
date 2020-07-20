#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos "new" plugin
#
# Copyright 2013 (C) cocos2d-x.org
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"new" plugin for cocos command line tool
'''
__docformat__ = 'restructuredtext'

# python
import os
import sys
import json
import shutil
import cocos
import cocos_project
import re
import utils
from collections import OrderedDict
from MultiLanguage import MultiLanguage

#
# Plugins should be a sublass of CCJSPlugin
#
class CCPluginNew(cocos.CCPlugin):

    DEFAULT_PROJ_NAME = {
        cocos_project.Project.CPP: 'MyCppGame',
        cocos_project.Project.LUA: 'MyLuaGame',
        cocos_project.Project.JS: 'MyJSGame'
    }

    @staticmethod
    def plugin_name():
        return "new"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('NEW_BRIEF')

    def init(self, args):
        self._projname = args.name
        self._projdir = unicode(
            os.path.abspath(os.path.join(args.directory, self._projname)), "utf-8")
        self._lang = args.language
        self._package = args.package
        self._tpname = args.template

        # new official ways to get the template and cocos paths
        self._templates_paths = self.get_templates_paths()
        self._cocosroot = self.get_cocos2d_path()

        # search for custom paths
        if args.engine_path is not None:
            self._cocosroot = os.path.abspath(args.engine_path)
            self._cocosroot = unicode(self._cocosroot, "utf-8")
            tp_path = os.path.join(self._cocosroot, "templates")
            if os.path.isdir(tp_path):
                self._templates_paths.append(tp_path)

        # remove duplicates keeping order
        o = OrderedDict.fromkeys(self._templates_paths)
        self._templates_paths = o.keys()

        self._other_opts = args
        self._mac_bundleid = args.mac_bundleid
        self._ios_bundleid = args.ios_bundleid

        self._tpdir = ''
        # old way to choose a template from args:
        #  --language + --template (???)
        # new way to choose a template from args:
        #  --template-name
        args.template_name = None   # issue #15958
        if args.template_name:
            # New Way
            dic = Templates.list(self.get_templates_paths())
            template_key = args.template_name
            if template_key in dic:
                self._tpdir = dic[template_key]["path"]
            else:
                raise cocos.CCPluginError(
                    "Template name '%s' not found. Available templates: %s" %
                    (template_key, dic.keys()),
                    cocos.CCPluginError.ERROR_PATH_NOT_FOUND)
        else:
            # Old way
            self._templates = Templates(args.language, self._templates_paths, args.template)
            if self._templates.none_active():
                self._templates.select_one()
            self._tpdir = self._templates.template_path()

    # parse arguments
    def parse_args(self, argv):
        """Custom and check param list.
        """
        from argparse import ArgumentParser
        # set the parser to parse input params
        # the correspond variable name of "-x, --xxx" is parser.xxx
        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())

        parser.add_argument(
            "name", metavar="PROJECT_NAME", nargs='?', help=MultiLanguage.get_string('NEW_ARG_NAME'))
        parser.add_argument(
            "-p", "--package", metavar="PACKAGE_NAME", help=MultiLanguage.get_string('NEW_ARG_PACKAGE'))
        parser.add_argument("-d", "--directory", metavar="DIRECTORY",
                            help=MultiLanguage.get_string('NEW_ARG_DIR'))
        parser.add_argument("-t", "--template", metavar="TEMPLATE_NAME",
                            help=MultiLanguage.get_string('NEW_ARG_TEMPLATE'))
        parser.add_argument(
            "--ios-bundleid", dest="ios_bundleid", help=MultiLanguage.get_string('NEW_ARG_IOS_BUNDLEID'))
        parser.add_argument(
            "--mac-bundleid", dest="mac_bundleid", help=MultiLanguage.get_string('NEW_ARG_MAC_BUNDLEID'))
        parser.add_argument("-e", "--engine-path", dest="engine_path",
                            help=MultiLanguage.get_string('NEW_ARG_ENGINE_PATH'))
        parser.add_argument("--portrait", action="store_true", dest="portrait",
                            help=MultiLanguage.get_string('NEW_ARG_PORTRAIT'))

        # REMOVE the option --no-native. Because it's added for Cocos Code IDE.
        # It will cause confusion: https://github.com/cocos2d/cocos2d-console/issues/401
        # group = parser.add_argument_group(MultiLanguage.get_string('NEW_ARG_GROUP_SCRIPT'))
        # group.add_argument(
        #     "--no-native", action="store_true", dest="no_native",
        #     help=MultiLanguage.get_string('NEW_ARG_NO_NATIVE'))

        # -l | --list-templates
        group = parser.add_mutually_exclusive_group(required=True)
        group.add_argument("-l", "--language",
                            choices=["cpp", "lua", "js"],
                            help=MultiLanguage.get_string('NEW_ARG_LANG'))
#        group.add_argument("--list-templates", action="store_true",
#                            help='List available templates. To be used with --template option.')
#        group.add_argument("-k", "--template-name",
#                            help='Name of the template to be used to create the game. To list available names, use --list-templates.')

        # parse the params
        args = parser.parse_args(argv)

#        if args.list_templates:
#            print(json.dumps(Templates.list(self.get_templates_paths())))
#            sys.exit(0)

        if args.name is None and args.language is not None:
            args.name = CCPluginNew.DEFAULT_PROJ_NAME[args.language]

        if not args.package:
            args.package = "org.cocos2dx.%s" % args.name

        if not args.ios_bundleid:
            args.ios_bundleid = args.package

        if not args.mac_bundleid:
            args.mac_bundleid = args.package

        if not args.directory:
            args.directory = os.getcwd()

        if not args.template:
            args.template = 'default'

        self.init(args)

        return args

    def _stat_engine_version(self):
        try:
            ver_str = None
            engine_type = None

            framework_ver_file = os.path.join(self._cocosroot, 'version')
            x_ver_file = os.path.join(self._cocosroot, 'cocos/cocos2d.cpp')
            js_ver_file = os.path.join(self._cocosroot, 'frameworks/js-bindings/bindings/manual/ScriptingCore.h')
            if os.path.isfile(framework_ver_file):
                # the engine is Cocos Framework
                f = open(framework_ver_file)
                ver_str = f.read()
                f.close()
                engine_type = 'cocosframework'
            else:
                ver_file = None
                pattern = None
                if os.path.isfile(x_ver_file):
                    # the engine is cocos2d-x
                    pattern = r".*return[ \t]+\"(.*)\";"
                    ver_file = x_ver_file
                    engine_type = 'cocos2d-x'
                elif os.path.isfile(js_ver_file):
                    # the engine is cocos2d-js
                    pattern = r".*#define[ \t]+ENGINE_VERSION[ \t]+\"(.*)\""
                    ver_file = js_ver_file
                    engine_type = 'cocos2d-js'

                if ver_file is not None:
                    f = open(ver_file)
                    import re
                    for line in f.readlines():
                        match = re.match(pattern, line)
                        if match:
                            ver_str = match.group(1)
                            break
                    f.close()

            if ver_str is not None:
                # stat the engine version info
                cocos.DataStatistic.stat_event('new_engine_ver', ver_str, engine_type)
        except:
            pass

    def _create_from_cmd(self):
        # check the dst project dir exists
        if os.path.exists(self._projdir):
            message = MultiLanguage.get_string('NEW_ERROR_FOLDER_EXISTED_FMT', self._projdir)
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        creator = TPCreator(self._lang, self._cocosroot, self._projname, self._projdir,
                            self._tpname, self._tpdir, self._package, self._mac_bundleid, self._ios_bundleid)
        # do the default creating step
        creator.do_default_step()

        data = None
        cfg_path = os.path.join(self._projdir, cocos_project.Project.CONFIG)
        if os.path.isfile(cfg_path):
            f = open(cfg_path)
            data = json.load(f)
            f.close()

        if data is None:
            data = {}

        if cocos_project.Project.KEY_PROJ_TYPE not in data:
            data[cocos_project.Project.KEY_PROJ_TYPE] = self._lang

        # script project may add native support
        if self._lang in (cocos_project.Project.LUA, cocos_project.Project.JS):
            # REMOVE the option --no-native. Because it's added for Cocos Code IDE.
            # It will cause confusion: https://github.com/cocos2d/cocos2d-console/issues/401
            # if self._other_opts.no_native is not self._other_opts.no_native:
            #     creator.do_other_step('do_add_native_support')
            #     data[cocos_project.Project.KEY_HAS_NATIVE] = True
            # else:
            #     data[cocos_project.Project.KEY_HAS_NATIVE] = False
            creator.do_other_step('do_add_native_support')
            data[cocos_project.Project.KEY_HAS_NATIVE] = True

        # record the engine version if not predefined
        if not data.has_key(cocos_project.Project.KEY_ENGINE_VERSION):
            engine_version = utils.get_engine_version(self._cocosroot)
            if engine_version is not None:
                data[cocos_project.Project.KEY_ENGINE_VERSION] = engine_version

        # if --portrait is specified, change the orientation
        if self._other_opts.portrait:
            creator.do_other_step("change_orientation", not_existed_error=False)

        # write config files
        with open(cfg_path, 'w') as outfile:
            json.dump(data, outfile, sort_keys=True, indent=4)

    # main entry point
    def run(self, argv, dependencies):
        self.parse_args(argv)
        action_str = 'new_%s' % (self._lang)
        cocos.DataStatistic.stat_event('new', action_str, self._tpname)
        self._create_from_cmd()
        self._stat_engine_version()

def replace_string(filepath, src_string, dst_string):
    """ From file's content replace specified string
    Arg:
        filepath: Specify a file contains the path
        src_string: old string
        dst_string: new string
    """
    if src_string is None or dst_string is None:
        raise TypeError

    content = ""
    f1 = open(filepath, "rb")
    for line in f1:
        strline = line.decode('utf8')
        if src_string in strline:
            content += strline.replace(src_string, dst_string)
        else:
            content += strline
    f1.close()
    f2 = open(filepath, "wb")
    f2.write(content.encode('utf8'))
    f2.close()
# end of replace_string


class Templates(object):

    @staticmethod
    def list(paths):
        dirs = []

        # enumerate directories and append then into 'dirs'
        for template_dir in paths:
            try:
                dirs += [os.path.join(template_dir, name) for name in os.listdir(template_dir) if os.path.isdir(
                    os.path.join(template_dir, name))]
            except Exception:
                continue

        # if dir contains the template_metadata.json file, then it is a valid template
        valid_templates = {}
        for d in dirs:
            try:
                f = open(os.path.join(d, 'template_metadata', 'config.json'))
                # python dictionary
                dictionary = json.load(f)
                # append current path
                dictionary['path'] = d
                # use 'key' as key
                name = dictionary['key']
                valid_templates[name] = dictionary
            except Exception:
                continue
        return valid_templates

    def __init__(self, lang, templates_paths, current):
        self._lang = lang
        self._templates_paths = templates_paths
        self._scan()
        self._current = None
        if current is not None:
            if current in self._template_folders:
                self._current = current
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_TEMPLATE_NOT_FOUND_FMT', current))

    def _scan(self):
        template_pattern = {
            "cpp": 'cpp-template-(.+)',
            "lua": 'lua-template-(.+)',
            "js": 'js-template-(.+)',
        }

        self._template_folders = {}

        for templates_dir in self._templates_paths:
            try:
                dirs = [name for name in os.listdir(templates_dir) if os.path.isdir(
                    os.path.join(templates_dir, name))]
            except Exception:
                continue

            pattern = template_pattern[self._lang]
            for name in dirs:
                match = re.search(pattern, name)
                if match is None:
                    continue

                template_name = match.group(1)
                if template_name in self._template_folders.keys():
                    continue

                self._template_folders[template_name] = os.path.join(templates_dir, name)

        if len(self._template_folders) == 0:
            cur_engine = "cocos2d-x" if self._lang == "js" else "cocos2d-js"
            need_engine = "cocos2d-js" if self._lang == "js" else "cocos2d-x"
            engine_tip = MultiLanguage.get_string('NEW_ERROR_ENGINE_TIP_FMT', need_engine)
            message = MultiLanguage.get_string('NEW_ERROR_TEMPLATE_NOT_FOUND_FMT', (self._lang, engine_tip))
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

    def none_active(self):
        return self._current is None

    def template_path(self):
        if self._current is None:
            return None
        return self._template_folders[self._current]

    def select_one(self):
        cocos.Logging.warning(MultiLanguage.get_string('NEW_SELECT_TEMPLATE_TIP1'))

        p = self._template_folders.keys()
        for i in range(len(p)):
            cocos.Logging.warning('%d %s' % (i + 1, p[i]))
        cocos.Logging.warning(MultiLanguage.get_string('NEW_SELECT_TEMPLATE_TIP2'))
        while True:
            option = raw_input()
            if option.isdigit():
                option = int(option) - 1
                if option in range(len(p)):
                    break

        self._current = p[option]


class TPCreator(object):

    def __init__(self, lang, cocos_root, project_name, project_dir, tp_name, tp_dir, project_package, mac_id, ios_id):
        self.lang = lang
        self.cocos_root = cocos_root
        self.project_dir = project_dir
        self.project_name = project_name
        self.package_name = project_package
        self.mac_bundleid = mac_id
        self.ios_bundleid = ios_id

        self.tp_name = tp_name
        self.tp_dir = tp_dir
        self.tp_json = 'cocos-project-template.json'

        # search in 'template_metadata' first
        tp_json_path = os.path.join(tp_dir, 'template_metadata', self.tp_json)
        if not os.path.exists(tp_json_path):
            # if not, search in the old place
            tp_json_path = os.path.join(tp_dir, self.tp_json)

        if not os.path.exists(tp_json_path):
            message = MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT', tp_json_path)
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        f = open(tp_json_path)
        # keep the key order
        tpinfo = json.load(f, encoding='utf8', object_pairs_hook=OrderedDict)

        # read the default creating step
        if 'do_default' not in tpinfo:
            message = (MultiLanguage.get_string('NEW_ERROR_DEFAILT_CFG_NOT_FOUND_FMT', tp_json_path))
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_WRONG_CONFIG)
        self.tp_default_step = tpinfo.pop('do_default')
        # keep the other steps
        self.tp_other_step = tpinfo

    def cp_self(self, project_dir, exclude_files):
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_COPY_TEMPLATE_FMT', project_dir))

        if not os.path.exists(self.project_dir):
            os.makedirs(self.project_dir)

        copy_cfg = {
            "from": self.tp_dir,
            "to": self.project_dir,
            "exclude": exclude_files
        }
        cocos.copy_files_with_config(copy_cfg, self.tp_dir, self.project_dir)

    def do_default_step(self):
        default_cmds = self.tp_default_step
        exclude_files = []
        if "exclude_from_template" in default_cmds:
            exclude_files = exclude_files + \
                default_cmds['exclude_from_template']
            default_cmds.pop('exclude_from_template')

        # should ignore teh xx-template-xx.json
        exclude_files.append(self.tp_json)
        self.cp_self(self.project_dir, exclude_files)
        self.do_cmds(default_cmds)

    def do_other_step(self, step, not_existed_error=True):
        if step not in self.tp_other_step:
            if not_existed_error:
                # handle as error
                message = MultiLanguage.get_string('NEW_ERROR_STEP_NOT_FOUND_FMT', step)
                raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_WRONG_CONFIG)
            else:
                # handle as warning
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_STEP_NOT_FOUND_FMT', step))
                return

        cmds = self.tp_other_step[step]
        self.do_cmds(cmds)

    def do_cmds(self, cmds):
        for k, v in cmds.iteritems():
            # call cmd method by method/cmd name
            # get from
            # http://stackoverflow.com/questions/3951840/python-how-to-invoke-an-function-on-an-object-dynamically-by-name
            try:
                cmd = getattr(self, k)
            except AttributeError:
                raise cocos.CCPluginError(MultiLanguage.get_string('NEW_ERROR_CMD_NOT_FOUND_FMT', k),
                                          cocos.CCPluginError.ERROR_WRONG_CONFIG)

            try:
                cmd(v)
            except Exception as e:
                raise cocos.CCPluginError(str(e), cocos.CCPluginError.ERROR_RUNNING_CMD)

# cmd methods below
    def append_h5_engine(self, v):
        src = os.path.join(self.cocos_root, v['from'])
        dst = os.path.join(self.project_dir, v['to'])
        # check cocos engine exist
        moduleConfig = 'moduleConfig.json'
        moudle_cfg = os.path.join(src, moduleConfig)
        if not os.path.exists(moudle_cfg):
            message = MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT', moudle_cfg)
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        f = open(moudle_cfg)
        data = json.load(f, 'utf8')
        f.close()
        modules = data['module']

        # must copy moduleConfig.json & CCBoot.js
        file_list = [moduleConfig, data['bootFile']]
        for k, v in modules.iteritems():
            module = modules[k]
            for f in module:
                if f[-2:] == 'js':
                    file_list.append(f)

        # begin copy engine
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_COPY_H5'))
        for index in range(len(file_list)):
            srcfile = os.path.join(src, file_list[index])
            dstfile = os.path.join(dst, file_list[index])

            srcfile = cocos.add_path_prefix(srcfile)
            dstfile = cocos.add_path_prefix(dstfile)

            if not os.path.exists(os.path.dirname(dstfile)):
                os.makedirs(cocos.add_path_prefix(os.path.dirname(dstfile)))

            # copy file or folder
            if os.path.exists(srcfile):
                if os.path.isdir(srcfile):
                    if os.path.exists(dstfile):
                        shutil.rmtree(dstfile)
                    shutil.copytree(srcfile, dstfile)
                else:
                    if os.path.exists(dstfile):
                        os.remove(dstfile)
                    shutil.copy2(srcfile, dstfile)

    def append_x_engine(self, v):
        # FIXME this is a hack, but in order to fix it correctly the cocos-project-template.json
        # file probably will need to be re-designed.
        # As a quick (horrible) fix, we check if we are in distro mode.
        # If so, we don't do the "append_x_engine" step
        if cocos.CCPlugin.get_cocos2d_mode() == 'distro':
            return

        src = os.path.join(self.cocos_root, v['from'])
        dst = os.path.join(self.project_dir, v['to'])

        # check cocos engine exist
        cocosx_files_json = os.path.join(
            src, 'templates', 'cocos2dx_files.json')
        if not os.path.exists(cocosx_files_json):
            message = MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT', cocosx_files_json)
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        f = open(cocosx_files_json)
        data = json.load(f)
        f.close()

        fileList = data['common']
        if self.lang == 'lua':
            fileList = fileList + data['lua']

        if self.lang == 'js' and 'js' in data.keys():
            fileList = fileList + data['js']

        # begin copy engine
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_COPY_X'))

        for index in range(len(fileList)):
            srcfile = os.path.join(src, fileList[index])
            dstfile = os.path.join(dst, fileList[index])

            srcfile = cocos.add_path_prefix(srcfile)
            dstfile = cocos.add_path_prefix(dstfile)

            if not os.path.exists(os.path.dirname(dstfile)):
                os.makedirs(cocos.add_path_prefix(os.path.dirname(dstfile)))

            # copy file or folder
            if os.path.exists(srcfile):
                if os.path.isdir(srcfile):
                    if os.path.exists(dstfile):
                        shutil.rmtree(dstfile)
                    shutil.copytree(srcfile, dstfile)
                else:
                    if os.path.exists(dstfile):
                        os.remove(dstfile)
                    shutil.copy2(srcfile, dstfile)

    def append_from_template(self, v):
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_APPEND_TEMPLATE'))
        cocos.copy_files_with_config(v, self.tp_dir, self.project_dir)

    def append_dir(self, v):
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_APPEND_DIR'))
        for item in v:
            cocos.copy_files_with_config(
                item, self.cocos_root, self.project_dir)

    def append_file(self, v):
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_APPEND_FILE'))
        for item in v:
            src = os.path.join(self.cocos_root, item['from'])
            dst = os.path.join(self.project_dir, item['to'])

            src = cocos.add_path_prefix(src)
            dst = cocos.add_path_prefix(dst)

            shutil.copy2(src, dst)

# project cmd
    def project_rename(self, v):
        """ will modify the file name of the file
        """
        dst_project_dir = self.project_dir
        dst_project_name = self.project_name
        src_project_name = v['src_project_name']
        if dst_project_name == src_project_name:
            return

        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_RENAME_PROJ_FMT',
                                                    (src_project_name, dst_project_name)))
        files = v['files']
        for f in files:
            src = f.replace("PROJECT_NAME", src_project_name)
            dst = f.replace("PROJECT_NAME", dst_project_name)
            src_file_path = os.path.join(dst_project_dir, src)
            dst_file_path = os.path.join(dst_project_dir, dst)
            if os.path.exists(src_file_path):
                if dst_project_name.lower() == src_project_name.lower():
                    temp_file_path = "%s-temp" % src_file_path
                    os.rename(src_file_path, temp_file_path)
                    os.rename(temp_file_path, dst_file_path)
                else:
                    if os.path.exists(dst_file_path):
                        os.remove(dst_file_path)
                    os.rename(src_file_path, dst_file_path)
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT',
                                                               os.path.join(dst_project_dir, src)))

    def project_replace_project_name(self, v):
        """ will modify the content of the file
        """
        dst_project_dir = self.project_dir
        dst_project_name = self.project_name
        src_project_name = v['src_project_name']
        if dst_project_name == src_project_name:
            return

        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_REPLACE_PROJ_FMT',
                                                    (src_project_name, dst_project_name)))
        files = v['files']
        for f in files:
            dst = f.replace("PROJECT_NAME", dst_project_name)
            if os.path.exists(os.path.join(dst_project_dir, dst)):
                replace_string(
                    os.path.join(dst_project_dir, dst), src_project_name, dst_project_name)
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT',
                                                               os.path.join(dst_project_dir, dst)))

    def project_replace_package_name(self, v):
        """ will modify the content of the file
        """
        dst_project_dir = self.project_dir
        dst_project_name = self.project_name
        src_package_name = v['src_package_name']
        dst_package_name = self.package_name
        if dst_package_name == src_package_name:
            return

        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_REPLACE_PKG_FMT',
                                                    (src_package_name, dst_package_name)))
        files = v['files']
        if not dst_package_name:
            raise cocos.CCPluginError(MultiLanguage.get_string('NEW_ERROR_PKG_NAME_NOT_SPECIFIED'),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)
        for f in files:
            dst = f.replace("PROJECT_NAME", dst_project_name)
            if os.path.exists(os.path.join(dst_project_dir, dst)):
                replace_string(
                    os.path.join(dst_project_dir, dst), src_package_name, dst_package_name)
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT',
                                                               os.path.join(dst_project_dir, dst)))

    def project_replace_mac_bundleid(self, v):
        """ will modify the content of the file
        """
        if self.mac_bundleid is None:
            return

        dst_project_dir = self.project_dir
        dst_project_name = self.project_name
        src_bundleid = v['src_bundle_id']
        dst_bundleid = self.mac_bundleid
        if src_bundleid == dst_bundleid:
            return

        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_MAC_BUNDLEID_FMT',
                                                    (src_bundleid, dst_bundleid)))
        files = v['files']
        for f in files:
            dst = f.replace("PROJECT_NAME", dst_project_name)
            if os.path.exists(os.path.join(dst_project_dir, dst)):
                replace_string(
                    os.path.join(dst_project_dir, dst), src_bundleid, dst_bundleid)
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT',
                                                               os.path.join(dst_project_dir, dst)))

    def project_replace_ios_bundleid(self, v):
        """ will modify the content of the file
        """
        if self.ios_bundleid is None:
            return

        dst_project_dir = self.project_dir
        dst_project_name = self.project_name
        src_bundleid = v['src_bundle_id']
        dst_bundleid = self.ios_bundleid
        if src_bundleid == dst_bundleid:
            return

        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_IOS_BUNDLEID_FMT',
                                                    (src_bundleid, dst_bundleid)))
        files = v['files']
        for f in files:
            dst = f.replace("PROJECT_NAME", dst_project_name)
            if os.path.exists(os.path.join(dst_project_dir, dst)):
                replace_string(
                    os.path.join(dst_project_dir, dst), src_bundleid, dst_bundleid)
            else:
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_FILE_NOT_FOUND_FMT',
                                                               os.path.join(dst_project_dir, dst)))

    def modify_files(self, v):
        """ will modify the content of the file
            format of v is :
            [
                {
                    "file_path": The path related with project directory,
                    "pattern": Find pattern,
                    "replace_string": Replaced string
                },
                ...
            ]
        """
        cocos.Logging.info(MultiLanguage.get_string('NEW_INFO_STEP_MODIFY_FILE'))
        for modify_info in v:
            modify_file = modify_info["file_path"]
            if not os.path.isabs(modify_file):
                modify_file = os.path.abspath(os.path.join(self.project_dir, modify_file))

            if not os.path.isfile(modify_file):
                cocos.Logging.warning(MultiLanguage.get_string('NEW_WARNING_NOT_A_FILE_FMT', modify_file))
                continue

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
