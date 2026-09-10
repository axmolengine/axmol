/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
