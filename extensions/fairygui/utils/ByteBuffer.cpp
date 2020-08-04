#include "ByteBuffer.h"

NS_FGUI_BEGIN
using namespace std;

ByteBuffer::ByteBuffer(char* buffer, int offset, int len, bool transferOwnerShip = false)
    : _buffer(buffer),
      _position(0),
      _offset(offset),
      _length(len),
      _littleEndian(false),
      _ownsBuffer(transferOwnerShip),
      _stringTable(nullptr),
      version(0)
{
}

ByteBuffer::~ByteBuffer()
{
    if (_ownsBuffer && _buffer != nullptr)
        delete _buffer;
}

int ByteBuffer::getBytesAvailable() const
{
    return _length - _position;
}

char ByteBuffer::readByte()
{
    signed char val = _buffer[_offset + _position];
    if (val > 127)
        val = val - 255;
    _position += 1;
    return val;
}

unsigned char ByteBuffer::readUbyte()
{
    unsigned char val = _buffer[_offset + _position];
    _position += 1;
    return val;
}

bool ByteBuffer::readBool()
{
    return readByte() == 1;
}

short ByteBuffer::readShort()
{
    int startIndex = _offset + _position;
    _position += 2;
    unsigned char* pbyte = (unsigned char*)(_buffer + startIndex);
    if (_littleEndian)
        return (short)((*pbyte) | (*(pbyte + 1) << 8));
    else
        return (short)((*pbyte << 8) | (*(pbyte + 1)));
}

unsigned short ByteBuffer::readUshort()
{
    return (unsigned short)readShort();
}

int ByteBuffer::readInt()
{
    int startIndex = _offset + _position;
    _position += 4;
    unsigned char* pbyte = (unsigned char*)(_buffer + startIndex);
    if (_littleEndian)
        return (*pbyte) | (*(pbyte + 1) << 8) | (*(pbyte + 2) << 16) | (*(pbyte + 3) << 24);
    else
        return (*pbyte << 24) | (*(pbyte + 1) << 16) | (*(pbyte + 2) << 8) | (*(pbyte + 3));
}

unsigned int ByteBuffer::readUint()
{
    return (unsigned int)readInt();
}

float ByteBuffer::readFloat()
{
    int val = readInt();
    return *(float*)&val;
}

std::string ByteBuffer::readString()
{
    int len = readUshort();
    return readString(len);
}

std::string ByteBuffer::readString(int len)
{
    char* value = new char[len + 1];

    value[len] = '\0';
    memcpy(value, _buffer + _position, len);
    _position += len;

    string str(value);
    delete[] value;
    value = nullptr;

    return str;
}

const std::string& ByteBuffer::readS()
{
    int index = readUshort();
    if (index == 65534 || index == 65533)
        return cocos2d::STD_STRING_EMPTY;
    else
        return (*_stringTable)[index];
}

bool ByteBuffer::readS(std::string& result)
{
    int index = readUshort();
    if (index == 65534) //null
        return false;
    else if (index == 65533)
    {
        result.clear();
        return true;
    }
    else
    {
        result = (*_stringTable)[index];
        return true;
    }
}

const string* ByteBuffer::readSP()
{
    int index = readUshort();
    if (index == 65534) //null
        return nullptr;
    else if (index == 65533)
        return &cocos2d::STD_STRING_EMPTY;
    else
        return &(*_stringTable)[index];
}

void ByteBuffer::readSArray(std::vector<std::string>& arr, int count)
{
    for (int i = 0; i < count; i++)
        arr.push_back(readS());
}

void ByteBuffer::writeS(const std::string& value)
{
    int index = readUshort();
    if (index != 65534 && index != 65533)
        (*_stringTable)[index] = value;
}

cocos2d::Color4B ByteBuffer::readColor()
{
    int startIndex = _offset + _position;
#if COCOS2D_VERSION >= 0x00040000
    uint8_t r = _buffer[startIndex];
    uint8_t g = _buffer[startIndex + 1];
    uint8_t b = _buffer[startIndex + 2];
    uint8_t a = _buffer[startIndex + 3];
#else
    GLubyte r = _buffer[startIndex];
    GLubyte g = _buffer[startIndex + 1];
    GLubyte b = _buffer[startIndex + 2];
    GLubyte a = _buffer[startIndex + 3];
#endif
    _position += 4;

    return cocos2d::Color4B(r, g, b, a);
}

ByteBuffer* ByteBuffer::readBuffer()
{
    int count = readInt();
    char* p = (char*)malloc(count);
    memcpy(p, _buffer + _position, count);
    ByteBuffer* ba = new ByteBuffer(p, 0, count, true);
    ba->_stringTable = _stringTable;
    ba->version = version;
    _position += count;
    return ba;
}

bool ByteBuffer::seek(int indexTablePos, int blockIndex)
{
    int tmp = _position;
    _position = indexTablePos;
    int segCount = _buffer[_offset + _position++];
    if (blockIndex < segCount)
    {
        bool useShort = _buffer[_offset + _position++] == 1;
        int newPos;
        if (useShort)
        {
            _position += 2 * blockIndex;
            newPos = readShort();
        }
        else
        {
            _position += 4 * blockIndex;
            newPos = readInt();
        }

        if (newPos > 0)
        {
            _position = indexTablePos + newPos;
            return true;
        }
        else
        {
            _position = tmp;
            return false;
        }
    }
    else
    {
        _position = tmp;
        return false;
    }
}

NS_FGUI_END
