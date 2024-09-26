#include "TranslationHelper.h"
#include "PackageItem.h"
#include "UIPackage.h"
#if defined(AX_VERSION)
#include "pugixml/pugixml.hpp"
#else
#include "tinyxml2/tinyxml2.h"
#endif
#include "utils/ByteBuffer.h"

#include "base/format.h"

using namespace ax;
NS_FGUI_BEGIN

using namespace std;

hlookup::string_map<hlookup::string_map<std::string> > TranslationHelper::strings;

void TranslationHelper::loadFromXML(const char* xmlString, size_t nBytes)
{
    TranslationHelper::strings.clear();

#if defined(AX_VERSION)
    pugi::xml_document doc;
    if (doc.load_buffer(xmlString, nBytes)) {
        auto root = doc.document_element();
        auto ele = doc.child("string"sv);
        while (ele)
        {
            auto key = ele.attribute("name"sv).value();
            auto text = ele.text().as_string();
            size_t i = key.find("-");
            if (i == std::string::npos)
                continue;

            auto key2 = key.substr(0, i);
            auto key3 = key.substr(i + 1);
            auto& col = TranslationHelper::strings[key2];
            col[key3] = text;

            ele = ele.next_sibling("string"sv);
        }
    }
#else
    tinyxml2::XMLDocument* xml = new tinyxml2::XMLDocument();
    xml->Parse(xmlString, nBytes);

    tinyxml2::XMLElement* root = xml->RootElement();
    tinyxml2::XMLElement* ele = root->FirstChildElement("string");
    while (ele)
    {
        std::string key = ele->Attribute("name");
        std::string text = ele->GetText();
        size_t i = key.find("-");
        if (i == std::string::npos)
            continue;

        std::string key2 = key.substr(0, i);
        std::string key3 = key.substr(i + 1);
        std::unordered_map<std::string, std::string>& col = strings[key2];
        col[key3] = text;

        ele = ele->NextSiblingElement("string");
    }

    delete xml;
#endif
}

void TranslationHelper::translateComponent(PackageItem* item)
{
    if (TranslationHelper::strings.empty())
        return;

    auto col = TranslationHelper::strings.find(item->owner->getId() + item->id);
    if (col == TranslationHelper::strings.end())
        return;

    auto& sstrings = col->second;

    ByteBuffer* buffer = item->rawData;

    buffer->seek(0, 2);

    std::string fmtbuf;

    int childCount = buffer->readShort();
    for (int i = 0; i < childCount; i++)
    {
        int dataLen = buffer->readUshort();
        int curPos = buffer->getPos();

        buffer->seek(curPos, 0);

        ObjectType baseType = (ObjectType)buffer->readByte();
        ObjectType type = baseType;

        buffer->skip(4);
        const string& elementId = buffer->readS();

        if (type == ObjectType::COMPONENT)
        {
            if (buffer->seek(curPos, 6))
                type = (ObjectType)buffer->readByte();
        }

        buffer->seek(curPos, 1);

        auto it = sstrings.find(elementId + "-tips");
        if (it != sstrings.end())
            buffer->writeS(it->second);

        buffer->seek(curPos, 2);

        int gearCnt = buffer->readShort();
        for (int j = 0; j < gearCnt; j++)
        {
            int nextPos = buffer->readUshort();
            nextPos += buffer->getPos();

            if (buffer->readByte() == 6) //gearText
            {
                buffer->skip(2); //controller
                int valueCnt = buffer->readShort();
                for (int k = 0; k < valueCnt; k++)
                {
                    const string& page = buffer->readS();
                    if (!page.empty())
                    {
                        if ((it = sstrings.find(fmt::format_to(fmtbuf, "{}-texts_{}", elementId, k))) != sstrings.end())
                            buffer->writeS(it->second);
                        else
                            buffer->skip(2);
                    }
                }

                if (buffer->readBool() && (it = sstrings.find(elementId + "-texts_def")) != sstrings.end())
                    buffer->writeS(it->second);
            }

            buffer->setPos(nextPos);
        }

        if (baseType == ObjectType::COMPONENT && buffer->version >= 2)
        {
            buffer->seek(curPos, 4);

            buffer->skip(2); //pageController

            buffer->skip(4 * buffer->readShort());

            int cpCount = buffer->readShort();
            for (int k = 0; k < cpCount; k++)
            {
                std::string target = buffer->readS();
                int propertyId = buffer->readShort();
                if (propertyId == 0 && (it = sstrings.find(elementId + "-cp-" + target)) != sstrings.end())
                    buffer->writeS(it->second);
                else
                    buffer->skip(2);
            }
        }

        switch (type)
        {
        case ObjectType::TEXT:
        case ObjectType::RICHTEXT:
        case ObjectType::INPUTTEXT:
        {
            if ((it = sstrings.find(elementId)) != sstrings.end())
            {
                buffer->seek(curPos, 6);
                buffer->writeS(it->second);
            }
            if ((it = sstrings.find(elementId + "-prompt")) != sstrings.end())
            {
                buffer->seek(curPos, 4);
                buffer->writeS(it->second);
            }
            break;
        }

        case ObjectType::LIST:
        {
            buffer->seek(curPos, 8);
            buffer->skip(2);
            int itemCount = buffer->readShort();
            for (int j = 0; j < itemCount; j++)
            {
                int nextPos = buffer->readUshort();
                nextPos += buffer->getPos();

                buffer->skip(2); //url
                if (type == ObjectType::TREE)
                    buffer->skip(2);
                //title
                if ((it = sstrings.find(fmt::format_to(fmtbuf, "{}-{}", elementId, j))) != sstrings.end())
                    buffer->writeS(it->second);
                else
                    buffer->skip(2);

                //selected title
                if ((it = sstrings.find(fmt::format_to(fmtbuf, "{}-{}-0", elementId, j))) != sstrings.end())
                    buffer->writeS(it->second);
                else
                    buffer->skip(2);

                if (buffer->version >= 2)
                {
                    buffer->skip(6);
                    buffer->skip(buffer->readShort() * 4); //controllers

                    int cpCount = buffer->readShort();
                    for (int k = 0; k < cpCount; k++)
                    {
                        std::string target = buffer->readS();
                        int propertyId = buffer->readShort();
                        if (propertyId == 0 && (it = sstrings.find(fmt::format_to(fmtbuf, "{}-{}-{}", elementId, j,
                                                                                  target))) != sstrings.end())
                            buffer->writeS(it->second);
                        else
                            buffer->skip(2);
                    }
                }

                buffer->setPos(nextPos);
            }
            break;
        }

        case ObjectType::LABEL:
        {
            if (buffer->seek(curPos, 6) && (ObjectType)buffer->readByte() == type)
            {
                if ((it = sstrings.find(elementId)) != sstrings.end())
                    buffer->writeS(it->second);
                else
                    buffer->skip(2);

                buffer->skip(2);
                if (buffer->readBool())
                    buffer->skip(4);
                buffer->skip(4);
                if (buffer->readBool() && (it = sstrings.find(elementId + "-prompt")) != sstrings.end())
                    buffer->writeS(it->second);
            }
            break;
        }

        case ObjectType::BUTTON:
        {
            if (buffer->seek(curPos, 6) && (ObjectType)buffer->readByte() == type)
            {
                if ((it = sstrings.find(elementId)) != sstrings.end())
                    buffer->writeS(it->second);
                else
                    buffer->skip(2);
                if ((it = sstrings.find(elementId + "-0")) != sstrings.end())
                    buffer->writeS(it->second);
            }
            break;
        }

        case ObjectType::COMBOBOX:
        {
            if (buffer->seek(curPos, 6) && (ObjectType)buffer->readByte() == type)
            {
                int itemCount = buffer->readShort();
                for (int j = 0; j < itemCount; j++)
                {
                    int nextPos = buffer->readUshort();
                    nextPos += buffer->getPos();

                    if ((it = sstrings.find(fmt::format_to(fmtbuf, "{}-{}", elementId, j))) != sstrings.end())
                        buffer->writeS(it->second);

                    buffer->setPos(nextPos);
                }

                if ((it = sstrings.find(elementId)) != sstrings.end())
                    buffer->writeS(it->second);
            }

            break;
        }
        default:
            break;
        }

        buffer->setPos(curPos + dataLen);
    }
}

NS_FGUI_END
