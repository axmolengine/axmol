/****************************************************************************
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

https://adxeproject.github.io/

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
#include "scripting/lua-bindings/manual/Cocos2dxLuaLoader.h"
#include <string>
#include <algorithm>

#include "scripting/lua-bindings/manual/CCLuaStack.h"
#include "scripting/lua-bindings/manual/CCLuaEngine.h"
#include "platform/CCFileUtils.h"
#include "yasio/cxx17/string_view.hpp"

using namespace cocos2d;

extern "C" {
int cocos2dx_lua_loader(lua_State* L)
{
    auto relativePath = adxelua_tostr(L, 1);

    //  convert any '.' to '/'
    size_t pos = relativePath.find_first_of('.');
    while (pos != std::string::npos)
    {
        relativePath[pos] = '/';
        pos               = relativePath.find_first_of('.');
    }

    // search file in package.path
    Data chunk;
    std::string resolvedPath;
    auto fileUtils = FileUtils::getInstance();

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");
    auto searchpath = adxelua_tosv(L, -1);
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
        pos = resolvedPath.find_last_of('?');
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
        CCLOG("can not get file data of %s", resolvedPath.c_str());
    return nret;
}
}
