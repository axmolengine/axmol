/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
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

// cocos2d includes
#include "base/Director.h"

// standard includes
#include <string>

#include "2d/SpriteFrameCache.h"
#include "platform/FileUtils.h"

#include "2d/ActionManager.h"
#include "2d/FontFNT.h"
#include "2d/FontAtlasCache.h"
#include "2d/AnimationCache.h"
#include "2d/Transition.h"
#include "2d/FontFreeType.h"
#include "2d/LabelAtlas.h"
#include "renderer/TextureCache.h"
#include "renderer/Renderer.h"
#include "renderer/RenderState.h"
#include "2d/Camera.h"
#include "base/UserDefault.h"
#include "base/Utils.h"
#include "base/FPSImages.h"
#include "base/Scheduler.h"
#include "base/Macros.h"
#include "base/EventDispatcher.h"
#include "base/EventCustom.h"
#include "base/Logging.h"
#include "base/AutoreleasePool.h"
#include "base/Configuration.h"
#include "base/AsyncTaskPool.h"
#include "base/ObjectFactory.h"
#include "platform/Application.h"
#if defined(AX_ENABLE_AUDIO)
#    include "audio/AudioEngine.h"
#endif

#if AX_ENABLE_SCRIPT_BINDING
#    include "base/ScriptSupport.h"
#endif

using namespace std;

NS_AX_BEGIN
// FIXME: it should be a Director ivar. Move it there once support for multiple directors is added

// singleton stuff
static Director* s_SharedDirector = nullptr;

#define kDefaultFPS 60  // 60 frames per second

const char* Director::EVENT_BEFORE_SET_NEXT_SCENE = "director_before_set_next_scene";
const char* Director::EVENT_AFTER_SET_NEXT_SCENE  = "director_after_set_next_scene";
const char* Director::EVENT_PROJECTION_CHANGED    = "director_projection_changed";
const char* Director::EVENT_AFTER_DRAW            = "director_after_draw";
const char* Director::EVENT_AFTER_VISIT           = "director_after_visit";
const char* Director::EVENT_BEFORE_UPDATE         = "director_before_update";
const char* Director::EVENT_AFTER_UPDATE          = "director_after_update";
const char* Director::EVENT_RESET                 = "director_reset";
const char* Director::EVENT_BEFORE_DRAW           = "director_before_draw";

Director* Director::getInstance()
{
    if (!s_SharedDirector)
    {
        s_SharedDirector = new Director();
        AXASSERT(s_SharedDirector, "FATAL: Not enough memory");
        s_SharedDirector->init();
    }

    return s_SharedDirector;
}

void Director::destroyInstance()
{
    AX_SAFE_DELETE(s_SharedDirector);
}

Director::Director() {}

bool Director::init()
{
    setDefaultValues();

    _scenesStack.reserve(15);

    // FPS
    _lastUpdate = std::chrono::steady_clock::now();

    auto concurrency = Configuration::getInstance()->getValue("axmol.concurrency", Value{-1}).asInt();
    _jobSystem = new JobSystem(concurrency);

#ifdef AX_ENABLE_CONSOLE
    _console = new Console();
#endif
    // scheduler
    _scheduler = new Scheduler();
    // action manager
    _actionManager = new ActionManager();
    _scheduler->scheduleUpdate(_actionManager, Scheduler::PRIORITY_SYSTEM, false);

    _eventDispatcher = new EventDispatcher();

    _beforeSetNextScene = new EventCustom(EVENT_BEFORE_SET_NEXT_SCENE);
    _beforeSetNextScene->setUserData(this);
    _afterSetNextScene = new EventCustom(EVENT_AFTER_SET_NEXT_SCENE);
    _afterSetNextScene->setUserData(this);
    _eventAfterDraw = new EventCustom(EVENT_AFTER_DRAW);
    _eventAfterDraw->setUserData(this);
    _eventBeforeDraw = new EventCustom(EVENT_BEFORE_DRAW);
    _eventBeforeDraw->setUserData(this);
    _eventAfterVisit = new EventCustom(EVENT_AFTER_VISIT);
    _eventAfterVisit->setUserData(this);
    _eventBeforeUpdate = new EventCustom(EVENT_BEFORE_UPDATE);
    _eventBeforeUpdate->setUserData(this);
    _eventAfterUpdate = new EventCustom(EVENT_AFTER_UPDATE);
    _eventAfterUpdate->setUserData(this);
    _eventProjectionChanged = new EventCustom(EVENT_PROJECTION_CHANGED);
    _eventProjectionChanged->setUserData(this);
    _eventResetDirector = new EventCustom(EVENT_RESET);
    // init TextureCache
    initTextureCache();
    initMatrixStack();

    _renderer = new Renderer;

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // listen the event that renderer was recreated on Android/WP8
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _isStatusLabelUpdated = true;  // Force recreation of textures
    });

    _eventDispatcher->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif

    return true;
}

Director::~Director()
{
    AXLOGINFO("deallocing Director: %p", this);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    _eventDispatcher->removeEventListener(_rendererRecreatedListener);
    _rendererRecreatedListener = nullptr;
#endif

    AX_SAFE_RELEASE(_scheduler);
    AX_SAFE_RELEASE(_actionManager);

    AX_SAFE_RELEASE(_beforeSetNextScene);
    AX_SAFE_RELEASE(_afterSetNextScene);
    AX_SAFE_RELEASE(_eventBeforeUpdate);
    AX_SAFE_RELEASE(_eventAfterUpdate);
    AX_SAFE_RELEASE(_eventAfterDraw);
    AX_SAFE_RELEASE(_eventBeforeDraw);
    AX_SAFE_RELEASE(_eventAfterVisit);
    AX_SAFE_RELEASE(_eventProjectionChanged);
    AX_SAFE_RELEASE(_eventResetDirector);
#ifdef AX_ENABLE_CONSOLE
    delete _console;
#endif
    AX_SAFE_RELEASE(_eventDispatcher);

    Configuration::destroyInstance();
    ObjectFactory::destroyInstance();
    QuadCommand::destroyIsolatedIndices();

#if AX_ENABLE_SCRIPT_BINDING
    ScriptEngineManager::destroyInstance();
#endif

    /** clean auto release pool. */
    PoolManager::destroyInstance();

    AX_SAFE_DELETE(_jobSystem);

    s_SharedDirector = nullptr;
}

void Director::setDefaultValues()
{
    Configuration* conf = Configuration::getInstance();

    // default FPS
    float fps             = conf->getValue("axmol.fps", Value(kDefaultFPS)).asFloat();
    _oldAnimationInterval = _animationInterval = 1.0f / fps;

    // Display FPS
    _statsDisplay = conf->getValue("axmol.display_fps", Value(false)).asBool();

    // GL projection
    std::string projection = conf->getValue("axmol.gl.projection", Value("3d")).asString();
    if (projection == "3d")
        _projection = Projection::_3D;
    else if (projection == "2d")
        _projection = Projection::_2D;
    else if (projection == "custom")
        _projection = Projection::CUSTOM;
    else
        AXASSERT(false, "Invalid projection value");

    // Default pixel format for PNG images with alpha
    std::string pixel_format = conf->getValue("axmol.texture.pixel_format_for_png", Value("rgba8888")).asString();
    if (pixel_format == "rgba8888")
        Texture2D::setDefaultAlphaPixelFormat(backend::PixelFormat::RGBA8);
    else if (pixel_format == "rgba4444")
        Texture2D::setDefaultAlphaPixelFormat(backend::PixelFormat::RGBA4);
    else if (pixel_format == "rgba5551")
        Texture2D::setDefaultAlphaPixelFormat(backend::PixelFormat::RGB5A1);

    /* !!!Notes
    ** All compressed image should do PMA at texture convert tools(such as astcenc-2.2+ with -pp-premultiply)
    ** or GPU fragment shader
    */

    // PVR v2 has alpha premultiplied ?
    bool pvr_alpha_premultiplied = conf->getValue("axmol.texture.pvrv2_has_alpha_premultiplied", Value(false)).asBool();
    Image::setCompressedImagesHavePMA(Image::CompressedImagePMAFlag::PVR, pvr_alpha_premultiplied);

    // ASTC has alpha premultiplied ?
    bool astc_alpha_premultiplied = conf->getValue("axmol.texture.astc_has_pma", Value{true}).asBool();
    Image::setCompressedImagesHavePMA(Image::CompressedImagePMAFlag::ASTC, astc_alpha_premultiplied);

    // ETC2 has alpha premultiplied ?
    // Note: no suitable tools(etc2comp, Mali Texture Compression Tool, PVRTexTool) support do PMA currently, so set
    // etc2 PMA default to `false`
    bool etc2_alpha_premultiplied = conf->getValue("axmol.texture.etc2_has_pma", Value{false}).asBool();
    Image::setCompressedImagesHavePMA(Image::CompressedImagePMAFlag::ETC2, etc2_alpha_premultiplied);
}

void Director::setGLDefaultValues()
{
    // This method SHOULD be called only after glView_ was initialized
    AXASSERT(_glView, "opengl view should not be null");

    _renderer->setDepthTest(false);
    _renderer->setDepthCompareFunction(backend::CompareFunction::LESS_EQUAL);
    setProjection(_projection);
}

// Draw the Scene
void Director::drawScene()
{
    _renderer->beginFrame();

    // calculate "global" dt
    calculateDeltaTime();

    if (_glView)
    {
        _glView->pollEvents();
    }

    // tick before glClear: issue #533
    if (!_paused)
    {
        _eventDispatcher->dispatchEvent(_eventBeforeUpdate);
        _scheduler->update(_deltaTime);
        _eventDispatcher->dispatchEvent(_eventAfterUpdate);
    }

    _renderer->clear(ClearFlag::ALL, _clearColor, 1, 0, -10000.0);

    _eventDispatcher->dispatchEvent(_eventBeforeDraw);

    /* to avoid flickr, nextScene MUST be here: after tick and before draw.
     * FIXME: Which bug is this one. It seems that it can't be reproduced with v0.9
     */
    if (_nextScene)
    {
        setNextScene();
    }

    pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    if (_runningScene)
    {
#if (defined(AX_ENABLE_PHYSICS) || (defined(AX_ENABLE_3D_PHYSICS) && AX_ENABLE_BULLET_INTEGRATION) || \
     defined(AX_ENABLE_NAVMESH))
        _runningScene->stepPhysicsAndNavigation(_deltaTime);
#endif
        // clear draw stats
        _renderer->clearDrawStats();

        // render the scene
        if (_glView)
            _glView->renderScene(_runningScene, _renderer);

        _eventDispatcher->dispatchEvent(_eventAfterVisit);
    }

    // draw the notifications node
    if (_notificationNode)
    {
        _notificationNode->visit(_renderer, Mat4::IDENTITY, 0);
    }

    updateFrameRate();

    if (_statsDisplay)
    {
#if !AX_STRIP_FPS
        showStats();
#endif
    }

    _renderer->render();

    _eventDispatcher->dispatchEvent(_eventAfterDraw);

    popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

    _totalFrames++;

    // swap buffers
    if (_glView)
    {
        _glView->swapBuffers();
    }

    _renderer->endFrame();

    if (_statsDisplay)
    {
#if !AX_STRIP_FPS
        calculateMPF();
#endif
    }
}

void Director::calculateDeltaTime()
{
    // new delta time. Re-fixed issue #1277
    if (_nextDeltaTimeZero)
    {
        _deltaTime         = 0;
        _nextDeltaTimeZero = false;
        _lastUpdate        = std::chrono::steady_clock::now();
    }
    else
    {
        // delta time may passed by invoke mainLoop(dt)
        if (!_deltaTimePassedByCaller)
        {
            auto now    = std::chrono::steady_clock::now();
            _deltaTime  = std::chrono::duration_cast<std::chrono::microseconds>(now - _lastUpdate).count() / 1000000.0f;
            _lastUpdate = now;
        }
        _deltaTime = MAX(0, _deltaTime);
    }

#if _AX_DEBUG
    // If we are debugging our code, prevent big delta time
    if (_deltaTime > 0.2f)
    {
        _deltaTime = 1 / 60.0f;
    }
#endif
}

float Director::getDeltaTime() const
{
    return _deltaTime;
}
void Director::setGLView(GLView* glView)
{
    AXASSERT(glView, "opengl view should not be null");

    if (_glView != glView)
    {
        // Configuration. Gather GPU info
        Configuration* conf = Configuration::getInstance();
        conf->gatherGPUInfo();
        AXLOGI("{}\n", conf->getInfo());

        if (_glView)
            _glView->release();
        _glView = glView;
        _glView->retain();

        // set size
        _winSizeInPoints = _glView->getDesignResolutionSize();

        _isStatusLabelUpdated = true;

        _renderer->init();

        if (_glView)
        {
            setGLDefaultValues();
        }

        if (_eventDispatcher)
        {
            _eventDispatcher->setEnabled(true);
        }
    }
}

TextureCache* Director::getTextureCache() const
{
    return _textureCache;
}

void Director::initTextureCache()
{
    _textureCache = new TextureCache();
}

void Director::destroyTextureCache()
{
    if (_textureCache)
    {
        _textureCache->waitForQuit();
        AX_SAFE_RELEASE_NULL(_textureCache);
    }
}

void Director::setViewport()
{
    if (_glView)
    {
        _glView->setViewPortInPoints(0, 0, _winSizeInPoints.width, _winSizeInPoints.height);
    }
}

void Director::setNextDeltaTimeZero(bool nextDeltaTimeZero)
{
    _nextDeltaTimeZero = nextDeltaTimeZero;
}

//
// FIXME TODO
// Matrix code MUST NOT be part of the Director
// MUST BE moved outside.
// Why the Director must have this code ?
//
void Director::initMatrixStack()
{
    while (!_modelViewMatrixStack.empty())
    {
        _modelViewMatrixStack.pop();
    }

    while (!_projectionMatrixStack.empty())
    {
        _projectionMatrixStack.pop();
    }

    while (!_textureMatrixStack.empty())
    {
        _textureMatrixStack.pop();
    }

    _modelViewMatrixStack.push(Mat4::IDENTITY);
    _projectionMatrixStack.push(Mat4::IDENTITY);
    _textureMatrixStack.push(Mat4::IDENTITY);
}

void Director::resetMatrixStack()
{
    initMatrixStack();
}

void Director::popMatrix(MATRIX_STACK_TYPE type)
{
    if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.pop();
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.pop();
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.pop();
    }
    else
    {
        AXASSERT(false, "unknown matrix stack type");
    }
}

void Director::loadIdentityMatrix(MATRIX_STACK_TYPE type)
{
    if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = Mat4::IDENTITY;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() = Mat4::IDENTITY;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = Mat4::IDENTITY;
    }
    else
    {
        AXASSERT(false, "unknown matrix stack type");
    }
}

void Director::loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = mat;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() = mat;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = mat;
    }
    else
    {
        AXASSERT(false, "unknown matrix stack type");
    }
}

void Director::multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() *= mat;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() *= mat;
    }
    else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() *= mat;
    }
    else
    {
        AXASSERT(false, "unknown matrix stack type");
    }
}

void Director::pushMatrix(MATRIX_STACK_TYPE type)
{
    if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
    {
        _modelViewMatrixStack.push(_modelViewMatrixStack.top());
    }
    else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
    {
        _projectionMatrixStack.push(_projectionMatrixStack.top());
    }
    else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
    {
        _textureMatrixStack.push(_textureMatrixStack.top());
    }
    else
    {
        AXASSERT(false, "unknown matrix stack type");
    }
}

const Mat4& Director::getMatrix(MATRIX_STACK_TYPE type) const
{
    if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
    {
        return _modelViewMatrixStack.top();
    }
    else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
    {
        return _projectionMatrixStack.top();
    }
    else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
    {
        return _textureMatrixStack.top();
    }

    AXASSERT(false, "unknown matrix stack type, will return modelview matrix instead");
    return _modelViewMatrixStack.top();
}

void Director::setProjection(Projection projection)
{
    Vec2 size = _winSizeInPoints;

    if (size.width == 0 || size.height == 0)
    {
        AXLOGERROR("axmol: warning, Director::setProjection() failed because size is 0");
        return;
    }

    setViewport();

    switch (projection)
    {
    case Projection::_2D:
    {
        Mat4 orthoMatrix;
        Mat4::createOrthographicOffCenter(0, size.width, 0, size.height, -1024, 1024, &orthoMatrix);
        loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
        loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        break;
    }

    case Projection::_3D:
    {
        float zeye = this->getZEye();

        Mat4 matrixPerspective, matrixLookup;

        // issue #1334
        Mat4::createPerspective(60, (float)size.width / size.height, 10, zeye + size.height / 2, &matrixPerspective);

        Vec3 eye(size.width / 2, size.height / 2, zeye), center(size.width / 2, size.height / 2, 0.0f),
            up(0.0f, 1.0f, 0.0f);
        Mat4::createLookAt(eye, center, up, &matrixLookup);
        Mat4 proj3d = matrixPerspective * matrixLookup;

        loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, proj3d);
        loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        break;
    }

    case Projection::CUSTOM:
        // Projection Delegate is no longer needed
        // since the event "PROJECTION CHANGED" is emitted
        break;

    default:
        AXLOG("axmol: Director: unrecognized projection");
        break;
    }

    _projection = projection;

    _eventDispatcher->dispatchEvent(_eventProjectionChanged);
}

void Director::purgeCachedData()
{
    FontFNT::purgeCachedData();
    FontAtlasCache::purgeCachedData();

    if (s_SharedDirector->getGLView())
    {
        SpriteFrameCache::getInstance()->removeUnusedSpriteFrames();
        _textureCache->removeUnusedTextures();

        // Note: some tests such as ActionsTest are leaking refcounted textures
        // There should be no test textures left in the cache
        AXLOGI("{}\n", _textureCache->getCachedTextureInfo());
    }
    FileUtils::getInstance()->purgeCachedEntries();
}

float Director::getZEye() const
{
    return (_winSizeInPoints.height / 1.154700538379252f);  //(2 * tanf(M_PI/6))
}

void Director::setClearColor(const Color4F& clearColor)
{
    _clearColor = clearColor;
}

static void GLToClipTransform(Mat4* transformOut)
{
    if (nullptr == transformOut)
        return;

    Director* director = Director::getInstance();
    AXASSERT(nullptr != director, "Director is null when setting matrix stack");

    auto& projection = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    auto& modelview  = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    *transformOut    = projection * modelview;
}

Vec2 Director::convertToGL(const Vec2& uiPoint)
{
    Mat4 transform;
    GLToClipTransform(&transform);

    Mat4 transformInv = transform.getInversed();

    // Calculate z=0 using -> transform*[0, 0, 0, 1]/w
    float zClip = transform.m[14] / transform.m[15];

    Vec2 glSize = _glView->getDesignResolutionSize();
    Vec4 clipCoord(2.0f * uiPoint.x / glSize.width - 1.0f, 1.0f - 2.0f * uiPoint.y / glSize.height, zClip, 1);

    Vec4 glCoord;
    // transformInv.transformPoint(clipCoord, &glCoord);
    transformInv.transformVector(clipCoord, &glCoord);
    float factor = 1.0f / glCoord.w;
    return Vec2(glCoord.x * factor, glCoord.y * factor);
}

Vec2 Director::convertToUI(const Vec2& glPoint)
{
    Mat4 transform;
    GLToClipTransform(&transform);

    Vec4 clipCoord;
    // Need to calculate the zero depth from the transform.
    Vec4 glCoord(glPoint.x, glPoint.y, 0.0, 1);
    transform.transformVector(glCoord, &clipCoord);

    /*
    BUG-FIX #5506

    a = (Vx, Vy, Vz, 1)
    b = (a×M)T
    Out = 1 ⁄ bw(bx, by, bz)
    */

    clipCoord.x = clipCoord.x / clipCoord.w;
    clipCoord.y = clipCoord.y / clipCoord.w;
    clipCoord.z = clipCoord.z / clipCoord.w;

    Vec2 glSize  = _glView->getDesignResolutionSize();
    float factor = 1.0f / glCoord.w;
    return Vec2(glSize.width * (clipCoord.x * 0.5f + 0.5f) * factor,
                glSize.height * (-clipCoord.y * 0.5f + 0.5f) * factor);
}

const Vec2& Director::getWinSize() const
{
    return _winSizeInPoints;
}

Vec2 Director::getWinSizeInPixels() const
{
    return Vec2(_winSizeInPoints.width * _contentScaleFactor, _winSizeInPoints.height * _contentScaleFactor);
}

Vec2 Director::getVisibleSize() const
{
    if (_glView)
    {
        return _glView->getVisibleSize();
    }
    else
    {
        return Vec2::ZERO;
    }
}

Vec2 Director::getVisibleOrigin() const
{
    if (_glView)
    {
        return _glView->getVisibleOrigin();
    }
    else
    {
        return Vec2::ZERO;
    }
}

Rect Director::getSafeAreaRect() const
{
    if (_glView)
    {
        return _glView->getSafeAreaRect();
    }
    else
    {
        return Rect::ZERO;
    }
}

// scene management

void Director::runWithScene(Scene* scene)
{
    AXASSERT(scene != nullptr,
             "This command can only be used to start the Director. There is already a scene present.");
    AXASSERT(_runningScene == nullptr, "_runningScene should be null");

    pushScene(scene);
    startAnimation();
}

void Director::replaceScene(Scene* scene)
{
    // AXASSERT(_runningScene, "Use runWithScene: instead to start the director");
    AXASSERT(scene != nullptr, "the scene should not be null");

    if (_runningScene == nullptr)
    {
        runWithScene(scene);
        return;
    }

    if (scene == _nextScene)
        return;

    if (_nextScene)
    {
        if (_nextScene->isRunning())
        {
            _nextScene->onExit();
        }
        if (_nextScene)
        {
            _nextScene->cleanup();
        }
        _nextScene = nullptr;
    }

    ssize_t index = _scenesStack.size() - 1;

    _sendCleanupToScene = true;
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->retainScriptObject(this, scene);
        sEngine->releaseScriptObject(this, _scenesStack.at(index));
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    _scenesStack.replace(index, scene);

    _nextScene = scene;
}

void Director::pushScene(Scene* scene)
{
    AXASSERT(scene, "the scene should not null");

    _sendCleanupToScene = false;

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->retainScriptObject(this, scene);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    _scenesStack.pushBack(scene);
    _nextScene = scene;
}

void Director::popScene()
{
    AXASSERT(_runningScene != nullptr, "running scene should not null");

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->releaseScriptObject(this, _scenesStack.back());
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    _scenesStack.popBack();
    ssize_t c = _scenesStack.size();

    if (c == 0)
    {
        end();
    }
    else
    {
        _sendCleanupToScene = true;
        _nextScene          = _scenesStack.at(c - 1);
    }
}

void Director::popToRootScene()
{
    popToSceneStackLevel(1);
}

void Director::popToSceneStackLevel(int level)
{
    AXASSERT(_runningScene != nullptr, "A running Scene is needed");
    ssize_t c = _scenesStack.size();

    // level 0? -> end
    if (level == 0)
    {
        end();
        return;
    }

    // current level or lower -> nothing
    if (level >= c)
        return;

    auto firstOnStackScene = _scenesStack.back();
    if (firstOnStackScene == _runningScene)
    {
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
        if (sEngine)
        {
            sEngine->releaseScriptObject(this, _scenesStack.back());
        }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        _scenesStack.popBack();
        --c;
    }

    // pop stack until reaching desired level
    while (c > level)
    {
        auto current = _scenesStack.back();

        if (current->isRunning())
        {
            current->onExit();
        }

        current->cleanup();
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
        if (sEngine)
        {
            sEngine->releaseScriptObject(this, _scenesStack.back());
        }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        _scenesStack.popBack();
        --c;
    }

    _nextScene = _scenesStack.back();

    // cleanup running scene
    _sendCleanupToScene = true;
}

void Director::end()
{
    _cleanupDirectorInNextLoop = true;
}

void Director::restart()
{
    _restartDirectorInNextLoop = true;
}

void Director::reset()
{
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    if (_runningScene)
    {
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        if (sEngine)
        {
            sEngine->releaseScriptObject(this, _runningScene);
        }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        _runningScene->onExit();
        _runningScene->cleanup();
        _runningScene->release();
    }

    _runningScene = nullptr;
    _nextScene    = nullptr;

    if (_eventDispatcher)
        _eventDispatcher->dispatchEvent(_eventResetDirector);

#if defined(AX_ENABLE_AUDIO)
    // Fix github issue: https://github.com/axmolengine/axmol/issues/550
    // !!!The AudioEngine hold scheduler must end before Director destroyed, otherwise, just lead app crash
    AudioEngine::end();
#endif

    // cleanup scheduler
    getScheduler()->unscheduleAll();

    // Remove all events
    if (_eventDispatcher)
    {
        _eventDispatcher->removeAllEventListeners();
    }

    if (_notificationNode)
    {
        _notificationNode->onExit();
        _notificationNode->cleanup();
        _notificationNode->release();
    }

    _notificationNode = nullptr;

    // remove all objects, but don't release it.
    // runWithScene might be executed after 'end'.
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    if (sEngine)
    {
        for (const auto& scene : _scenesStack)
        {
            if (scene)
                sEngine->releaseScriptObject(this, scene);
        }
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    while (!_scenesStack.empty())
    {
        _scenesStack.popBack();
    }

    stopAnimation();

    AX_SAFE_RELEASE_NULL(_notificationNode);
    AX_SAFE_RELEASE_NULL(_FPSLabel);
    AX_SAFE_RELEASE_NULL(_drawnBatchesLabel);
    AX_SAFE_RELEASE_NULL(_drawnVerticesLabel);

    // purge bitmap cache
    FontFNT::purgeCachedData();
    FontAtlasCache::purgeCachedData();

    FontFreeType::shutdownFreeType();

    // purge all managed caches
    AnimationCache::destroyInstance();
    SpriteFrameCache::destroyInstance();
    FileUtils::destroyInstance();
    AsyncTaskPool::destroyInstance();
    backend::ProgramStateRegistry::destroyInstance();
    backend::ProgramManager::destroyInstance();

    // axmol specific data structures
    UserDefault::destroyInstance();
    resetMatrixStack();

    destroyTextureCache();
}

void Director::cleanupDirector()
{
    reset();

    // cleanup graphics before release glView, otherwise, will cause crash on linux
    AX_SAFE_RELEASE(_FPSLabel);
    AX_SAFE_RELEASE(_drawnVerticesLabel);
    AX_SAFE_RELEASE(_drawnBatchesLabel);

    AX_SAFE_RELEASE(_runningScene);
    AX_SAFE_RELEASE(_notificationNode);

    AX_SAFE_DELETE(_renderer);
    backend::DriverBase::destroyInstance();

    // OpenGL view
    if (_glView)
    {
        _glView->end();
        _glView = nullptr;
    }

#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
    utils::killCurrentProcess();
#endif
}

void Director::restartDirector()
{
    reset();

    // Texture cache need to be reinitialized
    initTextureCache();

    // Reschedule for action manager
    getScheduler()->scheduleUpdate(getActionManager(), Scheduler::PRIORITY_SYSTEM, false);

    // release the objects
    PoolManager::getInstance()->getCurrentPool()->clear();

    // Restart animation
    startAnimation();

    // Real restart in script level
#if AX_ENABLE_SCRIPT_BINDING
    ScriptEvent scriptEvent(kRestartGame, nullptr);
    ScriptEngineManager::sendEventToLua(scriptEvent);
#endif

    setGLDefaultValues();

#if AX_ENABLE_CACHE_TEXTURE_DATA
    // listen the event that renderer was recreated on Android/WP8
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*) {
        _isStatusLabelUpdated = true;  // Force recreation of textures
    });

    _eventDispatcher->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void Director::setNextScene()
{
    _eventDispatcher->dispatchEvent(_beforeSetNextScene);

    bool runningIsTransition = dynamic_cast<TransitionScene*>(_runningScene) != nullptr;
    bool newIsTransition     = dynamic_cast<TransitionScene*>(_nextScene) != nullptr;

    // If it is not a transition, call onExit/cleanup
    if (!newIsTransition)
    {
        if (_runningScene)
        {
            _runningScene->onExitTransitionDidStart();
            _runningScene->onExit();
        }

        // issue #709. the root node (scene) should receive the cleanup message too
        // otherwise it might be leaked.
        if (_sendCleanupToScene && _runningScene)
        {
            _runningScene->cleanup();
        }
    }

    if (_runningScene)
    {
        _runningScene->release();
    }
    _runningScene = _nextScene;
    if (_nextScene)
    {
        _nextScene->retain();
    }
    _nextScene = nullptr;

    if ((!runningIsTransition) && _runningScene)
    {
        _runningScene->onEnter();
        _runningScene->onEnterTransitionDidFinish();
    }

    _eventDispatcher->dispatchEvent(_afterSetNextScene);
}

void Director::pause()
{
    if (_paused)
    {
        return;
    }

    _oldAnimationInterval = _animationInterval;

#if AX_REDUCE_PAUSED_CPU_USAGE
    // when paused, don't consume CPU
    setAnimationInterval(1 / 4.0, SetIntervalReason::BY_DIRECTOR_PAUSE);
#endif
    _paused = true;
}

void Director::resume()
{
    if (!_paused)
    {
        return;
    }

#if AX_REDUCE_PAUSED_CPU_USAGE
    setAnimationInterval(_oldAnimationInterval, SetIntervalReason::BY_ENGINE);
#endif

    _paused    = false;
    _deltaTime = 0;
    // fix issue #3509, skip one fps to avoid incorrect time calculation.
    setNextDeltaTimeZero(true);
}

void Director::updateFrameRate()
{
    //    static const float FPS_FILTER = 0.1f;
    //    static float prevDeltaTime = 0.016f; // 60FPS
    //
    //    float dt = _deltaTime * FPS_FILTER + (1.0f-FPS_FILTER) * prevDeltaTime;
    //    prevDeltaTime = dt;
    //    _frameRate = 1.0f/dt;

    // Frame rate should be the real value of current frame.
    _frameRate = 1.0f / _deltaTime;
}

#if !AX_STRIP_FPS

// display the FPS using a LabelAtlas
// updates the FPS every frame
void Director::showStats()
{
    if (_isStatusLabelUpdated)
    {
        createStatsLabel();
        _isStatusLabelUpdated = false;
    }

    static uint32_t prevCalls = 0;
    static uint32_t prevVerts = 0;

    ++_frames;
    _accumDt += _deltaTime;

    if (_statsDisplay && _FPSLabel && _drawnBatchesLabel && _drawnVerticesLabel)
    {
        char buffer[30] = {0};

        // Probably we don't need this anymore since
        // the framerate is using a low-pass filter
        // to make the FPS stable
        if (_accumDt > AX_DIRECTOR_STATS_INTERVAL)
        {
            snprintf(buffer, sizeof(buffer), "%.1f / %.3f", _frames / _accumDt, _secondsPerFrame);
            _FPSLabel->setString(buffer);
            _accumDt = 0;
            _frames  = 0;
        }

        auto currentCalls = (uint32_t)_renderer->getDrawnBatches();
        auto currentVerts = (uint32_t)_renderer->getDrawnVertices();
        if (currentCalls != prevCalls)
        {
            snprintf(buffer, sizeof(buffer), "GL calls:%6u", currentCalls);
            _drawnBatchesLabel->setString(buffer);
            prevCalls = currentCalls;
        }

        if (currentVerts != prevVerts)
        {
            snprintf(buffer, sizeof(buffer), "GL verts:%6u", currentVerts);
            _drawnVerticesLabel->setString(buffer);
            prevVerts = currentVerts;
        }

        const Mat4& identity = Mat4::IDENTITY;
        _drawnVerticesLabel->visit(_renderer, identity, 0);
        _drawnBatchesLabel->visit(_renderer, identity, 0);
        _FPSLabel->visit(_renderer, identity, 0);
    }
}

void Director::calculateMPF()
{
    static float prevSecondsPerFrame = 0;
    static const float MPF_FILTER    = 0.10f;

    _secondsPerFrame    = _deltaTime * MPF_FILTER + (1 - MPF_FILTER) * prevSecondsPerFrame;
    prevSecondsPerFrame = _secondsPerFrame;
}

// returns the FPS image data pointer and len
void Director::getFPSImageData(unsigned char** datapointer, ssize_t* length)
{
    // FIXME: fixed me if it should be used
    *datapointer = cc_fps_images_png;
    *length      = cc_fps_images_len();
}

void Director::createStatsLabel()
{
    Texture2D* texture             = nullptr;
    std::string fpsString          = "00.0";
    std::string drawBatchString    = "000";
    std::string drawVerticesString = "00000";
    if (_FPSLabel)
    {
        fpsString          = _FPSLabel->getString();
        drawBatchString    = _drawnBatchesLabel->getString();
        drawVerticesString = _drawnVerticesLabel->getString();

        AX_SAFE_RELEASE_NULL(_FPSLabel);
        AX_SAFE_RELEASE_NULL(_drawnBatchesLabel);
        AX_SAFE_RELEASE_NULL(_drawnVerticesLabel);
        _textureCache->removeTextureForKey("/cc_fps_images");
        FileUtils::getInstance()->purgeCachedEntries();
    }

    unsigned char* data = nullptr;
    ssize_t dataLength  = 0;
    getFPSImageData(&data, &dataLength);

    Image* image = new Image();
    bool isOK    = image->initWithImageData(data, dataLength, false);
    if (!isOK)
    {
        if (image)
            delete image;
        AXLOGERROR("%s", "Fails: init fps_images");
        return;
    }

    texture = _textureCache->addImage(image, "/cc_fps_images", PixelFormat::RGBA4);
    AX_SAFE_RELEASE(image);

    /*
     We want to use an image which is stored in the file named ccFPSImage.c
     for any design resolutions and all resource resolutions.

     To achieve this, we need to ignore 'contentScaleFactor' in 'AtlasNode' and 'LabelAtlas'.
     So I added a new method called 'setIgnoreContentScaleFactor' for 'AtlasNode',
     this is not exposed to game developers, it's only used for displaying FPS now.
     */
    float scaleFactor = 1 / AX_CONTENT_SCALE_FACTOR();

    _FPSLabel = LabelAtlas::create(fpsString, texture, 12, 32, '.');
    _FPSLabel->retain();
    _FPSLabel->setIgnoreContentScaleFactor(true);
    _FPSLabel->setScale(scaleFactor);

    _drawnBatchesLabel = LabelAtlas::create(drawBatchString, texture, 12, 32, '.');
    _drawnBatchesLabel->retain();
    _drawnBatchesLabel->setIgnoreContentScaleFactor(true);
    _drawnBatchesLabel->setScale(scaleFactor);

    _drawnVerticesLabel = LabelAtlas::create(drawVerticesString, texture, 12, 32, '.');
    _drawnVerticesLabel->retain();
    _drawnVerticesLabel->setIgnoreContentScaleFactor(true);
    _drawnVerticesLabel->setScale(scaleFactor);

    setStatsAnchor();
}

void Director::setStatsAnchor(AnchorPreset anchor)
{
    if (!_statsDisplay)
        return;

    // Initialize stat counters
    if (!_FPSLabel)
        showStats();

    {
        static Vec2 _fpsPosition = {0, 0};
        auto safeOrigin          = getSafeAreaRect().origin;
        auto safeSize            = getSafeAreaRect().size;
        const int height_spacing = (int)(22 / AX_CONTENT_SCALE_FACTOR());

        switch (anchor)
        {
        case AnchorPreset::BOTTOM_LEFT:
            _fpsPosition = Vec2(0, 0);
            _drawnVerticesLabel->setAnchorPoint({0, 0});
            _drawnBatchesLabel->setAnchorPoint({0, 0});
            _FPSLabel->setAnchorPoint({0, 0});
            break;
        case AnchorPreset::CENTER_LEFT:
            _fpsPosition = Vec2(0, safeSize.height / 2 - height_spacing * 1.5);
            _drawnVerticesLabel->setAnchorPoint({0, 0.0});
            _drawnBatchesLabel->setAnchorPoint({0, 0.0});
            _FPSLabel->setAnchorPoint({0, 0});
            break;
        case AnchorPreset::TOP_LEFT:
            _fpsPosition = Vec2(0, safeSize.height - height_spacing * 3);
            _drawnVerticesLabel->setAnchorPoint({0, 0});
            _drawnBatchesLabel->setAnchorPoint({0, 0});
            _FPSLabel->setAnchorPoint({0, 0});
            break;
        case AnchorPreset::BOTTOM_RIGHT:
            _fpsPosition = Vec2(safeSize.width, 0);
            _drawnVerticesLabel->setAnchorPoint({1, 0});
            _drawnBatchesLabel->setAnchorPoint({1, 0});
            _FPSLabel->setAnchorPoint({1, 0});
            break;
        case AnchorPreset::CENTER_RIGHT:
            _fpsPosition = Vec2(safeSize.width, safeSize.height / 2 - height_spacing * 1.5);
            _drawnVerticesLabel->setAnchorPoint({1, 0.0});
            _drawnBatchesLabel->setAnchorPoint({1, 0.0});
            _FPSLabel->setAnchorPoint({1, 0.0});
            break;
        case AnchorPreset::TOP_RIGHT:
            _fpsPosition = Vec2(safeSize.width, safeSize.height - height_spacing * 3);
            _drawnVerticesLabel->setAnchorPoint({1, 0});
            _drawnBatchesLabel->setAnchorPoint({1, 0});
            _FPSLabel->setAnchorPoint({1, 0});
            break;
        case AnchorPreset::BOTTOM_CENTER:
            _fpsPosition = Vec2(safeSize.width / 2, 0);
            _drawnVerticesLabel->setAnchorPoint({0.5, 0});
            _drawnBatchesLabel->setAnchorPoint({0.5, 0});
            _FPSLabel->setAnchorPoint({0.5, 0});
            break;
        case AnchorPreset::CENTER:
            _fpsPosition = Vec2(safeSize.width / 2, safeSize.height / 2 - height_spacing * 1.5);
            _drawnVerticesLabel->setAnchorPoint({0.5, 0.0});
            _drawnBatchesLabel->setAnchorPoint({0.5, 0.0});
            _FPSLabel->setAnchorPoint({0.5, 0.0});
            break;
        case AnchorPreset::TOP_CENTER:
            _fpsPosition = Vec2(safeSize.width / 2, safeSize.height - height_spacing * 3);
            _drawnVerticesLabel->setAnchorPoint({0.5, 0});
            _drawnBatchesLabel->setAnchorPoint({0.5, 0});
            _FPSLabel->setAnchorPoint({0.5, 0});
            break;
        default:  // FPSPosition::BOTTOM_LEFT
            _fpsPosition = Vec2(0, 0);
            _drawnVerticesLabel->setAnchorPoint({0, 0});
            _drawnBatchesLabel->setAnchorPoint({0, 0});
            _FPSLabel->setAnchorPoint({0, 0});
            break;
        }

        _drawnVerticesLabel->setPosition(Vec2(0, height_spacing * 2.0f) + _fpsPosition + safeOrigin);
        _drawnBatchesLabel->setPosition(Vec2(0, height_spacing * 1.0f) + _fpsPosition + safeOrigin);
        _FPSLabel->setPosition(Vec2(0, height_spacing * 0.0f) + _fpsPosition + safeOrigin);
    }
}

#endif  // #if !AX_STRIP_FPS

void Director::setContentScaleFactor(float scaleFactor)
{
    if (scaleFactor != _contentScaleFactor)
    {
        _contentScaleFactor   = scaleFactor;
        _isStatusLabelUpdated = true;
    }
}

void Director::setNotificationNode(Node* node)
{
    if (_notificationNode != nullptr)
    {
        _notificationNode->onExitTransitionDidStart();
        _notificationNode->onExit();
        _notificationNode->cleanup();
    }
    AX_SAFE_RELEASE(_notificationNode);

    _notificationNode = node;
    if (node == nullptr)
        return;
    _notificationNode->onEnter();
    _notificationNode->onEnterTransitionDidFinish();
    AX_SAFE_RETAIN(_notificationNode);
}

void Director::setScheduler(Scheduler* scheduler)
{
    if (_scheduler != scheduler)
    {
        AX_SAFE_RETAIN(scheduler);
        AX_SAFE_RELEASE(_scheduler);
        _scheduler = scheduler;
    }
}

void Director::setActionManager(ActionManager* actionManager)
{
    if (_actionManager != actionManager)
    {
        AX_SAFE_RETAIN(actionManager);
        AX_SAFE_RELEASE(_actionManager);
        _actionManager = actionManager;
    }
}

void Director::setEventDispatcher(EventDispatcher* dispatcher)
{
    if (_eventDispatcher != dispatcher)
    {
        AX_SAFE_RETAIN(dispatcher);
        AX_SAFE_RELEASE(_eventDispatcher);
        _eventDispatcher = dispatcher;
    }
}

void Director::startAnimation()
{
    startAnimation(SetIntervalReason::BY_ENGINE);
}

void Director::startAnimation(SetIntervalReason reason)
{
    _lastUpdate = std::chrono::steady_clock::now();

    _invalid = false;

    _axmol_thread_id = std::this_thread::get_id();

    Application::getInstance()->setAnimationInterval(_animationInterval);

    // fix issue #3509, skip one fps to avoid incorrect time calculation.
    setNextDeltaTimeZero(true);
}

void Director::queueOperation(AsyncOperation op, void* param)
{
#if defined(AX_PLATFORM_PC)
    _operations.enqueue([=]() { op(param); });
#else
    _glView->queueOperation(op, param);
#endif
}

#if defined(AX_PLATFORM_PC)
void Director::processOperations()
{
    std::function<void()> op;
    while (_operations.try_dequeue(op))
        op();
}
#endif

void Director::mainLoop()
{
#if defined(AX_PLATFORM_PC)
    processOperations();
#endif

    if (_cleanupDirectorInNextLoop)
    {
        _cleanupDirectorInNextLoop = false;
        cleanupDirector();
    }
    else if (_restartDirectorInNextLoop)
    {
        _restartDirectorInNextLoop = false;
        restartDirector();
    }
    else if (!_invalid)
    {
        drawScene();

        // release the objects
        PoolManager::getInstance()->getCurrentPool()->clear();
    }
}

void Director::mainLoop(float dt)
{
    _deltaTime               = dt;
    _deltaTimePassedByCaller = true;
    mainLoop();
}

void Director::stopAnimation()
{
    _invalid = true;
}

void Director::setAnimationInterval(float interval)
{
    setAnimationInterval(interval, SetIntervalReason::BY_GAME);
}

void Director::setAnimationInterval(float interval, SetIntervalReason reason)
{
    _animationInterval = interval;
    if (!_invalid)
    {
        stopAnimation();
        startAnimation(reason);
    }
}

NS_AX_END
