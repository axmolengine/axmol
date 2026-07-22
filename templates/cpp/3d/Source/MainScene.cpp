/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "MainScene.h"
#include "axmol/3d/MeshRenderer.h"
#include "axmol/3d/MeshMaterial.h"
#include "axmol/2d/Light.h"
#include "axmol/scene/CameraBackgroundBrush.h"

using namespace ax;

// 24 vertices (4 per face x 6 faces), each with position (3), normal (3), texcoord (2)
// clang-format off
static const std::vector<float> s_positions = {
    // Front (+Z)
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
    // Back (-Z)
    -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
    // Top (+Y)
    -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
    // Bottom (-Y)
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
    // Right (+X)
     0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
    // Left (-X)
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
};

static const std::vector<float> s_normals = {
    // Front (+Z)
     0,  0,  1,   0,  0,  1,   0,  0,  1,   0,  0,  1,
    // Back (-Z)
     0,  0, -1,   0,  0, -1,   0,  0, -1,   0,  0, -1,
    // Top (+Y)
     0,  1,  0,   0,  1,  0,   0,  1,  0,   0,  1,  0,
    // Bottom (-Y)
     0, -1,  0,   0, -1,  0,   0, -1,  0,   0, -1,  0,
    // Right (+X)
     1,  0,  0,   1,  0,  0,   1,  0,  0,   1,  0,  0,
    // Left (-X)
    -1,  0,  0,  -1,  0,  0,  -1,  0,  0,  -1,  0,  0,
};
// clang-format on

static const std::vector<float> s_texs = {
    0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1,
    0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1,
};

static const ilist_u16_t s_indices = {
    0,  1,  2,  0,  2,  3,   // front
    4,  5,  6,  4,  6,  7,   // back
    8,  9,  10, 8,  10, 11,  // top
    12, 13, 14, 12, 14, 15,  // bottom
    16, 17, 18, 16, 18, 19,  // right
    20, 21, 22, 20, 22, 23,  // left
};

MainScene::MainScene() {}

MainScene::~MainScene() {}

bool MainScene::init()
{
    if (!Scene::init())
        return false;

    auto s = _director->getCanvasSize();

    // ---- 3D camera ----
    auto mainCamera = getDefaultCamera();
    mainCamera->setCameraFlag(CameraFlag::USER1);
    // mainCamera->setPosition3D(Vec3(0, 1.5f, 5));
    // mainCamera->lookAt(Vec3(0, 0, 0));

    // Background brush
    mainCamera->setBackgroundBrush(CameraBackgroundBrush::createColorBrush(Color(Color32(50, 55, 80)), 1.0f));

    // ---- UI camera ----
    _uiCamera = Camera::create(CameraMode::Ortho);
    _uiCamera->setDepth(1);
    _uiCamera->setCameraFlag(CameraFlag::DEFAULT);
    addChild(_uiCamera);

    // ---- Lights ----
    auto ambient = AmbientLight::create(Color32(120, 120, 140));
    ambient->setCameraMask((int)CameraFlag::USER1);
    addChild(ambient);

    auto dirLight = DirectionLight::create(Vec3(-0.5f, -1.0f, -0.3f), Color32(220, 220, 220));
    dirLight->setCameraMask((int)CameraFlag::USER1);
    addChild(dirLight);

    // ---- Procedural cube ----
    auto mesh = Mesh::create(s_positions, s_normals, s_texs, s_indices);

    _cubeRenderer = MeshRenderer::create();
    _cubeRenderer->addMesh(mesh);
    _cubeRenderer->setCameraMask((int)CameraFlag::USER1);
    addChild(_cubeRenderer);

    auto material = MeshMaterial::createBuiltInMaterial(MeshMaterial::MaterialType::DIFFUSE_NOTEX, false);
    _cubeRenderer->setMaterial(material);
    _cubeRenderer->setColor(Color32(77, 153, 230));

    // ---- UI label ----
    auto label = Label::createWithSystemFont("Hello 3D!", "Arial", 32);
    label->setPosition(Vec2(s.width / 2, s.height - 50));
    addChild(label, 10);

    // ---- Pointer / Keyboard events ----
    auto pointerListener             = PointerEventListener::create();
    pointerListener->onPointerDown   = AX_CALLBACK_1(MainScene::onPointerDown, this);
    pointerListener->onPointerMove   = AX_CALLBACK_1(MainScene::onPointerMove, this);
    pointerListener->onPointerUp     = AX_CALLBACK_1(MainScene::onPointerUp, this);
    pointerListener->onPointerScroll = AX_CALLBACK_1(MainScene::onPointerScroll, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(pointerListener, this);

    auto keyboardListener           = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed  = AX_CALLBACK_1(MainScene::onKeyPressed, this);
    keyboardListener->onKeyReleased = AX_CALLBACK_1(MainScene::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, 11);

    // Close button
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           AX_CALLBACK_1(MainScene::menuCloseCallback, this));
    if (closeItem)
    {
        float x = s.width - closeItem->getContentSize().width / 2;
        float y = closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
        auto menu = Menu::create(closeItem, nullptr);
        menu->setPosition(Vec2::zero);
        addChild(menu, 100);
    }

    scheduleUpdate();
    return true;
}

void MainScene::update(float delta)
{
    _rotationAngle += 45.0f * delta;
    if (_rotationAngle > 360.0f)
        _rotationAngle -= 360.0f;

    _cubeRenderer->setRotation3D(Vec3(0, _rotationAngle, 0));
}

bool MainScene::onPointerDown(PointerEvent* ev)
{
    return true;
}
void MainScene::onPointerMove(PointerEvent* ev) {}
void MainScene::onPointerUp(PointerEvent* ev) {}
bool MainScene::onPointerScroll(PointerEvent* ev)
{
    return true;
}
void MainScene::onKeyPressed(KeyboardEvent* ev) {}
void MainScene::onKeyReleased(KeyboardEvent* ev) {}

void MainScene::menuCloseCallback(Object* sender)
{
    _director->end();
}
