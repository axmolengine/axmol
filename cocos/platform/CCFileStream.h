// Copyright (c) 2018-2019 HALX99
// Copyright (c) 2020 c4games.com
#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "platform/CCPlatformConfig.h"
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class CC_DLL FileStream {
public:
    virtual ~FileStream() = default;

    enum class Mode {
        READ,
        WRITE,
        APPEND,
    };

    class base_fstream
    {
    public:
        base_fstream();
        explicit base_fstream(std::unique_ptr<FileStream> file);
        virtual ~base_fstream();
        int length();

        virtual void open(std::string const& filename, FileStream::Mode openMode) = 0;
        virtual void close();
        virtual bool is_open();

    protected:
        std::unique_ptr<FileStream> _fileStream;
    };

    class ifstream : public base_fstream, public std::istream
    {
    public:
        ifstream();
        explicit ifstream(std::string const& filename);
        ~ifstream() override;

        void open(std::string const& filename, FileStream::Mode openMode = Mode::READ) override;
        void close() override;
    };

    class ofstream : public base_fstream, public std::ostream
    {
    public:
        ofstream();
        explicit ofstream(std::string const& filename, FileStream::Mode writeMode = FileStream::Mode::WRITE);
        ~ofstream() override;

        void open(std::string const& filename, FileStream::Mode openMode = FileStream::Mode::WRITE) override;
        void close() override;
    };

    class fstream : public base_fstream, public std::iostream
    {
    public:
        fstream();
        explicit fstream(std::string const& filename, FileStream::Mode openMode = FileStream::Mode::READ);
        ~fstream() override;

        void open(std::string const& filename, FileStream::Mode openMode = FileStream::Mode::READ) override;
        void close() override;
    };


    /**
    *  Open a file
    *  @param path file to open
    *  @param mode File open mode, being READ | WRITE | APPEND
    *  @return true if successful, false if not
    */
    virtual bool open(const std::string& path, FileStream::Mode mode) = 0;

    /**
    *  Close a file stream
    *  @return 0 if successful, -1 if not
    */
    virtual int close() = 0;

    /**
    *  Seek to position in a file stream
    *  @param offset how many bytes to move within the stream
    *  @param origin SEEK_SET | SEEK_CUR | SEEK_END
    *  @return 0 if successful, -1 if not
    */
    virtual int seek(long offset, int origin) = 0;

    /**
    *  Read data from file stream
    *  @param buf pointer to data
    *  @param size the amount of data to read in bytes
    *  @return amount of data read successfully, -1 if error
    */
    virtual int read(void* buf, unsigned int size) = 0;

    /**
    *  Write data to file stream
    *  @param buf pointer to data
    *  @param size the amount of data to write in bytes
    *  @return amount of data written successfully, -1 if error
    */
    virtual int write(const void* buf, unsigned int size) = 0;

    /**
    *  Get the current position in the file stream
    *  @return current position, -1 if error
    */
    virtual int tell() = 0;

    /**
    *  Get status of file stream
    *  @return true if open, false if closed
    */
    virtual bool isOpen() const = 0;

    /**
    *  Get the stream length
    *  @return -1 on error, else length
    */
    virtual int length() = 0;

    virtual operator bool() const { return isOpen(); }

    virtual bool isReadOnly() const = 0;

protected:
    FileStream() = default;
};

NS_CC_END