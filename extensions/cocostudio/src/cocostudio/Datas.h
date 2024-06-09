/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __CCARMATURE_DATAS_H__
#define __CCARMATURE_DATAS_H__

#include "base/Object.h"
#include "base/Types.h"
#include "base/Vector.h"
#include "base/Map.h"
#include "math/AffineTransform.h"

#include "ArmatureDefine.h"
#include "2d/TweenFunction.h"
#include "CocosStudioExport.h"

#define AX_CREATE_NO_PARAM_NO_INIT(varType) \
public:                                     \
    static inline varType* create(void)     \
    {                                       \
        varType* var = new varType();       \
        var->autorelease();                 \
        return var;                         \
    }

#define AX_CREATE_NO_PARAM(varType)     \
public:                                 \
    static inline varType* create(void) \
    {                                   \
        varType* var = new varType();   \
        if (var->init())                \
        {                               \
            var->autorelease();         \
            return var;                 \
        }                               \
        AX_SAFE_DELETE(var);            \
        return nullptr;                 \
    }

namespace cocostudio
{

/**
 * The base node include a lot of attributes.
 * @js NA
 * @lua NA
 */
class CCS_DLL BaseData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(BaseData)
public:
    /**
     * @js ctor
     */
    BaseData();
    /**
     * @js NA
     * @lua NA
     */
    ~BaseData(void);

    /*
     * Copy data from node
     * @param  node A BaseData to copy data
     */
    virtual void copy(const BaseData* node);

    /*
     * Calculate two BaseData's between value(to - from) and set to self
     *
     * @param  from   from BaseData
     * @param  to     to BaseData
     */
    virtual void subtract(BaseData* from, BaseData* to, bool limit);

    virtual void setColor(const ax::Color4B& color);
    virtual ax::Color4B getColor();

public:
    float x;     //! position x attribute
    float y;     //! position y attribute
    int zOrder;  //! zorder attribute, used to order the Bone's depth order

    /**
     * x y skewX skewY scaleX scaleY used to calculate transform matrix
     * skewX, skewY can have rotation effect
     * To get more matrix information, you can have a look at this paper :
     * http://www.senocular.com/flash/tutorials/transformmatrix/
     */
    float skewX;
    float skewY;
    float scaleX;
    float scaleY;

    float tweenRotate;  //! SkewX, SkewY, and TweenRotate effect the rotation

    bool isUseColorInfo;  //! Whether or not this frame have the color changed Info
    int a, r, g, b;
};

/**
 * DisplayType distinguish which type your display is.
 */
enum DisplayType
{
    CS_DISPLAY_SPRITE,    //! display is a single Sprite
    CS_DISPLAY_ARMATURE,  //! display is a Armature
    CS_DISPLAY_PARTICLE,  //! display is a CCParticle.

    CS_DISPLAY_MAX
};
/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL DisplayData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(DisplayData)

    static std::string changeDisplayToTexture(std::string_view displayName);

public:
    /**
     * @js ctor
     */
    DisplayData();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~DisplayData(void) {}

    virtual void copy(DisplayData* displayData);

    DisplayType displayType;  //! mark which type your display is
    std::string displayName;
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL SpriteDisplayData : public DisplayData
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(SpriteDisplayData)
public:
    /**
     * @js ctor
     */
    SpriteDisplayData();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~SpriteDisplayData(){};

    void copy(DisplayData* displayData);

public:
    BaseData skinData;
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL ArmatureDisplayData : public DisplayData
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(ArmatureDisplayData)
public:
    /**
     * @js ctor
     */
    ArmatureDisplayData();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ArmatureDisplayData() {}
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL ParticleDisplayData : public DisplayData
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(ParticleDisplayData)
public:
    /**
     * @js ctor
     */
    ParticleDisplayData();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ParticleDisplayData(){};
};

/**
 * BoneData used to init a Bone.
 * BoneData keeps a DisplayData list, a Bone can have many display to change.
 * The display information saved in the DisplayData
 * @js NA
 * @lua NA
 */
class CCS_DLL BoneData : public BaseData
{
public:
    AX_CREATE_NO_PARAM(BoneData)
public:
    /**
     * @js ctor
     */
    BoneData(void);
    /**
     * @js NA
     * @lua NA
     */
    ~BoneData(void);

    virtual bool init();

    void addDisplayData(DisplayData* displayData);
    DisplayData* getDisplayData(int index);

public:
    std::string name;                               //! the bone's name
    std::string parentName;                         //! the bone parent's name
    ax::Vector<DisplayData*> displayDataList;  //! save DisplayData informations for the Bone
    ax::AffineTransform boneDataTransform;
};

/**
 * ArmatureData saved the Armature name and Bonedata needed for the CCBones in this Armature
 * When we create a Armature, we need to get each Bone's BoneData as it's init information.
 * So we can get a BoneData from the Dictionary saved in the ArmatureData.
 * @js NA
 * @lua NA
 */
class CCS_DLL ArmatureData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM(ArmatureData)
public:
    /**
     * @js ctor
     */
    ArmatureData();
    /**
     * @js NA
     * @lua NA
     */
    ~ArmatureData();

    bool init();
    void addBoneData(BoneData* boneData);
    BoneData* getBoneData(std::string_view boneName);

public:
    std::string name;
    ax::StringMap<BoneData*> boneDataDic;
    float dataVersion;
};

enum BlendType
{
    BLEND_NORMAL,
    BLEND_LAYER,
    BLEND_DARKEN,
    BLEND_MULTIPLY,
    BLEND_LIGHTEN,
    BLEND_SCREEN,
    BLEND_OVERLAY,
    BLEND_HARD_LIGHT,
    BLEND_ADD,
    BLEND_SUBSTRACT,
    BLEND_DIFFERENCE,
    BLEND_INVERT,
    BLEND_ALPHA,
    BLEND_ERASE
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL FrameData : public BaseData
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(FrameData)
public:
    /**
     * @js ctor
     */
    FrameData();
    /**
     * @js NA
     * @lua NA
     */
    ~FrameData();

    virtual void copy(const BaseData* baseData);

public:
    int frameID;
    int duration;  //! The frame will last duration frames

    ax::tweenfunc::TweenType tweenEasing;  //! Every frame's tween easing effect
    int easingParamNumber;
    float* easingParams;

    bool isTween;  //! Whether it's a tween key frame

    /**
     * The current display index when change to this frame.
     * If value is -1, then display will not be shown.
     */
    int displayIndex;

    ax::BlendFunc blendFunc;

    std::string strEvent;
    /**
     * strMovement, strEvent, strSound, strSoundEffect do not support yet
     */
    std::string strMovement;
    std::string strSound;
    std::string strSoundEffect;
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL MovementBoneData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM(MovementBoneData)
public:
    /**
     * @js ctor
     */
    MovementBoneData();
    /**
     * @js NA
     * @lua NA
     */
    ~MovementBoneData(void);

    virtual bool init();

    void addFrameData(FrameData* frameData);
    FrameData* getFrameData(int index);

public:
    float delay;       //! movement delay percent, this value can produce a delay effect
    float scale;       //! scale this movement
    float duration;    //! this Bone in this movement will last m_iDuration frames
    std::string name;  //! bone name

    ax::Vector<FrameData*> frameList;
};

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL MovementData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(MovementData)
public:
    /**
     * @js ctor
     */
    MovementData(void);
    /**
     * @js NA
     * @lua NA
     */
    ~MovementData(void);

    void addMovementBoneData(MovementBoneData* movBoneData);
    MovementBoneData* getMovementBoneData(std::string_view boneName);

public:
    std::string name;
    int duration;  //! the frames this movement will last
    float scale;   //! scale this movement

    /**
     * Change to this movement will last durationTo frames. Use this effect can avoid too suddenly changing.
     *
     * Example : current movement is "stand", we want to change to "run", then we fill durationTo frames before
     * change to "run" instead of changing to "run" directly.
     */
    int durationTo;

    /*
     * This is different from duration, durationTween contain tween effect.
     * duration is the raw time that the animation will last, it's the same with the time you edit in the Action Editor.
     * durationTween is the actual time you want this animation last.
     * Example : If we edit 10 frames in the flash, then duration is 10. When we set durationTween to 50, the movement
     * will last 50 frames, the extra 40 frames will auto filled with tween effect
     */
    int durationTween;

    bool loop;  //! whether the movement was looped

    /**
     * Which tween easing effect the movement use
     * TWEEN_EASING_MAX : use the value from MovementData get from flash design panel
     */
    ax::tweenfunc::TweenType tweenEasing;

    /**
     * @brief    save movement bone data
     * @key    std::string_view
     * @value    MovementBoneData *
     */
    ax::StringMap<MovementBoneData*> movBoneDataDic;
};

/**
 *  AnimationData include all movement information for the Armature
 *  The struct is AnimationData -> MovementData -> MovementBoneData -> FrameData
 *                                              -> MovementFrameData
 *  @js NA
 *  @lua NA
 */
class CCS_DLL AnimationData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM_NO_INIT(AnimationData)
public:
    /**
     * @js ctor
     */
    AnimationData(void);
    /**
     * @js NA
     * @lua NA
     */
    ~AnimationData(void);

    void addMovement(MovementData* movData);
    MovementData* getMovement(std::string_view movementName);
    ssize_t getMovementCount();

public:
    std::string name;
    ax::StringMap<MovementData*> movementDataDic;
    std::vector<std::string> movementNames;
};

/*
 * ContourData include a contour vertex information
 * @js NA
 * @lua NA
 */
class CCS_DLL ContourData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM(ContourData)
public:
    /**
     * @js ctor
     */
    ContourData();
    /**
     * @js NA
     * @lua NA
     */
    ~ContourData(void);

    virtual bool init();
    virtual void addVertex(ax::Vec2& vertex);

public:
    std::vector<ax::Vec2> vertexList;  //! Save contour vertex info, vertex saved in a Vec2
};

/*
 * TextureData include a texture's information
 * @js NA
 * @lua NA
 */
class CCS_DLL TextureData : public ax::Object
{
public:
    AX_CREATE_NO_PARAM(TextureData)
public:
    /**
     * @js ctor
     */
    TextureData();
    /**
     * @js NA
     * @lua NA
     */
    ~TextureData(void);

    virtual bool init();

    void addContourData(ContourData* contourData);
    ContourData* getContourData(int index);

public:
    float height;  //! The texture's width, height
    float width;

    float pivotX;  //! The texture's anchor point
    float pivotY;

    std::string name;  //! The texture's name

    ax::Vector<ContourData*> contourDataList;
};

}  // namespace cocostudio

#endif /*__CCARMATURE_DATAS_H__*/
