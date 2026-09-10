/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/axmol.h"

class MainScene : public ax::Scene
{
    enum class GameState
    {
        init = 0,
        update,
        pause,
        end,
        menu1,
        menu2,
    };

public:
    bool init() override;
    void update(float delta) override;

    // PointerEvent touch/mouse
    bool onPointerDown(ax::PointerEvent* ev);
    void onPointerMove(ax::PointerEvent* ev);
    void onPointerUp(ax::PointerEvent* ev);

    bool onPointerScroll(ax::PointerEvent* ev);

    // Keyboard
    void onKeyPressed(ax::KeyboardEvent* ev);
    void onKeyReleased(ax::KeyboardEvent* ev);

    // a selector callback
    void menuCloseCallback(ax::Object* sender);

    MainScene();
    ~MainScene() override;

private:
    GameState _gameState                         = GameState::init;
    ax::PointerEventListener* _pointerListener   = nullptr;
    ax::EventListenerKeyboard* _keyboardListener = nullptr;
    int _sceneID                                 = 0;
};
