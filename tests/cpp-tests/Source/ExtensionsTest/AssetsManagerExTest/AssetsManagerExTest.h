/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#ifndef __AssetsManagerEx_Test_H__
#define __AssetsManagerEx_Test_H__

#include "extensions/axmol-ext.h"
#include "../../BaseTest.h"

DEFINE_TEST_SUITE(AssetsManagerExTests);

class AssetsManagerExLoaderScene : public TestCase
{
public:
    static AssetsManagerExLoaderScene* create(int testIndex);

    AssetsManagerExLoaderScene(int testIndex);

    void startDownloadCallback(ax::Object* sender);

    virtual std::string title() const override;

    virtual bool init() override;
    virtual void onExit() override;

private:
    int _testIndex;
    ax::Layer* _loadLayer;
    ax::Menu* _downloadMenu;

    ax::extension::AssetsManagerEx* _am;
    ax::Label* _progress;
    ax::extension::EventListenerAssetsManagerEx* _amListener;

    void onLoadEnd();
};

#endif /* defined(__AssetsManagerEx_Test_H__) */
