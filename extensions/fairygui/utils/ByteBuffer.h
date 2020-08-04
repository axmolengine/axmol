#ifndef __BYTEBUFFER_H_
#define __BYTEBUFFER_H_

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class ByteBuffer
{
public:
    ByteBuffer(char* buffer, int offset, int len, bool transferOwnerShip);
    ~ByteBuffer();

    const char* getBuffer() const { return _buffer; }

    bool isLittleEndian() const { return _littleEndian; }
    void setLittleEndian(bool value) { _littleEndian = value; }

    int getBytesAvailable() const;
    int getLength() const { return _length; }

    int getPos() const { return _position; }
    void setPos(int value) { _position = value; }
    void skip(int count) { _position += count; }

    char readByte();
    unsigned char readUbyte();
    bool readBool();
    short readShort();
    unsigned short readUshort();
    int readInt();
    unsigned int readUint();
    float readFloat();
    std::string readString();
    std::string readString(int len);
    const std::string& readS();
    void readSArray(std::vector<std::string>& arr, int count);
    bool readS(std::string& result);
    const std::string* readSP();
    void writeS(const std::string& value);
    cocos2d::Color4B readColor();
    ByteBuffer* readBuffer();
    bool seek(int indexTablePos, int blockIndex);

    std::vector<std::string>* getStringTable() const { return _stringTable; }
    void setStringTable(std::vector<std::string>* value) { _stringTable = value; }

    int version;

private:
    char* _buffer;
    int _offset;
    int _length;
    bool _littleEndian;
    bool _ownsBuffer;
    int _position;
    std::vector<std::string>* _stringTable;
};

NS_FGUI_END

#endif
