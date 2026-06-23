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

#include "axmol/scene/Node.h"
#if defined(AX_ENABLE_3D)
#    include "axmol/3d/Frustum.h"
#    include "axmol/3d/Ray.h"
#endif
#include "axmol/renderer/QuadCommand.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/base/Director.h"

namespace ax
{

class Scene;
class RenderView;
class RenderTexture;
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
    friend class NodeGrid;

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

    /**
     * Creates a 2D orthographic camera matching Director::getCanvasSize().
     * This is the preferred helper for temporary cameras that render regular scene/canvas content.
     */
    static Camera* createCanvasOrthographic(float nearPlane, float farPlane);

    /** create default camera, the camera type depends on Director::getProjection, the depth of the default camera is 0
     */
    static Camera* create();

    /**
     * Get the visiting camera , the visiting camera shall be set on Scene::render
     */
    static const Camera* getVisitingCamera() { return _visitingCamera; }

    /**
     * Set the visiting camera for draw context. Used by engine internals
     * for offscreen capture (Transition, Utils, etc.)
     */
    static void setVisitingCamera(Camera* camera) { _visitingCamera = camera; }

    /**
     * Get the view-projection matrix of the current draw context.
     */
    static const Mat4& getVisitingViewProjectionMatrix();

    static const Viewport& getDefaultViewport();
    static void setDefaultViewport(const Viewport& vp);

    /**
     * @brief Updates the view-projection update state from the camera transform state.
     *
     * This is used by renderers that manage render targets and viewports themselves
     * and therefore cannot call Camera::apply(), but still need the camera
     * view-projection update flag to match the current transform state.
     */
    void updateViewProjectionState() { _viewProjectionUpdated = _transformUpdated; }

    /**
     * Get the default camera of the current running scene.
     */
    static Camera* getDefaultCamera();

    /**get & set Camera flag*/
    CameraFlag getCameraFlag() const { return _cameraFlag; }
    void setCameraFlag(CameraFlag flag) { _cameraFlag = flag; }

    /**
     * Set a render texture as the camera's offscreen render target.
     * When set, Scene::render() will automatically bind this render target
     * before visiting the scene with this camera, and restore the previous
     * render target afterwards.
     *
     * @param target The render texture to render into, or nullptr to restore default behavior.
     */
    void setTargetTexture(RenderTexture* target);

    /**
     * Get the render texture currently bound as this camera's offscreen target.
     * @return The bound render texture, or nullptr if rendering to screen.
     */
    RenderTexture* getTargetTexture() const { return _targetTexture; }

    /**
     * Make Camera looks at target
     *
     * @param target The target camera is point at
     * @param up The up vector, usually it's Y axis
     */
    virtual void lookAt(const Vec3& target, const Vec3& up = Vec3::yAxis);

    /**
     * Gets the camera's projection matrix.
     *
     * @return The camera projection matrix.
     */
    const Mat4& getProjectionMatrix() const;
    /**
     * Sets the camera's projection matrix.
     *
     * @param mat The new projection matrix.
     */
    void setProjectionMatrix(const Mat4& mat);
    /**
     * Gets the camera's view matrix.
     *
     * @return The camera view matrix.
     */
    const Mat4& getViewMatrix() const;

    /**get view projection matrix*/
    const Mat4& getViewProjectionMatrix() const;

    /** Get the scene that currently owns this camera for rendering. */
    Scene* getOwnerScene() const { return _scene; }

#if defined(AX_ENABLE_3D)
    /**
     * @brief Converts a 2D screen point into a 3D ray in world space.
     *
     * This function serves as the core gateway for 3D raycast picking. It unprojects
     * a 2D screen coordinate to the near plane (Z=0) and far plane (Z=1) in world space
     * using the current camera's viewport, view matrix, and projection matrix to construct a 3D ray.
     *
     * @param screenPoint The 2D screen coordinate. Must comply with the new input system
     * specification: origin at top-left, with the Y-axis increasing downwards.
     *
     * @return A Ray structure representing the constructed 3D ray.
     * - Ray.origin: The starting point of the ray, located on the near clipping plane in world space.
     * - Ray.direction: The normalized direction vector of the ray.
     *
     * @note The function automatically handles the Y-axis viewport coordinate conversion from
     * the new system's top-left origin (Y-down) to the underlying graphics API's (OpenGL/Vulkan)
     * bottom-left origin (Y-up). Callers do not need to manually flip the Y-axis.
     *
     * @see Director::screenToWorld
     */
    Ray screenToRay(const Vec2& screenPoint) const;
#endif

    /**
     * Convert the specified point in 3D world-space coordinates into the screen-space coordinates.
     *
     * The screen-space coordinate system has its origin point at the left top corner.
     * This corresponds to the native platform/window input coordinates.
     *
     * @param src The 3D world-space position.
     * @return The screen-space position (left-top origin).
     */
    Vec2 projectWorldToScreen(const Vec3& src) const;

    /**
     * Convert the specified point of screen-space coordinate into the 3D world-space coordinate.
     *
     * The screen-space coordinate system has its origin point at the left top corner.
     *
     * @param src The screen-space position (left-top origin).
     * @return The 3D world-space position.
     */
    Vec3 deprojectScreenToWorld(const Vec3& src) const;

    /**
     * @brief Converts a 3D world-space coordinate into the 2D legacy Canvas coordinate space.
     *
     * This function maps a 3D position into the logical design resolution space used by the
     * 2D UI hierarchy (e.g., Node, Widget, Sprite). The returned coordinate system has its
     * origin (0, 0) at the **bottom-left corner** of the design resolution canvas.
     *
     * @note This replaces the legacy 'projectWorldToViewport' which was a misnomer, as it
     * scales against Director::getCanvasSize() rather than the actual RHI physical viewport.
     *
     * @param src The 3D world-space position to be projected.
     * @return The 2D logical canvas-space position (bottom-left origin).
     */
    Vec2 projectWorldToCanvas(const Vec3& src) const;

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
    void onEnter() override;
    void onExit() override;

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

    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

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
    bool initCanvasOrthographic(float nearPlane, float farPlane);
    void applyViewport();

    /**
     * Checks whether a 2D world/canvas point hits a local content rectangle.
     *
     * The input point is interpreted as a point in world/canvas XY space, not
     * native screen space. The function builds a line from (pt.x, pt.y, -1) to
     * (pt.x, pt.y, 1), transforms it into node local space, intersects it with
     * the local z = 0 plane, and checks whether the intersection lies inside rect.
     *
     * @param pt   Point in 2D world/canvas coordinates.
     * @param w2l  World-to-local transform.
     * @param rect Rectangle in local space.
     * @param p    Optional local-space intersection point.
     */
    bool isWorldPointInRect(const Vec2& pt, const Mat4& w2l, const Rect& rect, Vec3* p) const;
    bool isWorldPointInRect(const Vec2& pt, const Mat4& w2l, const Rect& rect) const
    {
        return isWorldPointInRect(pt, w2l, rect, nullptr);
    }

protected:
    static Camera* _visitingCamera;
    static Viewport _defaultViewport;

    bool initOrthographicView(const Vec2& size, float nearPlane, float farPlane);

    RenderViewCore* _renderView{nullptr};

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
    mutable Frustum _frustum;  // camera frustum
    mutable bool _frustumDirty = true;
#endif
    int8_t _depth = -1;  // camera depth, the depth of camera with CameraFlag::DEFAULT flag is 0 by default, a camera
                         // with larger depth is drawn on top of camera with smaller depth

    float _eyeZdistance;  // Z eye projection distance for 2D in 3D projection.
    float _zoomFactor =
        1.0F;  // The zoom factor of the camera. 3D = (cameraZDistance * _zoomFactor), 2D = (cameraScale * _zoomFactor)
    float _zoomFactorFarPlane;
    float _zoomFactorNearPlane;

    CameraBackgroundBrush* _clearBrush = nullptr;  // brush used to clear the back ground
    RenderTexture* _targetTexture      = nullptr;  // optional offscreen render target
};

}  // namespace ax
