//////////////////////////////////////////////////////////////////////////////////////////
// A multi-platform support c++11 library with focus on asynchronous socket I/O for any 
// client application.
//////////////////////////////////////////////////////////////////////////////////////////
/*
The MIT License (MIT)
Copyright (c) 2012-2021 HALX99
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Modules/ModuleManager.h"

#include "yasio/yasio.hpp"

using namespace yasio;
using namespace yasio::inet;

DECLARE_LOG_CATEGORY_EXTERN(yasio_ue4, Log, All);
DEFINE_LOG_CATEGORY(yasio_ue4);

YASIO_API void yasio_unreal_init()
{
    print_fn2_t log_cb = [](int level, const char* msg) {
        FString text(msg);
        const TCHAR* tstr = *text;
        UE_LOG(yasio_ue4, Log, TEXT("%s"), tstr);
    };
    io_service::init_globals(log_cb);
}
YASIO_API void yasio_unreal_cleanup()
{
    io_service::cleanup_globals();
}

#if defined(YASIO_INSIDE_UNREAL) && (YASIO_BUILD_AS_SHARED)
class yasio_unreal_module : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    void StartupModule() override {}
    void ShutdownModule() override {}
};
IMPLEMENT_MODULE(yasio_unreal_module, yasio)
#endif
