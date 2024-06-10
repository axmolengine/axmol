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
#ifndef __CCPARTICLE_SYSTEM_H__
#define __CCPARTICLE_SYSTEM_H__

#include "base/Protocols.h"
#include "2d/Node.h"
#include "base/Value.h"
#include "2d/SpriteFrame.h"
#include "2d/SpriteFrameCache.h"
#include "math/FastRNG.h"

NS_AX_BEGIN

class ParticleBatchNode;

struct particle_point
{
    float x;
    float y;
};

/**
 * Particle emission shapes.
 * Current supported shapes are Point, Rectangle, RectangularTorus, Circle, Torus, Cone, Cone Torus, Texture alpha
 * emission mask
 * @since axmol-1.0.0b8
 */
struct EmissionShape
{
    EmissionShapeType type;

    float x;
    float y;

    float innerWidth;
    float innerHeight;
    float outerWidth;
    float outerHeight;

    float innerRadius;
    float outerRadius;
    float coneOffset;
    float coneAngle;
    float edgeBias;

    uint32_t fourccId;
};

/**
 * Particle emission mask descriptor.
 * @since axmol-1.0.0b8
 */
struct ParticleEmissionMaskDescriptor
{
    Vec2 size;
    std::vector<ax::Vec2> points;
};

/** @struct ParticleAnimationDescriptor
Structure that contains animation description
*/
struct ParticleAnimationDescriptor
{
    float animationSpeed;
    float animationSpeedVariance;
    std::vector<unsigned short> animationIndices;
    bool reverseIndices;
};

/** @struct ParticleFrameDescriptor
Structure that contains frame description
*/
struct ParticleFrameDescriptor
{
    ax::Rect rect;
    bool isRotated;
};

class AX_DLL ParticleData
{
public:
    float* posx;
    float* posy;
    float* startPosX;
    float* startPosY;

    float* colorR;
    float* colorG;
    float* colorB;
    float* colorA;

    float* deltaColorR;
    float* deltaColorG;
    float* deltaColorB;
    float* deltaColorA;

    float* hue;
    float* sat;
    float* val;

    float* opacityFadeInDelta;
    float* opacityFadeInLength;

    float* scaleInDelta;
    float* scaleInLength;

    float* size;
    float* deltaSize;
    float* rotation;
    float* staticRotation;
    float* deltaRotation;
    float* totalTimeToLive;
    float* timeToLive;
    float* animTimeDelta;
    float* animTimeLength;
    unsigned short* animIndex;
    unsigned short* animCellIndex;
    unsigned int* atlasIndex;

    //! Mode A: gravity, direction, radial accel, tangential accel
    struct
    {
        float* dirX;
        float* dirY;
        float* radialAccel;
        float* tangentialAccel;
    } modeA;

    //! Mode B: radius mode
    struct
    {
        float* angle;
        float* degreesPerSecond;
        float* radius;
        float* deltaRadius;
    } modeB;

    unsigned int maxCount;
    ParticleData();
    bool init(int count);
    void release();
    unsigned int getMaxCount() { return maxCount; }

    void copyParticle(int p1, int p2)
    {
        posx[p1]      = posx[p2];
        posy[p1]      = posy[p2];
        startPosX[p1] = startPosX[p2];
        startPosY[p1] = startPosY[p2];

        colorR[p1] = colorR[p2];
        colorG[p1] = colorG[p2];
        colorB[p1] = colorB[p2];
        colorA[p1] = colorA[p2];

        deltaColorR[p1] = deltaColorR[p2];
        deltaColorG[p1] = deltaColorG[p2];
        deltaColorB[p1] = deltaColorB[p2];
        deltaColorA[p1] = deltaColorA[p2];

        if (hue && sat && val)
        {
            hue[p1] = hue[p2];
            sat[p1] = sat[p2];
            val[p1] = val[p2];
        }

        if (opacityFadeInDelta && opacityFadeInLength)
        {
            opacityFadeInDelta[p1]  = opacityFadeInDelta[p2];
            opacityFadeInLength[p1] = opacityFadeInLength[p2];
        }

        if (scaleInDelta && scaleInLength)
        {
            scaleInDelta[p1]  = scaleInDelta[p2];
            scaleInLength[p1] = scaleInLength[p2];
        }

        size[p1]           = size[p2];
        deltaSize[p1]      = deltaSize[p2];
        rotation[p1]       = rotation[p2];
        staticRotation[p1] = staticRotation[p2];
        deltaRotation[p1]  = deltaRotation[p2];

        totalTimeToLive[p1] = totalTimeToLive[p2];
        timeToLive[p1]      = timeToLive[p2];

        if (animTimeDelta && animTimeLength && animIndex && animCellIndex)
        {
            animTimeDelta[p1]  = animTimeDelta[p2];
            animTimeLength[p1] = animTimeLength[p2];
            animIndex[p1]      = animIndex[p2];
            animCellIndex[p1]  = animCellIndex[p2];
        }

        atlasIndex[p1] = atlasIndex[p2];

        modeA.dirX[p1]            = modeA.dirX[p2];
        modeA.dirY[p1]            = modeA.dirY[p2];
        modeA.radialAccel[p1]     = modeA.radialAccel[p2];
        modeA.tangentialAccel[p1] = modeA.tangentialAccel[p2];

        modeB.angle[p1]            = modeB.angle[p2];
        modeB.degreesPerSecond[p1] = modeB.degreesPerSecond[p2];
        modeB.radius[p1]           = modeB.radius[p2];
        modeB.deltaRadius[p1]      = modeB.deltaRadius[p2];
    }
};

/**
 * Particle emission mask cache.
 * @since axmol-1.0.0b8
 */
class AX_DLL ParticleEmissionMaskCache
{
public:
    static ParticleEmissionMaskCache* getInstance();

    /** Bakes a particle emission mask from texture data on cpu and stores it in memory by it's name.
     * If the mask already exists then it will be overwritten.
     *
     * @param maskId The id of the mask, FOURCC starts with '#', such as "#abcd"
     * @param texturePath Path of the texture that holds alpha data.
     * @param alphaThreshold The threshold at which pixels are picked, If a pixel's alpha channel is greater than
     * alphaThreshold then it will be picked.
     * @param inverted Inverts the pick condition so that If a pixel's alpha channel is lower than alphaThreshold then
     * it will be picked.
     * @param inbetweenSamples How many times should pixels be filled inbetween, this value should be increased If
     * you're planning to scale the emission shape up. WARNING: it will use more memory.
     */
    void bakeEmissionMask(std::string_view maskId,
                          std::string_view texturePath,
                          float alphaThreshold = 0.5F,
                          bool inverted        = false,
                          int inbetweenSamples = 1);

    /** Bakes a particle emission mask from texture data on cpu and stores it in memory by it's name.
     * If the mask already exists then it will be overwritten.
     *
     * @param maskId The id of the mask, FOURCC starts with '#', such as "#abcd"
     * @param imageTexture Image object containing texture data with alpha channel.
     * @param alphaThreshold The threshold at which pixels are picked, If a pixel's alpha channel is greater than
     * alphaThreshold then it will be picked.
     * @param inverted Inverts the pick condition so that If a pixel's alpha channel is lower than alphaThreshold then
     * it will be picked.
     * @param inbetweenSamples How many times should pixels be filled inbetween, this value should be increased If
     * you're planning to scale the emission shape up. WARNING: it will use more memory.
     */
    void bakeEmissionMask(std::string_view maskId,
                          Image* imageTexture,
                          float alphaThreshold = 0.5F,
                          bool inverted        = false,
                          int inbetweenSamples = 1);

    /** Returns a baked mask with the specified name if it exists. otherwise, it will return a dummy mask.
     *
     * @param fourccId The unsigned integer id of the mask.
     */
    const ParticleEmissionMaskDescriptor& getEmissionMask(uint32_t fourccId);

    /** Returns a baked mask with the specified name if it exists. otherwise, it will return a dummy mask.
     *
     * @param maskId The id of the mask, FOURCC starts with '#', such as "#abcd"
     */
    const ParticleEmissionMaskDescriptor& getEmissionMask(std::string_view maskId);

    /** Removes a baked mask and releases the data from memory with the specified name if it exists.
     *
     * @param maskId The id of the mask, FOURCC starts with '#', such as "#abcd"
     */
    void removeMask(std::string_view maskId);

    /** Remove all baked masks and releases their data from memory. */
    void removeAllMasks();

private:
    std::unordered_map<uint32_t, ParticleEmissionMaskDescriptor> masks;

};

// typedef void (*AX_UPDATE_PARTICLE_IMP)(id, SEL, tParticle*, Vec2);

class Texture2D;

/** @class ParticleSystem
 * @brief Particle System base class.
Attributes of a Particle System:
- emission rate of the particles
- Gravity Mode (Mode A):
- gravity
- direction
- speed +-  variance
- tangential acceleration +- variance
- radial acceleration +- variance
- Radius Mode (Mode B):
- startRadius +- variance
- endRadius +- variance
- rotate +- variance
- Properties common to all modes:
- life +- life variance
- start spin +- variance
- end spin +- variance
- start size +- variance
- end size +- variance
- start color +- variance
- end color +- variance
- life +- variance
- blending function
- texture

Cocos2d also supports particles generated by Particle Designer (http://particledesigner.71squared.com/).
'Radius Mode' in Particle Designer uses a fixed emit rate of 30 hz. Since that can't be guaranteed in cocos2d,
cocos2d uses a another approach, but the results are almost identical.

Cocos2d supports all the variables used by Particle Designer plus a bit more:
- spinning particles (supported when using ParticleSystemQuad)
- tangential acceleration (Gravity mode)
- radial acceleration (Gravity mode)
- radius direction (Radius mode) (Particle Designer supports outwards to inwards direction only)

It is possible to customize any of the above mentioned properties in runtime. Example:

@code
emitter.radialAccel = 15;
emitter.startSpin = 0;
@endcode

*/
class AX_DLL ParticleSystem : public Node, public TextureProtocol, public PlayableProtocol
{
public:
    /** Mode
     * @js cc.ParticleSystem.MODE_GRAVITY;
     */
    enum class Mode
    {
        GRAVITY,
        RADIUS,
    };

    /** PositionType
     Types of particle positioning.
     * @js cc.ParticleSystem.TYPE_FREE
     */
    enum class PositionType
    {
        FREE, /** Living particles are attached to the world and are unaffected by emitter repositioning. */

        RELATIVE, /** Living particles are attached to the world but will follow the emitter repositioning.
                   Use case: Attach an emitter to an sprite, and you want that the emitter follows the sprite.*/

        GROUPED, /** Living particles are attached to the emitter and are translated along with it. */

    };

    /** TexAnimDir
     Texture animation direction for the particles.
     */
    enum class TexAnimDir
    {
        VERTICAL, /** texture coordinates are read top to bottom within the texture */

        HORIZONTAL, /** texture coordinates are read left to right within the texture */

    };

    //* @enum
    enum
    {
        /** The Particle emitter lives forever. */
        DURATION_INFINITY = -1,

        /** The starting size of the particle is equal to the ending size. */
        START_SIZE_EQUAL_TO_END_SIZE = -1,

        /** The starting radius of the particle is equal to the ending radius. */
        START_RADIUS_EQUAL_TO_END_RADIUS = -1,

        /** The simulation's seconds are set to the particles' lifetime specified inclusive of variant. */
        SIMULATION_USE_PARTICLE_LIFETIME = -1,

        /** The simulation's framerate is set to the animation interval specified in director. */
        SIMULATION_USE_GAME_ANIMATION_INTERVAL = -1,
    };

    /** Creates an initializes a ParticleSystem from a plist file.
    This plist files can be created manually or with Particle Designer:
    http://particledesigner.71squared.com/
     @since v2.0
     *
     * @param plistFile Particle plist file name.
     * @return An autoreleased ParticleSystem object.
     */
    static ParticleSystem* create(std::string_view plistFile);

    /** Create a system with a fixed number of particles.
     *
     * @param numberOfParticles A given number of particles.
     * @return An autoreleased ParticleSystemQuad object.
     * @js NA
     */
    static ParticleSystem* createWithTotalParticles(int numberOfParticles);

    /** Gets all ParticleSystem references
     */
    static Vector<ParticleSystem*>& getAllParticleSystems();

protected:
    bool allocAnimationMem();
    void deallocAnimationMem();
    bool _isAnimAllocated;

    bool allocHSVMem();
    void deallocHSVMem();
    bool _isHSVAllocated;

    bool allocOpacityFadeInMem();
    void deallocOpacityFadeInMem();
    bool _isOpacityFadeInAllocated;

    bool allocScaleInMem();
    void deallocScaleInMem();
    bool _isScaleInAllocated;

public:
    void addParticles(int count, int animationIndex = -1, int animationCellIndex = -1);

    void stopSystem();
    /** Kill all living particles.
     */
    void resetSystem();
    /** Whether or not the system is full.
     *
     * @return True if the system is full.
     */
    bool isFull();

    /** Update the verts position data of particle,
     should be overridden by subclasses.
     */
    virtual void updateParticleQuads();
    /** Update the VBO verts buffer which does not use batch node,
     should be overridden by subclasses. */
    virtual void postStep();

    /** Call the update method with no time..
     */
    virtual void updateWithNoTime();

    /** Whether or not the particle system removed self on finish.
     *
     * @return True if the particle system removed self on finish.
     */
    virtual bool isAutoRemoveOnFinish() const;

    /** Set the particle system auto removed it self on finish.
     *
     * @param var True if the particle system removed self on finish.
     */
    virtual void setAutoRemoveOnFinish(bool var);

    // mode A
    /** Gets the gravity.
     *
     * @return The gravity.
     */
    virtual const Vec2& getGravity();
    /** Sets the gravity.
     *
     * @param g The gravity.
     */
    virtual void setGravity(const Vec2& g);
    /** Gets the speed.
     *
     * @return The speed.
     */
    virtual float getSpeed() const;
    /** Sets the speed.
     *
     * @param speed The speed.
     */
    virtual void setSpeed(float speed);
    /** Gets the speed variance.
     *
     * @return The speed variance.
     */
    virtual float getSpeedVar() const;
    /** Sets the speed variance.
     *
     * @param speed The speed variance.
     */
    virtual void setSpeedVar(float speed);
    /** Gets the tangential acceleration.
     *
     * @return The tangential acceleration.
     */
    virtual float getTangentialAccel() const;
    /** Sets the tangential acceleration.
     *
     * @param t The tangential acceleration.
     */
    virtual void setTangentialAccel(float t);
    /** Gets the tangential acceleration variance.
     *
     * @return The tangential acceleration variance.
     */
    virtual float getTangentialAccelVar() const;
    /** Sets the tangential acceleration variance.
     *
     * @param t The tangential acceleration variance.
     */
    virtual void setTangentialAccelVar(float t);
    /** Gets the radial acceleration.
     *
     * @return The radial acceleration.
     */
    virtual float getRadialAccel() const;
    /** Sets the radial acceleration.
     *
     * @param t The radial acceleration.
     */
    virtual void setRadialAccel(float t);
    /** Gets the radial acceleration variance.
     *
     * @return The radial acceleration variance.
     */
    virtual float getRadialAccelVar() const;
    /** Sets the radial acceleration variance.
     *
     * @param t The radial acceleration variance.
     */
    virtual void setRadialAccelVar(float t);
    /** Whether or not the rotation of each particle to its direction.
     *
     * @return True if the rotation is the direction.
     */
    virtual bool getRotationIsDir() const;
    /** Sets the rotation of each particle to its direction.
     *
     * @param t True if the rotation is the direction.
     */
    virtual void setRotationIsDir(bool t);
    // mode B
    /** Gets the start radius.
     *
     * @return The start radius.
     */
    virtual float getStartRadius() const;
    /** Sets the start radius.
     *
     * @param startRadius The start radius.
     */
    virtual void setStartRadius(float startRadius);
    /** Gets the start radius variance.
     *
     * @return The start radius variance.
     */
    virtual float getStartRadiusVar() const;
    /** Sets the start radius variance.
     *
     * @param startRadiusVar The start radius variance.
     */
    virtual void setStartRadiusVar(float startRadiusVar);
    /** Gets the end radius.
     *
     * @return The end radius.
     */
    virtual float getEndRadius() const;
    /** Sets the end radius.
     *
     * @param endRadius The end radius.
     */
    virtual void setEndRadius(float endRadius);
    /** Gets the end radius variance.
     *
     * @return The end radius variance.
     */
    virtual float getEndRadiusVar() const;
    /** Sets the end radius variance.
     *
     * @param endRadiusVar The end radius variance.
     */
    virtual void setEndRadiusVar(float endRadiusVar);
    /** Gets the number of degrees to rotate a particle around the source pos per second.
     *
     * @return The number of degrees to rotate a particle around the source pos per second.
     */
    virtual float getRotatePerSecond() const;
    /** Sets the number of degrees to rotate a particle around the source pos per second.
     *
     * @param degrees The number of degrees to rotate a particle around the source pos per second.
     */
    virtual void setRotatePerSecond(float degrees);
    /** Gets the rotate per second variance.
     *
     * @return The rotate per second variance.
     */
    virtual float getRotatePerSecondVar() const;
    /** Sets the rotate per second variance.
     *
     * @param degrees The rotate per second variance.
     */
    virtual void setRotatePerSecondVar(float degrees);

    virtual void setScale(float s) override;
    virtual void setRotation(float newRotation) override;
    virtual void setScaleX(float newScaleX) override;
    virtual void setScaleY(float newScaleY) override;

    /** Whether or not the particle system is active.
     *
     * @return True if the particle system is active.
     */
    virtual bool isActive() const;
    /** Whether or not the particle system is blend additive.
     *
     * @return True if the particle system is blend additive.
     */
    virtual bool isBlendAdditive() const;
    /** Sets the particle system blend additive.
     *
     * @param value True if the particle system is blend additive.
     */
    virtual void setBlendAdditive(bool value);

    /** Gets the batch node.
     *
     * @return The batch node.
     */
    virtual ParticleBatchNode* getBatchNode() const;
    /** Sets the batch node.
     *
     * @param batchNode The batch node.
     */
    virtual void setBatchNode(ParticleBatchNode* batchNode);

    /** Gets the index of system in batch node array.
     *
     * @return The index of system in batch node array.
     */
    int getAtlasIndex() const { return _atlasIndex; }
    /** Sets the index of system in batch node array.
     *
     * @param index The index of system in batch node array.
     */
    void setAtlasIndex(int index) { _atlasIndex = index; }

    /** Gets the Quantity of particles that are being simulated at the moment.
     *
     * @return The Quantity of particles that are being simulated at the moment.
     */
    unsigned int getParticleCount() const { return _particleCount; }

    /** Gets how many seconds the emitter will run. -1 means 'forever'.
     *
     * @return The seconds that the emitter will run. -1 means 'forever'.
     */
    float getDuration() const { return _duration; }
    /** Sets how many seconds the emitter will run. -1 means 'forever'.
     *
     * @param duration The seconds that the emitter will run. -1 means 'forever'.
     */
    void setDuration(float duration) { _duration = duration; }

    /** Gets the source position of the emitter.
     *
     * @return The source position of the emitter.
     */
    const Vec2& getSourcePosition() const { return _sourcePosition; }
    /** Sets the source position of the emitter.
     *
     * @param pos The source position of the emitter.
     */
    void setSourcePosition(const Vec2& pos) { _sourcePosition = pos; }

    /** Gets the position variance of the emitter.
     *
     * @return The position variance of the emitter.
     */
    const Vec2& getPosVar() const { return _posVar; }
    /** Sets the position variance of the emitter.
     *
     * @param pos The position variance of the emitter.
     */
    void setPosVar(const Vec2& pos) { _posVar = pos; }

    /** Gets the life of each particle.
     *
     * @return The life of each particle.
     */
    float getLife() const { return _life; }
    /** Sets the life of each particle.
     *
     * @param life The life of each particle.
     */
    void setLife(float life) { _life = life; }

    /** Gets the life variance of each particle.
     *
     * @return The life variance of each particle.
     */
    float getLifeVar() const { return _lifeVar; }
    /** Sets the life variance of each particle.
     *
     * @param lifeVar The life variance of each particle.
     */
    void setLifeVar(float lifeVar) { _lifeVar = lifeVar; }

    /** Gets the angle of each particle.
     *
     * @return The angle of each particle.
     */
    float getAngle() const { return _angle; }
    /** Sets the angle of each particle.
     *
     * @param angle The angle of each particle.
     */
    void setAngle(float angle) { _angle = angle; }

    /** Gets the angle variance of each particle.
     *
     * @return The angle variance of each particle.
     */
    float getAngleVar() const { return _angleVar; }
    /** Sets the angle variance of each particle.
     *
     * @param angleVar The angle variance of each particle.
     */
    void setAngleVar(float angleVar) { _angleVar = angleVar; }

    /** Switch between different kind of emitter modes:
     - kParticleModeGravity: uses gravity, speed, radial and tangential acceleration.
     - kParticleModeRadius: uses radius movement + rotation.
     *
     * @return The mode of the emitter.
     */
    Mode getEmitterMode() const { return _emitterMode; }
    /** Sets the mode of the emitter.
     *
     * @param mode The mode of the emitter.
     */
    void setEmitterMode(Mode mode) { _emitterMode = mode; }

    /** Gets the start size in pixels of each particle.
     *
     * @return The start size in pixels of each particle.
     */
    float getStartSize() const { return _startSize; }
    /** Sets the start size in pixels of each particle.
     *
     * @param startSize The start size in pixels of each particle.
     */
    void setStartSize(float startSize) { _startSize = startSize; }

    /** Gets the start size variance in pixels of each particle.
     *
     * @return The start size variance in pixels of each particle.
     */
    float getStartSizeVar() const { return _startSizeVar; }
    /** Sets the start size variance in pixels of each particle.
     *
     * @param sizeVar The start size variance in pixels of each particle.
     */
    void setStartSizeVar(float sizeVar) { _startSizeVar = sizeVar; }

    /** Gets the end size in pixels of each particle.
     *
     * @return The end size in pixels of each particle.
     */
    float getEndSize() const { return _endSize; }
    /** Sets the end size in pixels of each particle.
     *
     * @param endSize The end size in pixels of each particle.
     */
    void setEndSize(float endSize) { _endSize = endSize; }

    /** Gets the end size variance in pixels of each particle.
     *
     * @return The end size variance in pixels of each particle.
     */
    float getEndSizeVar() const { return _endSizeVar; }
    /** Sets the end size variance in pixels of each particle.
     *
     * @param sizeVar The end size variance in pixels of each particle.
     */
    void setEndSizeVar(float sizeVar) { _endSizeVar = sizeVar; }

    /** Gets the start color of each particle.
     *
     * @return The start color of each particle.
     */
    const Color4F& getStartColor() const { return _startColor; }
    /** Sets the start color of each particle.
     *
     * @param color The start color of each particle.
     */
    void setStartColor(const Color4F& color) { _startColor = color; }

    /** Gets the start color variance of each particle.
     *
     * @return The start color variance of each particle.
     */
    const Color4F& getStartColorVar() const { return _startColorVar; }
    /** Sets the start color variance of each particle.
     *
     * @param color The start color variance of each particle.
     */
    void setStartColorVar(const Color4F& color) { _startColorVar = color; }

    /** Gets the end color and end color variation of each particle.
     *
     * @return The end color and end color variation of each particle.
     */
    const Color4F& getEndColor() const { return _endColor; }
    /** Sets the end color and end color variation of each particle.
     *
     * @param color The end color and end color variation of each particle.
     */
    void setEndColor(const Color4F& color) { _endColor = color; }

    /** Gets the end color variance of each particle.
     *
     * @return The end color variance of each particle.
     */
    const Color4F& getEndColorVar() const { return _endColorVar; }
    /** Sets the end color variance of each particle.
     *
     * @param color The end color variance of each particle.
     */
    void setEndColorVar(const Color4F& color) { _endColorVar = color; }

    /** Sets wether to use HSV color system.
     * WARNING: becareful when using HSV with too many particles because it's expensive.
     *
     * @param hsv Use HSV color system.
     */
    void useHSV(bool hsv);
    bool isHSV() { return _isHSVAllocated; };

    /** Gets the hue of each particle.
     *
     * @return The hue of each particle.
     */
    float getHue() const { return _hsv.h; }
    /** Sets the hue of each particle.
     *
     * @param hsv The hue color of each particle.
     */
    void setHue(float hue) { _hsv.h = hue; }

    /** Gets the hue variance of each particle.
     *
     * @return The hue variance of each particle.
     */
    float getHueVar() const { return _hsvVar.h; }
    /** Sets the hue variance of each particle.
     *
     * @param hsv The hue variance color of each particle.
     */
    void setHueVar(float hue) { _hsvVar.h = hue; }

    /** Gets the HSV color of each particle.
     *
     * @return The HSV color of each particle.
     */
    const HSV& getHSV() const { return _hsv; }
    /** Sets the HSV color of each particle.
     *
     * @param hsv The HSV color of each particle.
     */
    void setHSV(const HSV& hsv) { _hsv = hsv; }

    /** Gets the HSV color variance of each particle.
     *
     * @return The HSV color variance of each particle.
     */
    const HSV& getHSVVar() const { return _hsvVar; }
    /** Sets the HSV color variance of each particle.
     *
     * @param hsv The HSV color variance of each particle.
     */
    void setHSVVar(const HSV& hsv) { _hsvVar = hsv; }

    /** Gets the start spin of each particle.
     *
     * @return The start spin of each particle.
     */
    float getStartSpin() const { return _startSpin; }
    /** Sets the start spin of each particle.
     *
     * @param spin The start spin of each particle.
     */
    void setStartSpin(float spin) { _startSpin = spin; }

    /** Gets the start spin variance of each particle.
     *
     * @return The start spin variance of each particle.
     */
    float getStartSpinVar() const { return _startSpinVar; }
    /** Sets the start spin variance of each particle.
     *
     * @param pinVar The start spin variance of each particle.
     */
    void setStartSpinVar(float pinVar) { _startSpinVar = pinVar; }

    /** Gets the end spin of each particle.
     *
     * @return The end spin of each particle.
     */
    float getEndSpin() const { return _endSpin; }
    /** Sets the end spin of each particle.
     *
     * @param endSpin The end spin of each particle.
     */
    void setEndSpin(float endSpin) { _endSpin = endSpin; }

    /** Gets the end spin variance of each particle.
     *
     * @return The end spin variance of each particle.
     */
    float getEndSpinVar() const { return _endSpinVar; }
    /** Sets the end spin variance of each particle.
     *
     * @param endSpinVar The end spin variance of each particle.
     */
    void setEndSpinVar(float endSpinVar) { _endSpinVar = endSpinVar; }

    /** Gets the spawn angle of each particle
     *
     * @return The angle in degrees of each particle.
     */
    float getSpawnAngle() { return _spawnAngle; }
    /** Sets the spawn angle of each particle
     *
     * @param angle The angle in degrees of each particle.
     */
    void setSpawnAngle(float angle) { _spawnAngle = angle; }

    /** Sets the spawn angle variance of each particle.
     *
     * @return The angle variance in degrees of each particle.
     */
    float getSpawnAngleVar() { return _spawnAngleVar; }
    /** Sets the spawn angle variance of each particle.
     *
     * @param angle The angle variance in degrees of each particle.
     */
    void setSpawnAngleVar(float angle) { _spawnAngleVar = angle; }

    /** Gets the spawn opacity fade in time of each particle.
     * Particles have the ability to spawn while having 0 opacity and gradually start going to 255 opacity with a
     specified time.

     * @return The spawn opacity fade in time in seconds.
     */
    float getSpawnFadeIn() { return _spawnFadeIn; }
    /** Sets the spawn opacity fade in time of each particle when it's created.
     * Particles have the ability to spawn while having 0 opacity and gradually start going to 255 opacity with a
     * specified time.
     *
     * @param time The spawn opacity fade in time in seconds.
     */
    void setSpawnFadeIn(float time);

    /** Gets the spawn opacity fade in time variance of each particle.
     * Particles have the ability to spawn while having 0 opacity and gradually start going to 255 opacity with a
     * specified time.
     *
     * @return The spawn opacity fade in time variance in seconds.
     */
    float getSpawnFadeInVar() { return _spawnFadeInVar; }
    /** Sets the spawn opacity fade in time variance of each particle when it's created.
     * Particles have the ability to spawn while having 0 opacity and gradually start going to 255 opacity with a
     * specified time.
     *
     * @param time The spawn opacity fade in time variance in seconds.
     */
    void setSpawnFadeInVar(float time);

    /** Gets the spawn scale fade in time of each particle.
     * Particles have the ability to spawn while having 0.0 size and gradually start going to 1.0 size with a specified
     * time.
     *
     * @return The spawn opacity fade in time in seconds.
     */
    float getSpawnScaleIn() { return _spawnScaleIn; }
    /** Sets the spawn scale fade in time of each particle when it's created.
     * Particles have the ability to spawn while having 0.0 size and gradually start going to 1.0 size with a specified
     * time.
     *
     * @param time The spawn opacity fade in time in seconds.
     */
    void setSpawnScaleIn(float time);

    /** Gets the spawn scale fade in time variance of each particle.
     * Particles have the ability to spawn while having 0.0 size and gradually start going to 1.0 size with a specified
     * time.
     *
     * @return The spawn opacity fade in time variance in seconds.
     */
    float getSpawnScaleInVar() { return _spawnScaleInVar; }
    /** Sets the spawn scale fade in time variance of each particle when it's created.
     * Particles have the ability to spawn while having 0.0 size and gradually start going to 1.0 size with a specified
     * time.
     *
     * @param time The spawn opacity fade in time variance in seconds.
     */
    void setSpawnScaleInVar(float time);

    /** Gets the emission rate of the particles.
     *
     * @return The emission rate of the particles.
     */
    float getEmissionRate() const { return _emissionRate; }
    /** Sets the emission rate of the particles.
     *
     * @param rate The emission rate of the particles.
     */
    void setEmissionRate(float rate) { _emissionRate = rate; }

    /** Gets the maximum particles of the system.
     *
     * @return The maximum particles of the system.
     */
    virtual int getTotalParticles() const;
    /** Sets the maximum particles of the system.
     *
     * @param totalParticles The maximum particles of the system.
     */
    virtual void setTotalParticles(int totalParticles);

    /** does the alpha value modify color */
    void setOpacityModifyRGB(bool opacityModifyRGB) override { _opacityModifyRGB = opacityModifyRGB; }
    bool isOpacityModifyRGB() const override { return _opacityModifyRGB; }

    /** Enables or disables tex coord animations that are set based on particle life. */
    void setLifeAnimation(bool enabled);

    /** Enables or disables tex coord animations that are set by the emitter randomly when a particle is emitted. */
    void setEmitterAnimation(bool enabled);

    /** Enables or disables tex coord animations that are used to make particles play a sequence forever until they die
     */
    void setLoopAnimation(bool enabled);

    bool isLifeAnimated() { return _isLifeAnimated; }
    bool isEmitterAnimated() { return _isEmitterAnimated; }
    bool isLoopAnimated() { return _isLoopAnimated; }

    /** Gets the total number of indices.
     *
     * @return The size of the list holding animation indices.
     */
    int getTotalAnimationIndices() { return _animIndexCount; }

    /** Sets wether to start from first cell and go forwards (normal) or last cell and go backwards (reversed) */
    void setAnimationReverse(bool reverse) { _isAnimationReversed = reverse; }
    bool isAnimationReversed() { return _isAnimationReversed; }

    /** Resets the count of indices to 0 and empties the animation index array */
    void resetAnimationIndices();

    /** Empties the container of animation descriptors */
    void resetAnimationDescriptors();

    /** Choose what animation descriptors are to be selected at random for particles.
     * This function should be called after you've inserted/overwritten any animation descriptors.
     *
     * @param animations Array of specific indices of animations to play at random
     */
    void setMultiAnimationRandomSpecific(const std::vector<unsigned short>& animations)
    {
        _randomAnimations = animations;
    };

    /** Choose ALL animation descriptors to be selected at random for particles.
     * This function should be called after you've inserted/overwritten any animation descriptors.
     */
    void setMultiAnimationRandom();

    /** Add all particle animation indices based on cells size and direction spicified using a texture atlas.
     * will erase the array and add new indices from the atlas.
     * This function will automatically figure out your atlas cell size and direction for you! thank her later :) */
    void setAnimationIndicesAtlas();

    /** Add all particle animation indices based on cell size and direction spicified if the method of rendering
     * preferred is texture atlas. will erase the array and add new indices from the atlas.
     *
     * @param unifiedCellSize The size of cell unified.
     * @param direction What direction is the atlas
     */
    void setAnimationIndicesAtlas(unsigned int unifiedCellSize, TexAnimDir direction = TexAnimDir::HORIZONTAL);

    /** Add a particle animation index based on tex coords spicified using a sprite frame.
     * The index is automatically incremented on each addition.
     *
     * @param frameName SpriteFrame name to search for
     *
     * @return Returns true of the index was successfully found and added. Otherwise, false
     */
    bool addAnimationIndex(std::string_view frameName);

    /** Add a particle animation index based on tex coords spicified using a sprite frame.
     *
     * @param index Index id to add the frame to or override it with the new frame
     * @param frameName SpriteFrame name to search for
     *
     * @return Returns true of the index was successfully found and added. Otherwise, false
     */
    bool addAnimationIndex(unsigned short index, std::string_view frameName);

    /** Add a particle animation index based on tex coords spicified using a sprite frame.
     * The index is automatically incremented on each addition.
     *
     * @param frame SpriteFrame containting data about tex coords
     *
     * @return Returns true of the index was successfully found and added. Otherwise, false
     */
    bool addAnimationIndex(ax::SpriteFrame* frame);

    /** Add a particle animation index based on tex coords spicified using a sprite frame.
     * you can specify which index you want to override in this function
     *
     * @param index Index id to add the frame to or override it with the new frame
     * @param frame SpriteFrame containting data about tex coords
     *
     * @return Returns true of the index was successfully found and added. Otherwise, false
     */
    bool addAnimationIndex(unsigned short index, ax::SpriteFrame* frame);

    /** Add a particle animation index based on tex coords spicified.
     * you can specify which index you want to override in this function
     *
     * @param index Index id to add the frame to or override it with the new rect
     * @param rect Rect containting data about tex coords in pixels
     * @param rotated Not implemented.
     *
     * @return Returns true of the index was successfully found and added. Otherwise, false
     */
    bool addAnimationIndex(unsigned short index, ax::Rect rect, bool rotated = false);

    /** You can specify what rect is used if an index in an animation descriptor wasn't found.
     *
     * @param rect Rect containting data about tex coords in pixels
     */
    void setRectForUndefinedIndices(ax::Rect rect) { _undefinedIndexRect = rect; };

    /** Add a particle animation descriptor with an index.
     *
     * @param indexOfDescriptor Index of the animation to be added, adding to the same index will just override the
     * pervious animation descriptor
     * @param time length of the animation in seconds
     * @param timeVariance Time randomly selected for each different particle added on the animation length
     * @param indices An array of the indicies
     * @param reverse Should the animation indicies be played backwards? (default: false)
     */
    void setAnimationDescriptor(unsigned short indexOfDescriptor,
                                float time,
                                float timeVariance,
                                const std::vector<unsigned short>& indices,
                                bool reverse = false);

    /** Add a particle animation descriptor with the index 0.
     *
     * @param indices An array of the indicies
     * @param reverse Should the animation indicies be played backwards? (default: false)
     */
    void setAnimationDescriptor(const std::vector<unsigned short>& indices, bool reverse = false)
    {
        setAnimationDescriptor(0, 0, 0, indices, reverse);
    };

    /** Sets wether the animation descriptors should follow the time scale of the system or not.
     *
     * @param independent Should the animation descriptor speeds be played independently? (default: false)
     */
    void setAnimationSpeedTimescaleIndependent(bool independent) { _animationTimescaleInd = independent; };
    bool isAnimationSpeedTimescaleIndependent() { return _animationTimescaleInd; };

    /** Sets wether to use emission shapes for this particle system or not */
    void setEmissionShapes(bool enabled) { _isEmissionShapes = enabled; }
    bool isEmissionShapes() { return _isEmissionShapes; }

    /** Resets the count of emission shapes to 0 and empties the emission shapes array */
    void resetEmissionShapes();

    /** Adds an emission shape to the system.
     * The index is automatically incremented on each addition.
     * 
     * @param shape Shape descriptor object.
     */
    void addEmissionShape(EmissionShape shape);

    /** Updates an existing emission shape or adds it.
     * @param index index of the shape descriptor.
     * @param shape Shape descriptor object.
     */
    void setEmissionShape(unsigned short index, EmissionShape shape);

    /** Adds an emission shape of type mask to the system.
     * The mask should be added using the ParticleEmissionMaskCache class.
     * 
     * @param maskId The id of the mask, FOURCC starts with '#', such as "#abcd"
     * @param pos Position of the emission shape in local space.
     * @param overrideSize Size of the emission mask in pixels, leave ZERO to use texture size.
     * @param scale Scale of the emission mask, the size will be multiplied by the specified scale.
     * @param angle Angle of the sampled points to be rotated in degrees.
     */
    static EmissionShape createMaskShape(std::string_view maskId,
                                         Vec2 pos          = Vec2::ZERO,
                                         Vec2 overrideSize = Vec2::ZERO,
                                         Vec2 scale        = Vec2::ONE,
                                         float angle       = 0.0F);

    /** Adds an emission shape of type point to the system. 
     * @param pos Position of the emission shape in local space.
     */
    static EmissionShape createPointShape(Vec2 pos);

    /** Adds an emission shape of type Rectangle to the system. 
     * @param pos Position of the emission shape in local space.
     * @param size Size of the rectangle.
     */
    static EmissionShape createRectShape(Vec2 pos, Size size);

    /** Adds an emission shape of type Rectangular Torus to the system. 
     * @param pos Position of the emission shape in local space.
     * @param innerSize Inner size offset of the rectangle.
     * @param outerSize Outer size of the rectangle.
     */
    static EmissionShape createRectTorusShape(Vec2 pos, Size innerSize, Size outerSize);

    /** Adds an emission shape of type Circle to the system.
     *
     * The default angle offset of the circle is 0 and the default angle of the circle is 360
     *
     * @param pos Position of the emission shape in local space.
     * @param radius Radius of the circle.
     * @param edgeBias circle edge center bias value, If the value is greater than 1.0 then particles will bias
     * towards the edge of the circle more often the greater the value is; If the value is lower than 1.0 then particles
     * will bias towards the center of the circle more often the closer the value is to 0.0; If the value is exactly 1.0
     * then there will be no bias behaviour.
     */
    static EmissionShape createCircleShape(Vec2 pos, float radius, float edgeBias = 1.0F);

    /** Adds an emission shape of type Cone to the system.
     *
     * The default angle offset of the circle is 0 and the default angle of the circle is 360
     *
     * @param pos Position of the emission shape in local space.
     * @param radius Radius of the circle.
     * @param offset Cone offset angle in degrees.
     * @param angle Cone angle in degrees.
     * @param edgeBias circle edge center bias value, If the value is greater than 1.0 then particles will bias
     * towards the edge of the circle more often the greater the value is; If the value is lower than 1.0 then particles
     * will bias towards the center of the circle more often the closer the value is to 0.0; If the value is exactly 1.0
     * then there will be no bias behaviour.
     */
    static EmissionShape createConeShape(Vec2 pos, float radius, float offset, float angle, float edgeBias = 1.0F);

    /** Adds an emission shape of type Torus to the system.
     *
     * The default angle offset of the torus is 0 and the default angle of the torus is 360
     *
     * @param pos Position of the emission shape in local space.
     * @param innerRadius Inner radius offset of the torus.
     * @param outerRadius Outer radius of the torus.
     * @param edgeBias torus edge center bias value, If the value is greater than 1.0 then particles will bias
     * towards the edge of the torus more often the greater the value is; If the value is lower than 1.0 then particles
     * will bias towards the center of the torus more often the closer the value is to 0.0; If the value is exactly 1.0
     * then there will be no bias behaviour.
     */
    static EmissionShape createTorusShape(Vec2 pos, float innerRadius, float outerRadius, float edgeBias = 1.0F);

    /** Adds an emission shape of type Torus to the system.
     *
     * The default angle offset of the torus is 0 and the default angle of the torus is 360
     *
     * @param pos Position of the emission shape in local space.
     * @param innerRadius Inner radius offset of the torus.
     * @param outerRadius Outer radius of the torus.
     * @param offset Cone offset angle in degrees.
     * @param angle Cone angle in degrees.
     * @param edgeBias torus edge center bias value, If the value is greater than 1.0 then particles will bias
     * towards the edge of the torus more often the greater the value is; If the value is lower than 1.0 then particles
     * will bias towards the center of the torus more often the closer the value is to 0.0; If the value is exactly 1.0
     * then there will be no bias behaviour.
     */
    static EmissionShape createConeTorusShape(Vec2 pos,
                                              float innerRadius,
                                              float outerRadius,
                                              float offset,
                                              float angle,
                                              float edgeBias = 1.0F);

    /** Gets the particles movement type: Free or Grouped.
     @since v0.8
     *
     * @return The particles movement type.
     */
    PositionType getPositionType() const { return _positionType; }
    /** Sets the particles movement type: Free or Grouped.
    @since v0.8
     *
     * @param type The particles movement type.
     */
    void setPositionType(PositionType type) { _positionType = type; }

    /** Advance the particle system and make it seem like it ran for this many seconds.
     *
     * @param seconds Seconds to advance. value of -1 means (SIMULATION_USE_PARTICLE_LIFETIME)
     * @param frameRate Frame rate to run the simulation with (preferred: 30.0) The higher this value is the more
     * accurate the simulation will be at the cost of performance. value of -1 means
     * (SIMULATION_USE_GAME_ANIMATION_INTERVAL)
     */
    void simulate(float seconds   = SIMULATION_USE_PARTICLE_LIFETIME,
                  float frameRate = SIMULATION_USE_GAME_ANIMATION_INTERVAL);

    /** Resets the particle system and then advances the particle system and make it seem like it ran for this many
     * seconds. The frame rate used for simulation accuracy is the director's animation interval.
     *
     * @param seconds Seconds to advance. value of -1 means (SIMULATION_USE_PARTICLE_LIFETIME)
     * @param frameRate Frame rate to run the simulation with (preferred: 30.0) The higher this value is the more
     * accurate the simulation will be at the cost of performance. value of -1 means
     * (SIMULATION_USE_GAME_ANIMATION_INTERVAL)
     */
    void resimulate(float seconds   = SIMULATION_USE_PARTICLE_LIFETIME,
                    float frameRate = SIMULATION_USE_GAME_ANIMATION_INTERVAL);

    // Overrides
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual void update(float dt) override;
    virtual Texture2D* getTexture() const override;
    virtual void setTexture(Texture2D* texture) override;
    /**
     *@code
     *When this function bound into js or lua,the parameter will be changed
     *In js: var setBlendFunc(var src, var dst)
     *In lua: local setBlendFunc(local src, local dst)
     *@endcode
     */
    virtual void setBlendFunc(const BlendFunc& blendFunc) override;
    /**
     * @js NA
     * @lua NA
     */
    virtual const BlendFunc& getBlendFunc() const override;

    std::string_view getResourceFile() const { return _plistFile; }

    /// @{
    /// @name implement Playable Protocol
    virtual void start() override;
    virtual void stop() override;
    /// @} end of PlayableProtocol

    void setSourcePositionCompatible(bool sourcePositionCompatible)
    {
        _sourcePositionCompatible = sourcePositionCompatible;
    }
    bool isSourcePositionCompatible() const { return _sourcePositionCompatible; }

    /**
     * @js ctor
     */
    ParticleSystem();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~ParticleSystem();

    /** initializes a ParticleSystem*/
    bool init() override;
    /** initializes a ParticleSystem from a plist file.
     This plist files can be created manually or with Particle Designer:
     http://particledesigner.71squared.com/
     @since v0.99.3
     */
    bool initWithFile(std::string_view plistFile);

    /** initializes a QuadParticleSystem from a Dictionary.
     @since v0.99.3
     */
    bool initWithDictionary(const ValueMap& dictionary);

    /** initializes a particle system from a NSDictionary and the path from where to load the png
     @since v2.1
     */
    bool initWithDictionary(const ValueMap& dictionary, std::string_view dirname);

    //! Initializes a system with a fixed number of particles
    virtual bool initWithTotalParticles(int numberOfParticles);

    /** Are the emissions paused
     @return True if the emissions are paused, else false
     */
    virtual bool isPaused() const;

    /* Pause the emissions */
    virtual void pauseEmissions();

    /* Unpause the emissions */
    virtual void resumeEmissions();

    /** Gets the fixed frame rate count of the particle system.
     @return Fixed frame rate count of the particle system.
     */
    virtual float getFixedFPS();

    /** Sets the fixed frame rate count of the particle system.
     * i.e. if the framerate is set to 30.0 while the refresh rate is greater than 30.0 then the particle system will
     wait until it hits the 30.0 FPS mark.
     * This is usefull for increasing performance or for creating old-school effects with it.
     @param Fixed frame rate count of the particle system. (default: 0.0)
     */
    virtual void setFixedFPS(float frameRate = 0.0F);

    /** Gets the time scale of the particle system.
     @return Time scale of the particle system.
     */
    virtual float getTimeScale();

    /** Sets the time scale of the particle system.
     @param Time scale of the particle system. (default: 1.0)
     */
    virtual void setTimeScale(float scale = 1.0F);

protected:
    virtual void updateBlendFunc();

private:
    friend class EngineDataManager;
    /** Internal use only, it's used by EngineDataManager class for Android platform */
    static void setTotalParticleCountFactor(float factor);

protected:
    /** whether or not the particles are using blend additive.
     If enabled, the following blending function will be used.
     @code
     source blend function = BlendFactor::SRC_ALPHA;
     dest blend function = BlendFactor::ONE;
     @endcode
     */
    bool _isBlendAdditive;

    /** whether or not the node will be auto-removed when it has no particles left.
     By default it is false.
     @since v0.8
     */
    bool _isAutoRemoveOnFinish;

    std::string _plistFile;
    //! time elapsed since the start of the system (in seconds)
    float _elapsed;

    // Different modes
    //! Mode A:Gravity + Tangential Accel + Radial Accel
    struct
    {
        /** Gravity value. Only available in 'Gravity' mode. */
        Vec2 gravity;
        /** speed of each particle. Only available in 'Gravity' mode.  */
        float speed;
        /** speed variance of each particle. Only available in 'Gravity' mode. */
        float speedVar;
        /** tangential acceleration of each particle. Only available in 'Gravity' mode. */
        float tangentialAccel;
        /** tangential acceleration variance of each particle. Only available in 'Gravity' mode. */
        float tangentialAccelVar;
        /** radial acceleration of each particle. Only available in 'Gravity' mode. */
        float radialAccel;
        /** radial acceleration variance of each particle. Only available in 'Gravity' mode. */
        float radialAccelVar;
        /** set the rotation of each particle to its direction Only available in 'Gravity' mode. */
        bool rotationIsDir;
    } modeA;

    //! Mode B: circular movement (gravity, radial accel and tangential accel don't are not used in this mode)
    struct
    {
        /** The starting radius of the particles. Only available in 'Radius' mode. */
        float startRadius;
        /** The starting radius variance of the particles. Only available in 'Radius' mode. */
        float startRadiusVar;
        /** The ending radius of the particles. Only available in 'Radius' mode. */
        float endRadius;
        /** The ending radius variance of the particles. Only available in 'Radius' mode. */
        float endRadiusVar;
        /** Number of degrees to rotate a particle around the source pos per second. Only available in 'Radius' mode. */
        float rotatePerSecond;
        /** Variance in degrees for rotatePerSecond. Only available in 'Radius' mode. */
        float rotatePerSecondVar;
    } modeB;

    // particle data
    ParticleData _particleData;

    // Emitter name
    std::string _configName;

    // color modulate
    //    BOOL colorModulate;

    //! How many particles can be emitted per second
    float _emitCounter;

    // Optimization
    // AX_UPDATE_PARTICLE_IMP    updateParticleImp;
    // SEL                        updateParticleSel;

    /** weak reference to the SpriteBatchNode that renders the Sprite */
    ParticleBatchNode* _batchNode;

    // index of system in batch node array
    int _atlasIndex;

    // true if scaled or rotated
    bool _transformSystemDirty;
    // Number of allocated particles
    int _allocatedParticles;

    /** Is the emitter active */
    bool _isActive;

    /** Quantity of particles that are being simulated at the moment */
    int _particleCount;
    /** The factor affects the total particle count, its value should be 0.0f ~ 1.0f, default 1.0f*/
    static float __totalParticleCountFactor;

    /** How many seconds the emitter will run. -1 means 'forever' */
    float _duration;
    /** sourcePosition of the emitter */
    Vec2 _sourcePosition;
    /** Position variance of the emitter */
    Vec2 _posVar;
    /** life, and life variation of each particle */
    float _life;
    /** life variance of each particle */
    float _lifeVar;
    /** angle and angle variation of each particle */
    float _angle;
    /** angle variance of each particle */
    float _angleVar;

    /** Switch between different kind of emitter modes:
     - kParticleModeGravity: uses gravity, speed, radial and tangential acceleration
     - kParticleModeRadius: uses radius movement + rotation
     */
    Mode _emitterMode;

    /** start size in pixels of each particle */
    float _startSize;
    /** size variance in pixels of each particle */
    float _startSizeVar;
    /** end size in pixels of each particle */
    float _endSize;
    /** end size variance in pixels of each particle */
    float _endSizeVar;
    /** start color of each particle */
    Color4F _startColor;
    /** start color variance of each particle */
    Color4F _startColorVar;
    /** end color and end color variation of each particle */
    Color4F _endColor;
    /** end color variance of each particle */
    Color4F _endColorVar;
    /** hsv color of each particle */
    HSV _hsv;
    /** hsv color variance of each particle */
    HSV _hsvVar;
    //* initial angle of each particle
    float _startSpin;
    //* initial angle of each particle
    float _startSpinVar;
    //* initial angle of each particle
    float _endSpin;
    //* initial angle of each particle
    float _endSpinVar;
    //* initial rotation of each particle
    float _spawnAngle;
    //* initial rotation of each particle
    float _spawnAngleVar;
    //* initial fade in time of each particle
    float _spawnFadeIn;
    //* initial fade in time variance of each particle
    float _spawnFadeInVar;
    //* initial scale in time of each particle
    float _spawnScaleIn;
    //* initial scale in time variance of each particle
    float _spawnScaleInVar;
    /** emission rate of the particles */
    float _emissionRate;
    /** maximum particles of the system */
    int _totalParticles;
    /** conforms to CocosNodeTexture protocol */
    Texture2D* _texture;
    /** conforms to CocosNodeTexture protocol */
    BlendFunc _blendFunc;
    /** does the alpha value modify color */
    bool _opacityModifyRGB;
    /** is the particle system animated */
    bool _isLifeAnimated;
    /** is the emitter particle system animated */
    bool _isEmitterAnimated;
    /** is the emitter particle system animated */
    bool _isLoopAnimated;
    /** variable keeping count of sprite frames or atlas indices added */
    int _animIndexCount;
    /** wether to start from first or last when using life animation */
    bool _isAnimationReversed;
    /** A map that stores particle animation index coords */
    std::unordered_map<unsigned short, ParticleFrameDescriptor> _animationIndices;
    /** A map that stores particle animation descriptors */
    std::unordered_map<unsigned short, ParticleAnimationDescriptor> _animations;
    /** A vector that stores ids of animation descriptors that are choosen at random */
    std::vector<unsigned short> _randomAnimations;
    /** Wether the animation goes with the time scale of the system or is independent. */
    bool _animationTimescaleInd;
    /** A rect that is used instead when an index is not found */
    ax::Rect _undefinedIndexRect;
    /** does FlippedY variance of each particle */
    int _yCoordFlipped;

    /** Wether to use emission shapes for this particle system or not */
    bool _isEmissionShapes;

    /** variable keeping count of emission shapes added */
    int _emissionShapeIndex;
    /** A map that stores emission shapes that are choosen at random */
    std::unordered_map<unsigned short, EmissionShape> _emissionShapes;

    /** particles movement type: Free or Grouped
     @since v0.8
     */
    PositionType _positionType;

    /** is the emitter paused */
    bool _paused;

    /** time scale of the particle system */
    float _timeScale;

    /** Fixed frame rate of the particle system */
    float _fixedFPS;

    /** Fixed frame rate delta (internal) */
    float _fixedFPSDelta;

    /** is sourcePosition compatible */
    bool _sourcePositionCompatible;

    static Vector<ParticleSystem*> __allInstances;

    FastRNG _rng;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(ParticleSystem);
};

// end of _2d group
/// @}

NS_AX_END

#endif  //__CCPARTICLE_SYSTEM_H__
