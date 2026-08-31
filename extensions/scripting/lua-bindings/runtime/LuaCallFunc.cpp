#include "lua-bindings/runtime/LuaCallFunc.h"

namespace ax
{
LuaCallFunc* LuaCallFunc::create(const std::function<void(void*, Node*)>& func)
{
    auto ret = new LuaCallFunc();
    if (ret->initWithFunction(func))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool LuaCallFunc::initWithFunction(const std::function<void(void*, Node*)>& func)
{
    _functionLua = func;
    return true;
}

void LuaCallFunc::execute()
{
    if (_functionLua)
        _functionLua(this, _target);
    else
        CallFuncN::execute();
}

LuaCallFunc* LuaCallFunc::clone() const
{
    auto ret = new LuaCallFunc();
    ret->initWithFunction(_functionLua);
    ret->autorelease();
    return ret;
}
}
