/****************************************************************************
 Copyright (c) 2016 Google Inc.
 Copyright (c) 2016 Chukong Technologies Inc.
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

// IMPORTANT
// For iOS/Mac, this file is treated as an "Objective-C++" file.
// To change this behaviour, use the File Inspector from Xcode

#include "axmol/vr/VRGenericHeadTracker.h"

#include <cmath>
#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/Device.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
#    import <CoreMotion/CoreMotion.h>
#    import <UIKit/UIKit.h>
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include <jni.h>
#    include "axmol/platform/android/jni/JniHelper.h"
#    define AXMOLENGINE_JAVA_CLASS_NAME "dev.axmol.lib.AxmolEngine"
#endif

namespace ax::experimental
{

//////

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
static Mat4 matrixFromRotationMatrix(const CMRotationMatrix& rotationMatrix)
{
    return Mat4(rotationMatrix.m11, rotationMatrix.m21, rotationMatrix.m31, 0.0f,

                rotationMatrix.m12, rotationMatrix.m22, rotationMatrix.m32, 0.0f,

                rotationMatrix.m13, rotationMatrix.m23, rotationMatrix.m33, 0.0f,

                0.0f, 0.0f, 0.0f, 1.0f);
}

#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

// getRotationMatrix taken from Android's SensorManager.java
static Mat4 getRotationMatrix(const Vec3& gravity, const Vec3& geomagnetic)
{
    float Ax = gravity.x;
    float Ay = gravity.y;
    float Az = gravity.z;

    const float normsqA                = (Ax * Ax + Ay * Ay + Az * Az);
    const float g                      = 9.81f;
    const float freeFallGravitySquared = 0.01f * g * g;
    if (normsqA < freeFallGravitySquared)
    {
        // gravity less than 10% of normal value
        return Mat4::IDENTITY;
    }

    const float Ex    = geomagnetic.x;
    const float Ey    = geomagnetic.y;
    const float Ez    = geomagnetic.z;
    float Hx          = Ey * Az - Ez * Ay;
    float Hy          = Ez * Ax - Ex * Az;
    float Hz          = Ex * Ay - Ey * Ax;
    const float normH = std::sqrt(Hx * Hx + Hy * Hy + Hz * Hz);

    if (normH < 0.1f)
    {
        // device is close to free fall (or in space?), or close to
        // magnetic north pole. Typical values are  > 100.
        return Mat4::IDENTITY;
    }
    const float invH = 1.0f / normH;
    Hx *= invH;
    Hy *= invH;
    Hz *= invH;
    const float invA = 1.0f / std::sqrt(Ax * Ax + Ay * Ay + Az * Az);
    Ax *= invA;
    Ay *= invA;
    Az *= invA;
    const float Mx = Ay * Hz - Az * Hy;
    const float My = Az * Hx - Ax * Hz;
    const float Mz = Ax * Hy - Ay * Hx;

    return Mat4(Hx, Mx, Ax, 0, Hy, My, Ay, 0, Hz, Mz, Az, 0, 0, 0, 0, 1);
}

static Vec3 lowPass(const Vec3& input, const Vec3& prev)
{
    // if ALPHA = 1 OR 0, no filter applies.
    static const float ALPHA = 0.12f;
    return Vec3(prev.x + ALPHA * (input.x - prev.x), prev.y + ALPHA * (input.y - prev.y),
                prev.z + ALPHA * (input.z - prev.z));
}

#endif  // (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)

static Mat4 getRotateEulerMatrix(float x, float y, float z)
{
    x *= static_cast<float>(M_PI) / 180.0f;
    y *= static_cast<float>(M_PI) / 180.0f;
    z *= static_cast<float>(M_PI) / 180.0f;
    float cx   = std::cos(x);
    float sx   = std::sin(x);
    float cy   = std::cos(y);
    float sy   = std::sin(y);
    float cz   = std::cos(z);
    float sz   = std::sin(z);
    float cxsy = cx * sy;
    float sxsy = sx * sy;
    Mat4 matrix;
    matrix.m[0]  = cy * cz;
    matrix.m[1]  = -cy * sz;
    matrix.m[2]  = sy;
    matrix.m[3]  = 0.0f;
    matrix.m[4]  = cxsy * cz + cx * sz;
    matrix.m[5]  = -cxsy * sz + cx * cz;
    matrix.m[6]  = -sx * cy;
    matrix.m[7]  = 0.0f;
    matrix.m[8]  = -sxsy * cz + sx * sz;
    matrix.m[9]  = sxsy * sz + sx * cz;
    matrix.m[10] = cx * cy;
    matrix.m[11] = 0.0f;
    matrix.m[12] = 0.0f;
    matrix.m[13] = 0.0f;
    matrix.m[14] = 0.0f;
    matrix.m[15] = 1.0f;
    return matrix;
}

#endif

VRGenericHeadTracker::VRGenericHeadTracker() : _localPosition(Vec3::ZERO)
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
    _motionMgr = [[CMMotionManager alloc] init];
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    startTracking();
#endif
}

VRGenericHeadTracker::~VRGenericHeadTracker()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS) || \
    (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    stopTracking();
#endif

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
    [(CMMotionManager*)_motionMgr release];
#endif
}

void VRGenericHeadTracker::startTracking()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
    CMMotionManager* motionMgr = (CMMotionManager*)_motionMgr;
    if (motionMgr.isDeviceMotionAvailable && !motionMgr.isDeviceMotionActive)
    {
        [motionMgr startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    }

    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    if (orientation == UIInterfaceOrientationLandscapeLeft)
    {
        _deviceToDisplay = getRotateEulerMatrix(0.f, 0.f, 90.f);
    }
    else if (orientation == UIInterfaceOrientationLandscapeRight)
    {
        _deviceToDisplay = getRotateEulerMatrix(0.f, 0.f, -90.f);
    }

    // the inertial reference frame has z up and x forward, while the world has z out and x right
    _worldToInertialReferenceFrame = getRotateEulerMatrix(-90.f, 0.f, 90.f);
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    _deviceToDisplay               = getRotateEulerMatrix(0.f, 0.f, -90.f);
    _worldToInertialReferenceFrame = getRotateEulerMatrix(-90.f, 0.f, 90.f);
    JniHelper::callStaticVoidMethod(AXMOLENGINE_JAVA_CLASS_NAME, "enableAccelerometer");
    JniHelper::callStaticVoidMethod(AXMOLENGINE_JAVA_CLASS_NAME, "enableCompass");
#endif
}

void VRGenericHeadTracker::stopTracking()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
    [(CMMotionManager*)_motionMgr stopDeviceMotionUpdates];
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    Device::setAccelerometerEnabled(false);
#endif
}

Vec3 VRGenericHeadTracker::getLocalPosition()
{
    return _localPosition;
}

Mat4 VRGenericHeadTracker::getLocalRotation()
{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) && !defined(AX_TARGET_OS_TVOS)
    CMMotionManager* motionMgr = (CMMotionManager*)_motionMgr;
    CMDeviceMotion* motion     = motionMgr.deviceMotion;

    if (motion)
    {
        CMRotationMatrix rotationMatrix      = motion.attitude.rotationMatrix;
        Mat4 inertialReferenceFrameToDevice0 = matrixFromRotationMatrix(rotationMatrix);  // note the matrix inversion
        Mat4 inertialReferenceFrameToDevice  = inertialReferenceFrameToDevice0.getTransposed();
        Mat4 worldToDevice                   = inertialReferenceFrameToDevice * _worldToInertialReferenceFrame;
        return _deviceToDisplay * worldToDevice;
    }
    // bug!
    return Mat4::IDENTITY;

#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    static Vec3 prevAccel   = Vec3(0, 0, 0);
    static Vec3 prevCompass = Vec3(0, 0, 0);

    Vec3 accel   = JniHelper::callStaticVec3Method(AXMOLENGINE_JAVA_CLASS_NAME, "getAccelValue");
    Vec3 compass = JniHelper::callStaticVec3Method(AXMOLENGINE_JAVA_CLASS_NAME, "getCompassValue");

    //    AXLOGD("accel: {}, {}, {}.... compass: {}, {}, {}", accel.x, accel.y, accel.z, compass.x, compass.y,
    //    compass.z);
    prevAccel   = lowPass(accel, prevAccel);
    prevCompass = lowPass(compass, prevCompass);
    //    AXLOGD("low pass accel: {}, {}, {}.... compass: {}, {}, {}", prevAccel.x, prevAccel.y, prevAccel.z,
    //    prevCompass.x, prevCompass.y, prevCompass.z);

    Mat4 rotMatrix = getRotationMatrix(prevAccel, prevCompass);

    Mat4 worldToDevice = rotMatrix * _worldToInertialReferenceFrame;
    return _deviceToDisplay * worldToDevice;
#else
    return Mat4::IDENTITY;
#endif
}

}  // namespace ax::experimental
