/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "3d/CCAnimation3D.h"
#include "3d/CCBundle3D.h"
#include "platform/CCFileUtils.h"

NS_AX_BEGIN

Animation3D* Animation3D::create(std::string_view fileName, std::string_view animationName)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    fullPath.append("#").append(animationName);
    auto animation = Animation3DCache::getInstance()->getAnimation(fullPath);
    if (animation != nullptr)
        return animation;

    animation = new Animation3D();
    if (animation->initWithFile(fileName, animationName))
    {
        animation->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(animation);
    }

    return animation;
}

bool Animation3D::initWithFile(std::string_view filename, std::string_view animationName)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);

    // load animation here
    auto bundle = Bundle3D::createBundle();
    Animation3DData animationdata;
    if (bundle->load(fullPath) && bundle->loadAnimationData(animationName, &animationdata) && init(animationdata))
    {
        fullPath.append("#").append(animationName);
        Animation3DCache::getInstance()->addAnimation(fullPath, this);
        Bundle3D::destroyBundle(bundle);
        return true;
    }

    Bundle3D::destroyBundle(bundle);

    return false;
}

Animation3D::Curve* Animation3D::getBoneCurveByName(std::string_view name) const
{
    auto it = _boneCurves.find(name);
    if (it != _boneCurves.end())
        return it->second;

    return nullptr;
}

Animation3D::Animation3D() : _duration(0) {}

Animation3D::~Animation3D()
{
    for (const auto& itor : _boneCurves)
    {
        Curve* curve = itor.second;
        AX_SAFE_DELETE(curve);
    }
}

Animation3D::Curve::Curve() : translateCurve(nullptr), rotCurve(nullptr), scaleCurve(nullptr) {}
Animation3D::Curve::~Curve()
{
    AX_SAFE_RELEASE_NULL(translateCurve);
    AX_SAFE_RELEASE_NULL(rotCurve);
    AX_SAFE_RELEASE_NULL(scaleCurve);
}

bool Animation3D::init(const Animation3DData& data)
{
    _duration = data._totalTime;

    {
        std::vector<float> keys;
        std::vector<Vec3> values;
        for (const auto& iter : data._translationKeys)
        {
            Curve* curve = _boneCurves[iter.first];
            if (curve == nullptr)
            {
                curve                   = new Curve();
                _boneCurves[iter.first] = curve;
            }

            if (iter.second.empty())
                continue;

            // resize still with zero filled which is not needs at here
            // some follow concepts needs standardized in the future, may be c++23
            //  - resize_and_overwrite:
            //    - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1072r10.html
            //  - boost vector resize with init behavior parameter
            //    - https://github.com/boostorg/container/blob/develop/include/boost/container/vector.hpp
            //  - pod_vector
            //    - https://stackoverflow.com/questions/15219984/using-vectorchar-as-a-buffer-without-initializing-it-on-resize/15220853#15220853
            //    - https://github.com/yasio/yasio/blob/perftest/tests/perf/pod_vector.h
            keys.resize(iter.second.size());
            values.resize(iter.second.size());

            std::transform(iter.second.begin(), iter.second.end(), keys.begin(),
                           [](const auto& keyIter) { return keyIter._time; });

            std::transform(iter.second.begin(), iter.second.end(), values.begin(),
                           [](const auto& keyIter) { return keyIter._key; });


            curve->translateCurve = Curve::AnimationCurveVec3::create(&keys[0], &values[0].x, (int)keys.size());
            if (curve->translateCurve)
                curve->translateCurve->retain();
        }
    }

    {
        std::vector<float> keys;
        std::vector<Quaternion> values;
        for (const auto& iter : data._rotationKeys)
        {
            Curve* curve = _boneCurves[iter.first];
            if (curve == nullptr)
            {
                curve                   = new Curve();
                _boneCurves[iter.first] = curve;
            }

            if (iter.second.empty())
                continue;

            // resize still with zero filled which is not needs at here
            // some follow concepts needs standardized in the future, may be c++23
            //  - resize_and_overwrite:
            //    - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1072r10.html
            //  - boost vector resize with init behavior parameter
            //    - https://github.com/boostorg/container/blob/develop/include/boost/container/vector.hpp
            //  - pod_vector
            //    -
            //    https://stackoverflow.com/questions/15219984/using-vectorchar-as-a-buffer-without-initializing-it-on-resize/15220853#15220853
            //    - https://github.com/yasio/yasio/blob/perftest/tests/perf/pod_vector.h
            keys.resize(iter.second.size());
            values.resize(iter.second.size());

            std::transform(iter.second.begin(), iter.second.end(), keys.begin(),
                           [](const auto& keyIter) { return keyIter._time; });

            std::transform(iter.second.begin(), iter.second.end(), values.begin(),
                           [](const auto& keyIter) { return keyIter._key; });

            curve->rotCurve = Curve::AnimationCurveQuat::create(&keys[0], &values[0].x, (int)keys.size());
            if (curve->rotCurve)
                curve->rotCurve->retain();
        }
    }

    {
        std::vector<float> keys;
        std::vector<Vec3> values;
        for (const auto& iter : data._scaleKeys)
        {
            Curve* curve = _boneCurves[iter.first];
            if (curve == nullptr)
            {
                curve                   = new Curve();
                _boneCurves[iter.first] = curve;
            }

            if (iter.second.empty())
                continue;

            // resize still with zero filled which is not needs at here
            // some follow concepts needs standardized in the future, may be c++23
            //  - resize_and_overwrite:
            //    - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p1072r10.html
            //  - boost vector resize with init behavior parameter
            //    - https://github.com/boostorg/container/blob/develop/include/boost/container/vector.hpp
            //  - pod_vector
            //    -
            //    https://stackoverflow.com/questions/15219984/using-vectorchar-as-a-buffer-without-initializing-it-on-resize/15220853#15220853
            //    - https://github.com/yasio/yasio/blob/perftest/tests/perf/pod_vector.h
            keys.resize(iter.second.size());
            values.resize(iter.second.size());

            std::transform(iter.second.begin(), iter.second.end(), keys.begin(),
                           [](const auto& keyIter) { return keyIter._time; });

            std::transform(iter.second.begin(), iter.second.end(), values.begin(),
                           [](const auto& keyIter) { return keyIter._key; });

            curve->scaleCurve = Curve::AnimationCurveVec3::create(&keys[0], &values[0].x, (int)keys.size());
            if (curve->scaleCurve)
                curve->scaleCurve->retain();
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////
Animation3DCache* Animation3DCache::_cacheInstance = nullptr;

Animation3DCache* Animation3DCache::getInstance()
{
    if (_cacheInstance == nullptr)
        _cacheInstance = new Animation3DCache();

    return _cacheInstance;
}
void Animation3DCache::destroyInstance()
{
    AX_SAFE_DELETE(_cacheInstance);
}

Animation3D* Animation3DCache::getAnimation(std::string_view key)
{
    auto it = _animations.find(key);
    if (it != _animations.end())
        return it->second;
    return nullptr;
}

void Animation3DCache::addAnimation(std::string_view key, Animation3D* animation)
{
    const auto& it = _animations.find(key);
    if (it != _animations.end())
    {
        return;  // already have this key
    }
    _animations.emplace(key, animation);  //_animations[key] = animation;
    animation->retain();
}

void Animation3DCache::removeAllAnimations()
{
    for (auto&& itor : _animations)
    {
        AX_SAFE_RELEASE(itor.second);
    }
    _animations.clear();
}
void Animation3DCache::removeUnusedAnimation()
{
    for (auto itor = _animations.begin(); itor != _animations.end();)
    {
        if (itor->second->getReferenceCount() == 1)
        {
            itor->second->release();
            itor = _animations.erase(itor);
        }
        else
            ++itor;
    }
}

Animation3DCache::Animation3DCache() {}
Animation3DCache::~Animation3DCache()
{
    removeAllAnimations();
}

NS_AX_END
