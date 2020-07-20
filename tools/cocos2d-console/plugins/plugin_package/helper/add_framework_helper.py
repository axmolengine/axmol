
import os
import os.path
import json
import re
import shlex
import shutil

import cocos
from MultiLanguage import MultiLanguage

from functions import *

class AddFrameworkHelper(object):
    IOS_MAC_PROJECT_FILE_REF_BEGIN_TAG = '/\* Begin PBXFileReference section \*/'
    IOS_MAC_PROJECT_FILE_REF_END_TAG = '/\* End PBXFileReference section \*/'
    IOS_MAC_PROJECT_FILE_REF_TAG = '(' + IOS_MAC_PROJECT_FILE_REF_BEGIN_TAG + ')(.*)(' + IOS_MAC_PROJECT_FILE_REF_END_TAG + ')'
    IOS_MAC_PROJECT_MAINGROUP_TAG = '(mainGroup\s=\s)(.+)(;)'
    IOS_MAC_PROJECT_REFERENCES_TAG = 'projectReferences = \('
    IOS_MAC_PBXGROUP_TAG = '(/\* Begin PBXGroup section \*/)(.*)(/\* End PBXGroup section \*/)'
    IOS_MAC_PBXCONTAINER_TAG = '(/\* Begin PBXContainerItemProxy section \*/)(.*)(/\* End PBXContainerItemProxy section \*/)'
    IOS_MAC_PBXPROXY_TAG = '(/\* Begin PBXReferenceProxy section \*/)(.*)(/\* End PBXReferenceProxy section \*/)'
    IOS_MAC_PBXBUILD_TAG = '(/\* Begin PBXBuildFile section \*/)(.*)(/\* End PBXBuildFile section \*/)'
    MAC_PBXFRAMEWORKBUILDPHASE_TAG = '\S+ /\* libcocos2d Mac.a in Frameworks \*/,'
    IOS_PBXFRAMEWORKBUILDPHASE_TAG = '\S+ /\* libcocos2d iOS.a in Frameworks \*/,'

    IOS_HEADER_MATCH_TAG = '(\$\(_COCOS_HEADER_IOS_BEGIN\))(.+)(\$\(_COCOS_HEADER_IOS_END\))'
    IOS_LIB_BEGIN_TAG = '\$\(_COCOS_LIB_IOS_BEGIN\)'
    IOS_LIB_END_TAG = '\$\(_COCOS_LIB_IOS_END\)'

    MAC_HEADER_MATCH_TAG = '(\$\(_COCOS_HEADER_MAC_BEGIN\))(.+)(\$\(_COCOS_HEADER_MAC_END\))'
    MAC_LIB_BEGIN_TAG = '\$\(_COCOS_LIB_MAC_BEGIN\)'
    MAC_LIB_END_TAG = '\$\(_COCOS_LIB_MAC_END\)'

    WIN32_HEADER_MATCH_TAG = '(\$\(_COCOS_HEADER_WIN32_BEGIN\))(.+)(\$\(_COCOS_HEADER_WIN32_END\))'
    WIN32_LIB_PATH_MATCH_TAG = '(\$\(_COCOS_LIB_PATH_WIN32_BEGIN\))(.+)(\$\(_COCOS_LIB_PATH_WIN32_END\))'
    WIN32_LIB_MATCH_TAG = '(\$\(_COCOS_LIB_WIN32_BEGIN\))(.+)(\$\(_COCOS_LIB_WIN32_END\))'

    ANDROID_HEADER_BEGIN_TAG = '# _COCOS_HEADER_ANDROID_BEGIN'
    ANDROID_HEADER_END_TAG = '# _COCOS_HEADER_ANDROID_END'
    ANDROID_HEADER_PREFIX = 'LOCAL_C_INCLUDES'
    ANDROID_LIB_BEGIN_TAG = '# _COCOS_LIB_ANDROID_BEGIN'
    ANDROID_LIB_END_TAG = '# _COCOS_LIB_ANDROID_END'
    ANDROID_LIB_PREFIX = 'LOCAL_STATIC_LIBRARIES'
    ANDROID_LIB_IMPORT_BEGIN_TAG = '# _COCOS_LIB_IMPORT_ANDROID_BEGIN'
    ANDROID_LIB_IMPORT_END_TAG = '# _COCOS_LIB_IMPORT_ANDROID_END'


    def __init__(self, project, package_data):
        self._package_name = package_data["name"]
        self._package_version = package_data["version"]
        self._package_path = project["packages_dir"] + os.sep + package_data["name"] + '-' + package_data["version"]
        self._install_json_path = self._package_path + os.sep + "install.json"
        f = open(self._install_json_path, "rb")
        self._commands = json.load(f)
        self._project = project
        f.close()
        self._uninstall_json_path = self._package_path + os.sep + "uninstall.json"
        self.get_uninstall_info()

    def run(self):
        for command in self._commands:
            try:
                name = "do_" + command["command"]
                cmd = getattr(self, name)
            except AttributeError:
                raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_CMD_NOT_FOUND_FMT', name),
                                          cocos.CCPluginError.ERROR_CMD_NOT_FOUND)

            try:
                cmd(command)
            except Exception as e:
                raise cocos.CCPluginError(str(e), cocos.CCPluginError.ERROR_OTHERS)

    def do_add_entry_function(self, command):
        self.add_entry_function(command)

    def do_add_files_and_dir(self, command):
        self.add_files_and_dir(command)

    def do_add_system_framework(self, command):
        self.add_system_framework(command)

    def do_add_project(self, command):
        platform = command["platform"]
        name = "do_add_project_on_" + platform
        cmd = getattr(self, name)
        cmd(command)

    def do_add_lib(self, command):
        platforms = command["platform"]
        for platform in platforms:
            name = "do_add_lib_on_" + platform
            cmd = getattr(self, name)
            cmd(command)

    def do_add_header_path(self, command):
        platforms = command["platform"]
        for platform in platforms:
            name = "do_add_header_path_on_" + platform
            cmd = getattr(self, name)
            cmd(command)

    def do_add_header_path_on_ios(self, command):
        self.add_header_path_on_ios_mac(command["source"].encode('UTF-8'), "ios")

    def do_add_header_path_on_mac(self, command):
        self.add_header_path_on_ios_mac(command["source"].encode('UTF-8'), "mac")

    def do_add_header_path_on_win(self, command):
        source = command["source"].encode('UTF-8')
        tag = self.__class__.WIN32_HEADER_MATCH_TAG
        self.do_add_header_lib_on_win(source, tag)

    def do_add_header_path_on_android(self, command):
        source = command["source"].encode('UTF-8')
        begin_tag = self.__class__.ANDROID_HEADER_BEGIN_TAG
        end_tag = self.__class__.ANDROID_HEADER_END_TAG
        prefix_tag = self.__class__.ANDROID_HEADER_PREFIX
        self.do_add_header_lib_on_android(source, begin_tag, end_tag, prefix_tag)

    def add_header_path_on_ios_mac(self, source, platform):
        if platform == "ios":
            tag = self.__class__.IOS_HEADER_MATCH_TAG
        elif platform == "mac":
            tag = self.__class__.MAC_HEADER_MATCH_TAG
        else:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ERROR_INVALID_PLATFORM_FMT', platform),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)

        workdir, proj_file_path, lines = self.load_proj_ios_mac()
        contents = []
        tag_found = False
        for line in lines:
            match = re.search(tag, line)
            if match is None:
                contents.append(line)
            else:
                includes = shlex.split(match.group(2))
                headers = []
                for include in includes:
                    include = self.get_ios_mac_path(workdir, include)
                    headers.append(include)

                str_to_add = self.get_ios_mac_path(workdir, source)
                headers.append(str_to_add)
                headers = list(set(headers))
                start, end = match.span(0)
                parts = []
                parts.append(line[:start])
                parts.append(match.group(1))
                parts.append(' ')
                for header in headers:
                    if header.find(' ') != -1:
                        header = '"' + header + '"'
                    parts.append(header)
                    parts.append(' ')
                parts.append(match.group(3))
                parts.append(line[end:])
                contents.append(''.join(parts))
                tag_found = True

        if tag_found == False:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("header", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            uninst_info = {
                'file': proj_file_path,
                'tags': [tag],
                'type': "header",
                'platform': "ios_mac",
                'workdir': workdir,
                'string':str_to_add
            }
            self.append_uninstall_info(uninst_info)
            self.update_file_content(proj_file_path, contents, True)

    def add_entry_function(self, command):
        declare_str = command["declare"]
        find_tag = '(\S*\s*)(\S*)(\(.*\);)'
        match = re.search(find_tag, declare_str)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ENTRY_DECLARE_FAILED'),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            str_to_add = 'extern ' + declare_str + '\n\t' + match.group(2) + '();' + '\n\t'

        file_path, all_text = self.load_appdelegate_file()
        find_tag = '(static int register_all_packages\(\)\s*\{.*)(return 0; //flag for packages manager\s*\})'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ERROR_IN_FILE_FMT', file_path),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add entry funtion
            split_index = match.end(1)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':file_path, 'string':str_to_add})

        self.update_file_content(file_path, all_text)

    def add_files_and_dir(self, command):
        backup_flag = command["backup_if_override"]
        package_name = self._package_name
        file_list = command["source"]
        src_dir = self._package_path + os.sep + command["src_dir"]
        dst_dir = self._project["path"] + os.sep + command["dst_dir"]

        for filename in file_list:
            src = src_dir + os.sep + filename
            dst = dst_dir + os.sep + filename

            if os.path.exists(dst):
                if backup_flag:
                    bak = dst + "_bak_by_" + package_name
                    if not os.path.exists(bak):
                        os.rename(dst, bak)
                        self.append_uninstall_info({'bak_file':bak, 'ori_file':dst})
                        self.save_uninstall_info()
                    else:
                        print MultiLanguage.get_string('PACKAGE_UNABLE_COPY_FMT', dst)
                        continue
                else:
                    if os.path.isdir(dst):
                        shutil.rmtree(dst)
                    else:
                        os.remove(dst)
            else:
                ensure_directory(os.path.dirname(dst))

            if os.path.isdir(src):
                shutil.copytree(src, dst)
            else:
                shutil.copy(src, dst)

    def add_system_framework(self, command):
        framework_name = command["name"].encode('UTF-8')
        file_id = command["file_id"].encode('UTF-8')
        file_path = command["path"].encode('UTF-8')
        sourceTree = command["sourceTree"].encode('UTF-8')
        framework_id = command["id"].encode('UTF-8')
        platform = command["platform"].encode('UTF-8')

        workdir, proj_pbx_path, all_text = self.load_proj_ios_mac(True)
        find_tag = self.__class__.IOS_MAC_PROJECT_FILE_REF_TAG
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXFileReference",platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add PBXFileReference of framework
            split_index = match.end(1)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = '\n\t\t' + file_id
            str_to_add += ' /* ' + framework_name + ' */ = {'
            str_to_add += 'isa = PBXFileReference; '
            str_to_add += 'lastKnownFileType = wrapper.framework; '
            str_to_add += 'name = ' + framework_name + '; '
            str_to_add += 'path = ' + file_path + '; '
            str_to_add += 'sourceTree = ' + sourceTree + '; '
            str_to_add += '};'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PBXBUILD_TAG
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXBuildFile", "platform")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add framework to PBXBuildFile
            split_index = match.start(3)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            skip_str = '\t\t'
            str_to_add = skip_str + framework_id + ' /* ' + framework_name + ' in Frameworks */ = {'
            str_to_add += 'isa = PBXBuildFile; '
            str_to_add += 'fileRef = ' + file_id + ' /* ' + framework_name + ' */; };\n'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        if platform == 'mac':
            find_tag = self.__class__.MAC_PBXFRAMEWORKBUILDPHASE_TAG
        else:
            find_tag = self.__class__.IOS_PBXFRAMEWORKBUILDPHASE_TAG
        match = re.search(find_tag, all_text)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXFrameworksBuildPhase", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add framework to PBXFrameworksBuildPhase
            split_index = match.start()
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = framework_id + ' /* ' + framework_name + ' in Frameworks */,\n\t\t\t\t'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        self.update_file_content(proj_pbx_path, all_text)

    def do_add_project_on_win(self, command):
        proj_name = command["name"].encode('UTF-8')
        project_id = command["project_id"].encode('UTF-8')
        build_id = command["build_id"].encode('UTF-8')
        project_path = '..\\..\\..\\packages\\' + self._package_name + '-' + self._package_version + '\\proj.win32\\' + proj_name + '.vcxproj'

        workdir, proj_pbx_path, all_text = self.load_sln_win32(True)
        if all_text is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ERROR_READ_SLN'),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)

        find_tag = '(EndProject)(\s*)(Global)'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("project", "win")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add project
            split_index = match.end(2)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = 'Project("{' + project_id + '}") = "' + proj_name + '", '
            str_to_add += '"' + project_path + '", "{' + build_id + '}"\n'
            str_to_add += 'EndProject\n'
            str_to_add = str_to_add.encode('ascii')
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = '(GlobalSection\(ProjectConfigurationPlatforms\) = postSolution)(.*)(Release|Win32)(\s*)(EndGlobalSection)'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("config", "win")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add build config
            split_index = match.end(4)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = '\t{' + build_id + '}.Debug|Win32.ActiveCfg = Debug|Win32\n'
            str_to_add += '\t\t{' + build_id + '}.Debug|Win32.Build.0 = Debug|Win32\n'
            str_to_add += '\t\t{' + build_id + '}.Release|Win32.ActiveCfg = Release|Win32\n'
            str_to_add += '\t\t{' + build_id + '}.Release|Win32.Build.0 = Release|Win32\n'
            str_to_add += '\t'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        self.update_file_content(proj_pbx_path, all_text)

        workdir, proj_pbx_path, all_text = self.load_proj_win32(True)
        if all_text is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_WIN_PROJ_NOT_FOUND'),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)

        find_tag = '(<ItemGroup>)(\s*)(<ProjectReference Include=)'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("ProjectReference", "win")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add ProjectReference
            split_index = match.end(2)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = '<ProjectReference Include="' + project_path + '">\n'
            str_to_add += '      <Project>{' + build_id + '}</Project>\n'
            str_to_add += '    </ProjectReference>\n    '
            str_to_add = str_to_add.encode('ascii')
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        self.update_file_content(proj_pbx_path, all_text)

    def do_add_project_on_android(self, command):
        proj_name = command["name"].encode('UTF-8')

        build_cfg_file = self.get_build_cfg_json_path()
        if build_cfg_file is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_BUILD_CFG_NOT_FOUND'),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)
        f = open(build_cfg_file, "rb")
        configs = json.load(f)
        f.close()
        if not isinstance(configs["ndk_module_path"], list):
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_NDK_MODULE_NOT_FOUND'),
                                      cocos.CCPluginError.ERROR_WRONG_CONFIG)
        moudle_path = '../../../packages/' + self._package_name + '-' + self._package_version
        configs["ndk_module_path"].append(moudle_path)
        self.append_uninstall_info({'json_file':build_cfg_file, 'items':[{'key':'ndk_module_path','items':[moudle_path]}]})
        self.save_uninstall_info()
        f = open(build_cfg_file, "w+b")
        str = json.dump(configs, f)
        f.close()

        workdir, proj_pbx_path, all_text = self.load_proj_android(True)

        find_tag = '(' + self.__class__.ANDROID_LIB_BEGIN_TAG+ ')(.*)(' + self.__class__.ANDROID_LIB_END_TAG + ')'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("lib", "android")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add project
            split_index = match.end(2)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = 'LOCAL_STATIC_LIBRARIES += ' + proj_name + '_static\n'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = '(' + self.__class__.ANDROID_LIB_IMPORT_BEGIN_TAG+ ')(.*)(' + self.__class__.ANDROID_LIB_IMPORT_END_TAG + ')'
        match = re.search(find_tag, all_text, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("lib", "android")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add import moudle
            split_index = match.end(2)
            headers = all_text[0:split_index]
            tails = all_text[split_index:]
            str_to_add = '$(call import-module,proj.android)\n'
            all_text = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        self.update_file_content(proj_pbx_path, all_text)

    def do_add_project_on_ios_mac(self, command):
        proj_name = command["name"].encode('UTF-8')
        pbx_id = command["id"].encode('UTF-8')

        mac_lib = command["mac_lib"]
        mac_lib_remote = mac_lib["remoteGlobalIDString"].encode('UTF-8')
        mac_lib_info = mac_lib["remoteInfo"]
        mac_lib_name = 'lib' + mac_lib_info + '.a'
        mac_lib_container = mac_lib["container"].encode('UTF-8')
        mac_lib_id = mac_lib["lib_id"].encode('UTF-8')
        mac_lib_build = mac_lib["build_id"].encode('UTF-8')

        ios_lib = command["ios_lib"]
        ios_lib_remote = ios_lib["remoteGlobalIDString"].encode('UTF-8')
        ios_lib_info = ios_lib["remoteInfo"]
        ios_lib_name = 'lib' + ios_lib_info + '.a'
        ios_lib_container = ios_lib["container"].encode('UTF-8')
        ios_lib_id = ios_lib["lib_id"].encode('UTF-8')
        ios_lib_build = ios_lib["build_id"].encode('UTF-8')

        productGroup_id = command["ProductGroup"].encode('UTF-8')

        platform = 'ios_mac'
        workdir, proj_pbx_path, lines = self.load_proj_ios_mac()

        begin_tag = self.__class__.IOS_MAC_PROJECT_FILE_REF_BEGIN_TAG
        end_tag = self.__class__.IOS_MAC_PROJECT_FILE_REF_END_TAG
        contents = []
        contents_str = ''
        file_ref_begin = False
        tag_found = False
        for line in lines:
            if file_ref_begin == False:
                contents.append(line)
                contents_str = contents_str + line
                match = re.search(begin_tag, line)
                if not match is None:
                    file_ref_begin = True
                    tag_found = True
            else:
                match = re.search(end_tag, line)
                if match is None:
                    contents.append(line)
                    contents_str = contents_str + line
                else:
                    # add PBXFileReference of project
                    file_ref_string = '\t\t' + pbx_id + ' /* ' + proj_name + '.xcodeproj */ = '
                    file_ref_string += '{isa = PBXFileReference; lastKnownFileType = "wrapper.pb-project"; name = '
                    file_ref_string += proj_name + '.xcodeproj; path = "../../../packages/'
                    file_ref_string += self._package_name + '-' + self._package_version + '/proj.ios_mac/'
                    file_ref_string += proj_name + '.xcodeproj"; sourceTree = "<group>"; };\n'
                    self.append_uninstall_info({'file':proj_pbx_path, 'string':file_ref_string})
                    contents.append(file_ref_string)
                    contents_str = contents_str + file_ref_string
                    contents.append(line)
                    contents_str = contents_str + line

        if tag_found == False:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXFileReference", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)

        # get id of mainGroup
        main_tag = self.__class__.IOS_MAC_PROJECT_MAINGROUP_TAG
        match = re.search(main_tag, contents_str)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("main group", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            main_group_id = match.group(2)

        find_tag = '(' + main_group_id + '\s=\s\{\s*isa\s=\sPBXGroup;\s*children\s=\s\()(\s*)(\S*\s/\*\s\S*\s\*/,\s*)+(\);.*\};)'
        match = re.search(find_tag, contents_str, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("children of main group", "platform")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add project to mainGroup
            split_index = match.end(1)
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = match.group(2)
            str_to_add = skip_str + pbx_id + ' /* ' + proj_name + '.xcodeproj */,'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PBXCONTAINER_TAG
        match = re.search(find_tag, contents_str, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXContainerItemProxy", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add PBXContainerItemProxy
            split_index = match.start(3)
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = '\t\t'
            str_to_add = skip_str + mac_lib_container + ' /* PBXContainerItemProxy */ = {\n'
            str_to_add += skip_str + '\tisa = PBXContainerItemProxy;\n'
            str_to_add += skip_str + '\tcontainerPortal = ' + pbx_id + ' /* ' + proj_name + '.xcodeproj */;\n'
            str_to_add += skip_str + '\tproxyType = 2;\n'
            str_to_add += skip_str + '\tremoteGlobalIDString = ' + mac_lib_remote + ';\n'
            str_to_add += skip_str + '\tremoteInfo = "' + mac_lib_info + '";\n'
            str_to_add += skip_str + '};\n'
            str_to_add += skip_str + ios_lib_container + ' /* PBXContainerItemProxy */ = {\n'
            str_to_add += skip_str + '\tisa = PBXContainerItemProxy;\n'
            str_to_add += skip_str + '\tcontainerPortal = ' + pbx_id + ' /* ' + proj_name + '.xcodeproj */;\n'
            str_to_add += skip_str + '\tproxyType = 2;\n'
            str_to_add += skip_str + '\tremoteGlobalIDString = ' + ios_lib_remote + ';\n'
            str_to_add += skip_str + '\tremoteInfo = "' + ios_lib_info + '";\n'
            str_to_add += skip_str + '};\n'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PBXPROXY_TAG
        match = re.search(find_tag, contents_str, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXReferenceProxy", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add PBXReferenceProxy
            split_index = match.start(3)
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = '\t\t'
            str_to_add = skip_str + mac_lib_id + ' /* ' + mac_lib_name + ' */ = {\n'
            str_to_add += skip_str + '\tisa = PBXReferenceProxy;\n'
            str_to_add += skip_str + '\tfileType = archive.ar;\n'
            str_to_add += skip_str + '\tpath = "' + mac_lib_name + '";\n'
            str_to_add += skip_str + '\tremoteRef = ' + mac_lib_container + ' /* PBXContainerItemProxy */;\n'
            str_to_add += skip_str + '\tsourceTree = BUILT_PRODUCTS_DIR;\n'
            str_to_add += skip_str + '};\n'
            str_to_add += skip_str + ios_lib_id + ' /* ' + ios_lib_name + ' */ = {\n'
            str_to_add += skip_str + '\tisa = PBXReferenceProxy;\n'
            str_to_add += skip_str + '\tfileType = archive.ar;\n'
            str_to_add += skip_str + '\tpath = "' + ios_lib_name + '";\n'
            str_to_add += skip_str + '\tremoteRef = ' + ios_lib_container + ' /* PBXContainerItemProxy */;\n'
            str_to_add += skip_str + '\tsourceTree = BUILT_PRODUCTS_DIR;\n'
            str_to_add += skip_str + '};\n'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PBXGROUP_TAG
        match = re.search(find_tag, contents_str, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXGroup", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add ProductGroup of project to PBXGroup
            split_index = match.end(1)
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = '\n\t\t'
            str_to_add = skip_str + productGroup_id + ' /* Products */ = {'
            str_to_add += skip_str + '\tisa = PBXGroup;'
            str_to_add += skip_str + '\tchildren = ('
            str_to_add += skip_str + '\t\t' + mac_lib_id + ' /* ' + mac_lib_name + ' */,'
            str_to_add += skip_str + '\t\t' + ios_lib_id + ' /* ' + ios_lib_name + ' */,'
            str_to_add += skip_str + '\t);'
            str_to_add += skip_str + '\tname = Products;'
            str_to_add += skip_str + '\tsourceTree = "<group>";'
            str_to_add += skip_str + '};'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PROJECT_REFERENCES_TAG
        match = re.search(find_tag, contents_str)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("projectReferences", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add ProductGroup & project to projectReferences
            split_index = match.end()
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = '\n\t\t\t\t'
            str_to_add = skip_str + '{'
            str_to_add += skip_str + '\tProductGroup = ' + productGroup_id + ' /* Products */;'
            str_to_add += skip_str + '\tProjectRef = ' + pbx_id + ' /* ' + proj_name + '.xcodeproj */;'
            str_to_add += skip_str + '},'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_MAC_PBXBUILD_TAG
        match = re.search(find_tag, contents_str, re.DOTALL)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("PBXBuildFile", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add lib to PBXBuildFile
            split_index = match.start(3)
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            skip_str = '\t\t'
            str_to_add = skip_str + mac_lib_build + ' /* ' + mac_lib_name + ' in Frameworks */ = {'
            str_to_add += 'isa = PBXBuildFile; '
            str_to_add += 'fileRef = ' + mac_lib_id + ' /* ' + mac_lib_name + ' */; };\n'
            str_to_add += skip_str + ios_lib_build + ' /* ' + ios_lib_name + ' in Frameworks */ = {'
            str_to_add += 'isa = PBXBuildFile; '
            str_to_add += 'fileRef = ' + ios_lib_id + ' /* ' + ios_lib_name + ' */; };\n'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.MAC_PBXFRAMEWORKBUILDPHASE_TAG
        match = re.search(find_tag, contents_str)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("Mac Frameworks", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add mac lib to PBXFrameworksBuildPhase
            split_index = match.start()
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            str_to_add = mac_lib_build + ' /* ' + mac_lib_name + ' in Frameworks */,\n\t\t\t\t'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        find_tag = self.__class__.IOS_PBXFRAMEWORKBUILDPHASE_TAG
        match = re.search(find_tag, contents_str)
        if match is None:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("iOS Frameworks", platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            # add ios lib to PBXFrameworksBuildPhase
            split_index = match.start()
            headers = contents_str[0:split_index]
            tails = contents_str[split_index:]
            str_to_add = ios_lib_build + ' /* ' + ios_lib_name + ' in Frameworks */,\n\t\t\t\t'
            contents_str = headers + str_to_add + tails
            self.append_uninstall_info({'file':proj_pbx_path, 'string':str_to_add})

        self.update_file_content(proj_pbx_path, contents_str)

    def do_add_lib_on_ios(self, command):
        self.add_lib_on_ios_mac(command["source"].encode('UTF-8'), "ios")

    def do_add_lib_on_mac(self, command):
        self.add_lib_on_ios_mac(command["source"].encode('UTF-8'), "mac")

    def do_add_lib_on_win(self, command):
        source = command["source"].encode('UTF-8')
        self.do_add_header_lib_on_win(os.path.basename(source), self.__class__.WIN32_LIB_MATCH_TAG)
        self.do_add_header_lib_on_win(os.path.dirname(source), self.__class__.WIN32_LIB_PATH_MATCH_TAG)

    def do_add_header_lib_on_win(self, source, tag):
        workdir, proj_file_path, lines = self.load_proj_win32()
        contents = []
        tag_found = False
        for line in lines:
            match = re.search(tag, line)
            if match is None:
                contents.append(line)
            else:
                includes = re.split(';', match.group(2))
                headers = []
                for include in includes:
                    include = self.get_win32_path(workdir, include)
                    if include is not None:
                        headers.append(include)

                str_to_add = self.get_win32_path(workdir, source)
                headers.append(str_to_add)
                headers = list(set(headers))
                start, end = match.span(0)
                parts = []
                parts.append(line[:start])
                parts.append(match.group(1))
                parts.append(';')
                for header in headers:
                    if header.find(' ') != -1:
                        header = '"' + header + '"'
                    parts.append(header)
                    parts.append(';')
                parts.append(match.group(3))
                parts.append(line[end:])
                contents.append(''.join(parts))
                tag_found = True

        if tag_found == False:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("header", "win32")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            uninst_info = {
                'file': proj_file_path,
                'tags': [tag],
                'type': "lib",
                'platform': "win",
                'workdir': workdir,
                'string':str_to_add
            }
            self.append_uninstall_info(uninst_info)
            self.update_file_content(proj_file_path, contents, True)

    def do_add_lib_on_android(self, command):
        source = command["source"].encode('UTF-8')
        begin_tag = self.__class__.ANDROID_LIB_BEGIN_TAG
        end_tag = self.__class__.ANDROID_LIB_END_TAG
        prefix_tag = self.__class__.ANDROID_LIB_PREFIX
        self.do_add_header_lib_on_android(source, begin_tag, end_tag, prefix_tag)

        source = command["import-module"].encode('UTF-8')
        begin_tag = self.__class__.ANDROID_LIB_IMPORT_BEGIN_TAG
        end_tag = self.__class__.ANDROID_LIB_IMPORT_END_TAG
        self.do_add_header_lib_on_android(source, begin_tag, end_tag, None, True)

    def do_add_header_lib_on_android(self, source, begin_tag, end_tag, prefix_tag, is_import = False):
        workdir, proj_pbx_path, lines = self.load_proj_android()
        contents = []
        lib_begin = False
        tag_found = False
        libs = []
        for line in lines:
            if lib_begin == False:
                contents.append(line)
                match = re.search(begin_tag, line)
                if not match is None:
                    lib_begin = True
                    tag_found = True
            else:
                if prefix_tag is not None:
                    match = re.search(prefix_tag, line)
                    if match is not None:
                        continue

                match = re.search(end_tag, line)
                if match is None:
                    libs.append(self.get_android_path(workdir, line, is_import))
                else:
                    # add new lib to libs
                    str_to_add = self.get_android_path(workdir, source, is_import)
                    libs.append(str_to_add)
                    libs = list(set(libs))
                    count = len(libs)
                    cur = 1
                    if count > 0 and prefix_tag is not None:
                        contents.append(prefix_tag)
                        contents.append(" += \\\n")
                    for lib in libs:
                        if cur < count and prefix_tag is not None:
                            contents.append('    ' + lib + ' \\')
                        elif is_import is False:
                            contents.append('    ' + lib)
                        else:
                            contents.append('$(call import-module,')
                            contents.append(lib)
                            contents.append(')')
                        contents.append("\n")

                    libs = []
                    lib_begin = False
                    contents.append(line)

        if tag_found == False:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("lib", "android")),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            uninst_info = {
                'file': proj_pbx_path,
                'tags': [begin_tag, end_tag, prefix_tag],
                'type': "lib",
                'platform': "android",
                'workdir': workdir,
                'is_import': is_import,
                'string':str_to_add
            }
            self.append_uninstall_info(uninst_info)
            self.update_file_content(proj_pbx_path, contents, True)

    def add_lib_on_ios_mac(self, source, platform):
        if platform == "ios":
            begin_tag = self.__class__.IOS_LIB_BEGIN_TAG
            end_tag = self.__class__.IOS_LIB_END_TAG
        elif platform == "mac":
            begin_tag = self.__class__.MAC_LIB_BEGIN_TAG
            end_tag = self.__class__.MAC_LIB_END_TAG
        else:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ERROR_INVALID_PLATFORM_FMT', platform),
                                      cocos.CCPluginError.ERROR_WRONG_ARGS)

        workdir, proj_pbx_path, lines = self.load_proj_ios_mac()
        contents = []
        lib_begin = False
        tag_found = False
        libs = []
        for line in lines:
            if lib_begin == False:
                contents.append(line)
                match = re.search(begin_tag, line)
                if not match is None:
                    lib_begin = True
                    tag_found = True
            else:
                match = re.search(end_tag, line)
                if match is None:
                    libs.append(self.get_ios_mac_path(workdir, line))
                else:
                    # add new lib to libs
                    str_to_add = self.get_ios_mac_path(workdir, source)
                    libs.append(str_to_add)
                    libs = list(set(libs))
                    for lib in libs:
                        contents.append('\t\t\t\t\t"' + lib + '",\n')

                    libs = []
                    lib_begin = False
                    contents.append(line)

        if tag_found == False:
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_TAG_NOT_FOUND_FMT',
                                      ("lib",  platform)),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)
        else:
            uninst_info = {
                'file': proj_pbx_path,
                'tags': [begin_tag, end_tag],
                'type': "lib",
                'platform': "ios_mac",
                'workdir': workdir,
                'string':str_to_add
            }
            self.append_uninstall_info(uninst_info)
            self.update_file_content(proj_pbx_path, contents, True)

    def get_ios_mac_path(self, project_path, source):
        source = source.strip(',"\t\n\r')
        if not source[:10] == '$(SRCROOT)':
            source = '$(SRCROOT)' + os.sep + os.path.relpath(self._project["packages_dir"] + os.sep + source,
                                                             project_path)

        return source.replace(os.sep, '/')

    def get_win32_path(self, project_path, source):
        if source == ";" or source == "":
            return None

        if source.find('\\') == -1 and source.find('/') == -1:
            return source

        source = source.strip(',"\t\n\r')
        if not source[:13] == '$(ProjectDir)':
            source = '$(ProjectDir)' + os.sep + os.path.relpath(self._project["packages_dir"] + os.sep + source,
                                                             project_path)

        return source.replace('/', '\\')

    def get_android_path(self, project_path, source, ignore_local_path):
        source = source.strip(' ,"\t\n\r')

        if source.find('\\') == -1 and source.find('/') == -1:
            return source

        if source[-2:] == ' \\':
            source = source[0:-2]
        if source[:21] == '$(call import-module,':
            # strip "$(call import-module, ../../../../packages/"
            source = source[21:-1].strip('./\\')
            if source[:8] == "packages":
                source = source[9:]
        if not source[:13] == '$(LOCAL_PATH)':
            source = os.path.relpath(self._project["packages_dir"] + os.sep + source, project_path)
            if ignore_local_path is False:
                source = '$(LOCAL_PATH)' + os.sep + source

        return source

    def load_proj_ios_mac(self, notSplitLines = False):
        if not "proj.ios_mac" in self._project:
            print MultiLanguage.get_string('PACKAGE_MAC_NOT_FOUND')
            return

        workdir = self._project["proj.ios_mac"]
        files = os.listdir(workdir)
        for filename in files:
            if filename[-10:] == ".xcodeproj":
                proj_dir = filename
                break

        if proj_dir is None:
            print MultiLanguage.get_string('PACKAGE_XCODE_PROJ_NOT_FOUND')
            return

        if not os.path.isdir(workdir + os.sep + proj_dir):
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_NOT_XCODE_PROJ_FMT', proj_dir),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        proj_file_path = workdir + os.sep + proj_dir + os.sep + "project.pbxproj"
        f = open(proj_file_path, "rb")
        if notSplitLines == True:
            lines = f.read()
        else:
            lines = f.readlines()
        f.close()

        return workdir, proj_file_path, lines

    def load_sln_win32(self, notSplitLines = False):
        if not "proj.win32" in self._project:
            print MultiLanguage.get_string('PACKAGE_ERROR_WIN32_NOT_FOUND')
            return

        workdir = self._project["proj.win32"]
        files = os.listdir(workdir)
        for filename in files:
            if filename[-4:] == ".sln":
                proj_file_path = workdir + os.sep +  filename
                break

        if proj_file_path is None:
            print MultiLanguage.get_string('PACKAGE_ERROR_NO_SLN_IN_WIN32')
            return

        f = open(proj_file_path, "rb")
        if notSplitLines == True:
            lines = f.read()
        else:
            lines = f.readlines()
        f.close()

        return workdir, proj_file_path, lines

    def load_proj_win32(self, notSplitLines = False):
        if not "proj.win32" in self._project:
            print MultiLanguage.get_string('PACKAGE_ERROR_WIN32_NOT_FOUND')
            return

        workdir = self._project["proj.win32"]
        files = os.listdir(workdir)
        for filename in files:
            if filename[-8:] == ".vcxproj":
                proj_file_path = workdir + os.sep +  filename
                break

        if proj_file_path is None:
            print MultiLanguage.get_string('PACKAGE_VS_PROJ_NOT_FOUND')
            return

        f = open(proj_file_path, "rb")
        if notSplitLines == True:
            lines = f.read()
        else:
            lines = f.readlines()
        f.close()

        return workdir, proj_file_path, lines

    def load_proj_android(self, notSplitLines = False):
        if not "proj.android" in self._project:
            print MultiLanguage.get_string('PACKAGE_ANDROID_PROJ_NOT_FOUND')
            return

        workdir = self._project["proj.android"] + os.sep + "jni"
        proj_file_path = workdir + os.sep + "Android.mk"
        if not os.path.isfile(proj_file_path):
            print MultiLanguage.get_string('PACKAGE_ANDROID_MK_NOT_FOUND')
            return

        f = open(proj_file_path, "rb")
        if notSplitLines == True:
            lines = f.read()
        else:
            lines = f.readlines()
        f.close()

        return workdir, proj_file_path, lines

    def load_appdelegate_file(self):
        file_path = self._project["classes_dir"] + os.sep + "AppDelegate.cpp"
        if not os.path.isfile(file_path):
            print MultiLanguage.get_string('PACKAGE_APPDELEGATE_NOT_FOUND')
            return

        f = open(file_path, "rb")
        all_text = f.read()
        f.close()

        return file_path, all_text

    def get_build_cfg_json_path(self):
        file_path = self._project["proj.android"] + os.sep + "build-cfg.json"
        if not os.path.isfile(file_path):
            print MultiLanguage.get_string('PACKAGE_BUILD_CFG_NOT_FOUND')
            return

        return file_path

    def get_uninstall_info(self):
        file_path = self._uninstall_json_path
        if os.path.isfile(file_path):
            f = open(file_path, "rb")
            self._uninstall_info = json.load(f)
            f.close()
        else:
            self._uninstall_info = []
            self.save_uninstall_info()

    def save_uninstall_info(self):
        f = open(self._uninstall_json_path, "w+b")
        json.dump(self._uninstall_info, f)
        f.close()

    def append_uninstall_info(self, info):
        self._uninstall_info.append(info)

    def update_file_content(self, file, text, isLines = False):
        self.save_uninstall_info()
        f = open(file, "wb")
        if isLines:
            f.writelines(text)
        else:
            f.write(text)
        f.close()
