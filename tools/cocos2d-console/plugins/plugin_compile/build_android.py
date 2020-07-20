#!/usr/bin/python
# build_native.py
# Build native codes


import sys
import os, os.path
import shutil
from optparse import OptionParser
import cocos
from MultiLanguage import MultiLanguage
import cocos_project
import json
import re
from xml.dom import minidom

import project_compile

BUILD_CFIG_FILE="build-cfg.json"

class AndroidBuilder(object):

    CFG_KEY_COPY_TO_ASSETS = "copy_to_assets"
    CFG_KEY_MUST_COPY_TO_ASSERTS = "must_copy_to_assets"
    CFG_KEY_STORE = "key_store"
    CFG_KEY_STORE_PASS = "key_store_pass"
    CFG_KEY_ALIAS = "alias"
    CFG_KEY_ALIAS_PASS = "alias_pass"

    GRADLE_KEY_STORE = "RELEASE_STORE_FILE"
    GRADLE_KEY_ALIAS = "RELEASE_KEY_ALIAS"
    GRADLE_KEY_STORE_PASS = "RELEASE_STORE_PASSWORD"
    GRADLE_KEY_ALIAS_PASS = "RELEASE_KEY_PASSWORD"

    GRADLE_PROP_TARGET_VERSION = 'PROP_TARGET_SDK_VERSION'
    GRADLE_PROP_BUILD_TYPE = 'PROP_BUILD_TYPE'
    GRADLE_PROP_APP_ABI = 'PROP_APP_ABI'
    GRADLE_PROP_COMPILE_SCRIPT = 'PROP_COMPILE_SCRIPT'
    GRADLE_PROP_LUA_ENCRYPT = 'PROP_LUA_ENCRYPT'
    GRADLE_PROP_LUA_ENCRYPT_KEY = 'PROP_LUA_ENCRYPT_KEY'
    GRADLE_PROP_LUA_ENCRYPT_SIGN = 'PROP_LUA_ENCRYPT_SIGN'

    def __init__(self, verbose, app_android_root, no_res, proj_obj, mode, build_type, app_abi, gradle_support_ndk=False):
        self._verbose = verbose

        self.app_android_root = app_android_root
        self._no_res = no_res
        self._project = proj_obj
        self.gradle_support_ndk = gradle_support_ndk
        self.app_abi = app_abi
        self.mode = mode
        self.build_type = build_type

        # check environment variable
        self.sdk_root = cocos.check_environment_variable('ANDROID_SDK_ROOT')
        self.ant_root = None
        if os.path.exists(os.path.join(self.app_android_root, "gradle.properties")):
            self.sign_prop_file = os.path.join(self.app_android_root, "gradle.properties")
        elif os.path.exists(os.path.join(self.app_android_root, 'app', "gradle.properties")):
            self.sign_prop_file = os.path.join(self.app_android_root, 'app', "gradle.properties")
        else:
            raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_GRADLE_PROPERTIES_NOT_FOUND'),
                                  cocos.CCPluginError.ERROR_PATH_NOT_FOUND)
        self._parse_cfg()

    def _run_cmd(self, command, cwd=None):
        cocos.CMDRunner.run_cmd(command, self._verbose, cwd=cwd)

    def _parse_cfg(self):
        # get the properties for sign release apk
        self.key_store_str = AndroidBuilder.GRADLE_KEY_STORE
        self.key_alias_str = AndroidBuilder.GRADLE_KEY_ALIAS
        self.key_store_pass_str = AndroidBuilder.GRADLE_KEY_STORE_PASS
        self.key_alias_pass_str = AndroidBuilder.GRADLE_KEY_ALIAS_PASS

        if self.gradle_support_ndk:
            return

        self.cfg_path = os.path.join(self.app_android_root, BUILD_CFIG_FILE)
        try:
            f = open(self.cfg_path)
            cfg = json.load(f, encoding='utf8')
            f.close()
        except Exception:
            raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_PARSE_CFG_FAILED_FMT', self.cfg_path),
                                      cocos.CCPluginError.ERROR_PARSE_FILE)

        if cfg.has_key(project_compile.CCPluginCompile.CFG_KEY_MUST_COPY_RESOURCES):
            if self._no_res:
                self.res_files = cfg[project_compile.CCPluginCompile.CFG_KEY_MUST_COPY_RESOURCES]
            else:
                self.res_files = cfg[project_compile.CCPluginCompile.CFG_KEY_MUST_COPY_RESOURCES] + cfg[project_compile.CCPluginCompile.CFG_KEY_COPY_RESOURCES]
        else:
            self.res_files = cfg[project_compile.CCPluginCompile.CFG_KEY_COPY_RESOURCES]

        move_cfg = {}
        self.key_store = None
        if cfg.has_key(AndroidBuilder.CFG_KEY_STORE):
            self.key_store = cfg[AndroidBuilder.CFG_KEY_STORE]
            move_cfg[self.key_store_str] = self.key_store
            del cfg[AndroidBuilder.CFG_KEY_STORE]

        self.key_store_pass = None
        if cfg.has_key(AndroidBuilder.CFG_KEY_STORE_PASS):
            self.key_store_pass = cfg[AndroidBuilder.CFG_KEY_STORE_PASS]
            move_cfg[self.key_store_pass_str] = self.key_store_pass
            del cfg[AndroidBuilder.CFG_KEY_STORE_PASS]

        self.alias = None
        if cfg.has_key(AndroidBuilder.CFG_KEY_ALIAS):
            self.alias = cfg[AndroidBuilder.CFG_KEY_ALIAS]
            move_cfg[self.key_alias_str] = self.alias
            del cfg[AndroidBuilder.CFG_KEY_ALIAS]

        self.alias_pass = None
        if cfg.has_key(AndroidBuilder.CFG_KEY_ALIAS_PASS):
            self.alias_pass = cfg[AndroidBuilder.CFG_KEY_ALIAS_PASS]
            move_cfg[self.key_alias_pass_str] = self.alias_pass
            del cfg[AndroidBuilder.CFG_KEY_ALIAS_PASS]

        if len(move_cfg) > 0:
            # move the config into ant.properties
            self._move_cfg(move_cfg)
            with open(self.cfg_path, 'w') as outfile:
                json.dump(cfg, outfile, sort_keys = True, indent = 4)
                outfile.close()

    def has_keystore_in_signprops(self):
        keystore = None
        pattern = re.compile(r"^RELEASE_STORE_FILE=(.+)")

        try:
            file_obj = open(self.sign_prop_file)
            for line in file_obj:
                str1 = line.replace(' ', '')
                str2 = str1.replace('\t', '')
                match = pattern.match(str2)
                if match is not None:
                    keystore = match.group(1)
                    break
            file_obj.close()
        except:
            pass

        if keystore is None:
            return False
        else:
            return True

    def _write_sign_properties(self, cfg):
        file_obj = open(self.sign_prop_file, "a+")
        for key in cfg.keys():
            str_cfg = "%s=%s\n" % (key, cfg[key])
            file_obj.write(str_cfg)

        file_obj.close()

    def _move_cfg(self, cfg):
        if not self.has_keystore_in_signprops():
            self._write_sign_properties(cfg)

    def remove_c_libs(self, libs_dir):
        for file_name in os.listdir(libs_dir):
            lib_file = os.path.join(libs_dir,  file_name)
            if os.path.isfile(lib_file):
                ext = os.path.splitext(lib_file)[1]
                if ext == ".a" or ext == ".so":
                    os.remove(lib_file)

    def _get_android_sdk_tools_ver(self, sdk_tools_path):
        cfg_file = os.path.join(sdk_tools_path, 'source.properties')

        if os.path.isfile(cfg_file):
            f = open(cfg_file)
            lines = f.readlines()
            pattern = r'^Pkg\.Revision=(\d+)\.(\d+)'
            for l in lines:
                match = re.match(pattern, l.strip())
                if match:
                    return ((int)(match.group(1)), (int)(match.group(2)))

        raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_UNKNOWN_ANDROID_SDK_TOOLS_VERSION'),
                                  cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

    def _update_project_properties(self, folder_path, target_str):
        props_path = os.path.join(folder_path, 'project.properties')
        f = open(props_path)
        lines = f.readlines()
        f.close()

        pattern = r'^target=(.*)$'
        matched = False
        new_line = 'target=%s\n' % target_str
        for i in range(0, len(lines)):
            l = lines[i]
            match = re.match(pattern, l.strip())
            if match:
                lines[i] = new_line
                matched = True

        if not matched:
            lines.append('\n')
            lines.append(new_line)

        f = open(props_path, 'w')
        f.writelines(lines)
        f.close()

    def _write_local_properties(self, folder_path):
        local_porps_path = os.path.join(folder_path, 'local.properties')
        sdk_dir = self.sdk_root
        ndk_dir = cocos.check_environment_variable('NDK_ROOT')
        if cocos.os_is_win32():
            # On Windows, the path should be like:
            # sdk.dir = C:\\path\\android-sdk
            sdk_dir = sdk_dir.replace('\\', '\\\\')
            ndk_dir = ndk_dir.replace('\\', '\\\\')
        lines = [
            'sdk.dir=%s\n' % sdk_dir,
            'ndk.dir=%s\n' % ndk_dir
        ]
        f = open(local_porps_path, 'w')
        f.writelines(lines)
        f.close()

    def update_project(self, android_platform):
        if self.gradle_support_ndk:
            # If gradle supports ndk build, should write local.properties manually
            self._write_local_properties(self.app_android_root)
            return

        manifest_path = os.path.join(self.app_android_root, 'app')

        # check the android platform
        target_str = self.check_android_platform(self.sdk_root, android_platform, manifest_path)

        # should manually update the project
        self._write_local_properties(manifest_path)
        self._update_project_properties(manifest_path, target_str)

        # copy the local.properties to the app_android_root
        file_name = 'local.properties'
        src_path = os.path.normpath(os.path.join(manifest_path, file_name))
        dst_path = os.path.normpath(os.path.join(self.app_android_root, file_name))
        if src_path != dst_path:
            if os.path.isfile(dst_path):
                os.remove(dst_path)
            shutil.copy(src_path, dst_path)

    def get_toolchain_version(self, ndk_root, compile_obj):
        # it should be possible to override the toolchain
        if 'NDK_TOOLCHAIN_VERSION' in os.environ:
            return os.environ['NDK_TOOLCHAIN_VERSION']
        return '4.9'


    def do_ndk_build(self, ndk_build_param, mode, build_type, compile_obj):
        cocos.Logging.info(MultiLanguage.get_string('COMPILE_INFO_NDK_BUILD_TYPE', build_type))
        ndk_root = cocos.check_environment_variable('NDK_ROOT')

        toolchain_version = self.get_toolchain_version(ndk_root, compile_obj)

        ndk_work_dir = os.path.join(self.app_android_root, 'app')
        reload(sys)
        sys.setdefaultencoding('utf8')
        ndk_path = cocos.CMDRunner.convert_path_to_cmd(os.path.join(ndk_root, "ndk-build"))

        # delete template static and dynamic files
        obj_local_dir = os.path.join(ndk_work_dir, "obj", "local")
        if os.path.isdir(obj_local_dir):
            for abi_dir in os.listdir(obj_local_dir):
                static_file_path = os.path.join(ndk_work_dir, "obj", "local", abi_dir)
                if os.path.isdir(static_file_path):
                    self.remove_c_libs(static_file_path)

        if ndk_build_param is None:
            ndk_build_cmd = '%s -C %s' % (ndk_path, ndk_work_dir)
        else:
            ndk_build_cmd = '%s -C %s %s' % (ndk_path, ndk_work_dir, ' '.join(ndk_build_param))

        ndk_build_cmd = '%s NDK_TOOLCHAIN_VERSION=%s' % (ndk_build_cmd, toolchain_version)

        if mode == 'debug':
            ndk_build_cmd = '%s NDK_DEBUG=1' % ndk_build_cmd

        self._run_cmd(ndk_build_cmd)


    def _xml_attr(self, dir, file_name, node_name, attr):
        doc = minidom.parse(os.path.join(dir, file_name))
        return doc.getElementsByTagName(node_name)[0].getAttribute(attr)

    def update_lib_projects(self, sdk_root, sdk_tool_path, android_platform, property_path):
        property_file = os.path.join(property_path, "project.properties")
        if not os.path.isfile(property_file):
            return

        patten = re.compile(r'^android\.library\.reference\.[\d]+=(.+)')
        for line in open(property_file):
            str1 = line.replace(' ', '')
            str2 = str1.replace('\t', '')
            match = patten.match(str2)
            if match is not None:
                # a lib project is found
                lib_path = match.group(1)
                abs_lib_path = os.path.join(property_path, lib_path)
                abs_lib_path = os.path.normpath(abs_lib_path)
                if os.path.isdir(abs_lib_path):
                    target_str = self.check_android_platform(sdk_root, android_platform, abs_lib_path)
                    command = "%s update lib-project -p %s -t %s" % (cocos.CMDRunner.convert_path_to_cmd(sdk_tool_path), abs_lib_path, target_str)
                    self._run_cmd(command)

                    self.update_lib_projects(sdk_root, sdk_tool_path, android_platform, abs_lib_path)

    def get_api_level(self, target_str, raise_error=True):
        match = re.match(r'android-(\d+)', target_str)
        if match is not None:
            ret = int(match.group(1))
        else:
            if raise_error:
                raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_NOT_VALID_AP_FMT', target_str),
                                          cocos.CCPluginError.ERROR_PARSE_FILE)
            else:
                ret = -1

        return ret

    def get_target_config(self, proj_path):
        property_file = os.path.join(proj_path, "project.properties")
        if not os.path.isfile(property_file):
            raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_FILE_NOT_FOUND_FMT', property_file),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        patten = re.compile(r'^target=(.+)')
        for line in open(property_file):
            str1 = line.replace(' ', '')
            str2 = str1.replace('\t', '')
            match = patten.match(str2)
            if match is not None:
                target = match.group(1)
                target_num = self.get_api_level(target)
                if target_num > 0:
                    return target_num

        raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_TARGET_NOT_FOUND_FMT', property_file),
                                  cocos.CCPluginError.ERROR_PARSE_FILE)

    # check the selected android platform
    def check_android_platform(self, sdk_root, android_platform, proj_path):
        ret = android_platform
        if android_platform is None:
            min_platform = self.get_target_config(proj_path)
            # not specified platform, use the one in project.properties
            ret = 'android-%d' % min_platform

        ret_path = os.path.join(cocos.CMDRunner.convert_path_to_python(sdk_root), "platforms", ret)
        if not os.path.isdir(ret_path):
            raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_NO_AP_IN_SDK_FMT', ret),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        return ret

    def gradle_build_apk(self, mode, android_platform, compile_obj):
        # check the compileSdkVersion & buildToolsVersion
        check_file = os.path.join(self.app_android_root, 'app', 'build.gradle')
        f = open(check_file)
        lines = f.readlines()
        f.close()

        compile_sdk_ver = None
        build_tools_ver = None
        compile_sdk_pattern = r'compileSdkVersion[ \t]+([\d]+)'
        build_tools_pattern = r'buildToolsVersion[ \t]+"(.+)"'
        for line in lines:
            line_str = line.strip()
            match1 = re.match(compile_sdk_pattern, line_str)
            if match1:
                compile_sdk_ver = match1.group(1)

            match2 = re.match(build_tools_pattern, line_str)
            if match2:
                build_tools_ver = match2.group(1)

        if compile_sdk_ver is not None:
            # check the compileSdkVersion
            check_folder_name = 'android-%s' % compile_sdk_ver
            check_path = os.path.join(self.sdk_root, 'platforms', check_folder_name)
            if not os.path.isdir(check_path):
                cocos.Logging.warning(MultiLanguage.get_string('COMPILE_WARNING_COMPILE_SDK_FMT',
                                                               (compile_sdk_ver, check_path)))

        if build_tools_ver is not None:
            # check the buildToolsVersion
            check_path = os.path.join(self.sdk_root, 'build-tools', build_tools_ver)
            if not os.path.isdir(check_path):
                cocos.Logging.warning(MultiLanguage.get_string('COMPILE_WARNING_BUILD_TOOLS_FMT',
                                                               (build_tools_ver, check_path)))

        # invoke gradlew for gradle building
        if cocos.os_is_win32():
            gradle_path = os.path.join(self.app_android_root, 'gradlew.bat')
        else:
            gradle_path = os.path.join(self.app_android_root, 'gradlew')

        if not os.path.isfile(gradle_path):
            raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_GRALEW_NOT_EXIST_FMT', gradle_path),
                                      cocos.CCPluginError.ERROR_PATH_NOT_FOUND)

        mode_str = 'Debug' if mode == 'debug' else 'Release'
        cmd = '"%s" --parallel --info assemble%s' % (gradle_path, mode_str)

        if self.gradle_support_ndk:
            add_props = {
                AndroidBuilder.GRADLE_PROP_BUILD_TYPE: self.build_type
            }
            if android_platform:
                ret = self.check_android_platform(self.sdk_root, android_platform, None)
                pattern = r'android-(\d+)'
                match = re.match(pattern, ret)
                if match:
                    add_props[AndroidBuilder.GRADLE_PROP_TARGET_VERSION] = (int)(match.group(1))

            if self.app_abi:
                add_props[AndroidBuilder.GRADLE_PROP_APP_ABI] = ':'.join(self.app_abi.split(' '))

            if self._project._is_script_project():
                if compile_obj._compile_script:
                    add_props[AndroidBuilder.GRADLE_PROP_COMPILE_SCRIPT] = 1
                else:
                    add_props[AndroidBuilder.GRADLE_PROP_COMPILE_SCRIPT] = 0

            if self._project._is_lua_project():
                if compile_obj._lua_encrypt:
                    add_props[AndroidBuilder.GRADLE_PROP_LUA_ENCRYPT] = 1
                    add_props[AndroidBuilder.GRADLE_PROP_LUA_ENCRYPT_KEY] = compile_obj._lua_encrypt_key
                    add_props[AndroidBuilder.GRADLE_PROP_LUA_ENCRYPT_SIGN] = compile_obj._lua_encrypt_sign

            for key in add_props.keys():
                cmd += ' -P%s=%s' % (key, add_props[key])

        self._run_cmd(cmd, cwd=self.app_android_root)

    class LuaBuildArch:
        UNKNOWN = -1
        ONLY_BUILD_64BIT = 1
        ONLY_BUILD_32BIT = 2
        BUILD_32BIT_AND_64BIT = 3

    def _do_get_build_arch(self, str):
        # remove the '#' and the contents after it
        str = str.split('#')[0]
        build_64bit = str.find('arm64-v8a') != -1

        # check if need to build other architecture
        build_other_arch = False
        other_archs = ('armeabi-v7a', 'x86') # other arches are not supported
        for arch in other_archs:
            if str.find(arch) != -1:
                build_other_arch = True
                break

        if build_64bit or build_other_arch:
            if build_64bit:
                if build_other_arch:
                    print 'build 64bit and 32bit'
                    return self.LuaBuildArch.BUILD_32BIT_AND_64BIT
                else:
                    print 'only build 64bit'
                    return self.LuaBuildArch.ONLY_BUILD_64BIT
            else:
                print 'only build 32bit'
                return self.LuaBuildArch.ONLY_BUILD_32BIT

        return self.LuaBuildArch.UNKNOWN

    # check if arm64-v8a is set in Application.mk
    def _get_build_arch(self, param_of_appabi):

        # get build type from parameter
        if param_of_appabi:
            return self._do_get_build_arch(param_of_appabi)
        
        # get build type from Application.mk
        applicationmk_path = os.path.join(self.app_android_root, "app/jni/Application.mk")
        with open(applicationmk_path) as f:
            for line in f:
                if line.find('APP_ABI') == -1:
                    continue
                build_arch = self._do_get_build_arch(line)
                if build_arch != self.LuaBuildArch.UNKNOWN:
                    return build_arch

        return self.LuaBuildArch.UNKNOWN

    def do_build_apk(self, mode, no_apk, no_sign, output_dir, custom_step_args, android_platform, compile_obj):
        assets_dir = os.path.join(self.app_android_root, "app", "assets")
        project_name = None
        setting_file = os.path.join(self.app_android_root, 'settings.gradle')
        if os.path.isfile(setting_file):
            # get project name from settings.gradle
            f = open(setting_file)
            lines = f.readlines()
            f.close()

            pattern = r"project\(':(.*)'\)\.projectDir[ \t]*=[ \t]*new[ \t]*File\(settingsDir, 'app'\)"
            for line in lines:
                line_str = line.strip()
                match = re.match(pattern, line_str)
                if match:
                    project_name = match.group(1)
                    break

        if project_name is None:
            # use default project name
            project_name = 'app'
        gen_apk_folder = os.path.join(self.app_android_root, 'app/build/outputs/apk', mode)

        # gradle supports copy assets & compile scripts from engine 3.15
        if not self.gradle_support_ndk:
            # copy resources
            self._copy_resources(custom_step_args, assets_dir)

            # check the project config & compile the script files
            if self._project._is_lua_project():
                src_dir = os.path.join(assets_dir, 'src')
                build_arch = self._get_build_arch(compile_obj.app_abi)

                # only build 64bit
                if build_arch == self.LuaBuildArch.ONLY_BUILD_64BIT:
                    dst_dir = os.path.join(assets_dir, 'src/64bit')
                    is_compiled = compile_obj.compile_lua_scripts(src_dir, dst_dir, True)
                    if is_compiled:
                        # remove unneeded lua files
                        compile_obj._remove_file_with_ext(src_dir, '.lua')
                        shutil.rmtree(os.path.join(src_dir, 'cocos'))

                # only build 32bit
                if build_arch == self.LuaBuildArch.ONLY_BUILD_32BIT:
                    # build 32-bit bytecode
                    compile_obj.compile_lua_scripts(src_dir, src_dir, False)

                # build 32bit and 64bit
                if build_arch == self.LuaBuildArch.BUILD_32BIT_AND_64BIT:
                    # build 64-bit bytecode
                    dst_dir = os.path.join(assets_dir, 'src/64bit')
                    compile_obj.compile_lua_scripts(src_dir, dst_dir, True)
                    # build 32-bit bytecode
                    compile_obj.compile_lua_scripts(src_dir, src_dir, False)

                if build_arch == self.LuaBuildArch.UNKNOWN:
                    # haven't set APP_ABI in parameter and Application.mk, default build 32bit
                    compile_obj.compile_lua_scripts(src_dir, src_dir, False)

            if self._project._is_js_project():
                compile_obj.compile_js_scripts(assets_dir, assets_dir)

        if not no_apk:
            # gather the sign info if necessary
            if not no_sign:
                if mode == "release" and not self.has_keystore_in_signprops():
                    self._gather_sign_info()

            # build apk
            self.gradle_build_apk(mode, android_platform, compile_obj)

            # copy the apk to output dir
            if output_dir:
                # support generate unsigned apk
                if mode == "release" and no_sign:
                    apk_name = '%s-%s-unsigned.apk' % (project_name, mode)
                else:
                    apk_name = '%s-%s.apk' % (project_name, mode)
                gen_apk_path = os.path.join(gen_apk_folder, apk_name)
                if not os.path.exists(output_dir):
                    os.makedirs(output_dir)
                shutil.copy(gen_apk_path, output_dir)
                cocos.Logging.info(MultiLanguage.get_string('COMPILE_INFO_MOVE_APK_FMT', output_dir))

                if mode == "release" and not no_sign:
                    signed_name = "%s-%s-signed.apk" % (project_name, mode)
                    apk_path = os.path.join(output_dir, signed_name)
                    if os.path.exists(apk_path):
                        os.remove(apk_path)
                    os.rename(os.path.join(output_dir, apk_name), apk_path)
                else:
                    apk_path = os.path.join(output_dir, apk_name)

                return apk_path
            else:
                raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_NOT_SPECIFY_OUTPUT'),
                                          cocos.CCPluginError.ERROR_WRONG_ARGS)

    def _gather_sign_info(self):
        user_cfg = {}
        # get the path of keystore file
        while True:
            inputed = self._get_user_input(MultiLanguage.get_string('COMPILE_TIP_INPUT_KEYSTORE'))
            inputed = inputed.strip()
            if not os.path.isabs(inputed):
                start_path = os.path.join(self.app_android_root, 'app')
                abs_path = os.path.join(start_path, inputed)
            else:
                abs_path = inputed

            if os.path.isfile(abs_path):
                user_cfg[self.key_store_str] = inputed.replace('\\', '/')
                break
            else:
                cocos.Logging.warning(MultiLanguage.get_string('COMPILE_INFO_NOT_A_FILE'))

        # get the alias of keystore file
        user_cfg[self.key_alias_str] = self._get_user_input(MultiLanguage.get_string('COMPILE_TIP_INPUT_ALIAS'))

        # get the keystore password
        user_cfg[self.key_store_pass_str] = self._get_user_input(MultiLanguage.get_string('COMPILE_TIP_INPUT_KEY_PASS'))

        # get the alias password
        user_cfg[self.key_alias_pass_str] = self._get_user_input(MultiLanguage.get_string('COMPILE_TIP_INPUT_ALIAS_PASS'))

        # write the config into ant.properties
        self._write_sign_properties(user_cfg)

    def _get_user_input(self, tip_msg):
        cocos.Logging.warning(tip_msg)
        ret = None
        while True:
            ret = raw_input()
            break

        return ret

    def _copy_resources(self, custom_step_args, assets_dir):
        app_android_root = self.app_android_root
        res_files = self.res_files

        # remove app_android_root/assets if it exists
        if os.path.isdir(assets_dir):
            shutil.rmtree(assets_dir)

        # generate parameters for custom steps
        target_platform = cocos_project.Platforms.ANDROID
        cur_custom_step_args = custom_step_args.copy()
        cur_custom_step_args["assets-dir"] = assets_dir

        # make dir
        os.mkdir(assets_dir)
 
        # invoke custom step : pre copy assets
        self._project.invoke_custom_step_script(cocos_project.Project.CUSTOM_STEP_PRE_COPY_ASSETS, target_platform, cur_custom_step_args)

        # copy resources
        for cfg in res_files:
            cocos.copy_files_with_config(cfg, app_android_root, assets_dir)

        # invoke custom step : post copy assets
        self._project.invoke_custom_step_script(cocos_project.Project.CUSTOM_STEP_POST_COPY_ASSETS, target_platform, cur_custom_step_args)

    def get_apk_info(self):
        manifest_path = os.path.join(self.app_android_root, 'app')
        gradle_cfg_path = os.path.join(manifest_path, 'build.gradle')
        package = None
        if os.path.isfile(gradle_cfg_path):
            # get package name from build.gradle
            f = open(gradle_cfg_path)
            for line in f.readlines():
                line_str = line.strip()
                pattern = r'applicationId[ \t]+"(.*)"'
                match = re.match(pattern, line_str)
                if match:
                    package = match.group(1)
                    break

        if package is None:
            # get package name from AndroidManifest.xml
            package = self._xml_attr(manifest_path, 'AndroidManifest.xml', 'manifest', 'package')

        activity_name = self._xml_attr(manifest_path, 'AndroidManifest.xml', 'activity', 'android:name')
        if activity_name.startswith('.'):
            activity = package + activity_name
        else:
            activity = activity_name
        ret = (package, activity)

        return ret
