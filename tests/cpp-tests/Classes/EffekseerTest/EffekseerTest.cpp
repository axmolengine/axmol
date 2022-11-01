/*******************************************************************************
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

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

    
    auto rsize = _director->getOpenGLView()->getDesignResolutionSize();

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

