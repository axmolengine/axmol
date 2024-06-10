/****************************************************************************
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

#include "EffekseerTest.h"
#include <iostream>
#include <fstream>
#include <string.h>

USING_NS_AX;


//------------------------------------------------------------------
//
// SpineTestScene
//
//------------------------------------------------------------------

EffekseerTests::EffekseerTests()
{
    auto fu      = FileUtils::getInstance();
    _searchPaths = fu->getSearchPaths();
    fu->addSearchPath("Effekseer", true);

    ADD_TEST_CASE(EffekseerTest);
}

EffekseerTests::~EffekseerTests()
{
    FileUtils::getInstance()->setSearchPaths(_searchPaths);
}

EffekseerTest::EffekseerTest() : _title("EffekseerTest") {}

EffekseerTest::~EffekseerTest() {
    /**
    終了時にマネージャーを破棄します。

    You will destroy the manager on exit.

    你會在最後摧毀經理。

    你会在最后摧毁经理。
	*/
	manager->release();
}

std::string EffekseerTest::title() const
{
    return _title;
}

bool EffekseerTest::init()
{
    if (!TestCase::init())
        return false;


    auto rsize = _director->getGLView()->getDesignResolutionSize();

	//auto sprite = Sprite::create("HelloWorld.png");
	//sprite->setPosition(Vec2(320, 200));
	//this->addChild(sprite, 0);

	// for update
	this->scheduleUpdate();

	/**
		efk::EffectManagerのインスタンスを生成します。

		You create an instance of efk::EffectManager.

		您創建一個efk::EffectManager的實例。

		您创建一个efk::EffectManager的实例。
	*/
	manager = efk::EffectManager::create(rsize);

    return true;
}

void EffekseerTest::update(float delta)
{
	// Effect1
	if (count % 300 == 0)
	{
		/**
			拡大率を指定してエフェクトファイルを読み込みます。

			You read an effect file with specifying scale.

			您通過指定比例讀取效果文件。

			您通过指定比例读取效果文件。
		*/
		auto effect = efk::Effect::create("Laser01.efk", 13.0f);
		if (effect != nullptr)
		{
			/**
				エミッターを生成し、パラメーターを設定してレイヤーに追加します。

				You generate an emitter, set parameters and add it to the layer.

				您會生成一個發射極，並通過將參數添加到該層。

				您会生成一个发射极，并通过将参数添加到该层。
			*/

			auto emitter = efk::EffectEmitter::create(manager);
			emitter->setEffect(effect);
			emitter->setPlayOnEnter(true);

			emitter->setRotation3D(cocos2d::Vec3(0, 90, 0));
			emitter->setPosition(Vec2(320, 150));

			// emitter->setScale(13);
			this->addChild(emitter, 0);

			// No need (because it uses autorelease after 1.41)
			//effect->release();
		}
	}

	// Effect2
	if (count % 300 == 120)
	{
		/**
		エフェクトファイルを読み込みます。

		You read an effect file.

		您讀取效果文件。

		您读取效果文件。
		*/
		auto effect = efk::Effect::create("Homing_Laser01.efk");
		if (effect != nullptr)
		{
			/**
			エミッターを生成し、パラメーターを設定してレイヤーに追加します。

			You generate an emitter, set parameters and add it to the layer.

			您會生成一個發射極，並通過將參數添加到該層。

			您会生成一个发射极，并通过将参数添加到该层。
			*/

			auto emitter = efk::EffectEmitter::create(manager);
			emitter->setEffect(effect);
			emitter->setPlayOnEnter(true);

			emitter->setPosition(Vec2(320, 150));
			emitter->setScale(4);
			this->addChild(emitter, 0);

			/**
			Some parameters are required to set after addChild

			一部のパラメーターはAddChildした後に設定する必要があります。
			*/
			emitter->setTargetPosition(cocos2d::Vec3(320, 480, 0));

			// No need (because it uses autorelease after 1.41)
			//effect->release();
		}
	}

	/**
		毎フレーム、マネージャーを更新します。

		You update the manager every frame.

		您將更新每一幀，經理。

		您将更新每一帧，经理。
	*/
	manager->update();

	count++;
}

void EffekseerTest::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
	/**
		visitを継承して、エフェクトを実際に描画する処理を追加します。

		You inherit visit and add a process to actually draw the effect.

		你繼承的visit，然後添加的實際繪製效果的過程。

		你继承的visit，然后添加的实际绘制效果的过程。
	*/
	manager->begin(renderer, _globalZOrder);
	cocos2d::Scene::visit(renderer, parentTransform, parentFlags);
	manager->end(renderer, _globalZOrder);
}

