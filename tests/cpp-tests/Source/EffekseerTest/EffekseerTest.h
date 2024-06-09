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
