// Copyright (c) 2018-2019 HALX99.
// Copyright (c) 2020 c4games.com
#include "platform/CCFileStream.h"
#include <streambuf>
#include <string>
#include <stdexcept>

#include "platform/CCFileUtils.h"

NS_CC_BEGIN

class filestreambuffer : public std::streambuf
{
public:
    filestreambuffer(const filestreambuffer& other) = delete;
    filestreambuffer& operator=(const filestreambuffer& other) = delete;

private:
    int_type underflow() override
    {
        // Check for EOF on read-only files. Write/Append do not have a EOF
        if (_fileStream->isReadOnly())
        {
            const auto pos = _fileStream->tell();
            const auto len = _fileStream->length();

            if ((pos < 0) || (len < 0) || pos >= len)
                return traits_type::eof();
        }

        const auto bytesRead = _fileStream->read(_buffer, _bufferSize);

        if (bytesRead < 1) {
            return traits_type::eof();
        }
        setg(_buffer, _buffer, _buffer + bytesRead);
        return (unsigned char)*gptr();
    }

    pos_type seekoff(off_type pos, std::ios_base::seekdir dir, std::ios_base::openmode mode) override
    {
        switch (dir)
        {
        case std::ios_base::beg:
            _fileStream->seek(pos, SEEK_SET);
            break;
        case std::ios_base::cur:
            // subtract characters currently in buffer from seek position
            _fileStream->seek((_fileStream->tell() + pos) - (egptr() - gptr()), SEEK_SET);
            break;
        case std::ios_base::end:
            _fileStream->seek((_fileStream->length() + pos), SEEK_SET);
            break;
        default:
            break;
        }

        if (mode & std::ios_base::in) 
        {
            setg(egptr(), egptr(), egptr());
        }

        if (mode & std::ios_base::out) 
        {
            setp(_buffer, _buffer);
        }
        return _fileStream->tell();
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override
    {
        _fileStream->seek(pos, SEEK_SET);
        if (mode & std::ios_base::in)
        {
            setg(egptr(), egptr(), egptr());
        }

        if (mode & std::ios_base::out) 
        {
            setp(_buffer, _buffer);
        }

        return _fileStream->tell();
    }

    int_type overflow(int_type c = traits_type::eof()) override
    {
        // Write buffer
        if (flush() < 0)
        {
            return traits_type::eof();
        }

        setp(_buffer, _buffer + _bufferSize); // reset the put pointers

        if (c != traits_type::eof()) 
        {
            *pptr() = (char)c; // Add the overflow character to the put buffer
            pbump(1); // increment the put pointer by one
        }

        return c;
    }

    int sync() override
    {
        const auto result = flush();
        setp(_buffer, _buffer + _bufferSize); // reset the put pointers

        return result;
    }

    int flush() const
    {
        const auto len = pptr() - pbase();
        if (len == 0)
        {
            return 0;  // nothing to do
        }

        if (_fileStream->write(pbase(), len) < len)
        {
            return -1;
        }

        return 0;
    }

    char* _buffer;
    size_t const _bufferSize;

protected:
    FileStream* const _fileStream;

public:
    explicit filestreambuffer(FileStream* file, std::size_t bufferSize = 2048) : _bufferSize(bufferSize), _fileStream(file)
    {
        _buffer = new char[bufferSize];
        char* end = _buffer + bufferSize;
        setg(end, end, end);
        setp(_buffer, end);
    }

    ~filestreambuffer() override
    {
        flush();
        delete[] _buffer;
    }
};

FileStream::base_fstream::base_fstream()
{
}

FileStream::base_fstream::base_fstream(std::unique_ptr<FileStream> file) : _fileStream(std::move(file))
{
    if (_fileStream == nullptr)
    {
        throw std::invalid_argument("Attempted to construct fstream with NULL ptr");
    }
}

FileStream::base_fstream::~base_fstream()
{
    _fileStream.reset();
}

int FileStream::base_fstream::length()
{
    if (!is_open())
    {
        return -1;
    }
    return _fileStream->length();
}

void FileStream::base_fstream::close()
{
    _fileStream.reset();
}

bool FileStream::base_fstream::is_open()
{
    if (_fileStream)
    {
        return _fileStream->isOpen();
    }

    return false;
}


FileStream::ifstream::ifstream()
    : base_fstream(), std::istream(nullptr)
{
}

FileStream::ifstream::ifstream(const std::string& filename)
    : base_fstream(FileUtils::getInstance()->openFileStream(filename, FileStream::Mode::READ)), std::istream(new filestreambuffer(_fileStream.get())) {}

FileStream::ifstream::~ifstream()
{
    delete rdbuf();
}

void FileStream::ifstream::open(std::string const& filename, FileStream::Mode openMode)
{
    close();
    _fileStream = FileUtils::getInstance()->openFileStream(filename, openMode);
    if (_fileStream)
    {
        set_rdbuf(new filestreambuffer(_fileStream.get()));
    }
}

void FileStream::ifstream::close()
{
    base_fstream::close();
    delete rdbuf();
    set_rdbuf(nullptr);
}

FileStream::ofstream::ofstream()
    : base_fstream(), std::ostream(nullptr)
{
}

FileStream::ofstream::ofstream(const std::string& filename, FileStream::Mode writeMode)
    : base_fstream(FileUtils::getInstance()->openFileStream(filename, writeMode)), std::ostream(new filestreambuffer(_fileStream.get())) {}

FileStream::ofstream::~ofstream()
{
    delete rdbuf();
}

void FileStream::ofstream::open(std::string const& filename, FileStream::Mode writeMode)
{
    close();
    _fileStream = FileUtils::getInstance()->openFileStream(filename, writeMode);
    if (_fileStream)
    {
        set_rdbuf(new filestreambuffer(_fileStream.get()));
    }
}

void FileStream::ofstream::close()
{
    base_fstream::close();
    delete rdbuf();
    set_rdbuf(nullptr);
}

FileStream::fstream::fstream()
    : base_fstream(), std::iostream(nullptr)
{
}

FileStream::fstream::fstream(const std::string& filename, FileStream::Mode openMode)
    : base_fstream(FileUtils::getInstance()->openFileStream(filename, openMode)), std::iostream(new filestreambuffer(_fileStream.get())) {}

FileStream::fstream::~fstream()
{
    delete rdbuf();
}

void FileStream::fstream::open(std::string const& filename, FileStream::Mode openMode)
{
    close();
    _fileStream = FileUtils::getInstance()->openFileStream(filename, openMode);
    if (_fileStream)
    {
        set_rdbuf(new filestreambuffer(_fileStream.get()));
    }
}

void FileStream::fstream::close()
{
    base_fstream::close();
    delete rdbuf();
    set_rdbuf(nullptr);
}

NS_CC_END
