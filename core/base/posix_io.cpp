#include "posix_io.h"

#if defined(_WIN32)
extern "C" int _ftruncate(int fd, int64_t size)
{
    auto handle = (HANDLE)_get_osfhandle(fd);
    if (handle == INVALID_HANDLE_VALUE)
        return -1;
    LARGE_INTEGER offset;
    offset.QuadPart = size;
    do
    {
        if (!::SetFilePointerEx(handle, offset, nullptr, FILE_BEGIN))
            break;
        if (!::SetEndOfFile(handle))
            break;
        return 0;
    } while (false);
    errno = GetLastError();
    return -1;
}
#endif
