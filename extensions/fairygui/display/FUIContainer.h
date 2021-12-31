#ifndef __FUICONTAINER_H__
#define __FUICONTAINER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class GObject;

class RectClippingSupport
{
public:
    RectClippingSupport();

    cocos2d::Rect _clippingRegion;
    bool _clippingEnabled;
    bool _scissorOldState;
    cocos2d::Rect _clippingOldRect;
    cocos2d::Rect _clippingRect;
    bool _clippingRectDirty;

#if COCOS2D_VERSION >= 0x00040000
    cocos2d::GroupCommand _groupCommand;
    cocos2d::CallbackCommand _beforeVisitCmdScissor;
    cocos2d::CallbackCommand _afterVisitCmdScissor;
#else
    cocos2d::CustomCommand _beforeVisitCmdScissor;
    cocos2d::CustomCommand _afterVisitCmdScissor;
#endif
};

class StencilClippingSupport
{
public:
    StencilClippingSupport();

    cocos2d::Node* _stencil;
    cocos2d::StencilStateManager* _stencilStateManager;
    cocos2d::GroupCommand _groupCommand;
#if COCOS2D_VERSION >= 0x00040000
    cocos2d::backend::ProgramState* _originStencilProgram;
    cocos2d::CallbackCommand _beforeVisitCmd;
    cocos2d::CallbackCommand _afterDrawStencilCmd;
    cocos2d::CallbackCommand _afterVisitCmd;
#else
    cocos2d::GLProgram* _originStencilProgram;
    cocos2d::CustomCommand _beforeVisitCmd;
    cocos2d::CustomCommand _afterDrawStencilCmd;
    cocos2d::CustomCommand _afterVisitCmd;
#endif
};

class FUIContainer : public cocos2d::Node
{
public:
    FUIContainer();
    virtual ~FUIContainer();

    CREATE_FUNC(FUIContainer);

    bool isClippingEnabled() const;
    void setClippingEnabled(bool value);
    const cocos2d::Rect& getClippingRegion() const;
    void setClippingRegion(const cocos2d::Rect& clippingRegion);

    cocos2d::Node* getStencil() const;
    void setStencil(cocos2d::Node* stencil);
    float getAlphaThreshold() const;
    void setAlphaThreshold(float alphaThreshold);
    bool isInverted() const;
    void setInverted(bool inverted);

    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExitTransitionDidStart() override;
    void onExit() override;
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;
    void setCameraMask(unsigned short mask, bool applyChildren = true) override;
    void setGlobalZOrder(float globalZOrder) override;

    GObject* gOwner;
private:
    void onBeforeVisitScissor();
    void onAfterVisitScissor();
    const cocos2d::Rect& getClippingRect();

    RectClippingSupport* _rectClippingSupport;
    StencilClippingSupport* _stencilClippingSupport;
    
#if COCOS2D_VERSION >= 0x00040000
    void setProgramStateRecursively(Node* node, cocos2d::backend::ProgramState* programState);
    void restoreAllProgramStates();
    
    std::unordered_map<Node*, cocos2d::backend::ProgramState*> _originalStencilProgramState;
#endif
};

//internal use
class FUIInnerContainer : public cocos2d::Node
{
public:
    CREATE_FUNC(FUIInnerContainer);

    void setPosition2(const cocos2d::Vec2 &position) { setPosition(position.x, _parent->getContentSize().height - position.y); }
    cocos2d::Vec2 getPosition2() { return cocos2d::Vec2(_position.x, _parent->getContentSize().height - _position.y); }
    void setPosition2(float x, float y) { setPosition(x, _parent->getContentSize().height - y); }
    void  setPositionY2(float y) { setPositionY(_parent->getContentSize().height - y); }
    float getPositionY2(void) const { return  _parent->getContentSize().height - _position.y; }
};

NS_FGUI_END

#endif
