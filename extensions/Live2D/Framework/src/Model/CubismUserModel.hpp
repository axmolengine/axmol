/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Effect/CubismPose.hpp"
#include "Effect/CubismEyeBlink.hpp"
#include "Effect/CubismBreath.hpp"
#include "Effect/CubismLook.hpp"
#include "Math/CubismModelMatrix.hpp"
#include "Math/CubismTargetPoint.hpp"
#include "Model/CubismMoc.hpp"
#include "Model/CubismModel.hpp"
#include "Motion/CubismMotionManager.hpp"
#include "Motion/CubismExpressionMotion.hpp"
#include "Physics/CubismPhysics.hpp"
#include "Rendering/CubismRenderer.hpp"
#include "Model/CubismModelUserData.hpp"
#include "Motion/CubismExpressionMotionManager.hpp"
#include "Motion/CubismUpdateScheduler.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * Base for models actually used by thegit a user.
 */
class CubismUserModel
{
public:
    /**
     * Constructor
     */
    CubismUserModel();

    /**
     * Destructor
     */
    virtual ~CubismUserModel();

    /**
     * Checks if it is initialized.
     *
     * @return true if initialized; otherwise false.
     */
    virtual csmBool         IsInitialized();

    /**
     * Sets the initialization state.
     *
     * @param v Initialization state. true if initialized.
     */
    virtual void            IsInitialized(csmBool v);

    /**
     * Checks if it is updated.
     *
     * @return true if updated; otherwise false.
     */
    virtual csmBool         IsUpdating();

    /**
     * Sets the update state.
     *
     * @param v Update state. true if updated.
     */
    virtual void            IsUpdating(csmBool v);

    /**
     * Set the offscreen buffer size.
     *
     * @param width Width of the offscreen buffer
     * @param height Height of the offscreen buffer
     */
    void SetRenderTargetSize(csmUint32 width, csmUint32 height);

    /**
     * Sets the information during mouse dragging.
     *
     * @param x X position of the mouse cursor during dragging
     * @param y Y position of the mouse cursor during dragging
     */
    virtual void            SetDragging(csmFloat32 x, csmFloat32 y);

    /**
     * Sets the acceleration information.
     *
     * @param x Acceleration in the X-axis direction
     * @param y Acceleration in the Y-axis direction
     * @param z Acceleration in the Z-axis direction
     */
    virtual void            SetAcceleration(csmFloat32 x, csmFloat32 y, csmFloat32 z);

    /**
     * Returns the matrix applied to the model.
     *
     * @return Matrix
     */
    CubismModelMatrix*      GetModelMatrix() const;

    /**
     * Sets the opacity.
     *
     * @param a Opacity
     */
    virtual void            SetOpacity(csmFloat32 a);

    /**
     * Returns the opacity.
     *
     * @return Opacity
     */
    virtual csmFloat32      GetOpacity();

    /**
     * Loads the model from a MOC3 file.
     *
     * @param buffer Buffer where the MOC3 file is loaded
     * @param size Number of bytes in the buffer
     */
    virtual void            LoadModel(const csmByte* buffer, csmSizeInt size, csmBool shouldCheckMocConsistency = false);

    /**
     * Loads motion from a motion file.
     * If a fade value is defined in model3.json, the fade value defined in motion3.json will be overwritten.
     *
     * @param buffer Buffer where the motion file is loaded
     * @param size Number of bytes in the buffer
     * @param name Name of the motion
     * @param onFinishedMotionHandler Callback function when motion playback finishes
     * @param modelSetting Model setting information
     * @param group – Name to the desired Motion Group
     * @param index – Index to the desired Motion
     *
     * @return Instance of the motion class
     */
    virtual ACubismMotion*  LoadMotion(const csmByte* buffer, csmSizeInt size, const csmChar* name,
                                       ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL, ACubismMotion::BeganMotionCallback onBeganMotionHandler = NULL,
                                       ICubismModelSetting* modelSetting = NULL, const csmChar* group = NULL, const csmInt32 index = -1, csmBool shouldCheckMotionConsistency = false);

    /**
     * Loads expression from an expression configuration file.
     *
     * @param buffer Buffer where the expression configuration file is loaded
     * @param size Number of bytes in the buffer
     * @param name Name of the expression
     *
     * @return Instance of the expression motion class
     */
    virtual ACubismMotion*   LoadExpression(const csmByte* buffer, csmSizeInt size, const csmChar* name);

    /**
     * Loads pose from a pose configuration file.
     *
     * @param buffer Buffer where the pose configuration file is loaded
     * @param size Number of bytes in the buffer
     */
    virtual void            LoadPose(const csmByte* buffer, csmSizeInt size);

    /**
     * Loads physics from a physics configuration file.
     *
     * @param buffer Buffer where the physics configuration file is loaded
     * @param size Number of bytes in the buffer
     */
    virtual void            LoadPhysics(const csmByte* buffer, csmSizeInt size);

    /**
     * Loads user data from a user data file.
     *
     * @param buffer Buffer where the user data file is loaded
     * @param size Number of bytes in the buffer
     */
    virtual void            LoadUserData(const csmByte* buffer, csmSizeInt size);

    /**
     * Returns whether the hit test of a drawable object hits at the specified position.
     *
     * @param drawableId ID of the drawable object to test
     * @param pointX X position
     * @param pointY Y position
     *
     * @return true if the hit test of the drawable object hits at the specified position; otherwise false.
     */
    virtual csmBool         IsHit(CubismIdHandle drawableId, csmFloat32 pointX, csmFloat32 pointY);

    /**
     * Returns the model.
     *
     * @return Instance of the model
     */
    CubismModel*            GetModel() const;

    /**
     * Returns the version of the unload MOC file.
     *
     * @param buffer Buffer where the MOC3 file is loaded
     * @param size Number of bytes in the buffer
     *
     * @return Version
     */
    static Core::csmMocVersion GetMocVersionFromBuffer(const csmByte* buffer, csmSizeInt size);

    /**
     * Returns the renderer.
     *
     * @return Instance of the renderer
     */
    template<class T> T*    GetRenderer() { return dynamic_cast<T*>(_renderer); }

    /**
     * Makes the renderer.
     *
     * @param width Width of the buffer where the model is rendered
     * @param height Height of the buffer where the model is rendered
     * @param maskBufferCount Number of buffers used for masking
     */
    void CreateRenderer(csmUint32 width, csmUint32 height, csmInt32 maskBufferCount = 1);

    /**
     * Destroys the renderer.
     */
    void DeleteRenderer();

    /**
     * Handles the event when user data fires during motion playback.
     *
     * @param eventValue Event value of the user data that fired
     *
     * @note This function is intended to be overridden.<br>
     * If not overridden, it outputs to the log.
     */
    virtual void   MotionEventFired(const csmString& eventValue);

    /**
     * Callback function to receive user data events registered in the motion management class.
     *
     * @param caller Motion management class that handled the fired user data event
     * @param eventValue Event value of the fired user data
     * @param customData Arbitrary data
     *
     * @note Calls the `MotionEventFired` of the CubismUserModel subclass.
     */
    static void   CubismDefaultMotionEventCallback(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData);
protected:
    CubismMoc*              _moc;
    CubismModel*            _model;

    CubismUpdateScheduler _updateScheduler;

    CubismMotionManager*    _motionManager;
    CubismExpressionMotionManager*    _expressionManager;
    CubismEyeBlink*         _eyeBlink;
    CubismBreath*           _breath;
    CubismModelMatrix*      _modelMatrix;
    CubismPose*             _pose;
    CubismTargetPoint*      _dragManager;
    CubismPhysics*          _physics;
    CubismModelUserData*    _modelUserData;
    CubismLook*             _look;

    csmBool     _initialized;
    csmBool     _updating;
    csmFloat32  _opacity;
    csmFloat32  _lastLipSyncValue;
    csmFloat32  _accelerationX;
    csmFloat32  _accelerationY;
    csmFloat32  _accelerationZ;
    csmBool     _mocConsistency;
    csmBool     _motionConsistency;
    csmBool     _debugMode;

private:
    Rendering::CubismRenderer* _renderer;
};

}}}
