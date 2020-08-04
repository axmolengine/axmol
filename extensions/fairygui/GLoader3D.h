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

    cocos2d::TextHAlignment getAlign() const { return _align; }
    void setAlign(cocos2d::TextHAlignment value);

    cocos2d::TextVAlignment getVerticalAlign() const { return _verticalAlign; }
    void setVerticalAlign(cocos2d::TextVAlignment value);

    bool getAutoSize() const { return _autoSize; }
    void setAutoSize(bool value);

    LoaderFillType getFill() const { return _fill; }
    void setFill(LoaderFillType value);

    bool isShrinkOnly() const { return _shrinkOnly; }
    void setShrinkOnly(bool value);

    const cocos2d::Node* getContent() { return _content; }
    void setContent(cocos2d::Node* value);

    cocos2d::Color3B getColor() const;
    void setColor(const cocos2d::Color3B& value);

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

    virtual cocos2d::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const cocos2d::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual GObject* hitTest(const cocos2d::Vec2 & worldPoint, const cocos2d::Camera * camera) override;

    virtual void loadExternal();
    virtual void freeExternal(cocos2d::SpriteFrame* spriteFrame);
    void onExternalLoadSuccess(cocos2d::SpriteFrame* spriteFrame);
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
    cocos2d::TextHAlignment _align;
    cocos2d::TextVAlignment _verticalAlign;
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
    cocos2d::Color3B _color;

    FUIContainer* _container;
    cocos2d::Node* _content;
};

NS_FGUI_END

#endif
