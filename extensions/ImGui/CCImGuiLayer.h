#pragma once
#include "cocos2d.h"
#include "ExtensionMacros.h"

NS_CC_EXT_BEGIN

class ImGuiLayer : public Layer
{
CC_CONSTRUCTOR_ACCESS:
    virtual bool init() override;

protected:
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;

    void frame();
};

NS_CC_EXT_END
