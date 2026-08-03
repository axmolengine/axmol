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

#include "axmol/rhi/GraphicsCore.h"

namespace ax::rhi
{
class GraphicsDeviceFactory
{
public:
    GraphicsDeviceFactory(GraphicsBackend driverType, int prio) : _driverType(driverType), _priority(prio) {}
    virtual ~GraphicsDeviceFactory()                 = default;
    virtual std::unique_ptr<GraphicsDevice> create() = 0;

    GraphicsBackend type() const { return _driverType; };

    int priority() const { return _priority; }
    void priority(int pri) { _priority = pri; }

protected:
    GraphicsBackend _driverType;
    int _priority;
};

struct D3D12GraphicsDeviceFactory : GraphicsDeviceFactory
{
    D3D12GraphicsDeviceFactory(int prio) : GraphicsDeviceFactory(GraphicsBackend::D3D12, prio) {}
    std::unique_ptr<GraphicsDevice> create() override;
};

struct D3D11GraphicsDeviceFactory : GraphicsDeviceFactory
{
    D3D11GraphicsDeviceFactory(int prio) : GraphicsDeviceFactory(GraphicsBackend::D3D11, prio) {}
    std::unique_ptr<GraphicsDevice> create() override;
};

struct VulkanGraphicsDeviceFactory : GraphicsDeviceFactory
{
    VulkanGraphicsDeviceFactory(int prio) : GraphicsDeviceFactory(GraphicsBackend::Vulkan, prio) {}
    std::unique_ptr<GraphicsDevice> create() override;
};

struct MetalGraphicsDeviceFactory : GraphicsDeviceFactory
{
    MetalGraphicsDeviceFactory(int prio) : GraphicsDeviceFactory(GraphicsBackend::Metal, prio) {}
    std::unique_ptr<GraphicsDevice> create() override;
};
}  // namespace ax::rhi
