/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2017-2020 c4games.com.

http://www.cocos2d-x.org

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
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <errno.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#include <inttypes.h>

#include "openssl/aes.h"
#include "openssl/modes.h"
#include "openssl/rc4.h"

#include "base/CCUserDefault.h"
#include "platform/CCCommon.h"
#include "platform/CCFileUtils.h"
#include "pugixml/pugixml.hpp"
#include "base/base64.h"
#include "base/ccUtils.h"

#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"
#include "yasio/detail/sz.hpp"

#include "CCFileStream.h"

#define USER_DEFAULT_PLAIN_MODE 0

#if !USER_DEFAULT_PLAIN_MODE
#define USER_DEFAULT_FILENAME "UserDefault.bin"
#else
#define USER_DEFAULT_FILENAME "UserDefault.xml"
#endif

typedef int32_t udflen_t;

NS_CC_BEGIN

/**
 * implements of UserDefault
 */

UserDefault* UserDefault::_userDefault = nullptr;

static void ud_setkey(std::string& lhs, const cxx17::string_view& rhs) {
    static const size_t keyLen = 16;
    if (!rhs.empty()) {
        lhs.assign(rhs.data(), std::min(rhs.length(), keyLen));
        if (lhs.size() < keyLen)
            lhs.insert(lhs.end(), keyLen - lhs.size(), '\0'); // fill 0, if key insufficient
    }
    else
        lhs.assign(keyLen, '\0');
}

static void ud_write_v_s(UserDefault* ud, yasio::obstream& obs, const cxx17::string_view value) 
{
    size_t value_offset = obs.length();
    obs.write_v(value);
    value_offset += (obs.length() - value_offset - value.length());
    if(!value.empty())
        ud->encrypt(obs.data() + value_offset, value.length(), AES_ENCRYPT);
}

void UserDefault::setEncryptEnabled(bool enabled, const std::string& key, const std::string& iv)
{
    _encryptEnabled = enabled;
    if (_encryptEnabled) {
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
    if (size > 0) {
        AES_KEY aeskey;
        AES_set_encrypt_key((const unsigned char*)_key.c_str(), 128, &aeskey);

        unsigned char iv[16] = { 0 };
        memcpy(iv, _iv.c_str(), std::min(sizeof(iv), _iv.size()));

        int ignored_num = 0;
        AES_cfb128_encrypt((unsigned char*)inout, (unsigned char*)inout, size, &aeskey, iv, &ignored_num, enc);
    }
}

UserDefault::~UserDefault()
{
    closeFileMapping();
}

UserDefault::UserDefault()
{
}

void UserDefault::closeFileMapping()
{
    _rwmmap.reset();
    if (_fd != -1) {
        posix_close(_fd);
        _fd = -1;
    }
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

double  UserDefault::getDoubleForKey(const char* pKey)
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

std::string UserDefault::getStringForKey(const char* pKey)
{
    return getStringForKey(pKey, "");
}

std::string UserDefault::getStringForKey(const char* pKey, const std::string & defaultValue)
{
    auto pValue = getValueForKey(pKey);
    if (pValue)
        return *pValue;

    return defaultValue;
}

const std::string* UserDefault::getValueForKey(const std::string& key)
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
    if (! pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%d", value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setLargeIntForKey(const char* pKey, int64_t value)
{// check key
    if (!pKey)
    {
        return;
    }

    // format the value
    char tmp[96];
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%" PRId64, value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setFloatForKey(const char* pKey, float value)
{
    setDoubleForKey(pKey, value);
}

void UserDefault::setDoubleForKey(const char* pKey, double value)
{
    // check key
    if (! pKey)
    {
        return;
    }

    // format the value
    char tmp[50];
    memset(tmp, 0, 50);
    sprintf(tmp, "%f", value);

    setStringForKey(pKey, tmp);
}

void UserDefault::setStringForKey(const char* pKey, const std::string & value)
{
    // ignore empty key
    if (! pKey || !*pKey)
    {
        return;
    }

    setValueForKey(pKey, value);

#if !USER_DEFAULT_PLAIN_MODE
    if (_rwmmap) {
        yasio::obstream obs;
        if (_encryptEnabled)
        {
            ud_write_v_s(this, obs, pKey);
            ud_write_v_s(this, obs, value);
        }
        else {
            obs.write_v(pKey);
            obs.write_v(value);
        }

        if ((_realSize + obs.length() + sizeof(udflen_t)) < _curMapSize)
        { // fast append entity without flush
            // increase entities count
            yasio::obstream::swrite(_rwmmap->data(), 1 + yasio::ibstream::sread<udflen_t>(_rwmmap->data()));

            // append entity
            ::memcpy(_rwmmap->data() + sizeof(udflen_t) + _realSize, obs.data(), obs.length());
            _realSize += obs.length();
        }
        else {
            flush();
        }
    }
#else
    flush();
#endif
}

void UserDefault::setValueForKey(const std::string& key, const std::string& value)
{
    // do lazyInit at here to make sure _encryptEnabled works well
    lazyInit();

    updateValueForKey(key, value);
}

void UserDefault::updateValueForKey(const std::string& key, const std::string& value)
{
    auto it = _values.find(key);
    if (it != _values.end())
        it->second = value;
    else
        _values.emplace(key, value);
}

UserDefault* UserDefault::getInstance()
{
    if (_userDefault) return _userDefault;

    return ( _userDefault = new (std::nothrow) UserDefault() );
}

void UserDefault::destroyInstance()
{
    CC_SAFE_DELETE(_userDefault);
}

void UserDefault::setDelegate(UserDefault *delegate)
{
    if (_userDefault)
        delete _userDefault;

    _userDefault = delegate;
}

void UserDefault::lazyInit()
{
    if (_initialized) return;

    _filePath = FileUtils::getInstance()->getWritablePath() + USER_DEFAULT_FILENAME;

#if !USER_DEFAULT_PLAIN_MODE
    // construct file mapping
    _fd = posix_open(_filePath.c_str(), O_OVERLAP_FLAGS);
    if (_fd == -1) {
        log("[Warnning] UserDefault::init open storage file '%s' failed!", _filePath.c_str());
        return;
    }

    int filesize = static_cast<int>(posix_lseek(_fd, 0, SEEK_END));
    posix_lseek(_fd, 0, SEEK_SET);

    if (filesize < _curMapSize) { // construct a empty file mapping
        posix_fsetsize(_fd, _curMapSize);
        _rwmmap = std::make_shared<mio::mmap_sink>(posix_fd2fh(_fd), 0, _curMapSize);
    }
    else { /// load to memory _values
        _rwmmap = std::make_shared<mio::mmap_sink>(posix_fd2fh(_fd), 0, mio::map_entire_file);
        if (_rwmmap->is_mapped()) { // no error
            yasio::ibstream_view ibs(_rwmmap->data(), _rwmmap->length());

            if (ibs.length() > 0) {
                // read count of keyvals.
                int count = ibs.read<int>();
                for (auto i = 0; i < count; ++i) {
                    std::string key(ibs.read_v());
                    std::string value(ibs.read_v());
                    if (_encryptEnabled)
                    {
                        this->encrypt(key, AES_DECRYPT);
                        this->encrypt(value, AES_DECRYPT);
                    }
                    updateValueForKey(key, value);
                }
                _realSize = static_cast<int>(ibs.seek(0, SEEK_CUR) - sizeof(udflen_t));
            }
        }
        else {
            closeFileMapping();
            ::remove(_filePath.c_str());
            log("[Warnning] UserDefault::init map file '%s' failed, we can't save data persisit this time, next time we will retry!", _filePath.c_str());
        }
    }
#else
    pugi::xml_document doc;
    pugi::xml_parse_result ret = doc.load_file(_filePath.c_str());
    if (ret) {
        for (auto& elem : doc.document_element())
            updateValueForKey(elem.name(), elem.text().as_string());
    }
    else {
        log("UserDefault::init load xml file: %s failed, %s", _filePath.c_str(), ret.description());
    }
#endif

    _initialized = true;
}

void UserDefault::flush()
{
#if !USER_DEFAULT_PLAIN_MODE
    if (_rwmmap) {
        yasio::obstream obs;
        obs.write<int>(static_cast<int>(this->_values.size()));
        for (auto& item : this->_values) {
            if (_encryptEnabled)
            {
                ud_write_v_s(this, obs, item.first);
                ud_write_v_s(this, obs, item.second);
            }
            else {
                obs.write_v(item.first);
                obs.write_v(item.second);
            }
        }

        std::error_code error;
        if (obs.length() > _curMapSize) {
            _rwmmap->unmap();
            _curMapSize <<= 1; // X2
            posix_fsetsize(_fd, _curMapSize);
            _rwmmap->map(posix_fd2fh(_fd), 0, _curMapSize, error);
        }

        if (!error && _rwmmap->is_mapped())
        { // mapping status is good
            ::memcpy(_rwmmap->data(), obs.data(), obs.length());
            _realSize = static_cast<int>(obs.length() - sizeof(udflen_t));
        }
        else {
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
    for(auto& kv : _values)
        r.append_child(kv.first.c_str())
        .append_child(pugi::xml_node_type::node_pcdata)
        .set_value(kv.second.c_str());
    doc.save_file(_filePath.c_str(), "  ");
#endif
}

void UserDefault::deleteValueForKey(const char* key)
{
    if(this->_values.erase(key) > 0)
        flush();
}

NS_CC_END
