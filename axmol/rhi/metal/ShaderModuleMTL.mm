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

#include "axmol/rhi/metal/ShaderModuleMTL.h"
#include "axmol/rhi/metal/DriverMTL.h"
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

    if (!_mtlFunction)
    {
        NSLog(@"metal shader is ---------------");
        AXLOGE("{}", std::string_view{(const char*)_codeSpan.data(), _codeSpan.size()});
        assert(false);
    }

    [library release];
}

ShaderModuleImpl::~ShaderModuleImpl()
{
    [_mtlFunction release];
}

}  // namespace ax::rhi::mtl
