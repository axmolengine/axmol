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
#include "2d/Camera.h"
#include "2d/CameraBackgroundBrush.h"
#include "platform/GLView.h"
#include "2d/Scene.h"
#include "renderer/Renderer.h"
#include "renderer/QuadCommand.h"

NS_AX_BEGIN

Camera* Camera::_visitingCamera = nullptr;
Viewport Camera::_defaultViewport;

// start static methods

Camera* Camera::create()
{
    Camera* camera = new Camera();
    camera->initDefault();
    camera->autorelease();

    return camera;
}

Camera* Camera::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    auto ret = new Camera();
    ret->initPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
    ret->autorelease();
    return ret;
}

Camera* Camera::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
    auto ret = new Camera();
    ret->initOrthographic(zoomX, zoomY, nearPlane, farPlane);
    ret->autorelease();
    return ret;
}

Camera* Camera::getDefaultCamera()
{
    // camera nullptr scene init fix #690

    auto scene = Director::getInstance()->getRunningScene();

    AXASSERT(scene, "Scene is not done initializing, please use this->_defaultCamera instead.");

    return scene->getDefaultCamera();
}

const Viewport& Camera::getDefaultViewport()
{
    return _defaultViewport;
}
void Camera::setDefaultViewport(const Viewport& vp)
{
    _defaultViewport = vp;
}

const Camera* Camera::getVisitingCamera()
{
    return _visitingCamera;
}

// end static methods

Camera::Camera()
    : _eyeZdistance(1)
    , _zoomFactor(1)
    , _nearPlane(-1024)
    , _farPlane(1024)
    , _zoomFactorNearPlane(10)
    , _zoomFactorFarPlane(1024)
{
    // minggo comment
    // _frustum.setClipZ(true);
}

Camera::~Camera()
{
    AX_SAFE_RELEASE(_clearBrush);
}

const Mat4& Camera::getProjectionMatrix() const
{
    return _projection;
}
const Mat4& Camera::getViewMatrix() const
{
    Mat4 viewInv(getNodeToWorldTransform());
    static int count = sizeof(float) * 16;
    if (memcmp(viewInv.m, _viewInv.m, count) != 0)
    {
        _viewProjectionDirty = true;
#if defined(AX_ENABLE_3D)
        _frustumDirty        = true;
#endif
        _viewInv             = viewInv;
        _view                = viewInv.getInversed();
    }
    return _view;
}
void Camera::lookAt(const Vec3& lookAtPos, const Vec3& up)
{
    Vec3 upv = up;
    upv.normalize();
    Vec3 zaxis;
    Vec3::subtract(this->getPosition3D(), lookAtPos, &zaxis);
    zaxis.normalize();

    Vec3 xaxis;
    Vec3::cross(upv, zaxis, &xaxis);
    xaxis.normalize();

    Vec3 yaxis;
    Vec3::cross(zaxis, xaxis, &yaxis);
    yaxis.normalize();
    Mat4 rotation;

    rotation.m[0] = xaxis.x;
    rotation.m[1] = xaxis.y;
    rotation.m[2] = xaxis.z;
    rotation.m[3] = 0;

    rotation.m[4]  = yaxis.x;
    rotation.m[5]  = yaxis.y;
    rotation.m[6]  = yaxis.z;
    rotation.m[7]  = 0;

    rotation.m[8]  = zaxis.x;
    rotation.m[9]  = zaxis.y;
    rotation.m[10] = zaxis.z;
    rotation.m[11] = 0;

    Quaternion quaternion;
    Quaternion::createFromRotationMatrix(rotation, &quaternion);
    quaternion.normalize();
    setRotationQuat(quaternion);
}

const Mat4& Camera::getViewProjectionMatrix() const
{
    getViewMatrix();
    if (_viewProjectionDirty)
    {
        _viewProjectionDirty = false;
        Mat4::multiply(_projection, _view, &_viewProjection);
    }

    return _viewProjection;
}

void Camera::setAdditionalProjection(const Mat4& mat)
{
    _projection = mat * _projection;
    getViewProjectionMatrix();
}

void Camera::initDefault()
{
    auto& size = _director->getWinSize();
    switch (_director->getProjection())
    {
        case Director::Projection::_2D:
        {
            _fieldOfView = 60.0F;
            _nearPlane   = -1024.0F;
            _farPlane    = 1024.0F;
            initOrthographic(size.width, size.height, _nearPlane, _farPlane);
            setPosition3D(Vec3(size.width / 2.0F, size.height / 2.0F, 0.f));
            setRotation3D(Vec3(0.f, 0.f, 0.f));
            break;
        }

        case Director::Projection::_3D:
        {
            float zeye   = _director->getZEye();
            _fieldOfView = 60.0F;
            _nearPlane   = 0.5F;
            _farPlane    = zeye + size.height / 2.0f;
            initPerspective(_fieldOfView, (float)size.width / size.height, _nearPlane, _farPlane);
            Vec3 eye(size.width / 2.0f, size.height / 2.0f, zeye), center(size.width / 2.0f, size.height / 2.0f, 0.0f),
                up(0.0f, 1.0f, 0.0f);
            setPosition3D(eye);
            lookAt(center, up);
            _eyeZdistance = eye.z;
            break;
        }
    }

    setDepth(0);
	
    if (_zoomFactor != 1.0F)
        applyZoom();
}

void Camera::updateTransform()
{
    auto& size = _director->getWinSize();
    // create default camera
    switch (_director->getProjection())
    {
        case Director::Projection::_2D:
        {
            initOrthographic(size.width, size.height, _nearPlane, _farPlane);
            break;
        }

        case Director::Projection::_3D:
        {
            float zeye = _director->getZEye();
            initPerspective(_fieldOfView, (float)size.width / size.height, _nearPlane, _farPlane);
            Vec3 eye(size.width / 2.0f, size.height / 2.0f, zeye), center(size.width / 2.0f, size.height / 2.0f, 0.0f),
                up(0.0f, 1.0f, 0.0f);
            _eyeZdistance = eye.z;
            break;
        }
    }
}

bool Camera::initPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    _fieldOfView        = fieldOfView;
    _nearPlane          = nearPlane;
    _farPlane           = farPlane;

    if (_zoomFactorFarPlane == 1024)
        _zoomFactorFarPlane = farPlane;
    if (_zoomFactorNearPlane == 10)
        _zoomFactorNearPlane = nearPlane;

    Mat4::createPerspective(_fieldOfView, aspectRatio, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
#if defined(AX_ENABLE_3D)
    _frustumDirty = true;
#endif

    return true;
}

bool Camera::initOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
    _zoom[0]   = zoomX;
    _zoom[1]   = zoomY;
    _nearPlane = nearPlane;
    _farPlane  = farPlane;
    Mat4::createOrthographic(_zoom[0] * _zoomFactor, _zoom[1] * _zoomFactor, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
#if defined(AX_ENABLE_3D)
    _frustumDirty = true;
#endif

    return true;
}

Vec2 Camera::project(const Vec3& src) const
{
    Vec2 screenPos;

    auto viewport = _director->getWinSize();
    Vec4 clipPos;
    getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);

    AXASSERT(clipPos.w != 0.0f, "clipPos.w can't be 0.0f!");
    float ndcX = clipPos.x / clipPos.w;
    float ndcY = clipPos.y / clipPos.w;

    screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
    screenPos.y = (1.0f - (ndcY + 1.0f) * 0.5f) * viewport.height;
    return screenPos;
}

Vec2 Camera::projectGL(const Vec3& src) const
{
    Vec2 screenPos;

    auto viewport = _director->getWinSize();
    Vec4 clipPos;
    getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);

    if (clipPos.w == 0.0f)
        AXLOGW("WARNING: Camera's clip position w is 0.0! a black screen should be expected.");

    float ndcX = clipPos.x / clipPos.w;
    float ndcY = clipPos.y / clipPos.w;

    screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
    screenPos.y = (ndcY + 1.0f) * 0.5f * viewport.height;
    return screenPos;
}

Vec3 Camera::unproject(const Vec3& src) const
{
    Vec3 dst;
    unproject(_director->getWinSize(), &src, &dst);
    return dst;
}

Vec3 Camera::unprojectGL(const Vec3& src) const
{
    Vec3 dst;
    unprojectGL(_director->getWinSize(), &src, &dst);
    return dst;
}

void Camera::unproject(const Vec2& viewport, const Vec3* src, Vec3* dst) const
{
    AXASSERT(src && dst, "vec3 can not be null");

    Vec4 screen(src->x / viewport.width, ((viewport.height - src->y)) / viewport.height, src->z, 1.0f);
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;

    getViewProjectionMatrix().getInversed().transformVector(screen, &screen);
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }

    dst->set(screen.x, screen.y, screen.z);
}

void Camera::unprojectGL(const Vec2& viewport, const Vec3* src, Vec3* dst) const
{
    AXASSERT(src && dst, "vec3 can not be null");

    Vec4 screen(src->x / viewport.width, src->y / viewport.height, src->z, 1.0f);
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;

    getViewProjectionMatrix().getInversed().transformVector(screen, &screen);
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }

    dst->set(screen.x, screen.y, screen.z);
}

#if defined(AX_ENABLE_3D)
bool Camera::isVisibleInFrustum(const AABB* aabb) const
{
    if (_frustumDirty)
    {
        _frustum.initFrustum(this);
        _frustumDirty = false;
    }
    return !_frustum.isOutOfFrustum(*aabb);
}
#endif

float Camera::getDepthInView(const Mat4& transform) const
{
    Mat4 camWorldMat    = getNodeToWorldTransform();
    const Mat4& viewMat = camWorldMat.getInversed();
    float depth = -(viewMat.m[2] * transform.m[12] + viewMat.m[6] * transform.m[13] + viewMat.m[10] * transform.m[14] +
                    viewMat.m[14]);
    return depth;
}

void Camera::setDepth(int8_t depth)
{
    if (_depth != depth)
    {
        _depth = depth;
        if (_scene)
        {
            // notify scene that the camera order is dirty
            _scene->setCameraOrderDirty();
        }
    }
}

void Camera::setZoom(float factor)
{
    _zoomFactor = factor;
    applyZoom();
}

void Camera::applyZoom()
{
    switch (_director->getProjection())
    {
    case ax::Director::Projection::_2D:
    {
        Mat4::createOrthographic(_zoom[0] * _zoomFactor, _zoom[1] * _zoomFactor, _nearPlane, _farPlane, &_projection);
        break;
    }
    case ax::Director::Projection::_3D:
    {
        // Push the far plane farther the more we zoom out.
        if (_zoomFactorFarPlane * _zoomFactor > _farPlane)
        {
            _farPlane = _zoomFactorFarPlane * _zoomFactor;
            Camera::updateTransform();
        }

        // Push the near plane closer the more we zoom in.
        if (_zoomFactorNearPlane * _zoomFactor < _nearPlane)
        {
            _nearPlane = _zoomFactorNearPlane * _zoomFactor;
            Camera::updateTransform();
        }

        this->setPositionZ(_eyeZdistance * _zoomFactor);
        break;
    }
    }
}

void Camera::onEnter()
{
    if (_scene == nullptr)
    {
        auto scene = getScene();
        if (scene)
        {
            setScene(scene);
        }
    }
    Node::onEnter();
}

void Camera::onExit()
{
    // remove this camera from scene
    setScene(nullptr);
    Node::onExit();
}

void Camera::setScene(Scene* scene)
{
    if (_scene != scene)
    {
        // remove old scene
        if (_scene)
        {
            auto& cameras = _scene->_cameras;
            auto it       = std::find(cameras.begin(), cameras.end(), this);
            if (it != cameras.end())
                cameras.erase(it);
            _scene = nullptr;
        }
        // set new scene
        if (scene)
        {
            _scene        = scene;
            auto& cameras = _scene->_cameras;
            auto it       = std::find(cameras.begin(), cameras.end(), this);
            if (it == cameras.end())
            {
                _scene->_cameras.emplace_back(this);
                // notify scene that the camera order is dirty
                _scene->setCameraOrderDirty();
            }
        }
    }
}

void Camera::clearBackground()
{
    if (_clearBrush)
    {
        _clearBrush->drawBackground(this);
    }
}

void Camera::apply()
{
    _viewProjectionUpdated = _transformUpdated;
    applyViewport();
}

void Camera::applyViewport()
{
    _director->getRenderer()->setViewPort(_defaultViewport.x, _defaultViewport.y, _defaultViewport.width,
                                          _defaultViewport.height);
}

int Camera::getRenderOrder() const
{
    int result(0);
    result = 127 << 8;
    result += _depth;
    return result;
}

void Camera::setFOV(float fieldOfView)
{
    _fieldOfView = fieldOfView;
    Camera::updateTransform();
}

void Camera::setFarPlane(float farPlane)
{
    _farPlane = farPlane;
    Camera::updateTransform();
}

void Camera::setNearPlane(float nearPlane)
{
    _nearPlane = nearPlane;
    Camera::updateTransform();
}

void Camera::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    _viewProjectionUpdated = _transformUpdated;
    return Node::visit(renderer, parentTransform, parentFlags);
}

void Camera::setBackgroundBrush(CameraBackgroundBrush* clearBrush)
{
    AX_SAFE_RETAIN(clearBrush);
    AX_SAFE_RELEASE(_clearBrush);
    _clearBrush = clearBrush;
}

bool Camera::isBrushValid()
{
    return _clearBrush != nullptr && _clearBrush->isValid();
}

NS_AX_END
