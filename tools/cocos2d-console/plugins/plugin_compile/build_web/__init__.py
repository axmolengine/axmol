#!/usr/bin/python
import os
import json
import cocos
from MultiLanguage import MultiLanguage
import sys
import subprocess


JDK_1_7 = "1.7"
JDK_1_6 = "1.6"

def check_jdk_version():
    commands = [
          "java",
          "-version"
      ]
    child = subprocess.Popen(commands, stderr=subprocess.PIPE)

    jdk_version = None
    for line in child.stderr:
        if 'java version' in line:
            if '1.6' in line:
                jdk_version = JDK_1_6
            else:
                jdk_version = JDK_1_7

    child.wait()

    if jdk_version is None:
        raise cocos.CCPluginError(MultiLanguage.get_string('COMPILE_ERROR_NO_VALID_JDK'),
                                  cocos.CCPluginError.ERROR_TOOLS_NOT_FOUND)

    return jdk_version

def gen_buildxml(project_dir, project_json, output_dir, build_opts):
    # get engine dir (not real)
    engineDir = project_json["engineDir"]
    # get real engine dir
    engine_dir = os.path.normpath(os.path.join(project_dir, engineDir))
    # get real publish dir
    publish_dir = output_dir
    # get tools dir
    if getattr(sys, 'frozen', None):
        tools_dir = os.path.realpath(os.path.dirname(sys.executable))
    else:
        tools_dir = os.path.realpath(os.path.dirname(__file__))

    # download the binary files
    compiler_1_6 = os.path.join(tools_dir, "bin", "compiler-1.6.jar")
    compiler_1_7 = os.path.join(tools_dir, "bin", "compiler-1.7.jar")
    if not os.path.exists(compiler_1_6) or not os.path.exists(compiler_1_7):
        download_cmd_path = os.path.join(tools_dir, os.pardir, os.pardir, os.pardir)
        subprocess.call("python %s -f" % (os.path.join(download_cmd_path, "download-bin.py")), shell=True, cwd=download_cmd_path)

    try:
        f = open(os.path.join(engine_dir, "moduleConfig.json"))
        module_cfg = json.load(f)
    finally:
        f.close()

    ccModuleMap = module_cfg["module"]
    modules = project_json.get("modules", ["core"])
    renderMode = project_json.get("renderMode", 0)
    mainJs = project_json.get("main", "main.js")
    ccJsList = [module_cfg["bootFile"]]
    userJsList = project_json.get("jsList", [])

    if renderMode != 1 and "base4webgl" not in modules:
        modules[0:0] = ["base4webgl"]

    for item in modules:
        arr = _getJsListOfModule(ccModuleMap, item)
        if arr != None:
            ccJsList += arr

    userJsList.append(mainJs)

    buildXmlTempFile = open(os.path.join(tools_dir, "template", "build.xml"))

    try:
        buildContent = buildXmlTempFile.read()
    finally:
        buildXmlTempFile.close()

    jdk_version = check_jdk_version()
    sourceMapOpened =  build_opts.get("sourceMapOpened")
    if jdk_version == JDK_1_6:
        sourceMapOpened = False
    sourceMapContent = 'sourceMapOutputFile="' + os.path.join(publish_dir, "sourcemap") + '" sourceMapFormat="V3"' if sourceMapOpened else ""



    buildContent = buildContent.replace("%projectDir%", project_dir)
    buildContent = buildContent.replace("%engineDir%", engine_dir)
    buildContent = buildContent.replace("%publishDir%", publish_dir)
    buildContent = buildContent.replace("%outputFileName%", build_opts["outputFileName"])
    buildContent = buildContent.replace("%toolsDir%", tools_dir)
    buildContent = buildContent.replace("%compiler%", "compiler-%s.jar" % jdk_version)
    buildContent = buildContent.replace("%compilationLevel%", build_opts["compilationLevel"])
    buildContent = buildContent.replace("%sourceMapCfg%",  sourceMapContent)
    buildContent = buildContent.replace("%ccJsList%", _getFileArrStr(ccJsList))
    buildContent = buildContent.replace("%userJsList%", _getFileArrStr(userJsList))
    buildContent = buildContent.replace("%debug%", build_opts["debug"])

    buildXmlOutputFile = open(os.path.join(publish_dir, "build.xml"), "w")
    buildXmlOutputFile.write(buildContent)
    buildXmlOutputFile.close()


_jsAddedCache = {}

def _getJsListOfModule(moduleMap, moduleName):
    if _jsAddedCache.get(moduleName) != None:
        return None
    _jsAddedCache[moduleName] = True
    jsList = []
    tempList = moduleMap[moduleName]

    for item in tempList:
        if _jsAddedCache.get(item):
            continue
        extname = os.path.splitext(item)[1]

        if extname == None or extname == "":
            arr = _getJsListOfModule(moduleMap, item)
            if arr != None:
                jsList += arr
        elif extname == ".js":
            jsList.append(item)

        _jsAddedCache[item] = True

    return jsList


def _getFileArrStr(jsList):
    str = ""

    index = 0
    for item in jsList:
        str += '                <file name="' + item + '"/>\r\n'

    return str

