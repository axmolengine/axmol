/****************************************************************************
Copyright (c) 2009      On-Core
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
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
#ifndef __ACTION_CCGRID_ACTION_H__
#define __ACTION_CCGRID_ACTION_H__

#include "2d/ActionInterval.h"
#include "2d/ActionInstant.h"

NS_AX_BEGIN

class GridBase;
class NodeGrid;

/**
 * @addtogroup actions
 * @{
 */

/**
@class GridAction
@brief Base class for Grid actions.
@details Grid actions are the actions take effect on GridBase.
*/
class AX_DLL GridAction : public ActionInterval
{
public:
    /**
    @brief Get the pointer of GridBase.
    @return The pointer of GridBase.
    */
    virtual GridBase* getGrid();

    // overrides
    virtual GridAction* clone() const override
    {
        AX_ASSERT(0);
        return nullptr;
    }
    virtual GridAction* reverse() const override;
    virtual void startWithTarget(Node* target) override;

    GridAction() {}
    virtual ~GridAction() {}
    /**
     * @brief Initializes the action with size and duration.
     * @param duration The duration of the GridAction. It's a value in seconds.
     * @param gridSize The size of the GridAction should be.
     * @return Return true when the initialization success, otherwise return false.
     */
    bool initWithDuration(float duration, const Vec2& gridSize);

protected:
    Vec2 _gridSize;

    NodeGrid* _gridNodeTarget;

    void cacheTargetAsGridNode();

private:
    AX_DISALLOW_COPY_AND_ASSIGN(GridAction);
};

/**
 @brief Base class for Grid3D actions.
 @details Grid3D actions can modify a non-tiled grid.
 */
class AX_DLL Grid3DAction : public GridAction
{
public:
    virtual GridBase* getGrid() override;
    /**
     * @brief Get the vertex that belongs to certain position in the grid.
     * @param position The position of the grid.
     * @return Return a pointer of vertex.
     * @js vertex
     * @lua NA
     */
    Vec3 getVertex(const Vec2& position) const;

    /**
     * @brief Get the non-transformed vertex that belongs to certain position in the grid.
     * @param position The position of the grid.
     * @return Return a pointer of vertex.
     * @js originalVertex
     * @lua NA
     */
    Vec3 getOriginalVertex(const Vec2& position) const;

    /**
     * @brief Set a new vertex to a certain position of the grid.
     * @param position The position of the grid.
     * @param vertex The vertex will be used on the certain position of grid.
     * @js setVertex
     * @lua NA
     */
    void setVertex(const Vec2& position, const Vec3& vertex);

    // Overrides
    virtual Grid3DAction* clone() const override
    {
        AX_ASSERT(0);
        return nullptr;
    }

    /**
     * @brief Get the effect grid rect.
     * @return Return the effect grid rect.
     */
    Rect getGridRect() const;
};

/**
@brief Base class for TiledGrid3D actions.
*/
class AX_DLL TiledGrid3DAction : public GridAction
{
public:
    /**
     * @brief Create the action with size and duration.
     * @param duration The duration of the action. It's a value in seconds.
     * @param gridSize Specify the grid size of the action.
     * @return A pointer of TiledGrid3DAction. If creation failed, return nil.
     * @lua NA
     */
    static TiledGrid3DAction* create(float duration, const Vec2& gridSize);

    /**
     * @brief Get the tile that belongs to a certain position of the grid.
     * @param position The position of the tile want to get.
     * @return A quadrilateral of the tile.
     * @js tile
     * @lua NA
     */
    Quad3 getTile(const Vec2& position) const;

    /**
     * @brief Get the non-transformed tile that belongs to a certain position of the grid.
     * @param position The position of the tile want to get.
     * @return A quadrilateral of the tile.
     * @js originalTile
     * @lua NA
     */
    Quad3 getOriginalTile(const Vec2& position) const;

    /**
     * @brief Set a new tile to a certain position of the grid.
     * @param position The position of the tile.
     * @param coords The quadrilateral of the new tile.
     * @lua NA
     */
    void setTile(const Vec2& position, const Quad3& coords);

    /** returns the grid */
    virtual GridBase* getGrid() override;

    // Override
    virtual TiledGrid3DAction* clone() const override
    {
        AX_ASSERT(0);
        return nullptr;
    }
};

/**
@brief AccelDeccelAmplitude action.
@js NA
*/
class AX_DLL AccelDeccelAmplitude : public ActionInterval
{
public:
    /**
    @brief Create the action with an inner action that has the amplitude property, and a duration time.
    @param action A pointer of the inner action.
    @param duration Specify the duration of the AccelDeccelAmplitude action.
    @return Return a pointer of AccelDeccelAmplitude action. When the creation failed, return nil.
    */
    static AccelDeccelAmplitude* create(Action* action, float duration);

    /**
    @brief Get the value of amplitude rate.
    @return the value of amplitude rate.
    */
    float getRate() const { return _rate; }
    /**
    @brief Set the value of amplitude rate.
    @param rate Specify the value of amplitude rate.
    */
    void setRate(float rate) { _rate = rate; }

    // Overrides
    virtual void startWithTarget(Node* target) override;
    virtual void update(float time) override;
    virtual AccelDeccelAmplitude* clone() const override;
    virtual AccelDeccelAmplitude* reverse() const override;

    AccelDeccelAmplitude() {}
    virtual ~AccelDeccelAmplitude();

    /**
    @brief Initializes the action with an inner action that has the amplitude property, and a duration time.
    @param action A pointer of the inner action.
    @param duration Specify the duration of the AccelDeccelAmplitude action.
    @return If the initialization success, return true; otherwise, return false.
    */
    bool initWithAction(Action* action, float duration);

protected:
    float _rate;
    ActionInterval* _other;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(AccelDeccelAmplitude);
};

/**
@brief AccelAmplitude action.
@js NA
*/
class AX_DLL AccelAmplitude : public ActionInterval
{
public:
    /**
    @brief Create the action with an inner action that has the amplitude property, and a duration time.
    @param action A pointer of the inner action.
    @param duration Specify the duration of the AccelAmplitude action.
    @return Return a pointer of AccelAmplitude action. When the creation failed, return nil.
     */
    static AccelAmplitude* create(Action* action, float duration);

    /**
    @brief Get the value of amplitude rate.
    @return The value of amplitude rate.
    */
    float getRate() const { return _rate; }
    /**
    @brief Set the value of amplitude rate.
    @param rate Specify the value of amplitude rate.
    */
    void setRate(float rate) { _rate = rate; }

    // Overrides
    virtual void startWithTarget(Node* target) override;
    virtual void update(float time) override;
    virtual AccelAmplitude* clone() const override;
    virtual AccelAmplitude* reverse() const override;

    AccelAmplitude() {}
    virtual ~AccelAmplitude();

    bool initWithAction(Action* action, float duration);

protected:
    float _rate;
    ActionInterval* _other;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(AccelAmplitude);
};

/**
@brief DeccelAmplitude action.
@js NA
*/
class AX_DLL DeccelAmplitude : public ActionInterval
{
public:
    /**
    @brief Creates the action with an inner action that has the amplitude property, and a duration time.
    @param action A pointer of the inner action.
    @param duration Specify the duration of the DeccelAmplitude action.
    @return Return a pointer of DeccelAmplitude. When the creation failed, return nil.
    */
    static DeccelAmplitude* create(Action* action, float duration);

    /**
    @brief Get the value of amplitude rate.
    @return The value of amplitude rate.
    */
    float getRate() const { return _rate; }
    /**
    @brief Set the value of amplitude rate.
    @param rate Specify the value.
    */
    void setRate(float rate) { _rate = rate; }

    // overrides
    virtual void startWithTarget(Node* target) override;
    virtual void update(float time) override;
    virtual DeccelAmplitude* clone() const override;
    virtual DeccelAmplitude* reverse() const override;

    DeccelAmplitude() {}
    virtual ~DeccelAmplitude();

    /**
    @brief Initializes the action with an inner action that has the amplitude property, and a duration time.
    @param action The pointer of inner action.
    @param duration The duration of the DeccelAmplitude action.
    @return If the initialization success, return true; otherwise, return false.
    */
    bool initWithAction(Action* action, float duration);

protected:
    float _rate;
    ActionInterval* _other;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(DeccelAmplitude);
};

/**
 @brief StopGrid action.
 @warning Don't call this action if another grid action is active.
 Call if you want to remove the grid effect. Example:
 @code
 Sequence::create(Lens3D::create(...), StopGrid::create(), nullptr);
 @endcode
 */
class AX_DLL StopGrid : public ActionInstant
{
public:
    /**
    @brief Create a StopGrid Action.
    @return Return a pointer of StopGrid. When the creation failed, return nil.
    */
    static StopGrid* create();

    // Overrides
    virtual void startWithTarget(Node* target) override;
    virtual StopGrid* clone() const override;
    virtual StopGrid* reverse() const override;

    StopGrid() {}
    virtual ~StopGrid() {}

protected:
    NodeGrid* _gridNodeTarget;

    void cacheTargetAsGridNode();

private:
    AX_DISALLOW_COPY_AND_ASSIGN(StopGrid);
};

/**
@brief ReuseGrid action.
*/
class AX_DLL ReuseGrid : public ActionInstant
{
public:
    /**
    @brief Create an action with the number of times that the current grid will be reused.
    @param times Specify times the grid will be reused.
    @return Return a pointer of ReuseGrid. When the creation failed, return nil.
    */
    static ReuseGrid* create(int times);

    // Override
    virtual void startWithTarget(Node* target) override;
    virtual ReuseGrid* clone() const override;
    virtual ReuseGrid* reverse() const override;

    ReuseGrid() {}
    virtual ~ReuseGrid() {}

    /**
    @brief Initializes an action with the number of times that the current grid will be reused.
    @param times Specify times the grid will be reused.
    @return If the initialization success, return true; otherwise, return false.
    */
    bool initWithTimes(int times);

protected:
    NodeGrid* _gridNodeTarget;

    void cacheTargetAsGridNode();

    int _times;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(ReuseGrid);
};

// end of actions group
/// @}

NS_AX_END

#endif  // __ACTION_CCGRID_ACTION_H__
