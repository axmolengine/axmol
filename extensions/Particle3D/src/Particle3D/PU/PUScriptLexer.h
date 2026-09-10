/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/base/Object.h"
#include <vector>
#include <string>
#include "axmol/base/Logging.h"
#include "extensions/ExtensionExport.h"

namespace ax
{
enum
{
    TID_LBRACKET = 0,  // {
    TID_RBRACKET,      // }
    TID_COLON,         // :
    TID_VARIABLE,      // $...
    TID_WORD,          // *
    TID_QUOTE,         // "*"
    TID_NEWLINE,       // \n
    TID_UNKNOWN,
    TID_END
};

/** This struct represents a token, which is an ID'd lexeme from the
 parsing input stream.
 */
struct PUScriptToken
{
    /// This is the lexeme for this token
    std::string lexeme, file;
    /// This is the id associated with the lexeme, which comes from a lexeme-token id mapping
    unsigned int type;
    /// This holds the line number of the input stream where the token was found.
    unsigned int line;
};

typedef std::vector<PUScriptToken*> PUScriptTokenList;

class PUScriptLexer
{
public:
    PUScriptLexer();
    ~PUScriptLexer();

    void openLexer(std::string_view str, std::string_view source, PUScriptTokenList& tokenList);

    /** Tokenizes the given input and returns the list of tokens found */
    // tokenize(const std::string &str, const std::string &source);
private:  // Private utility operations
    void setToken(std::string_view lexeme, int line, std::string_view source, PUScriptTokenList* tokens);
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
};

}  // namespace ax
