/****************************************************************************
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
#include <memory>
#include <vector>
#include <algorithm>
#include "axmol/rhi/GraphicsDevice.h"

namespace ax
{

namespace rhi
{
class VulkanInterop;

/**
 * @class GraphicsCore
 * @brief Centralized manager for graphics driver lifecycle and shader environment.
 *
 * GraphicsCore provides a unified interface to create, activate, and destroy
 * rendering drivers across multiple backends (D3D, Vulkan, Metal, OpenGL).
 * It abstracts backend differences and ensures consistent shader language/profile
 * setup for the current driver.
 *
 * @note GraphicsCore is the system-level entry point for all rendering operations.
 *       For actual GPU resource creation (e.g., textures, buffers, pipelines),
 *       use GraphicsCore::device() to obtain the active driver instance.
 */
class AX_DLL GraphicsCore
{
public:
    /**
     * @brief Sets the preferred rendering backend (RHI).
     *
     * This static method allows selecting a specific rendering backend
     * for the engine, such as OpenGL, D3D11, D3D12, Vulkan, or Metal.
     * If no preference is explicitly set, the default value is
     * GraphicsBackend::Auto.
     *
     * @param backend The desired rendering backend.
     */
    static void setPreferredBackend(GraphicsBackend backend);

    /**
     * @brief Sets the minimum Android API level required to enable Vulkan.
     *
     * This function allows applications to restrict Vulkan usage based on
     * the device's Android OS version. If the current device reports Vulkan
     * support but its API level is lower than the configured minimum, Vulkan
     * will be skipped during driver selection and the engine will fall back
     * to the next available backend (e.g. OpenGL ES).
     *
     * @note This call is optional. If not invoked, Vulkan will be considered
     *       only on devices running Android 12 (API level 31) or higher,
     *       which is the default minimum requirement.
     *
     * @warning To ensure the restriction takes effect, this function should
     *          be invoked as early as possible (e.g. in the application
     *          delegate's constructor/applicationWillLaunch), before any rendering
     *          context or driver initialization occurs.
     *
     * @param apiLevel The minimum Android API level required to allow Vulkan usage.
     *                 Default is 31 (Android 12).
     *                 References:
     *                   - https://apilevels.com/
     *                   - https://developer.android.com/tools/releases/platforms
     */
    static void setVulkanMinAndroidApiLevel(int apiLevel);

    /**
     * @brief Sets the Vulkan interoperability interface for external API integration.
     *
     * This function allows external rendering frameworks (such as OpenXR for VR/AR)
     * to share Vulkan resources with the engine's internal Vulkan driver. The provided
     * VulkanInterop instance serves as a bridge between the engine and the external
     * API, enabling coordinated Vulkan instance and device management.
     *
     * The typical use case is when the engine is used within an OpenXR application:
     * - OpenXR runtime creates and manages the Vulkan instance and physical device
     * - The engine needs to use the same Vulkan instance/device for rendering
     * - VulkanInterop provides the necessary handles and synchronization primitives
     *
     * @note This function must be called **before** GraphicsCore::initialize()
     *       to take effect. Once the driver is initialized, changing the interop
     *       object has no effect.
     *
     * @warning The caller is responsible for ensuring the VulkanInterop object
     *          remains valid for the entire lifetime of the Vulkan driver. The
     *          engine does not take ownership of the pointer.
     *
     * @param interop Pointer to a VulkanInterop implementation that provides
     *                the external Vulkan handles. Pass nullptr to clear the
     *                interop interface and revert to engine-managed Vulkan
     *                instance creation.
     *
     * @see VulkanInterop
     * @see initialize()
     */
    static void setVulkanInterop(VulkanInterop* interop);

    /**
     * @brief Sets the priority value for a specific rendering backend.
     *
     * This function allows advanced users to override the default backend
     * selection order by assigning a custom priority to a given backend.
     * Higher values indicate higher preference when selecting the active backend.
     *
     * @note This call is **optional**. If not invoked, the engine will use
     *       built-in default priorities to select the backend. It is primarily
     *       intended for advanced users who need fine-grained control over
     *       backend selection.
     *
     * @warning To ensure the priority takes effect, this function should be
     *          invoked as early as possible, typically in the application
     *          delegate's constructor/applicationWillLaunch, before any rendering context or window
     *          is created. Late changes may not apply if the backend has already
     *          been initialized.
     *          **This API does not affect the fallback OpenGL backend.**
     *
     * @param backend The backend to assign a priority to.
     * @param prio    The priority value (higher means more preferred).
     */
    static void setBackendPriority(GraphicsBackend backend, int prio);

    /**
     * @brief Retrieves the current priority value for a specific rendering backend.
     *
     * This function returns the priority value previously assigned to the
     * given backend, either by default or via setBackendPriority().
     *
     * @param backend The backend to query.
     * @return The priority value associated with the backend.
     */
    static int getBackendPriority(GraphicsBackend backend);

    /**
     * @brief Selects and prepares the active graphics device.
     *
     * This function chooses the most suitable device implementation based on
     * the configured priorities and available backends. For non-OpenGL backends,
     * the device is created and automatically initialized. For OpenGL, only the
     * device object is constructed here; initialization must be deferred until
     * a valid window/context is available.
     */
    static void initialize();

    /**
     * @brief Activates the active graphics device (OpenGL only).
     *
     * This function is only required when using OpenGL, since its initialization
     * cannot be performed at construction time. It should be called after the
     * window and GL context have been created, to complete the device setup.
     *
     * For non-OpenGL backends, initialization is already performed during
     * initialize(), so this function is not needed and will typically
     * be a no-op.
     */
    static void activate();

    /**
     * @brief Releases and destroys the active graphics device, freeing all associated resources.
     */
    static void shutdown();

    [[internal]] static VulkanInterop* getVulkanInterop();

    /**
     * @brief Returns the active graphics device instance, or nullptr if not initialized.
     */
    static GraphicsDevice* device();

    /**
     * @brief Returns the active (actually initialized) graphics backend.
     */
    static GraphicsBackend backend();

    static bool isOpenGL();
    static bool isMetal();
    static bool isD3D11();
    static bool isD3D12();
    static bool isVulkan();

    /**
     * @brief Returns the shader language (e.g. GLSL, ESSL, HLSL, SPIRV, MSL) used by the active backend.
     */
    static int shaderLanguage();

    /**
     * @brief Returns the shader profile version used by the active backend.
     */
    static int shaderProfile();

    /**
     * @brief Returns the shader intermediate-language profile used by the active backend.
     */
    static int shaderILProfile();

    /**
     * @brief Sets the shader intermediate-language profile for the active backend.
     * @param profile The shader IL profile value.
     */
    static void setShaderILProfile(int profile);

private:
    struct State;
    static State& state();
};

}  // namespace rhi

/**
 * @brief Shorthand for @c ax::rhi::GraphicsCore.
 *
 * GraphicsCore manages the rendering backend selection, device lifecycle,
 * and graphics runtime state for the engine.
 */
using GraphicsCore = rhi::GraphicsCore;

}  // namespace ax

#define axdrv ax::rhi::GraphicsCore::device()
