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
#if defined(_WIN32)
#    include <io.h>
#    include <direct.h>
#else
#    include <unistd.h>
#    include <errno.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#include <inttypes.h>
#include <sstream>

#include "openssl/aes.h"
#include "openssl/modes.h"
#include "openssl/rc4.h"

#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/sz.hpp"

#include "base/UserDefault.h"
#include "platform/Common.h"
#include "platform/FileUtils.h"
#include "pugixml/pugixml.hpp"
#include "base/Utils.h"

#define USER_DEFAULT_PLAIN_MODE 0

typedef int32_t udflen_t;

NS_AX_BEGIN

/**
 * implements of UserDefault
 */

UserDefault* UserDefault::_userDefault = nullptr;
#if !USER_DEFAULT_PLAIN_MODE
std::string UserDefault::_userDefalutFileName = "UserDefault.bin";
#else
std::string UserDefault::_userDefalutFileName = "UserDefault.xml";
#endif

static void ud_setkey(std::string& lhs, const cxx17::string_view& rhs)
{
    static const size_t keyLen = 16;
    if (!rhs.empty())
    {
        lhs.assign(rhs.data(), std::min(rhs.length(), keyLen));
        if (lhs.size() < keyLen)
            lhs.insert(lhs.end(), keyLen - lhs.size(), '\0');  // fill 0, if key insufficient
    }
    else
        lhs.assign(keyLen, '\0');
}

static void ud_write_v_s(UserDefault* ud, yasio::obstream& obs, const cxx17::string_view value)
{
    size_t value_offset = obs.length();
    obs.write_v(value);
    value_offset += (obs.length() - value_offset - value.length());
    if (!value.empty())
        ud->encrypt(obs.data() + value_offset, value.length(), AES_ENCRYPT);
}

void UserDefault::setEncryptEnabled(bool enabled, cxx17::string_view key, cxx17::string_view iv)
{
    _encryptEnabled = enabled;
    if (_encryptEnabled)
    {
        ud_setkey(_key, key);
        ud_setkey(_iv, iv);
    }
}

void UserDefault::encrypt(std::string& inout, int enc)
{
    if (!inout.empty())
        encrypt(&inout.front(), inout.size(), enc);
}

void UserDefault::encrypt(char* inout, size_t size, int enc)
{
    if (size > 0)
    {
        AES_KEY aeskey;
        AES_set_encrypt_key((const unsigned char*)_key.c_str(), 128, &aeskey);

        unsigned char iv[16] = {0};
        memcpy(iv, _iv.c_str(), std::min(sizeof(iv), _iv.size()));

        int ignored_num = 0;
        AES_cfb128_encrypt((unsigned char*)inout, (unsigned char*)inout, size, &aeskey, iv, &ignored_num, enc);
    }
}

UserDefault::~UserDefault()
{
    closeFileMapping();
}

UserDefault::UserDefault() {}

void UserDefault::closeFileMapping()
{
    _rwmmap.reset();
#if !USER_DEFAULT_PLAIN_MODE
    if (_fileStream.isOpen())
        _fileStream.close();
#endif
}

bool UserDefault::getBoolForKey(const char* pKey)
{
    return getBoolForKey(pKey, false);
}

bool UserDefault::getBoolForKey(const char* pKey, bool defaultValue)
{
    auto pValue = getValueForKey(pKey);
    if (pValue)
        return *pValue == "true";

    return defaultValue;
}

int UserDefault::getIntegerForKey(const char* pKey)
{
    return getIntegerForKey(pKey, 0);
}

int UserDefault::getIntegerForKey(const char* pKey, int defaultValue)
{
    auto pValue = getValueForKey(pKey);
    if (pValue)
        return atoi(pValue->c_str());

    return defaultValue;
}

int64_t UserDefault::getLargeIntForKey(const char* key)
{
    return getLargeIntForKey(key, 0);
}

int64_t UserDefault::getLargeIntForKey(const char* key, int64_t defaultValue)
{
    auto pValue = getValueForKey(key);
    if (pValue)
        return std::stoll(pValue->c_str());

    return defaultValue;
}

float UserDefault::getFloatForKey(const char* pKey)
{
    return getFloatForKey(pKey, 0.0f);
}

float UserDefault::getFloatForKey(const char* pKey, float defaultValue)
{
    return static_cast<float>(getDoubleForKey(pKey, defaultValue));
}

double UserDefault::getDoubleForKey(const char* pKey)
{
    return getDoubleForKey(pKey, 0.0);
}

double UserDefault::getDoubleForKey(const char* pKey, double defaultValue)
{
    auto pValue = getValueForKey(pKey);
    if (pValue)
        return utils::atof(pValue->c_str());

    return defaultValue;
}

std::string_view UserDefault::getStringForKey(const char* pKey)
{
    return getStringForKey(pKey, "");
}

std::string_view UserDefault::getStringForKey(const char* pKey, std::string_view defaultValue)
{
    auto pValue = getValueForKey(pKey);
    if (pValue)
        return *pValue;

    return defaultValue;
}

const std::string* UserDefault::getValueForKey(std::string_view key)
{
    // do lazyInit at here to make sure _encryptEnabled works well,
    lazyInit();

    auto it = this->_values.find(key);
    if (it != this->_values.end())
        return &it->second;
    return nullptr;
}

void UserDefault::setBoolForKey(const char* pKey, bool value)
{
    // save bool value as string
    setStringForKey(pKey, value ? "true" : "false");
}

void UserDefault::setIntegerForKey(const char* pKey, int value)
{
    // check key
    if (!pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%d", value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setLargeIntForKey(const char* pKey, int64_t value)
{  // check key
    if (!pKey)
    {
        return;
    }

    // format the value
    char tmp[96];
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%" PRId64, value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setFloatForKey(const char* pKey, float value)
{
    setDoubleForKey(pKey, value);
}

void UserDefault::setDoubleForKey(const char* pKey, double value)
{
    // check key
    if (!pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, 50);
    snprintf(tmp, sizeof(tmp), "%f", value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setStringForKey(const char* pKey, std::string_view value)
{
    // ignore empty key
    if (!pKey || !*pKey)
    {
        return;
    }

    setValueForKey(pKey, value);

#if !USER_DEFAULT_PLAIN_MODE
    if (_rwmmap)
    {
        yasio::obstream obs;
        if (_encryptEnabled)
        {
            ud_write_v_s(this, obs, pKey);
            ud_write_v_s(this, obs, value);
        }
        else
        {
            obs.write_v(pKey);
            obs.write_v(value);
        }

        if ((_realSize + obs.length() + sizeof(udflen_t)) < _curMapSize)
        {  // fast append entity without flush
            // increase entities count
            yasio::obstream::swrite(_rwmmap->data(), 1 + yasio::ibstream::sread<udflen_t>(_rwmmap->data()));

            // append entity
            ::memcpy(_rwmmap->data() + sizeof(udflen_t) + _realSize, obs.data(), obs.length());
            _realSize += static_cast<int>(obs.length());
        }
        else
        {
            flush();
        }
    }
#else
    flush();
#endif
}

void UserDefault::setValueForKey(std::string_view key, std::string_view value)
{
    // do lazyInit at here to make sure _encryptEnabled works well
    lazyInit();

    updateValueForKey(key, value);
}

void UserDefault::updateValueForKey(std::string_view key, std::string_view value)
{
    auto it = _values.find(key);
    if (it != _values.end())
        it->second = value;
    else
        _values.emplace(key, value);
}

UserDefault* UserDefault::getInstance()
{
    if (_userDefault)
        return _userDefault;

    return (_userDefault = new UserDefault());
}

void UserDefault::destroyInstance()
{
    AX_SAFE_DELETE(_userDefault);
}

void UserDefault::setDelegate(UserDefault* delegate)
{
    if (_userDefault)
        delete _userDefault;

    _userDefault = delegate;
}

void UserDefault::lazyInit()
{
    if (_initialized)
        return;

#if !USER_DEFAULT_PLAIN_MODE
    _filePath = FileUtils::getInstance()->getNativeWritableAbsolutePath() + _userDefalutFileName;

    // construct file mapping
    if (!_fileStream.open(_filePath, IFileStream::Mode::OVERLAPPED))
    {
        AXLOGW("UserDefault::init open storage file '{}' failed!", _filePath);
        return;
    }

    int filesize = static_cast<int>(_fileStream.size());

    if (filesize < _curMapSize)
    {  // construct a empty file mapping
        if (!_fileStream.resize(_curMapSize))
        {
            AXLOGW("UserDefault::init failed to truncate '{}'.", _filePath);
            return;
        }
        _rwmmap = std::make_shared<mio::mmap_sink>(_fileStream.nativeHandle(), 0, _curMapSize);
    }
    else
    {  /// load to memory _values
        _rwmmap = std::make_shared<mio::mmap_sink>(_fileStream.nativeHandle(), 0, mio::map_entire_file);
        if (_rwmmap->is_mapped())
        {  // no error
            yasio::ibstream_view ibs(_rwmmap->data(), _rwmmap->length());

            if (ibs.length() > 0)
            {
                // read count of keyvals.
                int count = ibs.read<int>();
                for (auto i = 0; i < count; ++i)
                {
                    if (_encryptEnabled)
                    {
                        std::string key(ibs.read_v());
                        std::string value(ibs.read_v());
                        this->encrypt(key, AES_DECRYPT);
                        this->encrypt(value, AES_DECRYPT);
                        updateValueForKey(key, value);
                    }
                    else
                    {
                        std::string_view key(ibs.read_v());
                        std::string_view value(ibs.read_v());
                        updateValueForKey(key, value);
                    }
                }
                _realSize = static_cast<int>(ibs.seek(0, SEEK_CUR) - sizeof(udflen_t));
            }
        }
        else
        {
            closeFileMapping();
            ::remove(_filePath.c_str());
            AXLOGW("UserDefault::init map file '{}' failed, we can't save data persisit this time, next time "
                "we will retry!",
                _filePath);
        }
    }
#else
    pugi::xml_document doc;

    _filePath = FileUtils::getInstance()->getWritablePath() + _userDefalutFileName;

    if (FileUtils::getInstance()->isFileExist(_filePath))
    {
        auto data                  = FileUtils::getInstance()->getDataFromFile(_filePath);
        pugi::xml_parse_result ret = doc.load_buffer_inplace(data.getBytes(), data.getSize());
        if (ret)
        {
            for (auto&& elem : doc.document_element())
                updateValueForKey(elem.name(), elem.text().as_string());
        }
        else
        {
            AXLOGE("UserDefault::init load xml file: {} failed, {}", _filePath, ret.description());
        }
    }

#endif

    _initialized = true;
}

void UserDefault::flush()
{
#if !USER_DEFAULT_PLAIN_MODE
    if (_rwmmap)
    {
        yasio::obstream obs;
        obs.write<int>(static_cast<int>(this->_values.size()));
        for (auto&& item : this->_values)
        {
            if (_encryptEnabled)
            {
                ud_write_v_s(this, obs, item.first);
                ud_write_v_s(this, obs, item.second);
            }
            else
            {
                obs.write_v(item.first);
                obs.write_v(item.second);
            }
        }

        std::error_code error;
        if (obs.length() > _curMapSize)
        {
            _rwmmap->unmap();
            while (obs.length() > _curMapSize)
                _curMapSize <<= 1;  // X2

            if (!_fileStream.resize(_curMapSize))
                AXLOGW("UserDefault::flush failed to truncate '{}'.", _filePath);

            _rwmmap->map(_fileStream.nativeHandle(), 0, _curMapSize, error);
        }

        if (!error && _rwmmap->is_mapped())
        {  // mapping status is good
            ::memcpy(_rwmmap->data(), obs.data(), obs.length());
            _realSize = static_cast<int>(obs.length() - sizeof(udflen_t));
        }
        else
        {
            // close file mapping and do a simple workaround fix to don't do persist later at this time
            closeFileMapping();
            ::remove(_filePath.c_str());
        }
    }
#else
    pugi::xml_document doc;
    doc.load_string(R"(<?xml version="1.0" ?>
<r />)");
    auto r = doc.document_element();
    for (auto&& kv : _values)
        r.append_child(kv.first.c_str()).append_child(pugi::xml_node_type::node_pcdata).set_value(kv.second.c_str());

    std::stringstream ss;
    doc.save(ss, "  ");
    FileUtils::getInstance()->writeStringToFile(ss.str(), _filePath);
#endif
}

void UserDefault::deleteValueForKey(const char* key)
{
    if (this->_values.erase(key) > 0)
        flush();
}

void UserDefault::setFileName(std::string_view nameFile)
{
    if (USER_DEFAULT_PLAIN_MODE)
        _userDefalutFileName.assign(nameFile).append("UserDefault.xml"sv);
    else
        _userDefalutFileName.assign(nameFile).append("UserDefault.bin"sv);
}

NS_AX_END
