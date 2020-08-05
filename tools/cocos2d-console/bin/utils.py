#!/usr/bin/python
#-*- coding: utf-8 -*-

import os
import sys
import shutil
import cocos
import re

VS_VERSION_MAP = {
    2012 : "11.0",
    2013 : "12.0",
    2015 : "14.0",
    2017 : "15.0",
    2019 : "16.0"
}

def get_msbuild_path(vs_version):
    if cocos.os_is_win32():
        import _winreg
    else:
        return None

    if isinstance(vs_version, int):
        # The value of vs_version is int. such as : 2013, 2015
        if vs_version in VS_VERSION_MAP.keys():
            vs_ver = VS_VERSION_MAP[vs_version]
        else:
            # not supported VS version
            return None
    elif isinstance(vs_version, str):
        # The value of vs_version is string. such as: "12.0", "14.0"
        vs_ver = vs_version
    else:
        return None

    # If the system is 64bit, find VS in both 32bit & 64bit registry
    # If the system is 32bit, only find VS in 32bit registry
    if cocos.os_is_32bit_windows():
        reg_flag_list = [ _winreg.KEY_WOW64_32KEY ]
    else:
        reg_flag_list = [ _winreg.KEY_WOW64_64KEY, _winreg.KEY_WOW64_32KEY ]

    # Find VS path
    msbuild_path = None
    for reg_flag in reg_flag_list:
        try:
            vs = _winreg.OpenKey(
                _winreg.HKEY_LOCAL_MACHINE,
                r"SOFTWARE\Microsoft\MSBuild\ToolsVersions\%s" % vs_ver,
                0,
                _winreg.KEY_READ | reg_flag
            )
            msbuild_path, type = _winreg.QueryValueEx(vs, 'MSBuildToolsPath')
        except:
            continue

        if msbuild_path is not None:
            msbuild_path = os.path.join(msbuild_path, "MSBuild.exe")
            if os.path.exists(msbuild_path):
                break
            else:
                msbuild_path = None

    return msbuild_path

def get_devenv_path(vs_version):
    if cocos.os_is_win32():
        import _winreg
    else:
        return None

    if isinstance(vs_version, int):
        # The value of vs_version is int. such as : 2013, 2015
        if vs_version in VS_VERSION_MAP.keys():
            vs_ver = VS_VERSION_MAP[vs_version]
        else:
            # not supported VS version
            return None
    elif isinstance(vs_version, str):
        # The value of vs_version is string. such as: "12.0", "14.0"
        vs_ver = vs_version
    else:
        return None

    # If the system is 64bit, find VS in both 32bit & 64bit registry
    # If the system is 32bit, only find VS in 32bit registry
    if cocos.os_is_32bit_windows():
        reg_flag_list = [ _winreg.KEY_WOW64_32KEY ]
    else:
        reg_flag_list = [ _winreg.KEY_WOW64_64KEY, _winreg.KEY_WOW64_32KEY ]

    devenv_path = None
    for reg_flag in reg_flag_list:
        try:
            vs = _winreg.OpenKey(
                _winreg.HKEY_LOCAL_MACHINE,
                r"SOFTWARE\Microsoft\VisualStudio",
                0,
                _winreg.KEY_READ | reg_flag
            )
        except:
            continue

        # find specified VS
        try:
            key = _winreg.OpenKey(vs, r"SxS\VS7")
            devenv_path, type = _winreg.QueryValueEx(key, vs_ver)
        except:
            pass

        if devenv_path is not None:
            devenv_path = os.path.join(devenv_path, "Common7", "IDE", "devenv.com")
            if os.path.exists(devenv_path):
                break
            else:
                devenv_path = None

    return devenv_path

def get_vs_versions():
    # Get the VS versions
    ret = []
    if cocos.os_is_win32():
        import _winreg
    else:
        return ret

    # If the system is 64bit, find VS in both 32bit & 64bit registry
    # If the system is 32bit, only find VS in 32bit registry
    if cocos.os_is_32bit_windows():
        reg_flag_list = [ _winreg.KEY_WOW64_32KEY ]
    else:
        reg_flag_list = [ _winreg.KEY_WOW64_64KEY, _winreg.KEY_WOW64_32KEY ]

    version_pattern = re.compile(r'(\d+)\.(\d+)')
    for reg_flag in reg_flag_list:
        try:
            vs = _winreg.OpenKey(
                _winreg.HKEY_LOCAL_MACHINE,
                r"SOFTWARE\Microsoft\VisualStudio",
                0,
                _winreg.KEY_READ | reg_flag
            )
        except:
            continue

        i = 0
        while True:
            # enum the keys in vs reg
            try:
                version = _winreg.EnumKey(vs, i)
            except:
                break
            i += 1

            match = re.match(version_pattern, version)
            if match is None:
                continue

            ver_str = '%s.%s' % (match.group(1), match.group(2))
            if ver_str not in ret:
                ret.append(ver_str)

    return ret

def get_newest_msbuild(min_ver=None):
    versions = get_vs_versions()
    cmp = cocos.version_compare
    if isinstance(min_ver, int) and min_ver in VS_VERSION_MAP.keys():
        # value of min_ver is int. such as : 2013, 2015
        min_ver = VS_VERSION_MAP[min_ver]

    find_ver = None
    find_path = None
    for cur_ver in versions:
        if cmp(cur_ver, "<", min_ver):
            continue

        v_path = get_msbuild_path(v)
        if v_path is not None:
            if (find_ver is None) or cmp(cur_ver, ">", find_ver):
                find_ver = cur_ver
                find_path = v_path

    return find_path

def get_newest_devenv(min_ver=None):
    versions = get_vs_versions()
    cmp = cocos.version_compare

    if min_ver is None: 
        # mininal required version "Visual Studio 14 2015"
        min_ver = "14.0"

    if isinstance(min_ver, int) and min_ver in VS_VERSION_MAP.keys():
        # value of min_ver is int. such as : 2013, 2015
        min_ver = VS_VERSION_MAP[min_ver]

    find_ver = None
    find_path = None
    for cur_ver in versions:
        if cmp(cur_ver, "<", min_ver):
            continue

        v_path = get_devenv_path(cur_ver)
        if v_path is not None:
            if (find_ver is None) or cmp(cur_ver, ">", find_ver):
                find_ver = cur_ver
                find_path = v_path

    if find_ver is None:
        return None

    if cmp(min_ver, ">", 0) and cmp(find_ver, ">", min_ver):
        need_upgrade = True
    else:
        need_upgrade = False

    return (need_upgrade, find_path, find_ver)

def rmdir(folder):
    if os.path.exists(folder):
        if sys.platform == 'win32':
            cocos.CMDRunner.run_cmd("rd /s/q \"%s\"" % folder, verbose=True)
        else:
            shutil.rmtree(folder)

VERSION_FILE_PATH = 'cocos/cocos2d.cpp'
VERSION_PATTERN = r".*return[ \t]+\"(.*)\";"
def get_engine_version(engine_path):
    ret = None

    try:
        version_file = os.path.join(engine_path, VERSION_FILE_PATH)
        if os.path.isfile(version_file):
            f = open(version_file)
            for line in f.readlines():
                match = re.match(VERSION_PATTERN, line)
                if match:
                    ret = match.group(1)
                    break
            f.close()
    except:
        pass

    return ret
