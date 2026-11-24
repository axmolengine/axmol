/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _EFFEKSEERTEST_H_
#define _EFFEKSEERTEST_H_

#include "axmol/axmol.h"
#include "../BaseTest.h"
#include "EffekseerAxmol.h"

void addEffekseerModelTests(TestSuite& suite);

struct TestEffect
{
    const char* filename;
    const char* displayName;
    float magnification;
    bool hasTargetPosition = false;
    ax::Vec3 targetPosition;
};

class EffekseerTests : public TestSuite
{
public:
    EffekseerTests();

    virtual ~EffekseerTests();

private:
    std::vector<std::string> _searchPaths;
};

class EffekseerViewTest : public TestCase
{
public:
    bool init() override;

protected:
    ax::Camera* getEffectCamera() const { return _effectCamera; }

private:
    ax::Camera* _effectCamera = nullptr;
    float _orbitAngle         = 0.0f;
};

class EffekseerTest : public EffekseerViewTest
{
public:
    CREATE_FUNC(EffekseerTest);

    EffekseerTest();
    virtual ~EffekseerTest();

    virtual bool init() override;
    virtual std::string title() const override;

    void update(float delta) override;
    void visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

private:
    void switchEffect(int direction);
    void playCurrentEffect();
    void updateLabel();

    efk::EffectManager* _manager = nullptr;
    efk::EffectEmitter* _emitter = nullptr;

    ax::Label* _label = nullptr;

    int _currentIndex = 0;
};

#endif  // _EFFEKSEERTEST_H_
