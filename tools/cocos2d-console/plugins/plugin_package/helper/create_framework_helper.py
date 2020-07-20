
import os
import os.path
import json
import re
import shlex
import uuid
import sys

import cocos
from MultiLanguage import MultiLanguage

def get_template_path():
    if getattr(sys, 'frozen', None):
        cur_path = os.path.realpath(os.path.dirname(sys.executable))
        ret = os.path.join(cur_path, 'helper', 'template')
    else:
        cur_path = os.path.realpath(os.path.dirname(__file__))
        ret = os.path.join(cur_path, 'template')

    return ret

class CreateFrameworkHelper(object):

    def __init__(self, project, package_name):
        self._package_name = package_name
        self._package_path = project["packages_dir"] + os.sep + package_name + "-1.0"
        self._vars = {}

    def run(self):
        package_path = self._package_path
        if os.path.isdir(package_path):
            raise cocos.CCPluginError(MultiLanguage.get_string('PACKAGE_PATH_EXISTED_FMT', package_path),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)
        os.makedirs(package_path)

        self._vars["__PACKAGE_NAME__"] = self._package_name
        self.generate_uuid_string()

        template_path = get_template_path()

        self.copy_files_from_template(template_path, package_path)
        

    def copy_files_from_template(self, src_dir, dst_dir):
        files = os.listdir(src_dir)
        for filename in files:
            src = src_dir + os.sep + filename
            dst = dst_dir + os.sep + self.get_format_string(filename)
            if os.path.isdir(src):
                os.makedirs(dst)
                self.copy_files_from_template(src, dst)
            else:
                self.copy_file_from_template(src, dst)

    def copy_file_from_template(self, src, dst):
        f = open(src, "rb")
        text = f.read()
        f.close()
        text = self.get_format_string(text)

        f = open(dst, "wb")
        f.write(text)
        f.close()
        print MultiLanguage.get_string('PACKAGE_CREATE_OK_FMT', dst)

    def get_format_string(self, src_str):
        vars = self._vars
        for var in vars:
            src_str = src_str.replace(var, vars[var])

        return src_str

    def generate_uuid_string(self):
        uuid_str = uuid.uuid1().hex.upper()
        str1 = uuid_str[16:20]
        str2 = uuid_str[0:8]
        vars = self._vars
        vars["__XCODE_PROJ_ID__"] = str1 + "98D41A82028A" + str2
        vars["__MAC_LIB_ORI_ID__"] = str1 + "966C1A81DF7F" + str2
        vars["__MAC_LIB_CTN_ID__"] = str1 + "98D91A82028A" + str2
        vars["__MAC_LIB_PRJ_ID__"] = str1 + "98DA1A82028A" + str2
        vars["__MAC_LIB_BLD_ID__"] = str1 + "98DE1A820D18" + str2
        vars["__IOS_LIB_ORI_ID__"] = str1 + "98A71A81E087" + str2
        vars["__IOS_LIB_CTN_ID__"] = str1 + "98DB1A82028A" + str2
        vars["__IOS_LIB_PRJ_ID__"] = str1 + "98DC1A82028A" + str2
        vars["__IOS_LIB_BLD_ID__"] = str1 + "994F1A821434" + str2
        vars["__XCODE_PRDGRP_ID__"] = str1 + "98D51A82028A" + str2


