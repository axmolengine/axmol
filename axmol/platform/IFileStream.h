// Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md)
#pragma once

#include <string>

#include "platform/PlatformConfig.h"
#include "platform/PlatformMacros.h"

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

#if defined(_WIN32)
using osfhnd_t = void*;
#else
using osfhnd_t = int;
#endif

class AX_DLL IFileStream
{
public:
    virtual ~IFileStream() = default;

    enum class Mode
    {
        READ,
        WRITE,
        APPEND,
        OVERLAPPED,
    };

    /**
     *  Open a file
     *  @param path file to open
     *  @param mode File open mode, being READ, WRITE, APPEND
     *  @return true if successful, false if not
     */
    virtual bool open(std::string_view path, IFileStream::Mode mode) = 0;

    /**
     *  Close a file stream
     *  @return 0 if successful, -1 if not
     */
    virtual int close() = 0;

    /**
     *  Seek to position in a file stream
     *  @param offset how many bytes to move within the stream
     *  @param origin SEEK_SET, SEEK_CUR, SEEK_END
     *  @return offset from file begining
     */
    virtual int64_t seek(int64_t offset, int origin) const = 0;

    /**
     *  Read data from file stream
     *  @param buf pointer to data
     *  @param size the amount of data to read in bytes
     *  @return amount of data read successfully, -1 if error
     */
    virtual int read(void* buf, unsigned int size) const = 0;

    /**
     *  Write data to file stream
     *  @param buf pointer to data
     *  @param size the amount of data to write in bytes
     *  @return amount of data written successfully, -1 if error
     */
    virtual int write(const void* buf, unsigned int size) const = 0;

    /**
     *  Get the current position in the file stream
     *  @return current position, -1 if error
     */
    virtual int64_t tell() const = 0;

    /**
     *  Get the size of the file stream
     *  @return stream size, -1 if error (Mode::WRITE and Mode::APPEND may return -1)
     */
    virtual int64_t size() const = 0;

    /*
     * Resize file
     */
    virtual bool resize(int64_t /*size*/) const
    {
        errno = ENOTSUP;
        return false;
    }

    /**
     *  Get status of file stream
     *  @return true if open, false if closed
     */
    virtual bool isOpen() const = 0;

    /*
     * Get native handle if support
     * @returns -1 if not a local disk file or file not open
     *   Windows: HANDLE
     *   Other: file descriptor
     */
    virtual osfhnd_t nativeHandle() const { return (osfhnd_t)-1; }

    virtual operator bool() const { return isOpen(); }

protected:
    IFileStream(){};
};

// end of support group
/** @} */

}
