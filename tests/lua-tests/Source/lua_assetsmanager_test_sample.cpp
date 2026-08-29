/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "lua_assetsmanager_test_sample.h"

#include "lua-bindings/runtime/axlua_adapter.h"

#include "axmol/axmol.h"
#include "extensions/axmol-ext.h"

#if !defined(_WIN32)
#    include <dirent.h>
#    include <sys/stat.h>
#endif

using namespace ax;
USING_NS_AX_EXT;

static int lua_ax_createDownloadDir(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = lua_gettop(L);

    if (0 == argc)
    {
        std::string pathToSave = FileUtils::getInstance()->getWritablePath();
        pathToSave += "tmpdir";

#if !defined(_WIN32)
        DIR* pDir = NULL;

        pDir = opendir(pathToSave.c_str());
        if (!pDir)
        {
            mkdir(pathToSave.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        }
#else
        if ((GetFileAttributesA(pathToSave.c_str())) == INVALID_FILE_ATTRIBUTES)
        {
            CreateDirectoryA(pathToSave.c_str(), 0);
        }
#endif
        axlua::adapter::push_string(L, pathToSave.c_str());
        return 1;
    }

    AXLOGD("'createDownloadDir' function wrong number of arguments: {}, was expecting {}\n", argc, 0);
    return 0;
}

static int lua_ax_deleteDownloadDir(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

    if (1 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 1, 0, &conversionError))
            goto argumentError;
#endif
        std::string pathToSave = axlua::adapter::to_string(L, 1, "");

#if AX_TARGET_OS_TVOS
        // Not implemented. "system" is not present on tvOS
        AXLOGD("'lua_ax_deleteDownloadDir' not implemented on tvOS");
        return 0;
#endif

        FileUtils::getInstance()->removeDirectory(pathToSave);
        return 0;
    }

    AXLOGD("'resetDownloadDir' function wrong number of arguments: {}, was expecting {}\n", argc, 1);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'resetDownloadDir'.", &conversionError);
    return 0;
#endif
}

static int lua_ax_addSearchPath(lua_State* L)
{
    if (nullptr == L)
        return 0;

    int argc = lua_gettop(L);

#if _AX_DEBUG >= 1
    axlua::adapter::Error conversionError;
#endif

    if (2 == argc)
    {
#if _AX_DEBUG >= 1
        if (!axlua::adapter::is_string(L, 1, 0, &conversionError) || !axlua::adapter::is_boolean(L, 2, 0, &conversionError))
            goto argumentError;
#endif
        std::string pathToSave = axlua::adapter::to_string(L, 1, "");
        bool before            = axlua::adapter::to_boolean(L, 2, 0);
        FileUtils::getInstance()->addSearchPath(pathToSave, before);
        return 0;
    }
    AXLOGD("'addSearchPath' function wrong number of arguments: {}, was expecting {}\n", argc, 2);
    return 0;

#if _AX_DEBUG >= 1
argumentError:
    axlua::adapter::raise_error(L, "#ferror in function 'addSearchPath'.", &conversionError);
    return 0;
#endif
}

int register_assetsmanager_test_sample(lua_State* L)
{
    axlua::adapter::open(L);
    axlua::adapter::module(L, NULL, 0);
    axlua::adapter::begin_module(L, NULL);
    axlua::adapter::set_function(L, "createDownloadDir", lua_ax_createDownloadDir);
    axlua::adapter::set_function(L, "deleteDownloadDir", lua_ax_deleteDownloadDir);
    axlua::adapter::set_function(L, "addSearchPath", lua_ax_addSearchPath);
    axlua::adapter::end_module(L);
    return 0;
}
