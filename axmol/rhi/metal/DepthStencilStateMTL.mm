/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/rhi/metal/DepthStencilStateMTL.h"
#include "axmol/rhi/RenderTarget.h"
#include "xxhash.h"

namespace ax::rhi::mtl
{

namespace
{
static MTLCompareFunction toMTLCompareFunc(CompareFunc func)
{
    MTLCompareFunction ret = MTLCompareFunctionNever;
    switch (func)
    {
    case CompareFunc::NEVER:
        ret = MTLCompareFunctionNever;
        break;
    case CompareFunc::LESS:
        ret = MTLCompareFunctionLess;
        break;
    case CompareFunc::LESS_EQUAL:
        ret = MTLCompareFunctionLessEqual;
        break;
    case CompareFunc::GREATER:
        ret = MTLCompareFunctionGreater;
        break;
    case CompareFunc::GREATER_EQUAL:
        ret = MTLCompareFunctionGreaterEqual;
        break;
    case CompareFunc::EQUAL:
        ret = MTLCompareFunctionEqual;
        break;
    case CompareFunc::NOT_EQUAL:
        ret = MTLCompareFunctionNotEqual;
        break;
    case CompareFunc::ALWAYS:
        ret = MTLCompareFunctionAlways;
        break;
    default:
        assert(false);
        break;
    }
    return ret;
}

static MTLStencilOperation toMTLStencilOp(StencilOp op)
{
    MTLStencilOperation ret = MTLStencilOperationKeep;
    switch (op)
    {
    case StencilOp::KEEP:
        ret = MTLStencilOperationKeep;
        break;
    case StencilOp::ZERO:
        ret = MTLStencilOperationZero;
        break;
    case StencilOp::REPLACE:
        ret = MTLStencilOperationReplace;
        break;
    case StencilOp::INVERT:
        ret = MTLStencilOperationInvert;
        break;
    case StencilOp::INCREMENT_WRAP:
        ret = MTLStencilOperationIncrementWrap;
        break;
    case StencilOp::DECREMENT_WRAP:
        ret = MTLStencilOperationDecrementWrap;
        break;
    default:
        assert(false);
        break;
    }
    return ret;
}

static void setMTLStencilDesc(MTLStencilDescriptor* stencilDesc, const StencilDesc& desc)
{
    stencilDesc.stencilFailureOperation   = toMTLStencilOp(desc.stencilFailureOp);
    stencilDesc.depthFailureOperation     = toMTLStencilOp(desc.depthFailureOp);
    stencilDesc.depthStencilPassOperation = toMTLStencilOp(desc.depthStencilPassOp);
    stencilDesc.stencilCompareFunction    = toMTLCompareFunc(desc.stencilCompareFunc);
    stencilDesc.readMask                  = desc.readMask;
    stencilDesc.writeMask                 = desc.writeMask;
}
}  // namespace

DepthStencilStateImpl::DepthStencilStateImpl(id<MTLDevice> mtlDevice) : _mtlDevice(mtlDevice)
{
    // By default MTLDepthStencilDescriptor disables depth and stencil access
    MTLDepthStencilDescriptor* mtlDesc = [MTLDepthStencilDescriptor new];
    _mtlDepthStencilDisabledState      = [mtlDevice newDepthStencilStateWithDescriptor:mtlDesc];
    [mtlDesc release];
}

void DepthStencilStateImpl::update(const DepthStencilDesc& dsDesc)
{
    DepthStencilState::update(dsDesc);

    if (!isEnabled())
    {
        _mtlDepthStencilState = _mtlDepthStencilDisabledState;
        return;
    }

    DepthStencilDesc hashMe;
    memset(&hashMe, 0, sizeof(hashMe));
    hashMe.depthCompareFunc = dsDesc.depthCompareFunc;
    hashMe.backFaceStencil  = dsDesc.backFaceStencil;
    hashMe.frontFaceStencil = dsDesc.frontFaceStencil;
    hashMe.flags            = dsDesc.flags;

    auto key = XXH32((const void*)&hashMe, sizeof(hashMe), 0);
    auto it  = _mtlStateCache.find(key);
    if (it != _mtlStateCache.end())
    {
        _mtlDepthStencilState = it->second;
        return;
    }

    MTLDepthStencilDescriptor* mtlDesc = [[MTLDepthStencilDescriptor alloc] init];

    if (bitmask::any(dsDesc.flags, DepthStencilFlags::DEPTH_TEST))
        mtlDesc.depthCompareFunction = toMTLCompareFunc(_dsDesc.depthCompareFunc);
    else
        mtlDesc.depthCompareFunction = MTLCompareFunctionAlways;

    mtlDesc.depthWriteEnabled = bitmask::any(dsDesc.flags, DepthStencilFlags::DEPTH_WRITE);

    if (bitmask::any(dsDesc.flags, DepthStencilFlags::STENCIL_TEST))
    {
        setMTLStencilDesc(mtlDesc.frontFaceStencil, _dsDesc.frontFaceStencil);
        setMTLStencilDesc(mtlDesc.backFaceStencil, _dsDesc.backFaceStencil);
    }

    _mtlDepthStencilState = [_mtlDevice newDepthStencilStateWithDescriptor:mtlDesc];
    [mtlDesc release];

    // emplace to state cache
    _mtlStateCache.emplace(key, _mtlDepthStencilState);
}

DepthStencilStateImpl::~DepthStencilStateImpl()
{
    _mtlDepthStencilState = nullptr;
    [_mtlDepthStencilDisabledState release];
    for (auto& stateItem : _mtlStateCache)
        [stateItem.second release];
    _mtlStateCache.clear();
}

}  // namespace ax::rhi::mtl
