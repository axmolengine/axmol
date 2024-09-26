/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#include "AssetsManagerExTest.h"
#include "../../testResource.h"
#include "axmol.h"

using namespace ax;
USING_NS_AX_EXT;

const char* sceneManifests[]  = {"Manifests/AMTestScene1/project.manifest", "Manifests/AMTestScene2/project.manifest",
                                "Manifests/AMTestScene3/project.manifest"};
const char* storagePaths[]    = {"CppTests/AssetsManagerExTest/scene1/", "CppTests/AssetsManagerExTest/scene2/",
                              "CppTests/AssetsManagerExTest/scene3"};
const char* backgroundPaths[] = {"Images/assetMgrBackground1.jpg", "Images/assetMgrBackground2.png",
                                 "Images/assetMgrBackground3.png"};

AssetsManagerExTests::AssetsManagerExTests()
{
    addTestCase("AssetsManager Test1", []() { return AssetsManagerExLoaderScene::create(0); });
    addTestCase("AssetsManager Test2", []() { return AssetsManagerExLoaderScene::create(1); });
    addTestCase("AssetsManager Test3", []() { return AssetsManagerExLoaderScene::create(2); });
}

AssetsManagerExLoaderScene* AssetsManagerExLoaderScene::create(int testIndex)
{
    auto scene = new AssetsManagerExLoaderScene(testIndex);
    if (scene->init())
    {
        scene->autorelease();
    }
    else
    {
        delete scene;
        scene = nullptr;
    }

    return scene;
}

AssetsManagerExLoaderScene::AssetsManagerExLoaderScene(int testIndex)
    : _testIndex(testIndex)
    , _loadLayer(nullptr)
    , _downloadMenu(nullptr)
    , _am(nullptr)
    , _progress(nullptr)
    , _amListener(nullptr)
{}

bool AssetsManagerExLoaderScene::init()
{
    if (!TestCase::init())
    {
        return false;
    }

    auto downloadLabel = Label::createWithTTF("Start Download", "fonts/arial.ttf", 16);
    auto downloadItem =
        MenuItemLabel::create(downloadLabel, AX_CALLBACK_1(AssetsManagerExLoaderScene::startDownloadCallback, this));
    downloadItem->setPosition(Vec2(VisibleRect::center().x, VisibleRect::bottom().y + 100));
    _downloadMenu = Menu::create(downloadItem, nullptr);
    _downloadMenu->setPosition(Vec2::ZERO);
    this->addChild(_downloadMenu, 1);

    _loadLayer = Layer::create();
    addChild(_loadLayer);

    auto sprite = Sprite::create("Images/Icon.png");
    _loadLayer->addChild(sprite);
    sprite->setPosition(VisibleRect::center());

    TTFConfig config("fonts/tahoma.ttf", 30);
    _progress = Label::createWithTTF(config, "0%", TextHAlignment::CENTER);
    _progress->setPosition(Vec2(VisibleRect::center().x, VisibleRect::center().y + 50));
    _loadLayer->addChild(_progress);

    std::string manifestPath = sceneManifests[_testIndex],
                storagePath  = FileUtils::getInstance()->getWritablePath() + storagePaths[_testIndex];
    AXLOGD("Storage path for this test : {}", storagePath);
    _am = AssetsManagerEx::create(manifestPath, storagePath);
    _am->retain();

    return true;
}

void AssetsManagerExLoaderScene::onLoadEnd()
{
    removeChild(_loadLayer, true);
    _loadLayer = nullptr;

    auto backgroundSprite = Sprite::create(backgroundPaths[_testIndex]);
    addChild(backgroundSprite, 1);
    backgroundSprite->setPosition(VisibleRect::center());
}

void AssetsManagerExLoaderScene::startDownloadCallback(Object* sender)
{
    removeChild(_downloadMenu);
    _downloadMenu = nullptr;

    if (!_am->getLocalManifest()->isLoaded())
    {
        AXLOGD("Fail to update assets, step skipped.");
        onLoadEnd();
    }
    else
    {
        _amListener =
            ax::extension::EventListenerAssetsManagerEx::create(_am, [this](EventAssetsManagerEx* event) {
                static int failCount = 0;
                switch (event->getEventCode())
                {
                case EventAssetsManagerEx::EventCode::ERROR_NO_LOCAL_MANIFEST:
                {
                    AXLOGD("No local manifest file found, skip assets update.");
                    this->onLoadEnd();
                }
                break;
                case EventAssetsManagerEx::EventCode::UPDATE_PROGRESSION:
                {
                    std::string assetId = event->getAssetId();
                    float percent       = event->getPercent();
                    std::string str;
                    if (assetId == AssetsManagerEx::VERSION_ID)
                    {
                        str = fmt::format("Version file: {:.2}%", percent);
                    }
                    else if (assetId == AssetsManagerEx::MANIFEST_ID)
                    {
                        str = fmt::format("Manifest file: {:.2}%", percent);
                    }
                    else
                    {
                        str = fmt::format("{:.2}%", percent);
                        AXLOGD("{:.2} Percent", percent);
                    }
                    if (this->_progress != nullptr)
                        this->_progress->setString(str);
                }
                break;
                case EventAssetsManagerEx::EventCode::ERROR_DOWNLOAD_MANIFEST:
                case EventAssetsManagerEx::EventCode::ERROR_PARSE_MANIFEST:
                {
                    AXLOGD("Fail to download manifest file, update skipped.");
                    this->onLoadEnd();
                }
                break;
                case EventAssetsManagerEx::EventCode::ALREADY_UP_TO_DATE:
                case EventAssetsManagerEx::EventCode::UPDATE_FINISHED:
                {
                    AXLOGD("Update finished. {}", event->getMessage());
                    this->onLoadEnd();
                }
                break;
                case EventAssetsManagerEx::EventCode::UPDATE_FAILED:
                {
                    AXLOGD("Update failed. {}", event->getMessage());

                    failCount++;
                    if (failCount < 5)
                    {
                        _am->downloadFailedAssets();
                    }
                    else
                    {
                        AXLOGD("Reach maximum fail count, exit update process");
                        failCount = 0;
                        this->onLoadEnd();
                    }
                }
                break;
                case EventAssetsManagerEx::EventCode::ERROR_UPDATING:
                {
                    AXLOGD("Asset {} : {}", event->getAssetId().c_str(), event->getMessage());
                }
                break;
                case EventAssetsManagerEx::EventCode::ERROR_DECOMPRESS:
                {
                    AXLOGD("{}", event->getMessage());
                }
                break;
                default:
                    break;
                }
            });
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_amListener, 1);

        _am->update();
    }
}

void AssetsManagerExLoaderScene::onExit()
{
    _eventDispatcher->removeEventListener(_amListener);
    _am->release();
    TestCase::onExit();
}

std::string AssetsManagerExLoaderScene::title() const
{
    return "AssetsManagerExTest";
}
