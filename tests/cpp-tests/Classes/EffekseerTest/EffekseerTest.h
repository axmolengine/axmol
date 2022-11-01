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

#ifndef _EFFEKSEERTEST_H_
#define _EFFEKSEERTEST_H_

#include "axmol.h"
#include "../BaseTest.h"
#include "EffekseerForCocos2d-x.h"

class EffekseerTests : public TestSuite
{
public:
    EffekseerTests();

    virtual ~EffekseerTests();

private:
    std::vector<std::string> _searchPaths;
};

class EffekseerTest : public TestCase
{
public:
    CREATE_FUNC(EffekseerTest);

    EffekseerTest();
    virtual ~EffekseerTest();

    virtual bool init();
    virtual std::string title() const override;

    void update(float delta) override;
    void visit(ax::Renderer *renderer, const ax::Mat4& parentTransform, uint32_t parentFlags) override;
    
protected:
    std::string _title;

    /**
    efk::EffectManager*をレイヤーのメンバ変数に追加します。このクラスはエフェクトを管理します。

    You add efk :: EffectManager * to the layer member variable. This class manages effects.

    您將efk :: EffectManager *添加到圖層成員變量。 這個類管理效果。

    您将efk :: EffectManager *添加到图层成员变量。 这个类管理效果。
	*/
	efk::EffectManager*		manager = nullptr;

	int count = 0;
};


#endif  // _EFFEKSEERTEST_H_
