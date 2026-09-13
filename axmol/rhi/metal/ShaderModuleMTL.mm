/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/rhi/metal/GraphicsDeviceMTL.h"
#include "axmol/base/Logging.h"

namespace ax::rhi::mtl
{

ShaderModuleImpl::ShaderModuleImpl(id<MTLDevice> mtlDevice, ShaderStage stage, Data& chunk) : ShaderModule(stage, chunk)
{
    NSString* shader = [[NSString alloc] initWithBytes:_codeSpan.data()
                                                length:_codeSpan.size()
                                              encoding:NSUTF8StringEncoding];
    NSError* error;
    id<MTLLibrary> library = [mtlDevice newLibraryWithSource:shader options:nil error:&error];
    if (!library)
    {
        NSLog(@"Can not compile metal shader: %@", error);
        AXLOGE("{}", std::string_view{(const char*)_codeSpan.data(), _codeSpan.size()});
        assert(false);
        return;
    }

    _mtlFunction = [library newFunctionWithName:@"main0"];
    _compiled    = _mtlFunction != nil;

    if (!_compiled)
    {
        NSLog(@"metal shader is ---------------");
        AXLOGE("{}", std::string_view{(const char*)_codeSpan.data(), _codeSpan.size()});
    }

    [library release];
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    [_mtlFunction release];
}

}  // namespace ax::rhi::mtl
