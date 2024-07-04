/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
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

#include "base/Controller.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include <functional>
#    include "base/Macros.h"
#    include "base/Director.h"
#    include "platform/android/jni/JniHelper.h"
#    include "base/EventController.h"

NS_AX_BEGIN

class ControllerImpl
{
public:
    ControllerImpl(Controller* controller) : _controller(controller) {}

    static std::vector<Controller*>::iterator findController(std::string_view deviceName, int deviceId)
    {
        auto iter = std::find_if(
            Controller::s_allController.begin(), Controller::s_allController.end(), [&](Controller* controller) {
                return (deviceName == controller->_deviceName) && (deviceId == controller->_deviceId);
            });

        return iter;
    }

    static void onConnected(std::string_view deviceName, int deviceId)
    {
        // Check whether the controller is already connected.
        AXLOGD("onConnected {},{}", deviceName, deviceId);

        auto iter = findController(deviceName, deviceId);
        if (iter != Controller::s_allController.end())
            return;

        // It's a new controller being connected.
        auto controller         = new ax::Controller();
        controller->_deviceId   = deviceId;
        controller->_deviceName = deviceName;
        Controller::s_allController.emplace_back(controller);

        controller->onConnected();
    }

    static void onDisconnected(std::string_view deviceName, int deviceId)
    {
        AXLOGD("onDisconnected {},{}", deviceName, deviceId);

        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            AXLOGE("Could not find the controller!");
            return;
        }

        (*iter)->onDisconnected();
        Controller::s_allController.erase(iter);
    }

    static void onButtonEvent(std::string_view deviceName,
                              int deviceId,
                              int keyCode,
                              bool isPressed,
                              float value,
                              bool isAnalog)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            AXLOGD("onButtonEvent:connect new controller.");
            onConnected(deviceName, deviceId);
            iter = findController(deviceName, deviceId);
        }

        (*iter)->onButtonEvent(keyCode, isPressed, value, isAnalog);
    }

    static void onAxisEvent(std::string_view deviceName, int deviceId, int axisCode, float value, bool isAnalog)
    {
        auto iter = findController(deviceName, deviceId);
        if (iter == Controller::s_allController.end())
        {
            AXLOGD("onAxisEvent:connect new controller.");
            onConnected(deviceName, deviceId);
            iter = findController(deviceName, deviceId);
        }

        (*iter)->onAxisEvent(axisCode, value, isAnalog);
    }

private:
    Controller* _controller;
};

void Controller::startDiscoveryController()
{
    // Empty implementation on Android
}

void Controller::stopDiscoveryController()
{
    // Empty implementation on Android
}

Controller::~Controller()
{
    delete _impl;

    delete _connectEvent;
    delete _keyEvent;
    delete _axisEvent;
}

void Controller::registerListeners() {}

bool Controller::isConnected() const
{
    // If there is a controller instance, it means that the controller is connected.
    // If a controller is disconnected, the instance will be destroyed.
    // So always returns true for this method.
    return true;
}

Controller::Controller()
    : _controllerTag(TAG_UNSET)
    , _impl(new ControllerImpl(this))
    , _connectEvent(nullptr)
    , _keyEvent(nullptr)
    , _axisEvent(nullptr)
{
    init();
}

void Controller::receiveExternalKeyEvent(int externalKeyCode, bool receive)
{
    JniHelper::callStaticVoidMethod("org.axmol.lib.GameControllerHelper", "receiveExternalKeyEvent", _deviceId,
                                    externalKeyCode, receive);
}

NS_AX_END

extern "C" {

JNIEXPORT void JNICALL Java_org_axmol_lib_GameControllerAdapter_nativeControllerConnected(JNIEnv*,
                                                                                             jclass,
                                                                                             jstring deviceName,
                                                                                             jint controllerID)
{
    AXLOGD("controller id: {} connected!", controllerID);
    ax::ControllerImpl::onConnected(ax::JniHelper::jstring2string(deviceName), controllerID);
}

JNIEXPORT void JNICALL Java_org_axmol_lib_GameControllerAdapter_nativeControllerDisconnected(JNIEnv*,
                                                                                                jclass,
                                                                                                jstring deviceName,
                                                                                                jint controllerID)
{
    AXLOGD("controller id: {} disconnected!", controllerID);
    ax::ControllerImpl::onDisconnected(ax::JniHelper::jstring2string(deviceName), controllerID);
}

JNIEXPORT void JNICALL Java_org_axmol_lib_GameControllerAdapter_nativeControllerButtonEvent(JNIEnv*,
                                                                                               jclass,
                                                                                               jstring deviceName,
                                                                                               jint controllerID,
                                                                                               jint button,
                                                                                               jboolean isPressed,
                                                                                               jfloat value,
                                                                                               jboolean isAnalog)
{
    ax::ControllerImpl::onButtonEvent(ax::JniHelper::jstring2string(deviceName), controllerID, button,
                                           isPressed, value, isAnalog);
}

JNIEXPORT void JNICALL Java_org_axmol_lib_GameControllerAdapter_nativeControllerAxisEvent(JNIEnv*,
                                                                                             jclass,
                                                                                             jstring deviceName,
                                                                                             jint controllerID,
                                                                                             jint axis,
                                                                                             jfloat value,
                                                                                             jboolean isAnalog)
{
    ax::ControllerImpl::onAxisEvent(ax::JniHelper::jstring2string(deviceName), controllerID, axis, value,
                                         isAnalog);
}

}  // extern "C" {

#endif  // #if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
