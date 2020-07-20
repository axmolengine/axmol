
import os
import os.path
import json
import re
import shlex

import cocos
from MultiLanguage import MultiLanguage


class SetFrameworkHelper(object):

    def __init__(self, project, package_name, version):
        self._package_name = package_name
        self._package_path = project["packages_dir"] + os.sep + package_name + "-" + version
        self._project = project

    def run(self):
        package_name = self._package_name
        package_path = self._package_path
        if not os.path.isdir(package_path):
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_ERROR_PATH_NOT_FOUND_FMT', package_path),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        sln_txt =  self.load_sln_win32()
        if sln_txt is None:
            print MultiLanguage.get_string('PACKAGE_ERROR_READ_SLN')
        else:
            find_tag = '(Project\(\"\{)(\S*)(\}\"\) = \"' + package_name + '\", \"\S*\", \"\{)(\S*)(\}\"\s*EndProject)'
            match = re.search(find_tag, sln_txt, re.DOTALL)
            if match is None:
                print MultiLanguage.get_string('PACKAGE_ERROR_NOT_FOUND_PROJ', package_name)
            else:
                proj_id_win = match.group(2)
                build_id_win = match.group(4)
                self.set_win32(proj_id_win, build_id_win)

    def set_win32(self, proj_id, build_id):
        text = self.load_install_json()
        if text is None:
            print MultiLanguage.get_string('PACKAGE_ERROR_JSON_READ_FAILED')
            return

        find_tag = '(\{\s*\"command\":\s*\"add_project\",\s*\"name\":\s*\"\S*\",\s*\"project_id\":\s*\")(\S*)(\",\s*\"build_id\":\s*\")(\S*)(\",\s*\"platform\":\s*\"win\"\s*\})'
        match = re.search(find_tag, text, re.DOTALL)
        if not match is None:
            old_id = match.group(2)
            text = text.replace(old_id, proj_id)
            old_id = match.group(4)
            text = text.replace(old_id, build_id)
            self.save_install_json(text)
            return

        index = text.find("[")
        if index<0:
            print MultiLanguage.get_string('PACKAGE_ERROR_JSON_ERROR')
            return

        headers = text[0:index+1]
        tails = text[index+1:]
        skip_str = '\n\t\t'
        str_to_add = '\n\t{'
        str_to_add += skip_str + '"command": "add_project",'
        str_to_add += skip_str + '"name": "' + self._package_name + '",'
        str_to_add += skip_str + '"project_id": "' + proj_id + '",'
        str_to_add += skip_str + '"build_id": "' + build_id + '",'
        str_to_add += skip_str + '"platform": "win"'
        str_to_add += '\n\t},'
        text = headers + str_to_add + tails
        self.save_install_json(text)

    def load_install_json(self):
        install_json_file = self._package_path + os.sep + "install.json"
        if not os.path.isfile(install_json_file):
            print MultiLanguage.get_string('PACKAGE_ERROR_NOT_FOUND_JSON')
            return

        f = open(install_json_file, "rb")
        text = f.read()
        f.close()

        return text

    def save_install_json(self, text):
        install_json_file = self._package_path + os.sep + "install.json"
        f = open(install_json_file, "wb")
        f.write(text)
        f.close()

    def load_sln_win32(self):
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
        text = f.read()
        f.close()

        return text
