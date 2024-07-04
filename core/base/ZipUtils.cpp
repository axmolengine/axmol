/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "base/ZipUtils.h"

#ifdef MINIZIP_FROM_SYSTEM
#    include <minizip/unzip.h>
#else  // from our embedded sources
#    include "unzip.h"
#endif
#include <ioapi.h>

#include <memory>

#include <zlib.h>
#include <assert.h>
#include <stdlib.h>
#include <set>

#include "base/Data.h"
#include "base/Macros.h"
#include "platform/FileUtils.h"
#include <map>
#include <mutex>

#include "yasio/string_view.hpp"

// minizip 1.2.0 is same with other platforms
#define unzGoToFirstFile64(A, B, C, D) unzGoToFirstFile2(A, B, C, D, NULL, 0, NULL, 0)
#define unzGoToNextFile64(A, B, C, D) unzGoToNextFile2(A, B, C, D, NULL, 0, NULL, 0)

NS_AX_BEGIN

unsigned int ZipUtils::s_uEncryptedPvrKeyParts[4] = {0, 0, 0, 0};
unsigned int ZipUtils::s_uEncryptionKey[1024];
bool ZipUtils::s_bEncryptionKeyIsValid = false;

// --------------------- ZipUtils ---------------------

yasio::byte_buffer ZipUtils::compressGZ(const void* in, size_t inlen, int level)
{
    int err;
    Bytef buffer[512];
    z_stream d_stream; /* compression stream */

    d_stream.zalloc = nullptr;
    d_stream.zfree  = nullptr;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in   = (Bytef*)in;
    d_stream.avail_in  = static_cast<uInt>(inlen);
    d_stream.next_out  = buffer;
    d_stream.avail_out = sizeof(buffer);
    yasio::byte_buffer output;
    err = deflateInit2(&d_stream, level, Z_DEFLATED, MAX_WBITS + 16 /*well: normaly, gzip is: 16*/, MAX_MEM_LEVEL - 1,
                       Z_DEFAULT_STRATEGY);
    if (err != Z_OK)  //
        return output;

    for (;;)
    {
        err = deflate(&d_stream, Z_FINISH);

        if (err == Z_STREAM_END)
        {
            output.insert(output.end(), buffer, buffer + sizeof(buffer) - d_stream.avail_out);
            break;
        }

        switch (err)
        {
        case Z_NEED_DICT:
            err = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            goto _L_end;
        }

        // not enough buffer ?
        if (err != Z_STREAM_END)
        {
            output.insert(output.end(), buffer, buffer + sizeof(buffer));

            d_stream.next_out  = buffer;
            d_stream.avail_out = sizeof(buffer);
        }
    }

_L_end:
    deflateEnd(&d_stream);
    if (err != Z_STREAM_END)
    {
        output.clear();
    }

    return output;
}

yasio::byte_buffer ZipUtils::decompressGZ(const void* in, size_t inlen, int expected_size)
{  // inflate
    int err;
    Bytef buffer[512];
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = nullptr;
    d_stream.zfree  = nullptr;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in   = (Bytef*)in;
    d_stream.avail_in  = static_cast<uInt>(inlen);
    d_stream.next_out  = buffer;
    d_stream.avail_out = sizeof(buffer);
    yasio::byte_buffer output;
    err = inflateInit2(&d_stream, MAX_WBITS + 32 /*well: normaly, gzip is: 16*/);
    if (err != Z_OK)  //
        return output;

    output.reserve(expected_size != -1 ? expected_size : (inlen << 2));

    for (;;)
    {
        err = inflate(&d_stream, Z_NO_FLUSH);

        if (err == Z_STREAM_END)
        {
            output.insert(output.end(), buffer, buffer + sizeof(buffer) - d_stream.avail_out);
            break;
        }

        switch (err)
        {
        case Z_NEED_DICT:
            err = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
            goto _L_end;
        }

        // not enough memory ?
        if (err != Z_STREAM_END)
        {
            // *out = (unsigned char*)realloc(*out, bufferSize * BUFFER_INC_FACTOR);
            output.insert(output.end(), buffer, buffer + sizeof(buffer));

            d_stream.next_out  = buffer;
            d_stream.avail_out = sizeof(buffer);
        }
    }

_L_end:
    inflateEnd(&d_stream);
    if (err != Z_STREAM_END)
    {
        switch (err)
        {
        case Z_MEM_ERROR:
            AXLOGW("ZipUtils: Out of memory while decompressing map data!");
            break;
        case Z_VERSION_ERROR:
            AXLOGW("ZipUtils: Incompatible zlib version!");
            break;
        case Z_DATA_ERROR:
            AXLOGW("ZipUtils: Incorrect zlib compressed data!");
            break;
        default:
            AXLOGW("ZipUtils: Unknown error while decompressing map data!");
        }
        output.clear();
        output.shrink_to_fit();
    }

    return output;
}

inline void ZipUtils::decodeEncodedPvr(unsigned int* data, ssize_t len)
{
    const int enclen    = 1024;
    const int securelen = 512;
    const int distance  = 64;

    // check if key was set
    // make sure to call caw_setkey_part() for all 4 key parts
    AXASSERT(s_uEncryptedPvrKeyParts[0] != 0,
             "axmol:CCZ file is encrypted but key part 0 is not set. Did you call "
             "ZipUtils::setPvrEncryptionKeyPart(...)?");
    AXASSERT(s_uEncryptedPvrKeyParts[1] != 0,
             "axmol:CCZ file is encrypted but key part 1 is not set. Did you call "
             "ZipUtils::setPvrEncryptionKeyPart(...)?");
    AXASSERT(s_uEncryptedPvrKeyParts[2] != 0,
             "axmol:CCZ file is encrypted but key part 2 is not set. Did you call "
             "ZipUtils::setPvrEncryptionKeyPart(...)?");
    AXASSERT(s_uEncryptedPvrKeyParts[3] != 0,
             "axmol:CCZ file is encrypted but key part 3 is not set. Did you call "
             "ZipUtils::setPvrEncryptionKeyPart(...)?");

    // create long key
    if (!s_bEncryptionKeyIsValid)
    {
        unsigned int y, p, e;
        unsigned int rounds = 6;
        unsigned int sum    = 0;
        unsigned int z      = s_uEncryptionKey[enclen - 1];

        do
        {
#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (s_uEncryptedPvrKeyParts[(p & 3) ^ e] ^ z)))

            sum += DELTA;
            e = (sum >> 2) & 3;

            for (p = 0; p < enclen - 1; p++)
            {
                y = s_uEncryptionKey[p + 1];
                z = s_uEncryptionKey[p] += MX;
            }

            y = s_uEncryptionKey[0];
            z = s_uEncryptionKey[enclen - 1] += MX;

        } while (--rounds);

        s_bEncryptionKeyIsValid = true;
    }

    int b = 0;
    int i = 0;

    // encrypt first part completely
    for (; i < len && i < securelen; i++)
    {
        data[i] ^= s_uEncryptionKey[b++];

        if (b >= enclen)
        {
            b = 0;
        }
    }

    // encrypt second section partially
    for (; i < len; i += distance)
    {
        data[i] ^= s_uEncryptionKey[b++];

        if (b >= enclen)
        {
            b = 0;
        }
    }
}

inline unsigned int ZipUtils::checksumPvr(const unsigned int* data, ssize_t len)
{
    unsigned int cs = 0;
    const int cslen = 128;

    len = (len < cslen) ? len : cslen;

    for (int i = 0; i < len; i++)
    {
        cs = cs ^ data[i];
    }

    return cs;
}

ssize_t ZipUtils::inflateMemoryWithHint(unsigned char* in, ssize_t inLength, unsigned char** out, ssize_t outLengthHint)
{
    auto outBuffer = decompressGZ(std::span{in, in + inLength}, static_cast<int>(outLengthHint));
    auto outLen    = outBuffer.size();
    if (out)
        *out           = outBuffer.release_pointer();
    return outLen;
}

ssize_t ZipUtils::inflateMemory(unsigned char* in, ssize_t inLength, unsigned char** out)
{
    // 256k for hint
    return inflateMemoryWithHint(in, inLength, out, 256 * 1024);
}

int ZipUtils::inflateGZipFile(const char* path, unsigned char** out)
{
    int len;
    unsigned int offset = 0;

    AXASSERT(out, "out can't be nullptr.");
    AXASSERT(&*out, "&*out can't be nullptr.");

    gzFile inFile = gzopen(path, "rb");
    if (inFile == nullptr)
    {
        AXLOGW("ZipUtils: error open gzip file: %s", path);
        return -1;
    }

    /* 512k initial decompress buffer */
    yasio::byte_buffer buffer;
    buffer.reserve(512);

    uint8_t readBuffer[512];

    for (;;)
    {
        len = gzread(inFile, readBuffer, sizeof(readBuffer));
        if (len < 0)
        {
            AXLOGW("ZipUtils: error in gzread");
            return -1;
        }
        if (len == 0)
        {
            break;
        }

        buffer.append(readBuffer, readBuffer + 512);
    }

    if (gzclose(inFile) != Z_OK)
    {
        AXLOGW("ZipUtils: gzclose failed");
    }


    auto totalSize = buffer.size();
    if (out)
        *out = buffer.release_pointer();
    return static_cast<int>(totalSize);
}

bool ZipUtils::isCCZFile(const char* path)
{
    // load file into memory
    Data compressedData = FileUtils::getInstance()->getDataFromFile(path);

    if (compressedData.isNull())
    {
        AXLOGW("ZipUtils: loading file failed");
        return false;
    }

    return isCCZBuffer(compressedData.getBytes(), compressedData.getSize());
}

bool ZipUtils::isCCZBuffer(const unsigned char* buffer, ssize_t len)
{
    if (static_cast<size_t>(len) < sizeof(struct CCZHeader))
    {
        return false;
    }

    struct CCZHeader* header = (struct CCZHeader*)buffer;
    return header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' &&
           (header->sig[3] == '!' || header->sig[3] == 'p');
}

bool ZipUtils::isGZipFile(const char* path)
{
    // load file into memory
    Data compressedData = FileUtils::getInstance()->getDataFromFile(path);

    if (compressedData.isNull())
    {
        AXLOGW("ZipUtils: loading file failed");
        return false;
    }

    return isGZipBuffer(compressedData.getBytes(), compressedData.getSize());
}

bool ZipUtils::isGZipBuffer(const unsigned char* buffer, ssize_t len)
{
    if (len < 2)
    {
        return false;
    }

    return buffer[0] == 0x1F && buffer[1] == 0x8B;
}

int ZipUtils::inflateCCZBuffer(const unsigned char* buffer, ssize_t bufferLen, unsigned char** out)
{
    struct CCZHeader* header = (struct CCZHeader*)buffer;

    // verify header
    if (header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == '!')
    {
        // verify header version
        unsigned int version = AX_SWAP_INT16_BIG_TO_HOST(header->version);
        if (version > 2)
        {
            AXLOGW("Unsupported CCZ header format");
            return -1;
        }

        // verify compression format
        if (AX_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB)
        {
            AXLOGW("CCZ Unsupported compression method");
            return -1;
        }
    }
    else if (header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == 'p')
    {
        // encrypted ccz file
        header = (struct CCZHeader*)buffer;

        // verify header version
        unsigned int version = AX_SWAP_INT16_BIG_TO_HOST(header->version);
        if (version > 0)
        {
            AXLOGW("Unsupported CCZ header format");
            return -1;
        }

        // verify compression format
        if (AX_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB)
        {
            AXLOGW("CCZ Unsupported compression method");
            return -1;
        }

        // decrypt
        unsigned int* ints = (unsigned int*)(buffer + 12);
        ssize_t enclen     = (bufferLen - 12) / 4;

        decodeEncodedPvr(ints, enclen);

#if _AX_DEBUG > 0
        // verify checksum in debug mode
        unsigned int calculated = checksumPvr(ints, enclen);
        unsigned int required   = AX_SWAP_INT32_BIG_TO_HOST(header->reserved);

        if (calculated != required)
        {
            AXLOGW("Can't decrypt image file. Is the decryption key valid?");
            return -1;
        }
#endif
    }
    else
    {
        AXLOGW("Invalid CCZ file");
        return -1;
    }

    unsigned int len = AX_SWAP_INT32_BIG_TO_HOST(header->len);
    if (!len)
    {
        AXLOGW("CCZ: Failed to allocate memory for texture");
        return -1;
    }

    axstd::byte_buffer outBuffer(len);

    unsigned long destlen = len;
    size_t source         = (size_t)buffer + sizeof(*header);
    int ret = uncompress(outBuffer.data(), &destlen, (Bytef*)source, static_cast<uLong>(bufferLen - sizeof(*header)));

    if (ret != Z_OK)
    {
        AXLOGW("CCZ: Failed to uncompress data");
        return -1;
    }
    *out = outBuffer.release_pointer();
    return len;
}

int ZipUtils::inflateCCZFile(const char* path, unsigned char** out)
{
    AXASSERT(out, "Invalid pointer for buffer!");

    // load file into memory
    Data compressedData = FileUtils::getInstance()->getDataFromFile(path);

    if (compressedData.isNull())
    {
        AXLOGW("Error loading CCZ compressed file");
        return -1;
    }

    return inflateCCZBuffer(compressedData.getBytes(), compressedData.getSize(), out);
}

void ZipUtils::setPvrEncryptionKeyPart(int index, unsigned int value)
{
    AXASSERT(index >= 0, "axmol:key part index cannot be less than 0");
    AXASSERT(index <= 3, "axmol:key part index cannot be greater than 3");

    if (s_uEncryptedPvrKeyParts[index] != value)
    {
        s_uEncryptedPvrKeyParts[index] = value;
        s_bEncryptionKeyIsValid        = false;
    }
}

void ZipUtils::setPvrEncryptionKey(unsigned int keyPart1,
                                   unsigned int keyPart2,
                                   unsigned int keyPart3,
                                   unsigned int keyPart4)
{
    setPvrEncryptionKeyPart(0, keyPart1);
    setPvrEncryptionKeyPart(1, keyPart2);
    setPvrEncryptionKeyPart(2, keyPart3);
    setPvrEncryptionKeyPart(3, keyPart4);
}

// --------------------- ZipFile ---------------------
// from unzip.cpp
#define UNZ_MAXFILENAMEINZIP 256

static const std::string emptyFilename("");

struct ZipEntryInfo
{
    unz_file_pos pos;
    uint64_t uncompressed_size;
    uint64_t offset;
};

struct ZipFilePrivate
{
    ZipFilePrivate()
    {
        functionOverrides.zopen64_file   = ZipFile_open_file_func;
        functionOverrides.zopendisk64_file = ZipFile_opendisk_file_func;
        functionOverrides.zread_file     = ZipFile_read_file_func;
        functionOverrides.zwrite_file    = ZipFile_write_file_func;
        functionOverrides.ztell64_file     = ZipFile_tell_file_func;
        functionOverrides.zseek64_file     = ZipFile_seek_file_func;
        functionOverrides.zclose_file    = ZipFile_close_file_func;
        functionOverrides.zerror_file    = ZipFile_error_file_func;
        functionOverrides.opaque         = this;
    }

    // unzip overrides to support IFileStream
    static uint64_t ZipFile_tell_file_func(voidpf opaque, voidpf stream)
    {
        if (stream == nullptr)
            return -1;

        auto* fs = (IFileStream*)stream;

        return fs->tell();
    }

    static long ZipFile_seek_file_func(voidpf opaque, voidpf stream, uint64_t offset, int origin)
    {
        if (stream == nullptr)
            return -1;

        auto* fs = (IFileStream*)stream;

        return fs->seek(offset, origin) != -1 ? 0 : -1;  // must return 0 for success or -1 for error
    }

    static voidpf ZCALLBACK ZipFile_open_file_func(voidpf opaque, const void* filename, int mode)
    {
        IFileStream::Mode fsMode;
        if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
            fsMode = IFileStream::Mode::READ;
        else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
            fsMode = IFileStream::Mode::APPEND;
        else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
            fsMode = IFileStream::Mode::WRITE;
        else
            return nullptr;

        return FileUtils::getInstance()->openFileStream((const char*)filename, fsMode).release();
    }

    static voidpf ZipFile_opendisk_file_func(voidpf opaque, voidpf stream, uint32_t number_disk, int mode)
    {
        if (stream == nullptr)
            return nullptr;

        auto* zipFileInfo        = (ZipFilePrivate*)opaque;
        std::string diskFilename = zipFileInfo->zipFileName;

        const auto pos = diskFilename.rfind('.', std::string::npos);

        if (pos != std::string::npos && pos != 0)
        {
            const size_t bufferSize = 5;
            char extensionBuffer[bufferSize];
            snprintf(&extensionBuffer[0], bufferSize, ".z%02u", number_disk + 1);
            diskFilename.replace(pos, std::min((size_t)4, zipFileInfo->zipFileName.size() - pos), extensionBuffer);
            return ZipFile_open_file_func(opaque, diskFilename.c_str(), mode);
        }

        return nullptr;
    }

    static uint32_t ZipFile_read_file_func(voidpf opaque, voidpf stream, void* buf, uint32_t size)
    {
        if (stream == nullptr)
            return (uint32_t)-1;

        return static_cast<IFileStream*>(stream)->read(buf, size);
    }

    static uint32_t ZipFile_write_file_func(voidpf opaque, voidpf stream, const void* buf, uint32_t size)
    {
        if (stream == nullptr)
            return (uint32_t)-1;

        return static_cast<IFileStream*>(stream)->write(buf, size);
    }

    static int ZipFile_close_file_func(voidpf opaque, voidpf stream)
    {
        if (stream == nullptr)
            return -1;

        auto* fs          = (IFileStream*)stream;
        const auto result = fs->close();  // 0 for success, -1 for error
        delete fs;
        return result;
    }

    // THis isn't supported by IFileStream, so just check if the stream is null and open
    static int ZipFile_error_file_func(voidpf opaque, voidpf stream)
    {
        if (stream == nullptr)
        {
            return -1;
        }

        auto* fs = (IFileStream*)stream;

        if (fs->isOpen())
        {
            return 0;
        }

        return -1;
    }
    // End of Overrides

    std::string zipFileName;
    unzFile zipFile;
    std::mutex zipFileMtx;

    // std::unordered_map is faster if available on the platform
    typedef hlookup::string_map<struct ZipEntryInfo> FileListContainer;
    FileListContainer fileList;

    zlib_filefunc64_def functionOverrides{};
};

ZipFile* ZipFile::createFromFile(std::string_view zipFile, std::string_view filter)
{
    auto zip = new ZipFile();
    if (zip->initWithFile(zipFile, filter))
        return zip;
    delete zip;
    return nullptr;
}

ZipFile::ZipFile() : _data(new ZipFilePrivate())
{
    _data->zipFile = nullptr;
}

ZipFile::~ZipFile()
{
    if (_data && _data->zipFile)
    {
        unzClose(_data->zipFile);
    }

    AX_SAFE_DELETE(_data);
}

bool ZipFile::initWithFile(std::string_view zipFile, std::string_view filter)
{
    _data->zipFileName = zipFile;
    _data->zipFile     = unzOpen2_64(zipFile.data(), &_data->functionOverrides);
    return setFilter(filter);
}

bool ZipFile::setFilter(std::string_view filter)
{
    bool ret = false;
    do
    {
        AX_BREAK_IF(!_data);
        AX_BREAK_IF(!_data->zipFile);

        // clear existing file list
        _data->fileList.clear();

        // UNZ_MAXFILENAMEINZIP + 1 - it is done so in unzLocateFile
        char szCurrentFileName[UNZ_MAXFILENAMEINZIP + 1];
        unz_file_info64 fileInfo;

        // go through all files and store position information about the required files
        int err = unzGoToFirstFile64(_data->zipFile, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName) - 1);
        while (err == UNZ_OK)
        {
            unz_file_pos posInfo;
            int posErr = unzGetFilePos(_data->zipFile, &posInfo);
            if (posErr == UNZ_OK)
            {
                std::string currentFileName = szCurrentFileName;
                // cache info about filtered files only (like 'assets/')
                if (filter.empty() || currentFileName.substr(0, filter.length()) == filter)
                {
                    _data->fileList[currentFileName] = ZipEntryInfo{posInfo, (uint64_t)fileInfo.uncompressed_size, 0};
                }
            }
            // next file - also get the information about it
            err = unzGoToNextFile64(_data->zipFile, &fileInfo, szCurrentFileName, sizeof(szCurrentFileName) - 1);
        }
        ret = true;

    } while (false);

    return ret;
}

bool ZipFile::fileExists(std::string_view fileName) const
{
    bool ret = false;
    do
    {
        AX_BREAK_IF(!_data);

        ret = _data->fileList.find(fileName) != _data->fileList.end();
    } while (false);

    return ret;
}

std::vector<std::string> ZipFile::listFiles(std::string_view pathname) const
{

    // filter files which `filename.startsWith(pathname)`
    // then make each path unique

    std::set<std::string_view> fileSet;
    ZipFilePrivate::FileListContainer::const_iterator it  = _data->fileList.begin();
    ZipFilePrivate::FileListContainer::const_iterator end = _data->fileList.end();
    // ensure pathname ends with `/` as a directory
    std::string ensureDir;
    std::string_view dirname = pathname[pathname.length() - 1] == '/' ? pathname : (ensureDir.append(pathname) += '/');
    for (auto&& item : _data->fileList)
    {
        std::string_view filename = item.first;
        if (cxx20::starts_with(filename, cxx17::string_view{dirname}))
        {
            std::string_view suffix{filename.substr(dirname.length())};
            auto pos = suffix.find('/');
            if (pos == std::string::npos)
            {
                fileSet.insert(suffix);
            }
            else
            {
                // fileSet.insert(parts[0] + "/");
                fileSet.insert(suffix.substr(0, pos + 1));
            }
        }
    }

    return std::vector<std::string>{fileSet.begin(), fileSet.end()};
}

bool ZipFile::getFileData(std::string_view fileName, ResizableBuffer* buffer)
{
    bool res = false;
    do
    {
        AX_BREAK_IF(!_data->zipFile);
        AX_BREAK_IF(fileName.empty());

        ZipFilePrivate::FileListContainer::iterator it = _data->fileList.find(fileName);
        AX_BREAK_IF(it == _data->fileList.end());

        ZipEntryInfo& fileInfo = it->second;

        std::unique_lock<std::mutex> lck(_data->zipFileMtx);

        int nRet = unzGoToFilePos(_data->zipFile, &fileInfo.pos);
        AX_BREAK_IF(UNZ_OK != nRet);

        nRet = unzOpenCurrentFile(_data->zipFile);
        AX_BREAK_IF(UNZ_OK != nRet);

        buffer->resize(fileInfo.uncompressed_size);
        int AX_UNUSED nSize =
            unzReadCurrentFile(_data->zipFile, buffer->buffer(), static_cast<unsigned int>(fileInfo.uncompressed_size));
        AXASSERT(nSize == 0 || nSize == (int)fileInfo.uncompressed_size, "the file size is wrong");
        unzCloseCurrentFile(_data->zipFile);
        res = true;
    } while (0);

    return res;
}

std::string ZipFile::getFirstFilename()
{
    if (unzGoToFirstFile(_data->zipFile) != UNZ_OK)
        return emptyFilename;
    std::string path;
    unz_file_info_s info;
    getCurrentFileInfo(&path, &info);
    return path;
}

std::string ZipFile::getNextFilename()
{
    if (unzGoToNextFile(_data->zipFile) != UNZ_OK)
        return emptyFilename;
    std::string path;
    unz_file_info_s info;
    getCurrentFileInfo(&path, &info);
    return path;
}

int ZipFile::getCurrentFileInfo(std::string* filename, unz_file_info_s* info)
{
    char path[FILENAME_MAX + 1];
    int ret = unzGetCurrentFileInfo(_data->zipFile, info, path, sizeof(path), nullptr, 0, nullptr, 0);
    if (ret != UNZ_OK)
    {
        *filename = emptyFilename;
    }
    else
    {
        filename->assign(path);
    }
    return ret;
}

ZipEntryInfo* ZipFile::vopen(std::string_view fileName)
{
    auto it = _data->fileList.find(fileName);
    if (it != _data->fileList.end())
        return &it->second;

    return nullptr;
}

int ZipFile::vread(ZipEntryInfo* entry, void* buf, unsigned int size)
{
    int n = 0;
    do
    {
        AX_BREAK_IF(entry == nullptr || entry->offset >= entry->uncompressed_size);

        std::unique_lock<std::mutex> lck(_data->zipFileMtx);

        int nRet = unzGoToFilePos(_data->zipFile, &entry->pos);
        AX_BREAK_IF(UNZ_OK != nRet);

        nRet = unzOpenCurrentFile(_data->zipFile);
        unzSeek64(_data->zipFile, entry->offset, SEEK_SET);
        n = unzReadCurrentFile(_data->zipFile, buf, size);
        if (n > 0)
            entry->offset += n;

        unzCloseCurrentFile(_data->zipFile);

    } while (false);

    return n;
}

int64_t ZipFile::vseek(ZipEntryInfo* entry, int64_t offset, int origin)
{
    int64_t result = -1;
    if (entry)
    {
        switch (origin)
        {
        case SEEK_SET:
            result = offset;
            break;
        case SEEK_CUR:
            result = entry->offset + offset;
            break;
        case SEEK_END:
            result = (int32_t)entry->uncompressed_size + offset;
            break;
        default:;
        }

        if (result >= 0)
            entry->offset = result;
        else
            result = -1;
    }

    return result;
}

void ZipFile::vclose(ZipEntryInfo* entry)
{
    if (entry != nullptr)
        entry->offset = 0;
}

int64_t ZipFile::vsize(ZipEntryInfo* entry)
{
    if (entry != nullptr)
        return entry->uncompressed_size;

    return 0;
}

NS_AX_END
