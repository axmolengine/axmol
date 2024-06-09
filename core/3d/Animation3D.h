/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#ifndef __CCANIMATION3D_H__
#define __CCANIMATION3D_H__

#include <unordered_map>

#include "3d/AnimationCurve.h"

#include "base/Macros.h"
#include "base/Object.h"
#include "3d/Bundle3DData.h"

NS_AX_BEGIN
/**
 * @addtogroup _3d
 * @{
 */

/**
 * @brief static animation data, shared
 */
class AX_DLL Animation3D : public Object
{
    friend class Bundle3D;

public:
    /**
     * animation curve, translation, rotation, and scale
     */
    class Curve
    {
    public:
        typedef AnimationCurve<3> AnimationCurveVec3;
        typedef AnimationCurve<4> AnimationCurveQuat;
        /**translation curve*/
        AnimationCurveVec3* translateCurve;
        /**rotation curve*/
        AnimationCurveQuat* rotCurve;
        /**scaling curve*/
        AnimationCurveVec3* scaleCurve;
        /**constructor */
        Curve();
        /**constructor */
        ~Curve();
    };

    /**read all animation or only the animation with given animationName? animationName == "" read the first.*/
    static Animation3D* create(std::string_view filename, std::string_view animationName = "");

    /**get duration*/
    float getDuration() const { return _duration; }

    /**
     * get bone curve
     *
     * @lua NA
     */
    Curve* getBoneCurveByName(std::string_view name) const;

    /**get the bone Curves set*/
    const hlookup::string_map<Curve*>& getBoneCurves() const { return _boneCurves; }

    Animation3D();
    virtual ~Animation3D();
    /**init Animation3D from bundle data*/
    bool init(const Animation3DData& data);

    /**init Animation3D with file name and animation name*/
    bool initWithFile(std::string_view filename, std::string_view animationName);

protected:
    hlookup::string_map<Curve*> _boneCurves;  // bone curves map, key bone name, value AnimationCurve

    float _duration;  // animation duration
};

/**
 * Animation3D Cache
 */
class Animation3DCache
{
public:
    /**get and destroy instance*/
    static Animation3DCache* getInstance();
    static void destroyInstance();

    /**get animation by key*/
    Animation3D* getAnimation(std::string_view key);

    /**add animation to cache*/
    void addAnimation(std::string_view key, Animation3D* animation);

    /**remove all animation*/
    void removeAllAnimations();
    /**remove unused animation*/
    void removeUnusedAnimation();

protected:
    Animation3DCache();
    ~Animation3DCache();

    static Animation3DCache* _cacheInstance;  // cache instance

    hlookup::string_map<Animation3D*> _animations;  // cached animations
};

// end of 3d group
/// @}
NS_AX_END

#endif  // __CCANIMATION3D_H__
