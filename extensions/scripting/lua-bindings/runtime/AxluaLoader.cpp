/****************************************************************************
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#include "lua-bindings/runtime/AxluaLoader.h"
#include <string>
#include <algorithm>

#include "lua-bindings/runtime/LuaStack.h"
#include "lua-bindings/runtime/LuaEngine.h"
#include "axmol/platform/FileUtils.h"

using namespace ax;

extern "C" {
int axlua_loader(lua_State* L)
{
    auto relativePath = axlua_tostr(L, 1);

    //  convert any '.' to '/'
    std::replace(relativePath.begin(), relativePath.end(), '.', '/');

    // search file in package.path
    Data chunk;
    std::string resolvedPath;
    auto fileUtils = FileUtils::getInstance();

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    auto searchpath = axlua_tosv(L, -1);
    lua_pop(L, 1);
    size_t begin = 0;
    size_t next  = searchpath.find_first_of(';', 0);

    do
    {
        if (next == std::string::npos)
            next = searchpath.length();
        auto prefix = searchpath.substr(begin, next - begin);
        if (prefix[0] == '.' && prefix[1] == '/')
            prefix = prefix.substr(2);

        // reserve enough for file path to avoid memory realloc when replace ? to strPath
        resolvedPath.reserve(prefix.length() + relativePath.length());

        resolvedPath.assign(prefix.data(), prefix.length());
        auto pos = resolvedPath.find_last_of('?');
        assert(pos != std::string::npos);  // package search path should have '?'
        if (pos != std::string::npos)
        {
            resolvedPath.replace(pos, 1, relativePath);
        }

        if (fileUtils->isFileExist(resolvedPath))
        {
            chunk = fileUtils->getDataFromFile(resolvedPath);
            break;
        }

        begin = next + 1;
        next  = searchpath.find_first_of(';', begin);
    } while (begin < searchpath.length());

    int nret = chunk.getSize() > 0 ? 1 : 0;
    if (nret)
    {
        LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
        resolvedPath.insert(resolvedPath.begin(), '@');  // lua standard, add file chunck mark '@'
        stack->luaLoadBuffer(L, reinterpret_cast<const char*>(chunk.getBytes()), static_cast<int>(chunk.getSize()),
                             resolvedPath.c_str());
    }
    else
        AXLOGD("can not get file data of {}", resolvedPath);
    return nret;
}
}
