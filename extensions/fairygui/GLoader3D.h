#ifndef __GLOADER3D_H__
#define __GLOADER3D_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GComponent;
class FUIContainer;

class GLoader3D : public GObject
{
public:
    GLoader3D();
    virtual ~GLoader3D();

    CREATE_FUNC(GLoader3D);

    const std::string& getURL() const { return _url; }
    void setURL(const std::string& value);

    virtual const std::string& getIcon() const override { return _url; }
    virtual void setIcon(const std::string& value) override { setURL(value); }

    ax::TextHAlignment getAlign() const { return _align; }
    void setAlign(ax::TextHAlignment value);

    ax::TextVAlignment getVerticalAlign() const { return _verticalAlign; }
    void setVerticalAlign(ax::TextVAlignment value);

    bool getAutoSize() const { return _autoSize; }
    void setAutoSize(bool value);

    LoaderFillType getFill() const { return _fill; }
    void setFill(LoaderFillType value);

    bool isShrinkOnly() const { return _shrinkOnly; }
    void setShrinkOnly(bool value);

    const ax::Node* getContent() { return _content; }
    void setContent(ax::Node* value);

    ax::Color3B getColor() const;
    void setColor(const ax::Color3B& value);

    bool isPlaying() const { return _playing; }
    void setPlaying(bool value);

    int getFrame() const;
    void setFrame(int value);

    const std::string& getAnimationName() const { return _animationName; }
    void setAnimationName(const std::string& value);

    const std::string& getSkinName() const { return _skinName; }
    void setSkinName(const std::string& value);

    bool getLoop() const { return _loop; }
    void setLoop(bool value);

    virtual ax::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const ax::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual GObject* hitTest(const ax::Vec2 & worldPoint, const ax::Camera * camera) override;

    virtual void loadExternal();
    virtual void freeExternal(ax::SpriteFrame* spriteFrame);
    void onExternalLoadSuccess(ax::SpriteFrame* spriteFrame);
    void onExternalLoadFailed();

    void onChange();
    void onChangeSpine();

private:
    void loadContent();
    void loadFromPackage();
    void clearContent();
    void updateLayout();
    void setErrorState();
    void clearErrorState();

    std::string _url;
    ax::TextHAlignment _align;
    ax::TextVAlignment _verticalAlign;
    bool _autoSize;
    LoaderFillType _fill;
    bool _shrinkOnly;
    bool _updatingLayout;
    PackageItem* _contentItem;
    bool _playing;
    int _frame;
    bool _loop;
    std::string _animationName;
    std::string _skinName;
    ax::Color3B _color;

    FUIContainer* _container;
    ax::Node* _content;
};

NS_FGUI_END

#endif
