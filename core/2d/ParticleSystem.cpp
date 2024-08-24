/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
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

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical.
//

#include "2d/ParticleSystem.h"

#include <string>

#include "2d/ParticleBatchNode.h"
#include "renderer/TextureAtlas.h"
#include "base/ZipUtils.h"
#include "base/Director.h"
#include "base/Profiling.h"
#include "base/UTF8.h"
#include "base/Utils.h"
#include "renderer/TextureCache.h"
#include "platform/FileUtils.h"

using namespace std;

namespace ax
{

// ideas taken from:
//     . The ocean spray in your face [Jeff Lander]
//        http://www.double.co.nz/dust/col0798.pdf
//     . Building an Advanced Particle System [John van der Burg]
//        http://www.gamasutra.com/features/20000623/vanderburg_01.htm
//   . LOVE game engine
//        http://love2d.org/
//
//
// Radius mode support, from 71 squared
//        http://particledesigner.71squared.com/
//
// IMPORTANT: Particle Designer is supported by cocos2d, but
// 'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
//  cocos2d uses a another approach, but the results are almost identical.
//

inline void normalize_point(float x, float y, particle_point* out)
{
    float n = x * x + y * y;
    // Already normalized.
    if (n == 1.0f)
        return;

    n = sqrt(n);
    // Too close to zero.
    if (n < MATH_TOLERANCE)
        return;

    n      = 1.0f / n;
    out->x = x * n;
    out->y = y * n;
}

ParticleData::ParticleData()
{
    memset(this, 0, sizeof(ParticleData));
}

bool ParticleData::init(int count)
{
    maxCount = count;

    posx        = (float*)malloc(count * sizeof(float));
    posy        = (float*)malloc(count * sizeof(float));
    startPosX   = (float*)malloc(count * sizeof(float));
    startPosY   = (float*)malloc(count * sizeof(float));
    colorR      = (float*)malloc(count * sizeof(float));
    colorG      = (float*)malloc(count * sizeof(float));
    colorB      = (float*)malloc(count * sizeof(float));
    colorA      = (float*)malloc(count * sizeof(float));
    deltaColorR = (float*)malloc(count * sizeof(float));
    deltaColorG = (float*)malloc(count * sizeof(float));
    deltaColorB = (float*)malloc(count * sizeof(float));
    deltaColorA = (float*)malloc(count * sizeof(float));

    size            = (float*)malloc(count * sizeof(float));
    deltaSize       = (float*)malloc(count * sizeof(float));
    rotation        = (float*)malloc(count * sizeof(float));
    staticRotation  = (float*)malloc(count * sizeof(float));
    deltaRotation   = (float*)malloc(count * sizeof(float));
    totalTimeToLive = (float*)malloc(count * sizeof(float));
    timeToLive      = (float*)malloc(count * sizeof(float));
    atlasIndex      = (unsigned int*)malloc(count * sizeof(unsigned int));

    modeA.dirX            = (float*)malloc(count * sizeof(float));
    modeA.dirY            = (float*)malloc(count * sizeof(float));
    modeA.radialAccel     = (float*)malloc(count * sizeof(float));
    modeA.tangentialAccel = (float*)malloc(count * sizeof(float));

    modeB.angle            = (float*)malloc(count * sizeof(float));
    modeB.degreesPerSecond = (float*)malloc(count * sizeof(float));
    modeB.deltaRadius      = (float*)malloc(count * sizeof(float));
    modeB.radius           = (float*)malloc(count * sizeof(float));

    return posx && posy && startPosX && startPosY && colorR && colorG && colorB && colorA && deltaColorR &&
           deltaColorG && deltaColorB && deltaColorA && size && deltaSize && rotation && staticRotation &&
           deltaRotation && totalTimeToLive && timeToLive && atlasIndex && modeA.dirX && modeA.dirY &&
           modeA.radialAccel && modeA.tangentialAccel && modeB.angle && modeB.degreesPerSecond && modeB.deltaRadius &&
           modeB.radius;
}

void ParticleData::release()
{
    AX_SAFE_FREE(posx);
    AX_SAFE_FREE(posy);
    AX_SAFE_FREE(startPosX);
    AX_SAFE_FREE(startPosY);
    AX_SAFE_FREE(colorR);
    AX_SAFE_FREE(colorG);
    AX_SAFE_FREE(colorB);
    AX_SAFE_FREE(colorA);
    AX_SAFE_FREE(deltaColorR);
    AX_SAFE_FREE(deltaColorG);
    AX_SAFE_FREE(deltaColorB);
    AX_SAFE_FREE(deltaColorA);
    AX_SAFE_FREE(hue);
    AX_SAFE_FREE(sat);
    AX_SAFE_FREE(val);
    AX_SAFE_FREE(opacityFadeInDelta);
    AX_SAFE_FREE(opacityFadeInLength);
    AX_SAFE_FREE(scaleInDelta);
    AX_SAFE_FREE(scaleInLength);
    AX_SAFE_FREE(size);
    AX_SAFE_FREE(deltaSize);
    AX_SAFE_FREE(rotation);
    AX_SAFE_FREE(staticRotation);
    AX_SAFE_FREE(deltaRotation);
    AX_SAFE_FREE(totalTimeToLive);
    AX_SAFE_FREE(timeToLive);
    AX_SAFE_FREE(animTimeLength);
    AX_SAFE_FREE(animTimeDelta);
    AX_SAFE_FREE(animIndex);
    AX_SAFE_FREE(animCellIndex);
    AX_SAFE_FREE(atlasIndex);

    AX_SAFE_FREE(modeA.dirX);
    AX_SAFE_FREE(modeA.dirY);
    AX_SAFE_FREE(modeA.radialAccel);
    AX_SAFE_FREE(modeA.tangentialAccel);

    AX_SAFE_FREE(modeB.angle);
    AX_SAFE_FREE(modeB.degreesPerSecond);
    AX_SAFE_FREE(modeB.deltaRadius);
    AX_SAFE_FREE(modeB.radius);
}

Vector<ParticleSystem*> ParticleSystem::__allInstances;
float ParticleSystem::__totalParticleCountFactor = 1.0f;

ParticleSystem::ParticleSystem()
    : _isBlendAdditive(false)
    , _isAutoRemoveOnFinish(false)
    , _plistFile("")
    , _elapsed(0)
    , _configName("")
    , _emitCounter(0)
    , _batchNode(nullptr)
    , _atlasIndex(0)
    , _transformSystemDirty(false)
    , _allocatedParticles(0)
    , _isAnimAllocated(false)
    , _isHSVAllocated(false)
    , _isOpacityFadeInAllocated(false)
    , _isScaleInAllocated(false)
    , _isActive(true)
    , _particleCount(0)
    , _duration(0)
    , _life(0)
    , _lifeVar(0)
    , _angle(0)
    , _angleVar(0)
    , _emitterMode(Mode::GRAVITY)
    , _startSize(0)
    , _startSizeVar(0)
    , _endSize(0)
    , _endSizeVar(0)
    , _startSpin(0)
    , _startSpinVar(0)
    , _endSpin(0)
    , _endSpinVar(0)
    , _spawnAngle(0)
    , _spawnAngleVar(0)
    , _hsv(0, 1, 1)
    , _hsvVar(0, 0, 0)
    , _spawnFadeIn(0)
    , _spawnFadeInVar(0)
    , _spawnScaleIn(0)
    , _spawnScaleInVar(0)
    , _emissionRate(0)
    , _totalParticles(0)
    , _texture(nullptr)
    , _blendFunc(BlendFunc::ALPHA_PREMULTIPLIED)
    , _opacityModifyRGB(false)
    , _isLifeAnimated(false)
    , _isEmitterAnimated(false)
    , _isLoopAnimated(false)
    , _animIndexCount(0)
    , _isAnimationReversed(false)
    , _undefinedIndexRect({0, 0, 0, 0})
    , _animationTimescaleInd(false)
    , _yCoordFlipped(1)
    , _isEmissionShapes(false)
    , _emissionShapeIndex(0)
    , _positionType(PositionType::FREE)
    , _paused(false)
    , _timeScale(1)
    , _fixedFPS(0)
    , _fixedFPSDelta(0)
    , _sourcePositionCompatible(true)  // In the furture this member's default value maybe false or be removed.
{
    modeA.gravity.setZero();
    modeA.speed              = 0;
    modeA.speedVar           = 0;
    modeA.tangentialAccel    = 0;
    modeA.tangentialAccelVar = 0;
    modeA.radialAccel        = 0;
    modeA.radialAccelVar     = 0;
    modeA.rotationIsDir      = false;
    modeB.startRadius        = 0;
    modeB.startRadiusVar     = 0;
    modeB.endRadius          = 0;
    modeB.endRadiusVar       = 0;
    modeB.rotatePerSecond    = 0;
    modeB.rotatePerSecondVar = 0;
}
// implementation ParticleSystem

ParticleSystem* ParticleSystem::create(std::string_view plistFile)
{
    ParticleSystem* ret = new ParticleSystem();
    if (ret->initWithFile(plistFile))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return ret;
}

ParticleSystem* ParticleSystem::createWithTotalParticles(int numberOfParticles)
{
    ParticleSystem* ret = new ParticleSystem();
    if (ret->initWithTotalParticles(numberOfParticles))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return ret;
}

// static
Vector<ParticleSystem*>& ParticleSystem::getAllParticleSystems()
{
    return __allInstances;
}

bool ParticleSystem::allocAnimationMem()
{
    if (!_isAnimAllocated)
    {
        _particleData.animTimeLength = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.animTimeDelta  = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.animIndex      = (unsigned short*)malloc(_totalParticles * sizeof(unsigned short));
        _particleData.animCellIndex  = (unsigned short*)malloc(_totalParticles * sizeof(unsigned short));
        if (_particleData.animTimeLength && _particleData.animTimeDelta && _particleData.animIndex &&
            _particleData.animCellIndex)
            return _isAnimAllocated = true;
        else
            // If any of the above allocations fail, then we safely deallocate the ones that succeeded.
            deallocAnimationMem();
    }
    return false;
}

void ParticleSystem::deallocAnimationMem()
{
    AX_SAFE_FREE(_particleData.animTimeLength);
    AX_SAFE_FREE(_particleData.animTimeDelta);
    AX_SAFE_FREE(_particleData.animIndex);
    AX_SAFE_FREE(_particleData.animCellIndex);
    _isAnimAllocated = false;
}

bool ParticleSystem::allocHSVMem()
{
    if (!_isHSVAllocated)
    {
        _particleData.hue = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.sat = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.val = (float*)malloc(_totalParticles * sizeof(float));
        if (_particleData.hue && _particleData.sat && _particleData.val)
            return _isHSVAllocated = true;
        else
            // If any of the above allocations fail, then we safely deallocate the ones that succeeded.
            deallocHSVMem();
    }
    return false;
}

void ParticleSystem::deallocHSVMem()
{
    AX_SAFE_FREE(_particleData.hue);
    AX_SAFE_FREE(_particleData.sat);
    AX_SAFE_FREE(_particleData.val);
    _isHSVAllocated = false;
}

bool ParticleSystem::allocOpacityFadeInMem()
{
    if (!_isOpacityFadeInAllocated)
    {
        _particleData.opacityFadeInDelta  = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.opacityFadeInLength = (float*)malloc(_totalParticles * sizeof(float));
        if (_particleData.opacityFadeInDelta && _particleData.opacityFadeInLength)
            return _isOpacityFadeInAllocated = true;
        else
            // If any of the above allocations fail, then we safely deallocate the ones that succeeded.
            deallocOpacityFadeInMem();
    }
    return false;
}

void ParticleSystem::deallocOpacityFadeInMem()
{
    AX_SAFE_FREE(_particleData.opacityFadeInDelta);
    AX_SAFE_FREE(_particleData.opacityFadeInLength);
    _isOpacityFadeInAllocated = false;
}

bool ParticleSystem::allocScaleInMem()
{
    if (!_isScaleInAllocated)
    {
        _particleData.scaleInDelta  = (float*)malloc(_totalParticles * sizeof(float));
        _particleData.scaleInLength = (float*)malloc(_totalParticles * sizeof(float));
        if (_particleData.scaleInDelta && _particleData.scaleInLength)
            return _isScaleInAllocated = true;
        else
            // If any of the above allocations fail, then we safely deallocate the ones that succeeded.
            deallocScaleInMem();
    }
    return false;
}

void ParticleSystem::deallocScaleInMem()
{
    AX_SAFE_FREE(_particleData.scaleInDelta);
    AX_SAFE_FREE(_particleData.scaleInLength);
    _isScaleInAllocated = false;
}

void ParticleSystem::setTotalParticleCountFactor(float factor)
{
    __totalParticleCountFactor = factor;
}

bool ParticleSystem::init()
{
    return initWithTotalParticles(150);
}

bool ParticleSystem::initWithFile(std::string_view plistFile)
{
    bool ret      = false;
    _plistFile    = FileUtils::getInstance()->fullPathForFilename(plistFile);
    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(_plistFile);

    AXASSERT(!dict.empty(), "Particles: file not found");

    // FIXME: compute path from a path, should define a function somewhere to do it
    auto listFilePath = plistFile;
    if (listFilePath.find('/') != string::npos)
    {
        listFilePath = listFilePath.substr(0, listFilePath.rfind('/') + 1);
        ret          = this->initWithDictionary(dict, listFilePath);
    }
    else
    {
        ret = this->initWithDictionary(dict, "");
    }

    return ret;
}

bool ParticleSystem::initWithDictionary(const ValueMap& dictionary)
{
    return initWithDictionary(dictionary, "");
}

bool ParticleSystem::initWithDictionary(const ValueMap& dictionary, std::string_view dirname)
{
    bool ret              = false;
    Image* image          = nullptr;
    do
    {
        int maxParticles = optValue(dictionary, "maxParticles").asInt();
        // self, not super
        if (this->initWithTotalParticles(maxParticles))
        {
            // Emitter name in particle designer 2.0
            _configName = optValue(dictionary, "configName").asString();

            // angle
            _angle    = optValue(dictionary, "angle").asFloat();
            _angleVar = optValue(dictionary, "angleVariance").asFloat();

            // duration
            _duration = optValue(dictionary, "duration").asFloat();

            // blend function
            if (!_configName.empty())
            {
                _blendFunc.src = utils::toBackendBlendFactor((int)optValue(dictionary, "blendFuncSource").asFloat());
            }
            else
            {
                _blendFunc.src = utils::toBackendBlendFactor(optValue(dictionary, "blendFuncSource").asInt());
            }
            _blendFunc.dst = utils::toBackendBlendFactor(optValue(dictionary, "blendFuncDestination").asInt());

            // color
            _startColor.r = optValue(dictionary, "startColorRed").asFloat();
            _startColor.g = optValue(dictionary, "startColorGreen").asFloat();
            _startColor.b = optValue(dictionary, "startColorBlue").asFloat();
            _startColor.a = optValue(dictionary, "startColorAlpha").asFloat();

            _startColorVar.r = optValue(dictionary, "startColorVarianceRed").asFloat();
            _startColorVar.g = optValue(dictionary, "startColorVarianceGreen").asFloat();
            _startColorVar.b = optValue(dictionary, "startColorVarianceBlue").asFloat();
            _startColorVar.a = optValue(dictionary, "startColorVarianceAlpha").asFloat();

            _endColor.r = optValue(dictionary, "finishColorRed").asFloat();
            _endColor.g = optValue(dictionary, "finishColorGreen").asFloat();
            _endColor.b = optValue(dictionary, "finishColorBlue").asFloat();
            _endColor.a = optValue(dictionary, "finishColorAlpha").asFloat();

            _endColorVar.r = optValue(dictionary, "finishColorVarianceRed").asFloat();
            _endColorVar.g = optValue(dictionary, "finishColorVarianceGreen").asFloat();
            _endColorVar.b = optValue(dictionary, "finishColorVarianceBlue").asFloat();
            _endColorVar.a = optValue(dictionary, "finishColorVarianceAlpha").asFloat();

            // particle size
            _startSize    = optValue(dictionary, "startParticleSize").asFloat();
            _startSizeVar = optValue(dictionary, "startParticleSizeVariance").asFloat();
            _endSize      = optValue(dictionary, "finishParticleSize").asFloat();
            _endSizeVar   = optValue(dictionary, "finishParticleSizeVariance").asFloat();

            // position
            float x = optValue(dictionary, "sourcePositionx").asFloat();
            float y = optValue(dictionary, "sourcePositiony").asFloat();
            if (!_sourcePositionCompatible)
            {
                this->setSourcePosition(Vec2(x, y));
            }
            else
            {
                this->setPosition(Vec2(x, y));
            }
            _posVar.x = optValue(dictionary, "sourcePositionVariancex").asFloat();
            _posVar.y = optValue(dictionary, "sourcePositionVariancey").asFloat();

            // Spinning
            _startSpin    = optValue(dictionary, "rotationStart").asFloat();
            _startSpinVar = optValue(dictionary, "rotationStartVariance").asFloat();
            _endSpin      = optValue(dictionary, "rotationEnd").asFloat();
            _endSpinVar   = optValue(dictionary, "rotationEndVariance").asFloat();

            _emitterMode = (Mode)optValue(dictionary, "emitterType").asInt();

            // Mode A: Gravity + tangential accel + radial accel
            if (_emitterMode == Mode::GRAVITY)
            {
                // gravity
                modeA.gravity.x = optValue(dictionary, "gravityx").asFloat();
                modeA.gravity.y = optValue(dictionary, "gravityy").asFloat();

                // speed
                modeA.speed    = optValue(dictionary, "speed").asFloat();
                modeA.speedVar = optValue(dictionary, "speedVariance").asFloat();

                // radial acceleration
                modeA.radialAccel    = optValue(dictionary, "radialAcceleration").asFloat();
                modeA.radialAccelVar = optValue(dictionary, "radialAccelVariance").asFloat();

                // tangential acceleration
                modeA.tangentialAccel    = optValue(dictionary, "tangentialAcceleration").asFloat();
                modeA.tangentialAccelVar = optValue(dictionary, "tangentialAccelVariance").asFloat();

                // rotation is dir
                modeA.rotationIsDir = optValue(dictionary, "rotationIsDir").asBool();
            }

            // or Mode B: radius movement
            else if (_emitterMode == Mode::RADIUS)
            {
                if (!_configName.empty())
                {
                    modeB.startRadius = optValue(dictionary, "maxRadius").asInt();
                }
                else
                {
                    modeB.startRadius = optValue(dictionary, "maxRadius").asFloat();
                }
                modeB.startRadiusVar = optValue(dictionary, "maxRadiusVariance").asFloat();
                if (!_configName.empty())
                {
                    modeB.endRadius = optValue(dictionary, "minRadius").asInt();
                }
                else
                {
                    modeB.endRadius = optValue(dictionary, "minRadius").asFloat();
                }

                modeB.endRadiusVar = optValue(dictionary, "minRadiusVariance").asFloat();

                if (!_configName.empty())
                {
                    modeB.rotatePerSecond = optValue(dictionary, "rotatePerSecond").asInt();
                }
                else
                {
                    modeB.rotatePerSecond = optValue(dictionary, "rotatePerSecond").asFloat();
                }
                modeB.rotatePerSecondVar = optValue(dictionary, "rotatePerSecondVariance").asFloat();
            }
            else
            {
                AXASSERT(false, "Invalid emitterType in config file");
                AX_BREAK_IF(true);
            }

            // life span
            _life    = optValue(dictionary, "particleLifespan").asFloat();
            _lifeVar = optValue(dictionary, "particleLifespanVariance").asFloat();

            // emission Rate
            _emissionRate = _totalParticles / _life;

            // don't get the internal texture if a batchNode is used
            if (!_batchNode)
            {
                // Set a compatible default for the alpha transfer
                _opacityModifyRGB = false;

                // texture
                // Try to get the texture from the cache
                std::string textureName = optValue(dictionary, "textureFileName").asString();

                size_t rPos = textureName.rfind('/');

                if (rPos != string::npos)
                {
                    string textureDir = textureName.substr(0, rPos + 1);

                    if (!dirname.empty() && textureDir != dirname)
                    {
                        textureName = textureName.substr(rPos + 1);
                        textureName.insert(0, dirname);  // textureName = dirname + textureName;
                    }
                }
                else if (!dirname.empty() && !textureName.empty())
                {
                    textureName.insert(0, dirname);  // textureName = dirname + textureName;
                }

                Texture2D* tex = nullptr;

                if (!textureName.empty())
                {
                    // set not pop-up message box when load image failed
                    bool notify = FileUtils::getInstance()->isPopupNotify();
                    FileUtils::getInstance()->setPopupNotify(false);
                    tex = _director->getTextureCache()->addImage(textureName);
                    // reset the value of UIImage notify
                    FileUtils::getInstance()->setPopupNotify(notify);
                }

                if (tex)
                {
                    setTexture(tex);
                }
                else if (dictionary.find("textureImageData") != dictionary.end())
                {
                    std::string textureData = dictionary.at("textureImageData").asString();
                    AXASSERT(!textureData.empty(), "textureData can't be empty!");

                    auto dataLen = textureData.size();
                    if (dataLen != 0)
                    {
                        // if it fails, try to get it from the base64-gzipped data
                        yasio::byte_buffer buffer = utils::base64Decode(textureData);
                        AXASSERT(!buffer.empty(), "CCParticleSystem: error decoding textureImageData");
                        AX_BREAK_IF(buffer.empty());

                        auto deflated           = ZipUtils::decompressGZ(std::span{buffer});
                        AXASSERT(!deflated.empty(), "CCParticleSystem: error ungzipping textureImageData");
                        AX_BREAK_IF(deflated.empty());

                        // For android, we should retain it in VolatileTexture::addImage which invoked in
                        // Director::getInstance()->getTextureCache()->addUIImage()
                        image     = new Image();
                        const auto imageDataLen = deflated.size();
                        bool isOK = image->initWithImageData(deflated.release_pointer(), imageDataLen, true);
                        AXASSERT(isOK, "CCParticleSystem: error init image with Data");
                        AX_BREAK_IF(!isOK);

                        setTexture(_director->getTextureCache()->addImage(image, _plistFile + textureName));

                        image->release();
                    }
                }

                _yCoordFlipped = optValue(dictionary, "yCoordFlipped").asInt(1);

                if (!this->_texture)
                    AXLOGW("axmol: Warning: ParticleSystemQuad system without a texture");
            }
            ret = true;
        }
    } while (0);
    return ret;
}

bool ParticleSystem::initWithTotalParticles(int numberOfParticles)
{
    _totalParticles = numberOfParticles;

    _particleData.release();

    if (!_particleData.init(_totalParticles))
    {
        AXLOGW("Particle system: not enough memory");
        this->release();
        return false;
    }
    _allocatedParticles = numberOfParticles;

    if (_batchNode)
    {
        for (int i = 0; i < _totalParticles; i++)
        {
            _particleData.atlasIndex[i] = i;
        }
    }
    // default, active
    _isActive = true;

    // default blend function
    _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;

    // default movement type;
    _positionType = PositionType::FREE;

    // by default be in mode A:
    _emitterMode = Mode::GRAVITY;

    // default: modulate
    // FIXME:: not used
    //    colorModulate = YES;

    _isAutoRemoveOnFinish = false;

    // Optimization: compile updateParticle method
    // updateParticleSel = @selector(updateQuadWithParticle:newPosition:);
    // updateParticleImp = (AX_UPDATE_PARTICLE_IMP) [self methodForSelector:updateParticleSel];
    // for batchNode
    _transformSystemDirty = false;

    return true;
}

ParticleSystem::~ParticleSystem()
{
    // Since the scheduler retains the "target (in this case the ParticleSystem)
    // it is not needed to call "unscheduleUpdate" here. In fact, it will be called in "cleanup"
    // unscheduleUpdate();
    _particleData.release();
    _animations.clear();
    AX_SAFE_RELEASE(_texture);
}

void ParticleSystem::addParticles(int count, int animationIndex, int animationCellIndex)
{
    if (_paused)
        return;

    // Try to add as many particles as possible without overflowing.
    count = MIN(int(_totalParticles * __totalParticleCountFactor) - _particleCount, count);

    animationCellIndex = MIN(animationCellIndex, _animIndexCount - 1);
    animationIndex     = MIN(animationIndex, _animIndexCount - 1);

    int start = _particleCount;
    _particleCount += count;

    // life
    for (int i = start; i < _particleCount; ++i)
    {
        float particleLife               = _life + _lifeVar * _rng.rangef();
        _particleData.totalTimeToLive[i] = MAX(0, particleLife);
        _particleData.timeToLive[i]      = MAX(0, particleLife);
    }

    if (_isEmissionShapes)
    {
        for (int i = start; i < _particleCount; ++i)
        {
            if (_emissionShapes.empty())
            {
                _particleData.posx[i] = _sourcePosition.x + _posVar.x * _rng.rangef();
                _particleData.posy[i] = _sourcePosition.y + _posVar.y * _rng.rangef();
                continue;
            }

            auto randElem = _rng.float01();
            auto& shape   = _emissionShapes[MIN(randElem * _emissionShapes.size(), _emissionShapes.size() - 1)];

            switch (shape.type)
            {
            case EmissionShapeType::POINT:
            {
                _particleData.posx[i] = _sourcePosition.x + shape.x;
                _particleData.posy[i] = _sourcePosition.y + shape.y;

                break;
            }
            case EmissionShapeType::RECT:
            {
                _particleData.posx[i] = _sourcePosition.x + shape.x + shape.innerWidth / 2 * _rng.rangef();
                _particleData.posy[i] = _sourcePosition.y + shape.y + shape.innerHeight / 2 * _rng.rangef();

                break;
            }
            case EmissionShapeType::RECTTORUS:
            {
                float width  = (shape.outerWidth - shape.innerWidth) * _rng.float01() + shape.innerWidth;
                float height = (shape.outerHeight - shape.innerHeight) * _rng.float01() + shape.innerHeight;
                width        = _rng.rangef() < 0.0F ? width * -1 : width;
                height       = _rng.rangef() < 0.0F ? height * -1 : height;
                float prob   = _rng.rangef();
                _particleData.posx[i] = _sourcePosition.x + shape.x + width / 2 * (prob >= 0.0F ? 1.0F : _rng.rangef());
                _particleData.posy[i] = _sourcePosition.y + shape.y + height / 2 * (prob < 0.0F ? 1.0F : _rng.rangef());

                break;
            }
            case EmissionShapeType::CIRCLE:
            {
                auto val              = _rng.float01() * shape.innerRadius / shape.innerRadius;
                val                   = powf(val, 1 / shape.edgeBias);
                auto point            = Vec2(0.0F, val * shape.innerRadius);
                point                 = point.rotateByAngle(Vec2::ZERO, -AX_DEGREES_TO_RADIANS(shape.coneOffset + shape.coneAngle / 2 * _rng.rangef()));
                _particleData.posx[i] = _sourcePosition.x + shape.x + point.x / 2;
                _particleData.posy[i] = _sourcePosition.y + shape.y + point.y / 2;

                break;
            }
            case EmissionShapeType::TORUS:
            {
                auto val              = _rng.float01() * shape.outerRadius / shape.outerRadius;
                val                   = powf(val, 1 / shape.edgeBias);
                auto point            = Vec2(0.0F, ((val * (shape.outerRadius - shape.innerRadius) + shape.outerRadius) - (shape.outerRadius - shape.innerRadius)));
                point                 = point.rotateByAngle(Vec2::ZERO, -AX_DEGREES_TO_RADIANS(shape.coneOffset + shape.coneAngle / 2 * _rng.rangef()));
                _particleData.posx[i] = _sourcePosition.x + shape.x + point.x / 2;
                _particleData.posy[i] = _sourcePosition.y + shape.y + point.y / 2;

                break;
            }
            case EmissionShapeType::TEXTURE_ALPHA_MASK:
            {
                auto& mask = ParticleEmissionMaskCache::getInstance()->getEmissionMask(shape.fourccId);

                Vec2 pos            = {shape.x, shape.y};
                Vec2 size           = mask.size;
                Vec2 overrideSize   = {shape.innerWidth, shape.innerHeight};
                Vec2 scale          = {shape.outerWidth, shape.outerHeight};
                float angle         = shape.coneOffset;

                if (overrideSize.isZero())
                    overrideSize = mask.size;

                Vec2 point = {0, 0};

                int rand0 = _rng.float01() * mask.points.size();
                auto index = MIN(rand0, mask.points.size() - 1);
                point = mask.points[index];

                point -= size / 2;

                point.x = point.x / size.x * overrideSize.x * scale.x;
                point.y = point.y / size.y * overrideSize.y * scale.y;

                point = point.rotateByAngle(Vec2::ZERO, -AX_DEGREES_TO_RADIANS(angle));

                _particleData.posx[i] = _sourcePosition.x + shape.x + point.x;
                _particleData.posy[i] = _sourcePosition.y + shape.y + point.y;

                break;
            }
            }
        }
    }
    else
    {
        // position
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.posx[i] = _sourcePosition.x + _posVar.x * _rng.rangef();
        }

        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.posy[i] = _sourcePosition.y + _posVar.y * _rng.rangef();
        }
    }

    if (animationCellIndex != -1 || animationIndex != -1)
        allocAnimationMem();

    if (_isAnimAllocated)
    {
        if (animationCellIndex != -1)
            std::fill_n(_particleData.animCellIndex + start, _particleCount - start, animationCellIndex);
        else
            std::fill_n(_particleData.animCellIndex + start, _particleCount - start, 0xFFFF);

        if (animationIndex != -1)
        {
            for (int i = start; i < _particleCount; ++i)
            {
                _particleData.animIndex[i] = animationIndex;
                auto& descriptor           = _animations.at(animationIndex);
                _particleData.animTimeLength[i] =
                    descriptor.animationSpeed + descriptor.animationSpeedVariance * _rng.rangef();
            }
        }
    }

    if (_isLifeAnimated || _isEmitterAnimated || _isLoopAnimated)
    {
        if (animationCellIndex == -1 && _isEmitterAnimated)
        {
            for (int i = start; i < _particleCount; ++i)
            {
                int rand0                      = _rng.float01() * _animIndexCount;
                _particleData.animCellIndex[i] = MIN(rand0, _animIndexCount - 1);
            }
        }

        if (animationIndex == -1 && !_animations.empty())
        {
            if (_randomAnimations.empty())
                setMultiAnimationRandom();

            for (int i = start; i < _particleCount; ++i)
            {
                int rand0                  = _rng.float01() * _randomAnimations.size();
                auto index                  = MIN(rand0, _randomAnimations.size() - 1);
                _particleData.animIndex[i] = _randomAnimations[index];
                auto& descriptor           = _animations.at(_particleData.animIndex[i]);
                _particleData.animTimeLength[i] =
                    descriptor.animationSpeed + descriptor.animationSpeedVariance * _rng.rangef();
            }
        }

        if (_isEmitterAnimated || _isLoopAnimated)
            std::fill_n(_particleData.animTimeDelta + start, _particleCount - start, 0.f);
    }

    // color
#define SET_COLOR(c, b, v)                          \
    for (int i = start; i < _particleCount; ++i)    \
    {                                               \
        c[i] = clampf(b + v * _rng.rangef(), 0, 1); \
    }

    SET_COLOR(_particleData.colorR, _startColor.r, _startColorVar.r);
    SET_COLOR(_particleData.colorG, _startColor.g, _startColorVar.g);
    SET_COLOR(_particleData.colorB, _startColor.b, _startColorVar.b);
    SET_COLOR(_particleData.colorA, _startColor.a, _startColorVar.a);

    SET_COLOR(_particleData.deltaColorR, _endColor.r, _endColorVar.r);
    SET_COLOR(_particleData.deltaColorG, _endColor.g, _endColorVar.g);
    SET_COLOR(_particleData.deltaColorB, _endColor.b, _endColorVar.b);
    SET_COLOR(_particleData.deltaColorA, _endColor.a, _endColorVar.a);

#define SET_DELTA_COLOR(c, dc)                                \
    for (int i = start; i < _particleCount; ++i)              \
    {                                                         \
        dc[i] = (dc[i] - c[i]) / _particleData.timeToLive[i]; \
    }

    SET_DELTA_COLOR(_particleData.colorR, _particleData.deltaColorR);
    SET_DELTA_COLOR(_particleData.colorG, _particleData.deltaColorG);
    SET_DELTA_COLOR(_particleData.colorB, _particleData.deltaColorB);
    SET_DELTA_COLOR(_particleData.colorA, _particleData.deltaColorA);

    // opacity fade in
    if (_isOpacityFadeInAllocated)
    {
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.opacityFadeInLength[i] = _spawnFadeIn + _spawnFadeInVar * _rng.rangef();
        }
        std::fill_n(_particleData.opacityFadeInDelta + start, _particleCount - start, 0.0F);
    }

    // scale fade in
    if (_isScaleInAllocated)
    {
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.scaleInLength[i] = _spawnScaleIn + _spawnScaleInVar * _rng.rangef();
        }
        std::fill_n(_particleData.scaleInDelta + start, _particleCount - start, 0.0F);
    }

    // hue saturation value color
    if (_isHSVAllocated)
    {
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.hue[i] = _hsv.h + _hsvVar.h * _rng.rangef();
        }

        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.sat[i] = _hsv.s + _hsvVar.s * _rng.rangef();
        }

        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.val[i] = _hsv.v + _hsvVar.v * _rng.rangef();
        }
    }

    // size
    for (int i = start; i < _particleCount; ++i)
    {
        _particleData.size[i] = _startSize + _startSizeVar * _rng.rangef();
        _particleData.size[i] = MAX(0, _particleData.size[i]);
    }

    if (_endSize != static_cast<float>(START_SIZE_EQUAL_TO_END_SIZE))
    {
        for (int i = start; i < _particleCount; ++i)
        {
            float endSize              = _endSize + _endSizeVar * _rng.rangef();
            endSize                    = MAX(0, endSize);
            _particleData.deltaSize[i] = (endSize - _particleData.size[i]) / _particleData.timeToLive[i];
        }
    }
    else
        std::fill_n(_particleData.deltaSize + start, _particleCount - start, 0.0F);

    // rotation
    for (int i = start; i < _particleCount; ++i)
    {
        _particleData.rotation[i] = _startSpin + _startSpinVar * _rng.rangef();
    }
    for (int i = start; i < _particleCount; ++i)
    {
        float endA                     = _endSpin + _endSpinVar * _rng.rangef();
        _particleData.deltaRotation[i] = (endA - _particleData.rotation[i]) / _particleData.timeToLive[i];
    }

    // static rotation
    for (int i = start; i < _particleCount; ++i)
    {
        _particleData.staticRotation[i] = _spawnAngle + _spawnAngleVar * _rng.rangef();
    }

    // position
    Vec2 pos;
    if (_positionType == PositionType::FREE)
    {
        pos = this->convertToWorldSpace(Vec2::ZERO);
    }
    else if (_positionType == PositionType::RELATIVE)
    {
        pos = _position;
    }
    std::fill_n(_particleData.startPosX + start, _particleCount - start, pos.x);
    std::fill_n(_particleData.startPosY + start, _particleCount - start, pos.y);

    // Mode Gravity: A
    if (_emitterMode == Mode::GRAVITY)
    {

        // radial accel
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.modeA.radialAccel[i] = modeA.radialAccel + modeA.radialAccelVar * _rng.rangef();
        }

        // tangential accel
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.modeA.tangentialAccel[i] = modeA.tangentialAccel + modeA.tangentialAccelVar * _rng.rangef();
        }

        // rotation is dir
        if (modeA.rotationIsDir)
        {
            for (int i = start; i < _particleCount; ++i)
            {
                float a = AX_DEGREES_TO_RADIANS(_angle + _angleVar * _rng.rangef());
                Vec2 v(cosf(a), sinf(a));
                float s                     = modeA.speed + modeA.speedVar * _rng.rangef();
                Vec2 dir                    = v * s;
                _particleData.modeA.dirX[i] = dir.x;  // v * s ;
                _particleData.modeA.dirY[i] = dir.y;
                _particleData.rotation[i]   = -AX_RADIANS_TO_DEGREES(dir.getAngle());
            }
        }
        else
        {
            for (int i = start; i < _particleCount; ++i)
            {
                float a = AX_DEGREES_TO_RADIANS(_angle + _angleVar * _rng.rangef());
                Vec2 v(cosf(a), sinf(a));
                float s                     = modeA.speed + modeA.speedVar * _rng.rangef();
                Vec2 dir                    = v * s;
                _particleData.modeA.dirX[i] = dir.x;  // v * s ;
                _particleData.modeA.dirY[i] = dir.y;
            }
        }
    }

    // Mode Radius: B
    else
    {
        // Need to check by Jacky
        //  Set the default diameter of the particle from the source position
        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.modeB.radius[i] = modeB.startRadius + modeB.startRadiusVar * _rng.rangef();
        }

        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.modeB.angle[i] = AX_DEGREES_TO_RADIANS(_angle + _angleVar * _rng.rangef());
        }

        for (int i = start; i < _particleCount; ++i)
        {
            _particleData.modeB.degreesPerSecond[i] =
                AX_DEGREES_TO_RADIANS(modeB.rotatePerSecond + modeB.rotatePerSecondVar * _rng.rangef());
        }

        if (modeB.endRadius == static_cast<float>(START_RADIUS_EQUAL_TO_END_RADIUS))
            std::fill_n(_particleData.modeB.deltaRadius + start, _particleCount - start, 0.0F);
        else
        {
            for (int i = start; i < _particleCount; ++i)
            {
                float endRadius = modeB.endRadius + modeB.endRadiusVar * _rng.rangef();
                _particleData.modeB.deltaRadius[i] =
                    (endRadius - _particleData.modeB.radius[i]) / _particleData.timeToLive[i];
            }
        }
    }
}

void ParticleSystem::setAnimationDescriptor(unsigned short indexOfDescriptor,
                                            float time,
                                            float timeVariance,
                                            const std::vector<unsigned short>& indices,
                                            bool reverse)
{
    auto iter = _animations.find(indexOfDescriptor);
    if (iter == _animations.end())
        iter = _animations.emplace(indexOfDescriptor, ParticleAnimationDescriptor{}).first;

    auto& desc                  = iter->second;
    desc.animationSpeed         = time;
    desc.animationSpeedVariance = timeVariance;
    desc.animationIndices       = std::move(indices);
    desc.reverseIndices         = reverse;
}

void ParticleSystem::resetEmissionShapes()
{
    _emissionShapeIndex = 0;
    _emissionShapes.clear();
}

void ParticleSystem::addEmissionShape(EmissionShape shape)
{
    setEmissionShape(_emissionShapeIndex, shape);
}

void ParticleSystem::setEmissionShape(unsigned short index, EmissionShape shape)
{
    auto iter = _emissionShapes.find(index);
    if (iter == _emissionShapes.end())
    {
        iter = _emissionShapes.emplace(index, EmissionShape{}).first;
        _emissionShapeIndex++;
    }

    iter->second = shape;
}

EmissionShape ParticleSystem::createMaskShape(std::string_view maskId,
                                              Vec2 pos,
                                              Vec2 overrideSize,
                                              Vec2 scale,
                                              float angle)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::TEXTURE_ALPHA_MASK;

    shape.fourccId = utils::fourccValue(maskId);

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerWidth  = overrideSize.x;
    shape.innerHeight = overrideSize.y;

    shape.outerWidth  = scale.x;
    shape.outerHeight = scale.y;

    shape.coneOffset = angle;

    return shape;
}

EmissionShape ParticleSystem::createPointShape(Vec2 pos)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::POINT;

    shape.x = pos.x;
    shape.y = pos.y;

    return shape;
}

EmissionShape ParticleSystem::createRectShape(Vec2 pos, Size size)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::RECT;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerWidth  = size.x;
    shape.innerHeight = size.y;

    return shape;
}

EmissionShape ParticleSystem::createRectTorusShape(Vec2 pos, Size innerSize, Size outerSize)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::RECTTORUS;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerWidth  = innerSize.x;
    shape.innerHeight = innerSize.y;

    shape.outerWidth  = outerSize.x;
    shape.outerHeight = outerSize.y;

    return shape;
}

EmissionShape ParticleSystem::createCircleShape(Vec2 pos, float radius, float edgeBias)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::CIRCLE;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerRadius = radius;

    shape.coneOffset = 0;
    shape.coneAngle  = 360;

    shape.edgeBias = edgeBias;

    return shape;
}

EmissionShape ParticleSystem::createConeShape(Vec2 pos,
                                                              float radius,
                                                              float offset,
                                                              float angle,
                                                              float edgeBias)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::CIRCLE;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerRadius = radius;

    shape.coneOffset = offset;
    shape.coneAngle  = angle;

    shape.edgeBias = edgeBias;

    return shape;
}

EmissionShape ParticleSystem::createTorusShape(Vec2 pos,
                                                               float innerRadius,
                                                               float outerRadius,
                                                               float edgeBias)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::TORUS;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerRadius = innerRadius;
    shape.outerRadius = outerRadius;

    shape.coneOffset = 0;
    shape.coneAngle  = 360;

    shape.edgeBias = edgeBias;

    return shape;
}

EmissionShape ParticleSystem::createConeTorusShape(Vec2 pos,
                                                                   float innerRadius,
                                                                   float outerRadius,
                                                                   float offset,
                                                                   float angle,
                                                                   float edgeBias)
{
    EmissionShape shape{};

    shape.type = EmissionShapeType::TORUS;

    shape.x = pos.x;
    shape.y = pos.y;

    shape.innerRadius = innerRadius;
    shape.outerRadius = outerRadius;

    shape.coneOffset = offset;
    shape.coneAngle  = angle;

    shape.edgeBias = edgeBias;

    return shape;
}

void ParticleSystem::setLifeAnimation(bool enabled)
{
    if (enabled && !allocAnimationMem())
        return;

    if (!enabled)
        deallocAnimationMem();

    _isLifeAnimated    = enabled;
    _isEmitterAnimated = false;
    _isLoopAnimated    = false;
}

void ParticleSystem::setEmitterAnimation(bool enabled)
{
    if (enabled && !allocAnimationMem())
        return;

    if (!enabled)
        deallocAnimationMem();

    _isEmitterAnimated = enabled;
    _isLifeAnimated    = false;
    _isLoopAnimated    = false;
}

void ParticleSystem::setLoopAnimation(bool enabled)
{
    if (enabled && !allocAnimationMem())
        return;

    if (!enabled)
        deallocAnimationMem();

    _isLoopAnimated    = enabled;
    _isEmitterAnimated = false;
    _isLifeAnimated    = false;
}

void ParticleSystem::resetAnimationIndices()
{
    _animIndexCount = 0;
    _animationIndices.clear();
}

void ParticleSystem::resetAnimationDescriptors()
{
    _animations.clear();
    _randomAnimations.clear();
}

void ParticleSystem::setMultiAnimationRandom()
{
    _randomAnimations.clear();
    for (auto&& a : _animations)
        _randomAnimations.emplace_back(a.first);
}

void ParticleSystem::setAnimationIndicesAtlas()
{
    // VERTICAL
    if (_texture->getPixelsHigh() > _texture->getPixelsWide())
    {
        setAnimationIndicesAtlas(_texture->getPixelsWide(), ParticleSystem::TexAnimDir::VERTICAL);
        return;
    }

    // HORIZONTAL
    if (_texture->getPixelsWide() > _texture->getPixelsHigh())
    {
        setAnimationIndicesAtlas(_texture->getPixelsHigh(), ParticleSystem::TexAnimDir::HORIZONTAL);
        return;
    }

    AXASSERT(false, "Couldn't figure out the atlas size and direction.");
}

void ParticleSystem::setAnimationIndicesAtlas(unsigned int unifiedCellSize, TexAnimDir direction)
{
    AXASSERT(unifiedCellSize > 0, "A cell cannot have a size of zero.");

    resetAnimationIndices();

    auto texWidth  = _texture->getPixelsWide();
    auto texHeight = _texture->getPixelsHigh();

    switch (direction)
    {
    case TexAnimDir::VERTICAL:
    {
        for (short i = 0; i < short(texHeight / unifiedCellSize); i++)
        {
            Rect frame{};

            frame.origin.x = 0;
            frame.origin.y = unifiedCellSize * i;

            frame.size.x = texWidth;
            frame.size.y = unifiedCellSize;

            addAnimationIndex(_animIndexCount, frame);
        }

        break;
    };
    case TexAnimDir::HORIZONTAL:
    {
        for (short i = 0; i < short(texWidth / unifiedCellSize); i++)
        {
            Rect frame{};

            frame.origin.x = unifiedCellSize * i;
            frame.origin.y = 0;

            frame.size.x = unifiedCellSize;
            frame.size.y = texHeight;

            addAnimationIndex(_animIndexCount, frame);
        }

        break;
    };
    }
}

bool ParticleSystem::addAnimationIndex(std::string_view frameName)
{
    return addAnimationIndex(_animIndexCount, frameName);
}

bool ParticleSystem::addAnimationIndex(unsigned short index, std::string_view frameName)
{
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);

    if (frame)
        return addAnimationIndex(index, frame);
    return false;
}

bool ParticleSystem::addAnimationIndex(ax::SpriteFrame* frame)
{
    return addAnimationIndex(_animIndexCount, frame);
}

bool ParticleSystem::addAnimationIndex(unsigned short index, ax::SpriteFrame* frame)
{
    if (frame)
    {
        auto rect   = frame->getRectInPixels();
        rect.size.x = frame->getOriginalSizeInPixels().x;
        rect.size.y = frame->getOriginalSizeInPixels().y;
        return addAnimationIndex(index, rect, frame->isRotated());
    }
    return false;
}

bool ParticleSystem::addAnimationIndex(unsigned short index, ax::Rect rect, bool rotated)
{
    auto iter = _animationIndices.find(index);
    if (iter == _animationIndices.end())
    {
        iter = _animationIndices.emplace(index, ParticleFrameDescriptor{}).first;
        _animIndexCount++;
    }

    auto& desc     = iter->second;
    desc.rect      = rect;
    desc.isRotated = rotated;

    return true;
}

void ParticleSystem::simulate(float seconds, float frameRate)
{
    seconds             = seconds == static_cast<float>(SIMULATION_USE_PARTICLE_LIFETIME) ? getLife() + getLifeVar() : seconds;
    frameRate           = frameRate == static_cast<float>(SIMULATION_USE_GAME_ANIMATION_INTERVAL)
                              ? 1.0F / Director::getInstance()->getAnimationInterval()
                              : frameRate;
    auto delta          = 1.0F / frameRate;
    if (seconds > delta)
    {
        while (seconds > 0.0F)
        {
            this->update(delta);
            seconds -= delta;
        }
        this->update(seconds);
    }
    else
        this->update(seconds);
}

void ParticleSystem::resimulate(float seconds, float frameRate)
{
    this->resetSystem();
    this->simulate(seconds, frameRate);
}

void ParticleSystem::onEnter()
{
    Node::onEnter();

    // update after action in run!
    this->scheduleUpdateWithPriority(1);

    __allInstances.pushBack(this);
}

void ParticleSystem::onExit()
{
    this->unscheduleUpdate();
    Node::onExit();

    auto iter = std::find(std::begin(__allInstances), std::end(__allInstances), this);
    if (iter != std::end(__allInstances))
    {
        __allInstances.erase(iter);
    }
}

void ParticleSystem::stopSystem()
{
    _isActive    = false;
    _elapsed     = _duration;
    _emitCounter = 0;
}

void ParticleSystem::resetSystem()
{
    _isActive = true;
    _elapsed  = 0;
    std::fill_n(_particleData.timeToLive, _particleCount, 0.0F);
}

bool ParticleSystem::isFull()
{
    return (_particleCount == _totalParticles);
}

// ParticleSystem - MainLoop
void ParticleSystem::update(float dt)
{
    // don't process particles nor update gl buffer when this node is invisible.
    if (!_visible)
        return;

    AX_PROFILER_START_CATEGORY(kProfilerCategoryParticles, "CCParticleSystem - update");

    if (_componentContainer && !_componentContainer->isEmpty())
    {
        _componentContainer->visit(dt);
    }

    if (_fixedFPS != 0)
    {
        _fixedFPSDelta += dt;
        if (_fixedFPSDelta < 1.0F / _fixedFPS)
        {
            updateParticleQuads();
            _transformSystemDirty = false;
            AX_PROFILER_STOP_CATEGORY(kProfilerCategoryParticles, "CCParticleSystem - update");
            return;
        }
        dt             = _fixedFPSDelta;
        _fixedFPSDelta = 0.0F;
    }

    float pureDt = dt;
    dt *= _timeScale;

    if (_isActive && _emissionRate)
    {
        float rate         = 1.0f / _emissionRate;
        int totalParticles = static_cast<int>(_totalParticles * __totalParticleCountFactor);

        // issue #1201, prevent bursts of particles, due to too high emitCounter
        if (_particleCount < totalParticles)
        {
            _emitCounter += dt;
            _emitCounter = MAX(0.0F, _emitCounter);
        }

        int emitCount = MIN(totalParticles - _particleCount, _emitCounter / rate);
        addParticles(emitCount);
        _emitCounter -= rate * emitCount;

        _elapsed += dt;
        if (_elapsed < 0.f)
            _elapsed = 0.f;
        if (_duration != static_cast<float>(DURATION_INFINITY) && _duration < _elapsed)
        {
            this->stopSystem();
        }
    }

    // The reason for using for-loops separately for every property is because
    // When the processor needs to read from or write to a location in memory,
    // it first checks whether a copy of that data is in the cpu's cache.
    // And wether if every property's memory of the particle system is continuous,
    // for the purpose of improving cache hit rate, we should process only one property in one for-loop.
    // It was proved to be effective especially for low-end devices.
    {
        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.timeToLive[i] -= dt;
        }

        if (_isOpacityFadeInAllocated)
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.opacityFadeInDelta[i] += dt;
                _particleData.opacityFadeInDelta[i] =
                    MIN(_particleData.opacityFadeInDelta[i], _particleData.opacityFadeInLength[i]);
            }
        }

        if (_isScaleInAllocated)
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.scaleInDelta[i] += dt;
                _particleData.scaleInDelta[i] = MIN(_particleData.scaleInDelta[i], _particleData.scaleInLength[i]);
            }
        }

        if (_isLifeAnimated || _isEmitterAnimated || _isLoopAnimated)
        {
            if (_isEmitterAnimated && !_animations.empty())
            {
                for (int i = 0; i < _particleCount; ++i)
                {
                    _particleData.animTimeDelta[i] += (_animationTimescaleInd ? pureDt : dt);
                    if (_particleData.animTimeDelta[i] > _particleData.animTimeLength[i])
                    {
                        auto& anim    = _animations.at(_particleData.animIndex[i]);
                        float percent = _rng.float01();
                        percent       = anim.reverseIndices ? 1.0F - percent : percent;

                        _particleData.animCellIndex[i] = anim.animationIndices[MIN(
                            percent * anim.animationIndices.size(), anim.animationIndices.size() - 1)];
                        _particleData.animTimeDelta[i] = 0;
                    }
                }
            }
            if (_isLifeAnimated && _animations.empty())
            {
                for (int i = 0; i < _particleCount; ++i)
                {
                    float percent = (_particleData.totalTimeToLive[i] - _particleData.timeToLive[i]) /
                                    _particleData.totalTimeToLive[i];
                    percent = _isAnimationReversed ? 1.0F - percent : percent;
                    _particleData.animCellIndex[i] =
                        (unsigned short)MIN(percent * _animIndexCount, _animIndexCount - 1);
                }
            }
            if (_isLifeAnimated && !_animations.empty())
            {
                for (int i = 0; i < _particleCount; ++i)
                {
                    auto& anim = _animations.at(_particleData.animIndex[i]);

                    float percent = (_particleData.totalTimeToLive[i] - _particleData.timeToLive[i]) /
                                    _particleData.totalTimeToLive[i];
                    percent = (!!_isAnimationReversed != !!anim.reverseIndices) ? 1.0F - percent : percent;
                    percent = MAX(0.0F, percent);

                    _particleData.animCellIndex[i] = anim.animationIndices[MIN(percent * anim.animationIndices.size(),
                                                                               anim.animationIndices.size() - 1)];
                }
            }
            if (_isLoopAnimated && !_animations.empty())
            {
                for (int i = 0; i < _particleCount; ++i)
                {
                    auto& anim = _animations.at(_particleData.animIndex[i]);

                    _particleData.animTimeDelta[i] += (_animationTimescaleInd ? pureDt : dt);
                    if (_particleData.animTimeDelta[i] >= _particleData.animTimeLength[i])
                        _particleData.animTimeDelta[i] = 0;

                    float percent = _particleData.animTimeDelta[i] / _particleData.animTimeLength[i];
                    percent       = anim.reverseIndices ? 1.0F - percent : percent;
                    percent       = MAX(0.0F, percent);

                    _particleData.animCellIndex[i] = anim.animationIndices[MIN(percent * anim.animationIndices.size(),
                                                                               anim.animationIndices.size() - 1)];
                }
            }
            if (_isLoopAnimated && _animations.empty())
                std::fill_n(_particleData.animTimeDelta, _particleCount, 0.f);
        }

        for (int i = 0; i < _particleCount; ++i)
        {
            if (_particleData.timeToLive[i] <= 0.0f)
            {
                int j = _particleCount - 1;
                while (j > 0 && _particleData.timeToLive[j] <= 0)
                {
                    _particleCount--;
                    j--;
                }
                _particleData.copyParticle(i, _particleCount - 1);
                if (_batchNode)
                {
                    // disable the switched particle
                    int currentIndex = _particleData.atlasIndex[i];
                    _batchNode->disableParticle(_atlasIndex + currentIndex);
                    // switch indexes
                    _particleData.atlasIndex[_particleCount - 1] = currentIndex;
                }
                --_particleCount;
                if (_particleCount == 0 && _isAutoRemoveOnFinish)
                {
                    this->unscheduleUpdate();
                    _parent->removeChild(this, true);
                    return;
                }
            }
        }

        if (_emitterMode == Mode::GRAVITY)
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                particle_point tmp, radial = {0.0f, 0.0f}, tangential;

                // radial acceleration
                if (_particleData.posx[i] || _particleData.posy[i])
                {
                    normalize_point(_particleData.posx[i], _particleData.posy[i], &radial);
                }
                tangential = radial;
                radial.x *= _particleData.modeA.radialAccel[i];
                radial.y *= _particleData.modeA.radialAccel[i];

                // tangential acceleration
                std::swap(tangential.x, tangential.y);
                tangential.x *= -_particleData.modeA.tangentialAccel[i];
                tangential.y *= _particleData.modeA.tangentialAccel[i];

                // (gravity + radial + tangential) * dt
                tmp.x = radial.x + tangential.x + modeA.gravity.x;
                tmp.y = radial.y + tangential.y + modeA.gravity.y;
                tmp.x *= dt;
                tmp.y *= dt;

                _particleData.modeA.dirX[i] += tmp.x;
                _particleData.modeA.dirY[i] += tmp.y;

                // this is cocos2d-x v3.0
                // if (_configName.length()>0 && _yCoordFlipped != -1)

                // this is cocos2d-x v3.0
                tmp.x = _particleData.modeA.dirX[i] * dt * _yCoordFlipped;
                tmp.y = _particleData.modeA.dirY[i] * dt * _yCoordFlipped;
                _particleData.posx[i] += tmp.x;
                _particleData.posy[i] += tmp.y;
            }
        }
        else
        {
            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.modeB.angle[i] += _particleData.modeB.degreesPerSecond[i] * dt;
            }

            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.modeB.radius[i] += _particleData.modeB.deltaRadius[i] * dt;
            }

            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.posx[i] = -cosf(_particleData.modeB.angle[i]) * _particleData.modeB.radius[i];
            }
            for (int i = 0; i < _particleCount; ++i)
            {
                _particleData.posy[i] =
                    -sinf(_particleData.modeB.angle[i]) * _particleData.modeB.radius[i] * _yCoordFlipped;
            }
        }

        // color r,g,b,a
        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.colorR[i] += _particleData.deltaColorR[i] * dt;
        }

        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.colorG[i] += _particleData.deltaColorG[i] * dt;
        }

        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.colorB[i] += _particleData.deltaColorB[i] * dt;
        }

        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.colorA[i] += _particleData.deltaColorA[i] * dt;
        }
        // size
        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.size[i] += (_particleData.deltaSize[i] * dt);
            _particleData.size[i] = MAX(0, _particleData.size[i]);
        }
        // angle
        for (int i = 0; i < _particleCount; ++i)
        {
            _particleData.rotation[i] += _particleData.deltaRotation[i] * dt;
        }

        updateParticleQuads();
        _transformSystemDirty = false;
    }

    // update and send gl buffer only when this node is visible.
    if (_visible && !_batchNode)
    {
        postStep();
    }

    AX_PROFILER_STOP_CATEGORY(kProfilerCategoryParticles, "CCParticleSystem - update");
}

void ParticleSystem::updateWithNoTime()
{
    this->update(0.0f);
}

void ParticleSystem::updateParticleQuads()
{
    // should be overridden
}

void ParticleSystem::postStep()
{
    // should be overridden
}

// ParticleSystem - Texture protocol
void ParticleSystem::setTexture(Texture2D* var)
{
    if (_texture != var)
    {
        AX_SAFE_RETAIN(var);
        AX_SAFE_RELEASE(_texture);
        _texture = var;
        updateBlendFunc();
    }
}

void ParticleSystem::updateBlendFunc()
{
    AXASSERT(!_batchNode, "Can't change blending functions when the particle is being batched");

    if (_texture)
    {
        bool premultiplied = _texture->hasPremultipliedAlpha();

        _opacityModifyRGB = false;

        if (_texture && (_blendFunc.src == AX_BLEND_SRC && _blendFunc.dst == AX_BLEND_DST))
        {
            if (premultiplied)
            {
                _opacityModifyRGB = true;
            }
            else
            {
                _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
            }
        }
    }
}

Texture2D* ParticleSystem::getTexture() const
{
    return _texture;
}

// ParticleSystem - Additive Blending
void ParticleSystem::setBlendAdditive(bool additive)
{
    if (additive)
    {
        _blendFunc = BlendFunc::ADDITIVE;
    }
    else
    {
        if (_texture && !_texture->hasPremultipliedAlpha())
            _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
        else
            _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    }
}

bool ParticleSystem::isBlendAdditive() const
{
    return (_blendFunc.src == backend::BlendFactor::SRC_ALPHA && _blendFunc.dst == backend::BlendFactor::ONE);
}

// ParticleSystem - Properties of Gravity Mode
void ParticleSystem::setTangentialAccel(float t)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.tangentialAccel = t;
}

float ParticleSystem::getTangentialAccel() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.tangentialAccel;
}

void ParticleSystem::setTangentialAccelVar(float t)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.tangentialAccelVar = t;
}

float ParticleSystem::getTangentialAccelVar() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.tangentialAccelVar;
}

void ParticleSystem::setRadialAccel(float t)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.radialAccel = t;
}

float ParticleSystem::getRadialAccel() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.radialAccel;
}

void ParticleSystem::setRadialAccelVar(float t)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.radialAccelVar = t;
}

float ParticleSystem::getRadialAccelVar() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.radialAccelVar;
}

void ParticleSystem::setRotationIsDir(bool t)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.rotationIsDir = t;
}

bool ParticleSystem::getRotationIsDir() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.rotationIsDir;
}

void ParticleSystem::setGravity(const Vec2& g)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.gravity = g;
}

const Vec2& ParticleSystem::getGravity()
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.gravity;
}

void ParticleSystem::setSpeed(float speed)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.speed = speed;
}

float ParticleSystem::getSpeed() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.speed;
}

void ParticleSystem::setSpeedVar(float speedVar)
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    modeA.speedVar = speedVar;
}

float ParticleSystem::getSpeedVar() const
{
    AXASSERT(_emitterMode == Mode::GRAVITY, "Particle Mode should be Gravity");
    return modeA.speedVar;
}

// ParticleSystem - Properties of Radius Mode
void ParticleSystem::setStartRadius(float startRadius)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.startRadius = startRadius;
}

float ParticleSystem::getStartRadius() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.startRadius;
}

void ParticleSystem::setStartRadiusVar(float startRadiusVar)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.startRadiusVar = startRadiusVar;
}

float ParticleSystem::getStartRadiusVar() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.startRadiusVar;
}

void ParticleSystem::setEndRadius(float endRadius)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.endRadius = endRadius;
}

float ParticleSystem::getEndRadius() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.endRadius;
}

void ParticleSystem::setEndRadiusVar(float endRadiusVar)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.endRadiusVar = endRadiusVar;
}

float ParticleSystem::getEndRadiusVar() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.endRadiusVar;
}

void ParticleSystem::setRotatePerSecond(float degrees)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.rotatePerSecond = degrees;
}

float ParticleSystem::getRotatePerSecond() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.rotatePerSecond;
}

void ParticleSystem::setRotatePerSecondVar(float degrees)
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    modeB.rotatePerSecondVar = degrees;
}

float ParticleSystem::getRotatePerSecondVar() const
{
    AXASSERT(_emitterMode == Mode::RADIUS, "Particle Mode should be Radius");
    return modeB.rotatePerSecondVar;
}

bool ParticleSystem::isActive() const
{
    return _isActive;
}

void ParticleSystem::useHSV(bool hsv)
{
    if (hsv && !allocHSVMem())
        return;

    if (!hsv)
        deallocHSVMem();
};

void ParticleSystem::setSpawnFadeIn(float time)
{
    if (time != 0.0F && !allocOpacityFadeInMem())
        return;

    _spawnFadeIn = time;
}

void ParticleSystem::setSpawnFadeInVar(float time)
{
    if (time != 0.0F && !allocOpacityFadeInMem())
        return;

    _spawnFadeInVar = time;
}

void ParticleSystem::setSpawnScaleIn(float time)
{
    if (time != 0.0F && !allocScaleInMem())
        return;

    _spawnScaleIn = time;
}

void ParticleSystem::setSpawnScaleInVar(float time)
{
    if (time != 0.0F && !allocScaleInMem())
        return;

    _spawnScaleInVar = time;
}

int ParticleSystem::getTotalParticles() const
{
    return _totalParticles;
}

void ParticleSystem::setTotalParticles(int var)
{
    AXASSERT(var <= _allocatedParticles, "Particle: resizing particle array only supported for quads");
    _totalParticles = var;
}

const BlendFunc& ParticleSystem::getBlendFunc() const
{
    return _blendFunc;
}

void ParticleSystem::setBlendFunc(const BlendFunc& blendFunc)
{
    if (_blendFunc.src != blendFunc.src || _blendFunc.dst != blendFunc.dst)
    {
        _blendFunc = blendFunc;
        this->updateBlendFunc();
    }
}

bool ParticleSystem::isAutoRemoveOnFinish() const
{
    return _isAutoRemoveOnFinish;
}

void ParticleSystem::setAutoRemoveOnFinish(bool var)
{
    _isAutoRemoveOnFinish = var;
}

// ParticleSystem - methods for batchNode rendering

ParticleBatchNode* ParticleSystem::getBatchNode() const
{
    return _batchNode;
}

void ParticleSystem::setBatchNode(ParticleBatchNode* batchNode)
{
    if (_batchNode != batchNode)
    {

        _batchNode = batchNode;  // weak reference

        if (batchNode)
        {
            // each particle needs a unique index
            for (int i = 0; i < _totalParticles; i++)
            {
                _particleData.atlasIndex[i] = i;
            }
        }
    }
}

// don't use a transform matrix, this is faster
void ParticleSystem::setScale(float s)
{
    _transformSystemDirty = true;
    Node::setScale(s);
}

void ParticleSystem::setRotation(float newRotation)
{
    _transformSystemDirty = true;
    Node::setRotation(newRotation);
}

void ParticleSystem::setScaleX(float newScaleX)
{
    _transformSystemDirty = true;
    Node::setScaleX(newScaleX);
}

void ParticleSystem::setScaleY(float newScaleY)
{
    _transformSystemDirty = true;
    Node::setScaleY(newScaleY);
}

void ParticleSystem::start()
{
    resetSystem();
}

void ParticleSystem::stop()
{
    stopSystem();
}

bool ParticleSystem::isPaused() const
{
    return _paused;
}

void ParticleSystem::pauseEmissions()
{
    _paused = true;
}

void ParticleSystem::resumeEmissions()
{
    _paused = false;
}

float ParticleSystem::getFixedFPS()
{
    return _fixedFPS;
}

void ParticleSystem::setFixedFPS(float frameRate)
{
    _fixedFPS = frameRate;
}

float ParticleSystem::getTimeScale()
{
    return _timeScale;
}

void ParticleSystem::setTimeScale(float scale)
{
    _timeScale = scale;
}

static ParticleEmissionMaskCache* emissionMaskCache;

ParticleEmissionMaskCache* ParticleEmissionMaskCache::getInstance()
{
    if (emissionMaskCache == nullptr)
    {
        emissionMaskCache = new ParticleEmissionMaskCache();
        return emissionMaskCache;
    }
    return emissionMaskCache;
}

void ParticleEmissionMaskCache::bakeEmissionMask(std::string_view maskId,
                                                 std::string_view texturePath,
                                                 float alphaThreshold,
                                                 bool inverted,
                                                 int inbetweenSamples)
{
    auto img = new Image();
    img->Image::initWithImageFile(texturePath);
    img->autorelease();

    AXASSERT(img, "image texture was nullptr.");
    bakeEmissionMask(maskId, img, alphaThreshold, inverted, inbetweenSamples);
}

void ParticleEmissionMaskCache::bakeEmissionMask(std::string_view maskId,
                                                 Image* imageTexture,
                                                 float alphaThreshold,
                                                 bool inverted,
                                                 int inbetweenSamples)
{
    auto img = imageTexture;
    AXASSERT(img, "image texture was nullptr.");
    AXASSERT(img->hasAlpha(), "image data should contain an alpha channel.");

    vector<Vec2> points;

    auto data = img->getData();
    auto w    = img->getWidth();
    auto h    = img->getHeight();

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
        {
            if (inbetweenSamples > 1)
            {
                float a = data[(y * w + x) * 4 + 3] / 255.0F;
                if (a >= alphaThreshold && !inverted || a < alphaThreshold && inverted)
                    for (float i = 0; i < 1.0F; i += 1.0F / inbetweenSamples)
                        points.emplace_back(Vec2{float(x + i), float(h - y + i)});
            }
            else
            {
                float a = data[(y * w + x) * 4 + 3] / 255.0F;
                if (a >= alphaThreshold && !inverted || a < alphaThreshold && inverted)
                    points.emplace_back(Vec2{float(x), float(h - y)});
            }
        }

    auto fourccId = utils::fourccValue(maskId);

    auto iter = this->masks.find(fourccId);
    if (iter == this->masks.end())
        iter = this->masks.emplace(fourccId, ParticleEmissionMaskDescriptor{}).first;

    ParticleEmissionMaskDescriptor desc;
    desc.size   = {float(w), float(h)};
    desc.points = std::move(points);

    iter->second = desc;

    AXLOGD("Particle emission mask '{}' baked ({}x{}), {} samples generated taking {:.2f}mb of memory.",
          (unsigned int)htonl(fourccId), w, h, desc.points.size(), desc.points.size() * 8 / 1e+6);
}

const ParticleEmissionMaskDescriptor& ParticleEmissionMaskCache::getEmissionMask(uint32_t fourccId)
{
    auto iter = this->masks.find(fourccId);
    if (iter == this->masks.end())
    {
        iter                = this->masks.emplace(fourccId, ParticleEmissionMaskDescriptor{}).first;
        iter->second.size   = {float(1), float(1)};
        iter->second.points = {{0, 0}};
        return iter->second;
    }
    return iter->second;
}

const ParticleEmissionMaskDescriptor& ParticleEmissionMaskCache::getEmissionMask(std::string_view maskId)
{
    auto fourccId = utils::fourccValue(maskId);

    auto iter = this->masks.find(fourccId);
    if (iter == this->masks.end())
    {
        iter                = this->masks.emplace(fourccId, ParticleEmissionMaskDescriptor{}).first;
        iter->second.size   = {float(1), float(1)};
        iter->second.points = {{0, 0}};
        return iter->second;
    }
    return iter->second;
}

void ParticleEmissionMaskCache::removeMask(std::string_view maskId)
{
    this->masks.erase(utils::fourccValue(maskId));
}

void ParticleEmissionMaskCache::removeAllMasks()
{
    this->masks.clear();
}

}
