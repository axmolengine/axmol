/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 Copyright (c) 2010 Максим Аксенов
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

/// @cond DO_NOT_SHOW

#include "axmol/platform/PlatformConfig.h"
#include "axmol/platform/Common.h"
#include <string>

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

typedef unsigned char AX_XML_CHAR;

class AX_DLL SAXDelegator
{
public:
    virtual ~SAXDelegator() {}

    /**
     * @lua NA
     */
    virtual void startElement(void* ctx, const char* name, const char** atts) = 0;
    /**
     * @lua NA
     */
    virtual void endElement(void* ctx, const char* name) = 0;
    /**
     * @lua NA
     */
    virtual void textHandler(void* ctx, const char* s, size_t len) = 0;
};

class AX_DLL SAXParser
{
    SAXDelegator* _delegator;

public:
    enum class ParseOption
    {
        NORMAL,           // parse_normal
        HTML,             // parse_normal|parse_html_entity_translation|parse_normalize_whitespace
        TRIM_WHITESPACE,  // parse_normal|parse_trim_whitespace
    };

    /**
     * @lua NA
     */
    SAXParser();
    /**
     * @lua NA
     */
    ~SAXParser();
    /**
     * @lua NA
     */
    bool init(const char* encoding);
    /**
     * @lua NA
     */
    bool parse(const char* xmlData, size_t dataLength, ParseOption opt = ParseOption::NORMAL);
    /**
     * @lua NA
     */
    bool parse(std::string_view filename, ParseOption opt = ParseOption::NORMAL);

    /**
     * New API for performance.
     */
    bool parseIntrusive(char* xmlData, size_t dataLength, ParseOption opt = ParseOption::NORMAL);

    /**
     * @lua NA
     */
    void setDelegator(SAXDelegator* delegator);
    /**
     * @lua NA
     */
    static void startElement(void* ctx, const AX_XML_CHAR* name, const AX_XML_CHAR** atts);
    /**
     * @lua NA
     */
    static void endElement(void* ctx, const AX_XML_CHAR* name);
    /**
     * @lua NA
     */
    static void textHandler(void* ctx, const AX_XML_CHAR* name, size_t len);
};

// end of platform group
/// @}

}  // namespace ax
