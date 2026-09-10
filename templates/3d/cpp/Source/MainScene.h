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
public:
    ax::CameraMode getDefaultCameraMode() const override { return ax::CameraMode::Perspective; }

    bool init() override;
    void update(float delta) override;

    bool onPointerDown(ax::PointerEvent* ev);
    void onPointerMove(ax::PointerEvent* ev);
    void onPointerUp(ax::PointerEvent* ev);
    bool onPointerScroll(ax::PointerEvent* ev);
    void onKeyPressed(ax::KeyboardEvent* ev);
    void onKeyReleased(ax::KeyboardEvent* ev);
    void menuCloseCallback(ax::Object* sender);

    MainScene();
    ~MainScene() override;

private:
    ax::Camera* _uiCamera           = nullptr;
    ax::MeshRenderer* _cubeRenderer = nullptr;
    float _rotationAngle            = 0.0f;
};
