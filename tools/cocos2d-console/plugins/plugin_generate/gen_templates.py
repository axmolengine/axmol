#!/usr/bin/python
#-*- coding: UTF-8 -*-

import os
import sys
import json
import utils
import modify_template
import re
import cocos

from MultiLanguage import MultiLanguage
from cocos import CCPluginError, Logging

from argparse import ArgumentParser

class TemplateGenerator(cocos.CCPlugin):

    CONFIG_FILE = "configs/gen_templates_config.json"

    KEY_COPY_FROM_ENGINE = 'copy_from_engine'
    KEY_COPY_FROM_BIN_TEMP = 'copy_from_bin_templates'
    KEY_WIN32_CFG = 'win32_cfg'
    KEY_XCODE_PROJS = 'xcode_projs'
    KEY_BUILD_CFG_FILES = 'build_cfg_files'
    KEY_VS_PROJS = 'vs_projs'

    KEY_RM_COPY_RES = 'rm_copy_res'
    KEY_REPLACE_STRING = 'replace_string'
    KEY_MODIFY_CFG = 'modify_files'

    @staticmethod
    def plugin_name():
      return "gen-templates"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('GEN_TEMP_BRIEF')

    def parse_args(self, argv):
        parser = ArgumentParser(prog="cocos %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())
        (args, unknown) = parser.parse_known_args()
        self.init(args)

        return args

    def init(self, args):
        if getattr(sys, 'frozen', None):
            self.cur_dir = os.path.realpath(os.path.dirname(sys.executable))
            self.engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir)
        else:
            self.cur_dir = os.path.realpath(os.path.dirname(__file__))
            self.engine_path = os.path.join(self.cur_dir, os.pardir, os.pardir, os.pardir, os.pardir)
        self.engine_path = os.path.normpath(self.engine_path)

        # get path variables
        self.lib_dir = os.path.join(self.engine_path, 'prebuilt')
        self.engine_template_dir = os.path.join(self.engine_path, "templates")

        # get the template config info
        self.config_json = self.getConfigJson()

        # get engine version
        self.version = self.get_version_from_source()
        self.fw_version = self.version.replace(' ', '-')
        self._verbose = True

    def generate(self):
        self.clean_template()
        self.copy_template()
        self.gen_templates()

    def clean_template(self):
        utils.rmdir(os.path.join(self.engine_template_dir, "cpp-template-binary"))
        utils.rmdir(os.path.join(self.engine_template_dir, "lua-template-binary"))
        utils.rmdir(os.path.join(self.engine_template_dir, "js-template-binary"))

    def copy_template(self):
        for item in self.config_json[TemplateGenerator.KEY_COPY_FROM_ENGINE]:
            cocos.copy_files_with_config(item, self.engine_path, self.engine_template_dir)

        for item in self.config_json[TemplateGenerator.KEY_COPY_FROM_BIN_TEMP]:
            cocos.copy_files_with_config(item, os.path.join(self.cur_dir, 'bin-templates'), self.engine_template_dir)

    def getConfigJson(self):
        cfg_json_path = os.path.join(self.cur_dir, TemplateGenerator.CONFIG_FILE)
        f = open(cfg_json_path)
        config_json = json.load(f)
        f.close()

        return config_json

    def gen_templates(self):
        dst_dir = self.engine_template_dir
        x_path = self.engine_path
        lib_dir = self.lib_dir

        version = self.fw_version
        modifier = modify_template.TemplateModifier(x_path, lib_dir, version)

        # modify the VS project file of templates
        for proj_file in self.config_json[TemplateGenerator.KEY_VS_PROJS]:
            proj_file_path = os.path.join(self.engine_template_dir, proj_file)
            modifier.modify_vs_proj(proj_file_path)

        # modify the main.cpp in proj.win32
        self.modify_win32_maincpp()

        # modify the xcode project file of templates
        for proj_file in self.config_json[TemplateGenerator.KEY_XCODE_PROJS]:
            proj_file_path = os.path.join(self.engine_template_dir, proj_file)
            modifier.modify_xcode_proj(proj_file_path)

        # modify the build-cfg.json for templates
        self.modify_build_cfg()

        self.modify_version_json(os.path.join(dst_dir, "lua-template-binary/.settings/version.json"))

        self.gen_template_config(dst_dir, self.version)

        # modify files in config file
        self.modify_files()

    def modify_win32_maincpp(self):
        win32_cfg = self.config_json[TemplateGenerator.KEY_WIN32_CFG]
        cpp_files = win32_cfg['main_cpps']
        link_libs_cfg = win32_cfg['link_libs']

        check_pattern1 = r'^int[\s]+APIENTRY[\s]+_tWinMain'
        check_pattern2 = r'^int[\s]+WINAPI[\s]+_tWinMain'
        for main_cpp in cpp_files:
            cpp_full_path = os.path.join(self.engine_template_dir, main_cpp)

            if main_cpp.find('lua-template') >= 0:
                link_libs = link_libs_cfg['base'] + link_libs_cfg['lua']
            elif main_cpp.find('js-template') >= 0:
                link_libs = link_libs_cfg['base'] + link_libs_cfg['js']
            else:
                link_libs = link_libs_cfg['base']

            f = open(cpp_full_path)
            old_lines = f.readlines()
            f.close()

            new_lines = []
            for line in old_lines:
                strip_line = line.strip()
                if re.match(check_pattern1, strip_line) or re.match(check_pattern2, strip_line):
                    new_lines.append('#if _MSC_VER > 1800\n')
                    for lib in link_libs:
                        new_lines.append('%s\n' % self.get_lib_str(lib, 2015))
                    new_lines.append('#else\n')
                    for lib in link_libs:
                        new_lines.append('%s\n' % self.get_lib_str(lib, 2013))
                    new_lines.append('#endif\n\n')
                    new_lines.append(line)
                else:
                    new_lines.append(line)

            f = open(cpp_full_path, 'w')
            f.writelines(new_lines)
            f.close()

    def get_lib_str(self, lib_name, ver):
        base, ext = os.path.splitext(lib_name)
        ret = '#pragma comment(lib,"%s_%d%s")' % (base, ver, ext)
        return ret

    def modify_file(self, file_path, pattern, replace_str):
        f = open(file_path)
        lines = f.readlines()
        f.close()

        new_lines = []
        for line in lines:
            new_line = re.sub(pattern, replace_str, line)
            new_lines.append(new_line)

        f = open(file_path, "w")
        f.writelines(new_lines)
        f.close()

    def modify_files(self):
        modify_cfg = self.config_json[TemplateGenerator.KEY_MODIFY_CFG]
        for cfg in modify_cfg:
            file_path = cfg["file_path"]
            if not os.path.isabs(file_path):
                file_path = os.path.abspath(os.path.join(self.engine_template_dir, file_path))

            if not os.path.isfile(file_path):
                print("%s is not a file." % file_path)
                continue

            pattern = cfg["pattern"]
            replace_str = cfg["replace_string"]
            self.modify_file(file_path, pattern, replace_str)

    def modify_version_json(self, file_path):
        f = open(file_path)
        version_info = json.load(f)
        f.close()

        version_info["engineVersion"] = self.version

        f = open(file_path, "w")
        json.dump(version_info, f, sort_keys=True, indent=4)
        f.close()

    def get_version_from_source(self):
        src_engine_path = self.engine_path
        version_file_path = os.path.join(src_engine_path, "cocos/cocos2d.cpp")
        pattern = r".*return[ \t]+\"(.*)\";"

        # restore the version of engine
        ver = ""
        f = open(version_file_path)
        for line in f.readlines():
            match = re.match(pattern, line)
            if match:
                ver = match.group(1)
                break
        f.close()

        if len(ver) <= 0:
            raise CCPluginError(MultiLanguage.get_string('GEN_TEMP_ERROR_VER_NOT_FOUND_FMT', version_file_path),
                                CCPluginError.ERROR_PARSE_FILE)

        return ver

    def gen_template_config(self, template_path, engine_ver):
        for name in os.listdir(template_path):
            fullPath = os.path.join(template_path, name)
            if not os.path.isdir(fullPath):
                continue

            if not re.match(".*-template-binary", name):
                continue

            cfg_path = os.path.join(fullPath, ".cocos-project.json")
            cfg_info = {}
            if os.path.exists(cfg_path):
                f = open(cfg_path)
                cfg_info = json.load(f)
                f.close()

            cfg_info["engine_version"] = engine_ver
            cfg_info["engine_type"] = "prebuilt"

            f = open(cfg_path, "w")
            json.dump(cfg_info, f, sort_keys=True, indent=4)
            f.close()

    def rm_copy_res(self, file_path, keyword):
        f = open(file_path)
        info = json.load(f)
        f.close()

        changed = False
        for cpy_res in info['copy_resources']:
            if cpy_res['from'].find(keyword) >= 0:
                info['copy_resources'].remove(cpy_res)
                changed = True
                break

        if changed:
            f = open(file_path, 'w')
            json.dump(info, f, indent=4)
            f.close()

    def modify_project_properties(self, cfg_path):
        f = open(cfg_path)
        lines = f.readlines()
        f.close()

        new_lines = []
        pattern = r'android\.library\.reference.*'
        for line in lines:
            temp_str = line.strip()
            if not re.match(pattern, temp_str):
                new_lines.append(line)

        f = open(cfg_path, 'w')
        f.writelines(new_lines)
        f.close()

    def modify_build_cfg(self):
        build_cfg_files = self.config_json[TemplateGenerator.KEY_BUILD_CFG_FILES]
        fw_version_path = "${COCOS_X_ROOT}/%s" % self.fw_version

        for build_cfg_file in build_cfg_files:
            cfg_full_path = os.path.join(self.engine_template_dir, build_cfg_file)
            file_cfg_info = build_cfg_files[build_cfg_file]

            if file_cfg_info.has_key(TemplateGenerator.KEY_RM_COPY_RES):
                rm_copy_cfg = file_cfg_info[TemplateGenerator.KEY_RM_COPY_RES]
                for keyword in rm_copy_cfg:
                    self.rm_copy_res(cfg_full_path, keyword)

            if file_cfg_info.has_key(TemplateGenerator.KEY_REPLACE_STRING):
                replace_cfg = file_cfg_info[TemplateGenerator.KEY_REPLACE_STRING]
                f = open(cfg_full_path)
                file_content = f.read()
                f.close()

                for replace_cfg_info in replace_cfg:
                    src_str = replace_cfg_info['src_str']
                    dst_str = replace_cfg_info['dst_str']
                    dst_str = dst_str.replace('${FW_VERSION_PATH}', fw_version_path)
                    file_content = file_content.replace(src_str, dst_str)

                f = open(cfg_full_path, "w")
                f.write(file_content)
                f.close()

    def run(self, argv, dependencies):
        self.parse_args(argv)
        self.generate()
        Logging.info('')
        Logging.info(MultiLanguage.get_string('GEN_TEMP_SUCCESS_FMT', self.engine_template_dir))
