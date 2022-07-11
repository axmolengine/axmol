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

    axis::Rect _clippingRegion;
    bool _clippingEnabled;
    bool _scissorOldState;
    axis::Rect _clippingOldRect;
    axis::Rect _clippingRect;
    bool _clippingRectDirty;

#if COCOS2D_VERSION >= 0x00040000
    axis::GroupCommand _groupCommand;
    //axis::CallbackCommand _beforeVisitCmdScissor;
    //axis::CallbackCommand _afterVisitCmdScissor;
#else
    axis::CustomCommand _beforeVisitCmdScissor;
    axis::CustomCommand _afterVisitCmdScissor;
#endif
};

class StencilClippingSupport
{
public:
    StencilClippingSupport();

    axis::Node* _stencil;
    axis::StencilStateManager* _stencilStateManager;
    axis::GroupCommand _groupCommand;
#if COCOS2D_VERSION >= 0x00040000
    axis::backend::ProgramState* _originStencilProgram;
    //axis::CallbackCommand _beforeVisitCmd;
    //axis::CallbackCommand _afterDrawStencilCmd;
    //axis::CallbackCommand _afterVisitCmd;
#else
    axis::GLProgram* _originStencilProgram;
    axis::CustomCommand _beforeVisitCmd;
    axis::CustomCommand _afterDrawStencilCmd;
    axis::CustomCommand _afterVisitCmd;
#endif
};

class FUIContainer : public axis::Node
{
public:
    FUIContainer();
    virtual ~FUIContainer();

    CREATE_FUNC(FUIContainer);

    bool isClippingEnabled() const;
    void setClippingEnabled(bool value);
    const axis::Rect& getClippingRegion() const;
    void setClippingRegion(const axis::Rect& clippingRegion);

    axis::Node* getStencil() const;
    void setStencil(axis::Node* stencil);
    float getAlphaThreshold() const;
    void setAlphaThreshold(float alphaThreshold);
    bool isInverted() const;
    void setInverted(bool inverted);

    void onEnter() override;
    void onEnterTransitionDidFinish() override;
    void onExitTransitionDidStart() override;
    void onExit() override;
    void visit(axis::Renderer *renderer, const axis::Mat4 &parentTransform, uint32_t parentFlags) override;
    void setCameraMask(unsigned short mask, bool applyChildren = true) override;
    void setGlobalZOrder(float globalZOrder) override;

    GObject* gOwner;
private:
    void onBeforeVisitScissor();
    void onAfterVisitScissor();
    const axis::Rect& getClippingRect();

    RectClippingSupport* _rectClippingSupport;
    StencilClippingSupport* _stencilClippingSupport;
    
#if COCOS2D_VERSION >= 0x00040000
    void setProgramStateRecursively(Node* node, axis::backend::ProgramState* programState);
    void restoreAllProgramStates();
    
    std::unordered_map<Node*, axis::backend::ProgramState*> _originalStencilProgramState;
#endif
};

//internal use
class FUIInnerContainer : public axis::Node
{
public:
    CREATE_FUNC(FUIInnerContainer);

    void setPosition2(const axis::Vec2 &position) { setPosition(position.x, _parent->getContentSize().height - position.y); }
    axis::Vec2 getPosition2() { return axis::Vec2(_position.x, _parent->getContentSize().height - _position.y); }
    void setPosition2(float x, float y) { setPosition(x, _parent->getContentSize().height - y); }
    void  setPositionY2(float y) { setPositionY(_parent->getContentSize().height - y); }
    float getPositionY2(void) const { return  _parent->getContentSize().height - _position.y; }
};

NS_FGUI_END

#endif
