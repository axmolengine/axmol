/****************************************************************************
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2021 Bytedance Inc.

https://adxe.org

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

static cxx17::string_view adxelua_tosv(lua_State* L, int arg) {
    size_t l    = 0;
    const char* s = lua_tolstring(L, arg, &l);
    return cxx17::string_view{s, l};
}

extern "C"
{
    int cocos2dx_lua_loader(lua_State *L)
    {
        using namespace cxx17;
        const auto BYTECODE_FILE_EXT    = ".luac"_sv;
        const auto NOT_BYTECODE_FILE_EXT = ".lua"_sv;

        auto path = adxelua_tosv(L, 1);

        std::string strPath{path};
        size_t pos = strPath.find_first_of('.');
        while (pos != std::string::npos)
        {
            strPath.replace(pos, 1, "/");
            pos = strPath.find_first_of('.');
        }

        // search file in package.path
        Data chunk;
        std::string filePath;
        auto fileUtils = FileUtils::getInstance();

        lua_getglobal(L, "package");
        lua_getfield(L, -1, "path");
        auto searchpath = adxelua_tosv(L, -1);
        lua_pop(L, 1);
        size_t begin = 0;
        size_t next = searchpath.find_first_of(';', 0);

        do
        {
            if (next == std::string::npos)
                next = searchpath.length();
            auto prefix = searchpath.substr(begin, next - begin);
            if (prefix[0] == '.' && prefix[1] == '/')
                prefix = prefix.substr(2);

            filePath.assign(prefix.data(), prefix.length());
            pos = filePath.find_first_of('?', 0);
            while (pos != std::string::npos)
            {
                filePath.replace(pos, 1, strPath);
                pos = filePath.find_first_of('?', pos + strPath.length() + 1);
            }

            if (fileUtils->isFileExist(filePath))
            {
                chunk = fileUtils->getDataFromFile(filePath);
                break;
            }

            begin = next + 1;
            next = searchpath.find_first_of(';', begin);
        } while (begin < searchpath.length());

        if (chunk.getSize() > 0)
        {
            LuaStack* stack = LuaEngine::getInstance()->getLuaStack();
            filePath.insert(filePath.begin(), '@');  // lua standard, add file chunck mark '@'
            stack->luaLoadBuffer(L, reinterpret_cast<const char*>(chunk.getBytes()), static_cast<int>(chunk.getSize()),
                                 filePath.c_str());
        }
        else
        {
            CCLOG("can not get file data of %s", filePath.c_str());
            return 0;
        }

        return 1;
    }
}
