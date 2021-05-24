
import os
import os.path
import json
import re
import shlex
import shutil

import cocos


class RemoveFrameworkHelper(object):

    def __init__(self, project, package_path):
        self._project = project
        self._package_path = package_path
        self._uninstall_json_path = self._package_path + os.sep + "uninstall.json"
        self.get_uninstall_info()

    def run(self):
        for remove_info in self._uninstall_info:
            if "file" in remove_info:
                if "tags" in remove_info:
                    self.do_remove_string_with_tag(remove_info)
                else:
                    self.do_remove_string_no_tag(remove_info)
            elif "json_file" in remove_info:
                filename = remove_info["json_file"]
                remove_items = remove_info["items"]
                self.do_remove_string_from_jsonfile(filename, remove_items)
            elif "bak_file" in remove_info:
                ori = remove_info["ori_file"]
                bak = remove_info["bak_file"]
                if os.path.exists(bak):
                    self.do_remove_file(ori)
                    os.rename(bak, ori)

        if os.path.isfile(self._uninstall_json_path):
            os.remove(self._uninstall_json_path)

    def do_remove_header_path_on_ios_mac(self, remove_info):
        filename = remove_info["file"]
        if not os.path.isfile(filename):
            return
        tag = remove_info["tags"][0]
        workdir = remove_info["workdir"]
        remove_string = remove_info["string"]

        f = open(filename, "rb")
        lines = f.readlines()
        f.close()

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

                if remove_string in headers:
                    headers.remove(remove_string)
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

        if tag_found:
            f = open(filename, "wb")
            f.writelines(contents)
            f.close()

    def do_remove_header_path(self, remove_info):
        platform = remove_info["platform"]
        name = "do_remove_header_path_on_" + platform
        cmd = getattr(self, name)
        cmd(remove_info)

    def do_remove_lib_on_win(self, remove_info):
        filename = remove_info["file"]
        if not os.path.isfile(filename):
            return
        tag = remove_info["tags"][0]
        workdir = remove_info["workdir"]
        remove_string = remove_info["string"]

        f = open(filename, "rb")
        lines = f.readlines()
        f.close()

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

                if remove_string in headers:
                    headers.remove(remove_string)
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

        if tag_found:
            f = open(filename, "wb")
            f.writelines(contents)
            f.close()

    def do_remove_lib_on_android(self, remove_info):
        filename = remove_info["file"]
        if not os.path.isfile(filename):
            return
        begin_tag = remove_info["tags"][0]
        end_tag = remove_info["tags"][1]
        prefix_tag = remove_info["tags"][2]
        remove_string = remove_info["string"]
        workdir = remove_info["workdir"]
        is_import = remove_info["is_import"]

        f = open(filename, "rb")
        lines = f.readlines()
        f.close()

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
                    # remove lib
                    if remove_string in libs:
                        libs.remove(remove_string)
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

        if tag_found:
            f = open(filename, "wb")
            f.writelines(contents)
            f.close()

    def do_remove_lib_on_ios_mac(self, remove_info):
        filename = remove_info["file"]
        if not os.path.isfile(filename):
            return
        begin_tag = remove_info["tags"][0]
        end_tag = remove_info["tags"][1]
        remove_string = remove_info["string"]
        workdir = remove_info["workdir"]

        f = open(filename, "rb")
        lines = f.readlines()
        f.close()

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
                    # remove lib
                    if remove_string in libs:
                        libs.remove(remove_string)
                    libs = list(set(libs))
                    for lib in libs:
                        contents.append('\t\t\t\t\t"' + lib + '",\n')

                    libs = []
                    lib_begin = False
                    contents.append(line)

        if tag_found:
            f = open(filename, "wb")
            f.writelines(contents)
            f.close()

    def do_remove_lib(self, remove_info):
        platform = remove_info["platform"]
        name = "do_remove_lib_on_" + platform
        cmd = getattr(self, name)
        cmd(remove_info)

    def do_remove_string_with_tag(self, remove_info):
        info_type = remove_info["type"]
        if info_type == "header":
            name = "do_remove_header_path"
        elif info_type == "lib":
            name = "do_remove_lib"
        else:
            return

        cmd = getattr(self, name)
        cmd(remove_info)

    def do_remove_string_no_tag(self, remove_info):
        filename = remove_info["file"]
        remove_string = remove_info["string"]
        self.do_remove_string_from_file(filename, remove_string)

    def do_remove_file(self, file_path):
        if not os.path.exists(file_path):
            return

        if os.path.isdir(file_path):
            shutil.rmtree(file_path)
        else:
            os.remove(file_path)

    def do_remove_string_from_file(self, filename, remove_string):
        if not os.path.isfile(filename):
            return

        f = open(filename, "rb")
        all_text = f.read()
        f.close()

        find_index = all_text.find(remove_string.encode("ascii"))
        if find_index >= 0:
            headers = all_text[0:find_index]
            tails = all_text[find_index+len(remove_string):]
            all_text = headers + tails
            f = open(filename, "wb")
            f.write(all_text)
            f.close()

    def do_remove_string_from_jsonfile(self, filename, remove_items):
        if not os.path.isfile(filename):
            return

        f = open(filename, "rb")
        configs = json.load(f)
        f.close()

        for remove_item in remove_items:
            key = remove_item["key"]
            if not key in configs:
                continue

            # found the key need to remove or to remove items
            if "items" in remove_item:
                # remove items in configs[key]
                self.remove_items_from_json(configs[key], remove_item["items"])
            else:
                # remove configs[key]
                del(configs[key])

        f = open(filename, "w+b")
        str = json.dump(configs, f)
        f.close()

    def remove_items_from_json(self, configs, remove_items):
        if isinstance(configs, list):
            # delete string in list
            for item in remove_items:
                if item in configs:
                    configs.remove(item)

        else:
            # delete key in dict
            for item in remove_items:
                if "key" in item:
                    key = item["key"]
                    if not key in configs:
                        continue
                    # found the key need to remove or to remove items
                    if "items" in item:
                        # remove items in configs[key]
                        self.remove_items_from_json(configs[key], item["items"])
                    else:
                        # remove configs[key]
                        del(configs[key])

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

    def get_uninstall_info(self):
        file_path = self._uninstall_json_path
        if os.path.isfile(file_path):
            f = open(file_path, "rb")
            self._uninstall_info = json.load(f)
            f.close()
        else:
            self._uninstall_info = []
