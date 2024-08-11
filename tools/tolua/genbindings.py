#!/usr/bin/python

# This script is used to generate luabinding glue codes.
# Android ndk version must be ndk-r9b.


import argparse
import sys
import os, os.path
import shutil
import subprocess
import re
from contextlib import contextmanager

g_ndk_root = None

def _check_ndk_root_env():
    ''' Checking the environment ANDROID_NDK, which will be used for building
    '''

    try:

        ANDROID_NDK = None

        sdkRoot = os.environ.get('ANDROID_HOME', None)
        for _, ndkVers, _ in os.walk("{0}{1}ndk".format(sdkRoot, os.path.sep)):
            for ndkVer in ndkVers:
                if (ndkVer == '23.2.8568313'):
                    ANDROID_NDK = "{0}{1}ndk{1}{2}".format(sdkRoot, os.path.sep, ndkVer)
                    break
            break

        if ANDROID_NDK == None:
            ANDROID_NDK = os.environ.get('ANDROID_NDK', None)

    except:
        print('Exception occurred when check_ndk_root_env!')

    return ANDROID_NDK

def _check_python_bin_env():
    ''' Checking the environment PYTHON_BIN, which will be used for building
    '''

    try:
        PYTHON_BIN = os.environ['PYTHON_BIN']
    except Exception:
        print("PYTHON_BIN not defined, use current python.")
        PYTHON_BIN = sys.executable

    return PYTHON_BIN

def _find_first_file_in_dir(dir, fn):
    if os.path.isfile(dir):
        if os.path.basename(dir) == fn:
            return os.path.join(os.path.dirname(dir), fn)
        else :
            return None
    elif os.path.isdir(dir):
      for subdir in os.listdir(dir):
          searchPath = _find_first_file_in_dir(os.path.join(dir, subdir), fn)
          if searchPath is not None:
              return searchPath
    else:
        return None          

def _find_all_files_match(dir, cond, all):
    if cond(dir):
        all.append(dir)
    elif os.path.isdir(dir):
        for subdir in os.listdir(dir):
            _find_all_files_match(os.path.join(dir, subdir), cond, all)


def _find_toolchain_include_path():
    global g_ndk_root
    '''
    Search gcc prebuilt include path
    for instance: "$ANDROID_NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/windows-x86_64/lib/gcc/arm-linux-androideabi/4.9.x/include"
    '''
    foundFiles = []
    _find_all_files_match(os.path.join(g_ndk_root, "toolchains"), lambda x : os.path.basename(x) == "stdarg.h" and "arm-linux-androideabi" in x , foundFiles)
    if len(foundFiles) == 0:
        return ""
    else:
        return "-I" + os.path.dirname(foundFiles[0])

def _find_llvm_include_path():
    '''
    Search llvm prebuilt include path.
    for instance: "$ANDROID_NDK/toolchains/llvm/prebuilt/windows-x86_64/lib64/clang/6.0.2/include"
    '''
    global g_ndk_root
    versionFile = _find_first_file_in_dir(g_ndk_root, "AndroidVersion.txt")
    if versionFile is None:
        return ""
    versionDir = os.path.dirname(versionFile)
    includeDir = _find_first_file_in_dir(versionDir, "stdarg.h")
    llvmIncludePath = os.path.dirname(includeDir)
    return "-I"+llvmIncludePath
  

def _defaultIncludePath():
    '''default include path for libclang, llvm & gcc include path
    '''
    llvmInclude = _find_llvm_include_path()
    toolchainInclude = _find_toolchain_include_path()
    exactIncludes =  llvmInclude + " " + toolchainInclude
    return exactIncludes


class CmdError(Exception):
    pass


@contextmanager
def _pushd(newDir):
    previousDir = os.getcwd()
    os.chdir(newDir)
    yield
    os.chdir(previousDir)

def _run_cmd(command):
    ret = subprocess.call(command, shell=True)
    if ret != 0:
        message = "Error running command"
        raise CmdError(message)

def main():
    global g_ndk_root

    cur_platform= '??'
    llvm_path = '??'
    if (g_ndk_root == None or not os.path.isdir(g_ndk_root)):
        g_ndk_root = _check_ndk_root_env()

    if not os.path.isdir(g_ndk_root): 
        print("The ndk-r23c root not specified, please specifiy via --ndk_root '/path/to/ndk'")
        sys.exit(1)

    # del the " in the path
    g_ndk_root = re.sub(r"\"", "", g_ndk_root)
    python_bin = _check_python_bin_env()

    platform = sys.platform
    if platform == 'win32':
        cur_platform = 'windows'
    elif platform == 'darwin':
        cur_platform = platform
    elif 'linux' in platform:
        cur_platform = 'linux'
    else:
        print('Your platform is not supported!')
        sys.exit(1)

    x86_llvm_path = ""
    x64_llvm_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/llvm/prebuilt', '%s-%s' % (cur_platform, 'x86_64')))
    if not os.path.exists(x64_llvm_path):
        x86_llvm_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/llvm/prebuilt', '%s' % (cur_platform)))
    if not os.path.exists(x86_llvm_path):
        x86_llvm_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/llvm/prebuilt', '%s-%s' % (cur_platform, 'x86')))

    if os.path.isdir(x64_llvm_path):
        llvm_path = x64_llvm_path
    elif os.path.isdir(x86_llvm_path):
        llvm_path = x86_llvm_path
    else:
        print('llvm toolchain not found!')
        print('path: %s or path: %s are not valid! ' % (x86_llvm_path, x64_llvm_path))
        sys.exit(1)

    # x86_gcc_toolchain_path = ""
    # x64_gcc_toolchain_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s-%s' % (cur_platform, 'x86_64')))
    # if not os.path.exists(x64_gcc_toolchain_path):
    #     x86_gcc_toolchain_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s' % (cur_platform)))
    # if not os.path.exists(x86_gcc_toolchain_path):
    #     x86_gcc_toolchain_path = os.path.abspath(os.path.join(g_ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s-%s' % (cur_platform, 'x86')))

    # if os.path.isdir(x64_gcc_toolchain_path):
    #     gcc_toolchain_path = x64_gcc_toolchain_path
    # elif os.path.isdir(x86_gcc_toolchain_path):
    #     gcc_toolchain_path = x86_gcc_toolchain_path
    # else:
    #     print('gcc toolchain not found!')
    #     print('path: %s or path: %s are not valid! ' % (x64_gcc_toolchain_path, x86_gcc_toolchain_path))
    #     sys.exit(1)


    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    ax_root = os.path.abspath(os.path.join(project_root, ''))
    cxx_generator_root = os.path.abspath(os.path.join(project_root, 'tools/bindings-generator'))

    extraFlags = _defaultIncludePath()
    extraFlags += " -DFMT_USE_CONCEPTS=0"
    extraFlags += " -DAX_ENABLE_MEDIA=1"
    
    # save config to file
    if(sys.version_info.major >= 3):
        import configparser # import ConfigParser
        config = configparser.ConfigParser()
    else:
        import ConfigParser
        config = ConfigParser.ConfigParser()
    
    config.set('DEFAULT', 'androidndkdir', g_ndk_root)
    config.set('DEFAULT', 'clangllvmdir', llvm_path)
    # config.set('DEFAULT', 'gcc_toolchain_dir', gcc_toolchain_path)
    config.set('DEFAULT', 'axdir', ax_root)
    config.set('DEFAULT', 'cxxgeneratordir', cxx_generator_root)
    config.set('DEFAULT', 'extra_flags', extraFlags)
    config.set('DEFAULT', 'cxx_std', 'c++20')

    conf_ini_file = os.path.abspath(os.path.join(os.path.dirname(__file__), 'userconf.ini'))

    print('generating userconf.ini...')
    with open(conf_ini_file, 'w', newline='\n') as configfile:
      config.write(configfile)


    # set proper environment variables
    if 'linux' in platform or platform == 'darwin':
        os.putenv('LD_LIBRARY_PATH', '%s/libclang' % cxx_generator_root)
    if platform == 'win32':
        path_env = os.environ['PATH']
        os.putenv('PATH', r'%s;%s\libclang;%s\tools\win32;' % (path_env, cxx_generator_root, cxx_generator_root))


    try:

        tolua_root = '%s/tools/tolua' % project_root
        output_dir = '%s/extensions/scripting/lua-bindings/auto' % project_root

        cmd_args = {
                    'ax_base.ini' : ('ax_base', 'axlua_base_auto'), \
                    'ax_backend.ini' : ('ax_backend', 'axlua_backend_auto'), \
                    'ax_extension.ini' : ('ax_extension', 'axlua_extension_auto'), \
                    'ax_ui.ini' : ('ax_ui', 'axlua_ui_auto'), \
                    'ax_studio.ini' : ('ax_studio', 'axlua_studio_auto'), \
                    'ax_spine.ini' : ('ax_spine', 'axlua_spine_auto'), \
                    'ax_physics.ini' : ('ax_physics', 'axlua_physics_auto'), \
                    'ax_video.ini' : ('ax_video', 'axlua_video_auto'), \
                    'ax_controller.ini' : ('ax_controller', 'axlua_controller_auto'), \
                    'ax_3d.ini': ('ax_3d', 'axlua_3d_auto'), \
                    'ax_audioengine.ini': ('ax_audioengine', 'axlua_audioengine_auto'), \
                    'ax_csloader.ini' : ('ax_csloader', 'axlua_csloader_auto'), \
                    'ax_webview.ini' : ('ax_webview', 'axlua_webview_auto'), \
                    'ax_physics3d.ini' : ('ax_physics3d', 'axlua_physics3d_auto'), \
                    'ax_navmesh.ini' : ('ax_navmesh', 'axlua_navmesh_auto'), \
                    'ax_fairygui.ini' : ('ax_fairygui', 'axlua_fairygui_auto'), \
                    }
        target = 'lua'
        generator_py = '%s/generator.py' % cxx_generator_root
        for key in cmd_args.keys():
            args = cmd_args[key]
            cfg = '%s/%s' % (tolua_root, key)
            print('Generating bindings for %s...' % (key[:-4]))
            command = '"%s" %s %s -s %s -t %s -o %s -n %s' % (python_bin, generator_py, cfg, args[0], target, output_dir, args[1])
            print(command)
            _run_cmd(command)

        print('---------------------------------')
        print('Generating lua bindings succeeds.')
        print('---------------------------------')

    except Exception as e:
        if e.__class__.__name__ == 'CmdError':
            print('---------------------------------')
            print('Generating lua bindings fails.')
            print('---------------------------------')
            sys.exit(1)
        else:
            raise


# -------------- main --------------
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Install android sdk/ndk')
    parser.add_argument("--ndk_root", help="Specificy ndk root")
    args = parser.parse_args()
    g_ndk_root = args.ndk_root
    main()
