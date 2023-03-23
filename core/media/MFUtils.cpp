#if defined(_WIN32)

#include "MFUtils.h"

#include <windows.h>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>

#pragma comment(lib, "mf")
#pragma comment(lib, "Mfplat")
#pragma comment(lib, "mfplay")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "shlwapi")

namespace MFUtils
{
struct MFMedia {
    MFMedia(){
        hr = MFStartup(MF_VERSION);
    }
    ~MFMedia() {
        MFShutdown();
    }
    HRESULT hr;
};

HRESULT InitializeMFOnce() {
    static MFMedia s_mfMedia;
    return s_mfMedia.hr;
}

}
#endif
