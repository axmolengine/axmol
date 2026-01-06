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

#include "axmol/rhi/DriverContext.h"

namespace ax::rhi
{
class DriverFactory
{
public:
    DriverFactory(DriverType driverType, int prio) : _driverType(driverType), _priority(prio) {}
    virtual ~DriverFactory()                     = default;
    virtual std::unique_ptr<DriverBase> create() = 0;

    DriverType type() const { return _driverType; };

    int priority() const { return _priority; }
    void priority(int pri) { _priority = pri; }

protected:
    DriverType _driverType;
    int _priority;
};

struct D3D12DriverFactory : DriverFactory
{
    D3D12DriverFactory(int prio) : DriverFactory(DriverType::D3D12, prio) {}
    std::unique_ptr<DriverBase> create() override;
};

struct D3D11DriverFactory : DriverFactory
{
    D3D11DriverFactory(int prio) : DriverFactory(DriverType::D3D11, prio) {}
    std::unique_ptr<DriverBase> create() override;
};

struct VulkanDriverFactory : DriverFactory
{
    VulkanDriverFactory(int prio) : DriverFactory(DriverType::Vulkan, prio) {}
    std::unique_ptr<DriverBase> create() override;
};

struct MetalDriverFactory : DriverFactory
{
    MetalDriverFactory(int prio) : DriverFactory(DriverType::Metal, prio) {}
    std::unique_ptr<DriverBase> create() override;
};
}  // namespace ax::rhi
