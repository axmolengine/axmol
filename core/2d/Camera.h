/****************************************************************************
Copyright (c) 2014-2016 Chukong Technologies Inc.
Copyright (c) 2017-2019 Xiamen Yaji Software Co., Ltd.
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

 Code based GamePlay3D's Camera: http://gameplay3d.org

 ****************************************************************************/
#pragma once

#include "2d/Node.h"
#if defined(AX_ENABLE_3D)
#include "3d/Frustum.h"
#endif
#include "renderer/QuadCommand.h"
#include "renderer/CustomCommand.h"
#include "base/Director.h"

namespace ax
{

class Scene;
class CameraBackgroundBrush;

/**
 * Note:
 * Scene creates a default camera. And the default camera mask of Node is 1, therefore it can be seen by the default
 * camera. During rendering the scene, it draws the objects seen by each camera in the added order except default
 * camera. The default camera is the last one being drawn with. It's usually a good idea to render 3D objects in a
 * separate camera. And set the 3d camera flag to CameraFlag::USER1 or anything else except DEFAULT. Dedicate The
 * DEFAULT camera for UI, because it is rendered at last. You can change the camera order to get different result when
 * depth test is not enabled. For each camera, transparent 3d sprite is rendered after opaque 3d sprite and other 2d
 * objects.
 */
enum class CameraFlag
{
    DEFAULT = 1,
    USER1   = 1 << 1,
    USER2   = 1 << 2,
    USER3   = 1 << 3,
    USER4   = 1 << 4,
    USER5   = 1 << 5,
    USER6   = 1 << 6,
    USER7   = 1 << 7,
    USER8   = 1 << 8,
};
/**
 * Defines a camera .
 */
class AX_DLL Camera : public Node
{
    friend class Scene;
    friend class Director;
    friend class EventDispatcher;

public:
    /**
     * Creates a perspective camera.
     *
     * @param fieldOfView The field of view for the perspective camera (normally in the range of 40-60 degrees).
     * @param aspectRatio The aspect ratio of the camera (normally the width of the viewport divided by the height of
     * the viewport).
     * @param nearPlane The near plane distance.
     * @param farPlane The far plane distance.
     */
    static Camera* createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
    /**
     * Creates an orthographic camera.
     *
     * @param zoomX The zoom factor along the X-axis of the orthographic projection (the width of the ortho projection).
     * @param zoomY The zoom factor along the Y-axis of the orthographic projection (the height of the ortho
     * projection).
     * @param nearPlane The near plane distance.
     * @param farPlane The far plane distance.
     */
    static Camera* createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane);

    /** create default camera, the camera type depends on Director::getProjection, the depth of the default camera is 0
     */
    static Camera* create();

    /**
     * Get the visiting camera , the visiting camera shall be set on Scene::render
     */
    static const Camera* getVisitingCamera();

    static const Viewport& getDefaultViewport();
    static void setDefaultViewport(const Viewport& vp);

    /**
     * Get the default camera of the current running scene.
     */
    static Camera* getDefaultCamera();

    /**get & set Camera flag*/
    CameraFlag getCameraFlag() const { return _cameraFlag; }
    void setCameraFlag(CameraFlag flag) { _cameraFlag = flag; }

    /**
     * Make Camera looks at target
     *
     * @param target The target camera is point at
     * @param up The up vector, usually it's Y axis
     */
    virtual void lookAt(const Vec3& target, const Vec3& up = Vec3::UNIT_Y);

    /**
     * Gets the camera's projection matrix.
     *
     * @return The camera projection matrix.
     */
    const Mat4& getProjectionMatrix() const;
    /**
     * Gets the camera's view matrix.
     *
     * @return The camera view matrix.
     */
    const Mat4& getViewMatrix() const;

    /**get view projection matrix*/
    const Mat4& getViewProjectionMatrix() const;

    /* convert the specified point in 3D world-space coordinates into the screen-space coordinates.
     *
     * Origin point at left top corner in screen-space.
     * @param src The world-space position.
     * @return The screen-space position.
     */
    Vec2 project(const Vec3& src) const;

    /* convert the specified point in 3D world-space coordinates into the GL-screen-space coordinates.
     *
     * Origin point at left bottom corner in GL-screen-space.
     * @param src The 3D world-space position.
     * @return The GL-screen-space position.
     */
    Vec2 projectGL(const Vec3& src) const;

    /**
     * Convert the specified point of screen-space coordinate into the 3D world-space coordinate.
     *
     * Origin point at left top corner in screen-space.
     * @param src The screen-space position.
     * @return The 3D world-space position.
     */
    Vec3 unproject(const Vec3& src) const;

    /**
     * Convert the specified point of GL-screen-space coordinate into the 3D world-space coordinate.
     *
     * Origin point at left bottom corner in GL-screen-space.
     * @param src The GL-screen-space position.
     * @return The 3D world-space position.
     */
    Vec3 unprojectGL(const Vec3& src) const;

    /**
     * Convert the specified point of screen-space coordinate into the 3D world-space coordinate.
     *
     * Origin point at left top corner in screen-space.
     * @param size The window size to use.
     * @param src  The screen-space position.
     * @param dst  The 3D world-space position.
     */
    void unproject(const Vec2& size, const Vec3* src, Vec3* dst) const;

    /**
     * Convert the specified point of GL-screen-space coordinate into the 3D world-space coordinate.
     *
     * Origin point at left bottom corner in GL-screen-space.
     * @param size The window size to use.
     * @param src  The GL-screen-space position.
     * @param dst  The 3D world-space position.
     */
    void unprojectGL(const Vec2& size, const Vec3* src, Vec3* dst) const;

#if defined(AX_ENABLE_3D)
    /**
     * Is this aabb visible in frustum
     */
    bool isVisibleInFrustum(const AABB* aabb) const;
#endif

    /**
     * Get object depth towards camera
     */
    float getDepthInView(const Mat4& transform) const;

    /**
     * set depth, camera with larger depth is drawn on top of camera with smaller depth, the depth of camera with
     * CameraFlag::DEFAULT is 0, user defined camera is -1 by default
     */
    void setDepth(int8_t depth);

    /**
     * get depth, camera with larger depth is drawn on top of camera with smaller depth, the depth of camera with
     * CameraFlag::DEFAULT is 0, user defined camera is -1 by default
     */
    int8_t getDepth() const { return _depth; }

    /**
     get rendered order
     */
    int getRenderOrder() const;

    /**
     * Gets the field of view of the camera if the projection mode is 3D.
     *
     * @since axmol-1.0.0b8
     */
    float getFOV() const { return _fieldOfView; }

    /**
     * Sets the field of view of the camera if the projection mode is 3D.
     *
     * @since axmol-1.0.0b8
     */
    void setFOV(float fov);

    /**
     * Gets the frustum's far plane.
     */
    float getFarPlane() const { return _farPlane; }

    /**
     * Sets the frustum's far plane.
     *
     * @since axmol-1.0.0b8
     */
    void setFarPlane(float farPlane);

    /**
     * Gets the frustum's near plane.
     */
    float getNearPlane() const { return _nearPlane; }

    /**
     * Gets the frustum's near plane.
     *
     * @since axmol-1.0.0b8
     */
    void setNearPlane(float nearPlane);

    /**
     * Gets the zoom multiplier of the camera.
     *
     * @since axmol-1.0.0b8
     */
    float getZoom() const { return _zoomFactor; }

    /**
     * Sets the zoom multiplier of the camera.
     * This is designed to be used with 2D views only.
     *
     * @param factor The zoom factor of the camera.
     *
     * @since axmol-1.0.0b8
     */
    void setZoom(float factor);

    /**
     Apply the zoom factor.
     *
     * @since axmol-1.0.0b8
     */
    void applyZoom();

    // override
    virtual void onEnter() override;
    virtual void onExit() override;

    /**
     Before rendering the scene with this camera, the background needs to be cleared.
     It will clear the depth buffer with max depth by default.
     Use setBackgroundBrush to modify this default behavior.
     */
    void clearBackground();
    /**
     Apply the FBO, RenderTargets and viewport.
     */
    void apply();

    /**
     * Whether or not the viewprojection matrix was updated last frame.
     * @return True if the viewprojection matrix was updated last frame.
     */
    bool isViewProjectionUpdated() const { return _viewProjectionUpdated; }

    /**
     * set the background brush. See CameraBackgroundBrush for more information.
     * @param clearBrush Brush used to clear the background
     */
    virtual void setBackgroundBrush(CameraBackgroundBrush* clearBrush);

    /**
     * Get clear brush
     */
    CameraBackgroundBrush* getBackgroundBrush() const { return _clearBrush; }

    virtual void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    bool isBrushValid();

    Camera();
    ~Camera();

    /**
     * Set the owner scene of the camera, this method shall not be invoked manually
     */
    void setScene(Scene* scene);

    /**set additional matrix for the projection matrix, it multiplies mat to projection matrix when called, used by
     * WP8*/
    void setAdditionalProjection(const Mat4& mat);

    /** Init default camera with director current projection,
    !!!Note: Must invoke this function again when director projection or winsize changed */
    void initDefault();

    /** Update camera transformations */
    void updateTransform() override;

    bool initPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
    bool initOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane);
    void applyViewport();

protected:
    static Camera* _visitingCamera;
    static Viewport _defaultViewport;

    //* Scene that owns this camera.
    Scene* _scene = nullptr;
    Mat4 _projection;
    mutable Mat4 _view;
    mutable Mat4 _viewInv;
    mutable Mat4 _viewProjection;

    Vec3 _up;
    float _fieldOfView                = 0.f;
    float _zoom[2]                    = {0.f};
    float _nearPlane                  = 0.f;
    float _farPlane                   = 0.f;
    mutable bool _viewProjectionDirty = true;
    bool _viewProjectionUpdated = false;  // Whether or not the viewprojection matrix was updated since the last frame.
    CameraFlag _cameraFlag      = CameraFlag::DEFAULT;  // camera flag
#if defined(AX_ENABLE_3D)
    mutable Frustum _frustum;                           // camera frustum
    mutable bool _frustumDirty = true;
#endif
    int8_t _depth = -1;  // camera depth, the depth of camera with CameraFlag::DEFAULT flag is 0 by default, a camera
                         // with larger depth is drawn on top of camera with smaller depth

    float _eyeZdistance; // Z eye projection distance for 2D in 3D projection.
    float _zoomFactor = 1.0F; // The zoom factor of the camera. 3D = (cameraZDistance * _zoomFactor), 2D = (cameraScale * _zoomFactor)
    float _zoomFactorFarPlane;
    float _zoomFactorNearPlane;

    CameraBackgroundBrush* _clearBrush = nullptr;  // brush used to clear the back ground
};

}
