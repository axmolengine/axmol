#pragma once

#include "axmol/2d/ActionInstant.h"

namespace ax
{
class LuaCallFunc final : public CallFuncN
{
public:
    LuaCallFunc() = default;
    static LuaCallFunc* create(const std::function<void(void*, Node*)>& func);
    bool initWithFunction(const std::function<void(void*, Node*)>& func);
    LuaCallFunc* clone() const override;
    void execute() override;

private:
    std::function<void(void*, Node*)> _functionLua;
};
}  // namespace ax
