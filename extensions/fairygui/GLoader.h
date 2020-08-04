#ifndef __GLOADER_H__
#define __GLOADER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GComponent;
class ActionMovieClip;
class FUISprite;

class GLoader : public GObject
{
public:
    GLoader();
    virtual ~GLoader();

    CREATE_FUNC(GLoader);

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

    const cocos2d::Size& getContentSize();

    cocos2d::Color3B getColor() const;
    void setColor(const cocos2d::Color3B& value);

    bool isPlaying() const { return _playing; }
    void setPlaying(bool value);

    int getFrame() const;
    void setFrame(int value);

    FillMethod getFillMethod() const;
    void setFillMethod(FillMethod value);

    FillOrigin getFillOrigin() const;
    void setFillOrigin(FillOrigin value);

    bool isFillClockwise() const;
    void setFillClockwise(bool value);

    float getFillAmount() const;
    void setFillAmount(float value);

    GComponent* getComponent() const { return _content2; }

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
    int _contentStatus;
    bool _playing;
    int _frame;

    FUISprite* _content;
    GComponent* _content2;
    ActionMovieClip* _playAction;
};

NS_FGUI_END

#endif
