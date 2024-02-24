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

    ax::Rect _clippingRegion;
    bool _clippingEnabled;
    bool _scissorOldState;
    ax::Rect _clippingOldRect;
    ax::Rect _clippingRect;
    bool _clippingRectDirty;

#if COCOS2D_VERSION >= 0x00040000
#if !defined(AX_VERSION)
    ax::GroupCommand _groupCommand;
#endif
    //ax::CallbackCommand _beforeVisitCmdScissor;
    //ax::CallbackCommand _afterVisitCmdScissor;
#else
    ax::CustomCommand _beforeVisitCmdScissor;
    ax::CustomCommand _afterVisitCmdScissor;
#endif
};

class StencilClippingSupport
{
public:
    StencilClippingSupport();

    ax::Node* _stencil;
    ax::StencilStateManager* _stencilStateManager;
#if !defined(AX_VERSION) 
    ax::GroupCommand _groupCommand;
#endif
#if COCOS2D_VERSION >= 0x00040000
    ax::backend::ProgramState* _originStencilProgram;
    //ax::CallbackCommand _beforeVisitCmd;
    //ax::CallbackCommand _afterDrawStencilCmd;
    //ax::CallbackCommand _afterVisitCmd;
#else
    ax::GLProgram* _originStencilProgram;
    ax::CustomCommand _beforeVisitCmd;
    ax::CustomCommand _afterDrawStencilCmd;
    ax::CustomCommand _afterVisitCmd;
#endif
};

class FUIContainer : public ax::Node
{
public:
    FUIContainer();
    virtual ~FUIContainer();

    CREATE_FUNC(FUIContainer);

    bool isClippingEnabled() const;
    void setClippingEnabled(bool value);
    const ax::Rect& getClippingRegion() const;
    void setClippingRegion(const ax::Rect& clippingRegion);

    ax::Node* getStencil() const;
    void setStencil(ax::Node* stencil);
    float getAlphaThreshold() const;
    void setAlphaThreshold(float alphaThreshold);
    bool isInverted() const;
    void setInverted(bool inverted);

    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExitTransitionDidStart() override;
    void onExit() override;
    void visit(ax::Renderer *renderer, const ax::Mat4 &parentTransform, uint32_t parentFlags) override;
    void setCameraMask(unsigned short mask, bool applyChildren = true) override;
    void setGlobalZOrder(float globalZOrder) override;

    GObject* gOwner;
private:
    void onBeforeVisitScissor();
    void onAfterVisitScissor();
    const ax::Rect& getClippingRect();

    RectClippingSupport* _rectClippingSupport;
    StencilClippingSupport* _stencilClippingSupport;
    
#if COCOS2D_VERSION >= 0x00040000
    void setProgramStateRecursively(Node* node, ax::backend::ProgramState* programState);
    void restoreAllProgramStates();
    
    std::unordered_map<Node*, ax::backend::ProgramState*> _originalStencilProgramState;
#endif
};

//internal use
class FUIInnerContainer : public ax::Node
{
public:
    CREATE_FUNC(FUIInnerContainer);

    void setPosition2(const ax::Vec2 &position) { setPosition(position.x, _parent->getContentSize().height - position.y); }
    ax::Vec2 getPosition2() { return ax::Vec2(_position.x, _parent->getContentSize().height - _position.y); }
    void setPosition2(float x, float y) { setPosition(x, _parent->getContentSize().height - y); }
    void  setPositionY2(float y) { setPositionY(_parent->getContentSize().height - y); }
    float getPositionY2(void) const { return  _parent->getContentSize().height - _position.y; }
};

NS_FGUI_END

#endif
