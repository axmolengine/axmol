/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#ifndef __SAMPLE_SCENE_H__
#define __SAMPLE_SCENE_H__

#include "cocos2d.h"
#include <Rendering/axis/CubismOffscreenSurface_Cocos2dx.hpp>

class LAppView;

class SampleScene : public axis::Scene
{
public:
    static axis::Scene* createScene();

    static SampleScene* getInstance();

    virtual bool init() override;

    virtual void update(float delta) override;

    virtual void onExit() override;

    // a selector callback
    void menuCloseCallback(axis::Ref* pSender);
    void menuChangeCallback(axis::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(SampleScene);

    // constructor
    SampleScene();

private:
    LAppView* _pView;

    axis::MenuItemImage* _changeItem;
    axis::MenuItemImage* _closeItem;
};

#endif // __SAMPLE_SCENE_H__
