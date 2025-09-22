/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#pragma once

#include "platform/PlatformMacros.h"
#include "base/Macros.h"
#include "base/Data.h"

namespace ax
{

struct FontDefinition;

/**
 * @addtogroup platform
 * @{
 */

/**
 * @class Device
 * @brief
 */
class AX_DLL Device
{
public:
    /** Defines the alignment of text. */
    enum class TextAlign
    {
        CENTER       = 0x33, /** Horizontal center and vertical center. */
        TOP          = 0x13, /** Horizontal center and vertical top. */
        TOP_RIGHT    = 0x12, /** Horizontal right and vertical top. */
        RIGHT        = 0x32, /** Horizontal right and vertical center. */
        BOTTOM_RIGHT = 0x22, /** Horizontal right and vertical bottom. */
        BOTTOM       = 0x23, /** Horizontal center and vertical bottom. */
        BOTTOM_LEFT  = 0x21, /** Horizontal left and vertical bottom. */
        LEFT         = 0x31, /** Horizontal left and vertical center. */
        TOP_LEFT     = 0x11, /** Horizontal left and vertical top. */
    };

    /** Defines the impact haptic feedback style. */
    enum ImpactFeedbackStyle
    {
        ImpactFeedbackStyleLight = 0,
        ImpactFeedbackStyleMedium,
        ImpactFeedbackStyleHeavy
    };

    /** Defines the notification haptic feedback style. */
    enum NotificationFeedbackType
    {
        NotificationFeedbackTypeSuccess = 0,
        NotificationFeedbackTypeWarning,
        NotificationFeedbackTypeError
    };

    /**
     *  Gets the DPI of device
     *  @return The DPI of device.
     */
    static int getDPI();

    /**
     * Gets the device pixel ratio
     * @since axmol-2.1.0
     */
    static float getPixelRatio();

    /**
     * To enable or disable accelerometer.
     */
    static void setAccelerometerEnabled(bool isEnabled);

    /**
     *  Sets the interval of accelerometer.
     */
    static void setAccelerometerInterval(float interval);

    /**
     * Controls whether the screen should remain on.
     *
     * @param keepScreenOn One flag indicating that the screen should remain on.
     */
    static void setKeepScreenOn(bool keepScreenOn);

    /**
     * Vibrate for the specified amount of time.
     * If vibrate is not supported, then invoking this method has no effect.
     * Some platforms limit to a maximum duration of 5 seconds.
     * Duration is ignored on iOS due to API limitations.
     * @param duration The duration in seconds.
     */
    static void vibrate(float duration);

    /**
     * Prepare haptic feedback impact with selected style
     * If haptic feedback is not supported, then invoking this method has no effect.
     * @param style The style of the feedback.
     */
    static void prepareImpactFeedbackGenerator(ImpactFeedbackStyle style);

    /**
     * Generate haptic feedback impact with selected style
     * If haptic feedback is not supported, then invoking this method has no effect.
     */
    static void impactOccurred(ImpactFeedbackStyle style);

    /**
     * Prepare haptic feedback notification
     * If haptic feedback is not supported, then invoking this method has no effect.
     * @param style The style of the feedback.
     */
    static void prepareNotificationFeedbackGenerator();

    /**
     * Generate haptic feedback notification with selected type
     * If haptic feedback is not supported, then invoking this method has no effect.
     */
    static void notificationOccurred(NotificationFeedbackType type);

    /**
     * Prepare haptic feedback selection
     * If haptic feedback is not supported, then invoking this method has no effect.
     * @param style The style of the feedback.
     */
    static void prepareSelectionFeedbackGenerator();

    /**
     * Generate haptic feedback selection changed
     * If haptic feedback is not supported, then invoking this method has no effect.
     */
    static void selectionChanged();

    /**
     * Gets texture data for text.
     */
    static Data getTextureDataForText(std::string_view text,
                                      const FontDefinition& textDefinition,
                                      TextAlign align,
                                      int& width,
                                      int& height,
                                      bool& hasPremultipliedAlpha);

#pragma region Orientation control support
    enum class Orientation
    {
        Unknown,
        Portrait,         // Portrait (upright)
        ReversePortrait,  // Portrait upside down
        SensorPortrait,   // Portrait + reverse portrait (auto-rotate with sensor)

        Landscape,         // Landscape (left)
        ReverseLandscape,  // Landscape (right)
        SensorLandscape,   // Landscape + reverse landscape (auto-rotate with sensor)

        Sensor,     // All orientations except upside down (auto-rotate with sensor)
        FullSensor  // All orientations including upside down (auto-rotate with sensor)
    };

    enum class OrientationMask
    {
        Portrait         = 1 << 0,
        ReversePortrait  = 1 << 1,  // Portrait upside down
        Landscape        = 1 << 2,  // Landscape (left)
        ReverseLandscape = 1 << 3,  // Landscape (right)
        All              = Portrait | ReversePortrait | Landscape | ReverseLandscape
    };

    /**
     * @brief Sets the preferred screen orientation for the application.
     *
     * This method attempts to switch the screen orientation to the specified value.
     * If the orientation is supported by the platform (as declared in Info.plist or AndroidManifest),
     * the system will rotate and lock to that orientation.
     * Use Orientation::Auto to unlock and allow automatic rotation.
     *
     * @param orientation The desired screen orientation. Use Orientation::Auto to reset.
     * @since axmol-2.9.0
     */
    static void setPreferredOrientation(Orientation orientation);

    /**
     * @brief Gets the currently preferred (locked) screen orientation.
     *
     * Returns the orientation that was last set via setPreferredOrientation().
     * If Orientation::Auto is returned, the application is currently allowing automatic rotation.
     *
     * @return The current preferred orientation.
     * @since axmol-2.9.0
     */
    static Orientation getPreferredOrientation();

    /**
     * @brief Gets the set of orientations supported by the platform.
     *
     * This reflects the maximum orientation capabilities declared in Info.plist (iOS)
     * or AndroidManifest.xml (Android). The application can only rotate within this set.
     *
     * @return A bitmask representing supported orientations.
     * @since axmol-2.9.0
     */
    static OrientationMask getSupportedOrientations();

    /**
     * @brief Gets the current screen orientation as rendered by the system.
     *
     * This reflects the actual orientation currently applied to the screen,
     * which may differ from the preferred orientation if Orientation::Sensor is set.
     *
     * @return The current screen orientation.
     * @since axmol-2.9.0
     */
    static Orientation getCurrentOrientation();

    /**
     * @brief Returns the device's physical orientation (hardware posture).
     *
     * Unlike getCurrentOrientation(), which reflects the UI's current interface
     * orientation, this method reports the device's actual physical posture as
     * detected by sensors (e.g., accelerometer). It may differ from the UI
     * orientation when rotation is locked or when the app restricts supported
     * orientations.
     *
     * Platform notes:
     * - iOS: Maps from UIDeviceOrientation. FaceUp/FaceDown are treated as Unknown.
     *        You should ensure orientation notifications are active internally
     *        (beginGeneratingDeviceOrientationNotifications) if needed.
     * - Android: Maps from display rotation and/or sensor readings to the closest
     *            Orientation value. Sensor-based "flat" states map to Unknown.
     *
     * Typical usage:
     * - Use getCurrentOrientation() for layout, rendering, and UI decisions.
     * - Use getPhysicalOrientation() for gameplay/input mechanics that depend on
     *   how the user is holding the device, independent of UI rotation.
     *
     * @return Orientation The device's physical orientation.
     * @since axmol-2.9.0
     */
    static Orientation getPhysicalOrientation();

    /**
     * Resolve the orientation of the application window.
     *
     * This method combines three sources of information:
     *   1. Supported orientations (from getSupportedOrientations)
     *   2. Preferred orientation (from getPreferredOrientation)
     *   3. Current physical device orientation (from getPhysicalOrientation)
     *
     * Resolution rules:
     *   - If the preferred orientation is a concrete value (Portrait, Landscape, etc.)
     *     and is included in the supported mask, it will be used.
     *   - If the preferred orientation is a sensor-based value (SensorPortrait,
     *     SensorLandscape, Sensor, FullSensor), the physical orientation is used
     *     if it is supported; otherwise a fallback orientation is chosen.
     *   - If no valid orientation can be resolved, the first available orientation
     *     from the supported mask is used as a fallback.
     *
     * The returned Orientation is guaranteed to be compatible with the supported
     * orientation mask, and can be used by RenderViewImpl-ios to compute the
     * logical screen size and viewport before app->run().
     *
     * @return Orientation  The resolved final orientation for the application.
     * @since axmol-2.9.0
     */
    static Orientation resolveOrientation();

#pragma endregion Orientation control support

private:
    AX_DISALLOW_IMPLICIT_CONSTRUCTORS(Device);
};

AX_ENABLE_BITMASK_OPS(Device::OrientationMask)

// end group
/// @}

}  // namespace ax
