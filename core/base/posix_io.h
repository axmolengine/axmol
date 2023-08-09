#pragma once

#if defined(_WIN32)
#    include <errno.h>
#    include <io.h>
#    include <direct.h>
#else
#    include <unistd.h>
#    include <errno.h>
#endif
#include <fcntl.h>

#if defined(_WIN32)
extern "C" int _ftruncate(int fd, int64_t size);
#    include "ntcvt/ntcvt.hpp"
#    define O_READ_FLAGS O_BINARY | O_RDONLY, 0
#    define O_WRITE_FLAGS O_CREAT | O_RDWR | O_BINARY | O_TRUNC, S_IWRITE | S_IREAD
#    define O_APPEND_FLAGS O_APPEND | O_CREAT | O_RDWR | O_BINARY, S_IWRITE | S_IREAD

#    define O_OVERLAP_FLAGS O_CREAT | O_RDWR | O_BINARY, S_IWRITE | S_IREAD
#    define posix_open_cxx(path, ...) ::_wopen(ntcvt::from_chars(path).c_str(), ##__VA_ARGS__)
#    define posix_open(path, ...) ::_wopen(ntcvt::from_chars(path).c_str(), ##__VA_ARGS__)
#    define posix_close ::_close
#    define posix_lseek ::_lseek
#    define posix_read ::_read
#    define posix_write ::_write
#    define posix_fd2fh(fd) reinterpret_cast<HANDLE>(_get_osfhandle(fd))
#    define posix_ftruncate(fd, size) ::_ftruncate(fd, size)
#    define posix_ftruncate64 posix_ftruncate
#    define posix_lseek64 ::_lseeki64
#else
#    define O_READ_FLAGS O_RDONLY, 0
#    define O_WRITE_FLAGS O_CREAT | O_RDWR | O_TRUNC, S_IRWXU
#    define O_APPEND_FLAGS O_APPEND | O_CREAT | O_RDWR, S_IRWXU

#    define O_OVERLAP_FLAGS O_CREAT | O_RDWR, S_IRWXU
#    define posix_open_cxx(path, ...) ::open(path.data(), ##__VA_ARGS__)
#    define posix_open ::open
#    define posix_close ::close
#    define posix_lseek ::lseek
#    define posix_read ::read
#    define posix_write ::write
#    define posix_fd2fh(fd) (fd)
#    define posix_ftruncate ::ftruncate
#    if defined(__APPLE__)
#        define posix_lseek64 ::lseek
#        define posix_ftruncate64 ::ftruncate
#    else
#        define posix_lseek64 ::lseek64
#        define posix_ftruncate64 ::ftruncate64
#    endif
#endif
