/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <iostream>
#include <string>
#include <list>
#include <map>

#include "axmol/base/Logging.h"
#include "axmol/base/Object.h"
#include "Particle3D/PU/PUScriptLexer.h"

namespace ax
{

struct PUConcreteNode;

enum PUConcreteNodeType
{
    CNT_VARIABLE,
    CNT_VARIABLE_ASSIGN,
    CNT_WORD,
    CNT_IMPORT,
    CNT_QUOTE,
    CNT_LBRACE,
    CNT_RBRACE,
    CNT_COLON
};

typedef std::list<PUConcreteNode*> PUConcreteNodeList;

struct PUConcreteNode
{
    std::string token, file;
    unsigned int line;
    PUConcreteNodeType type;
    PUConcreteNodeList children;
    PUConcreteNode* parent;

    ~PUConcreteNode();
};

void traceScriptParser(PUConcreteNodeList& nodes);

class PUScriptParser
{
public:
    PUScriptParser();
    ~PUScriptParser();

    void parse(PUConcreteNodeList& nodes, const PUScriptTokenList& tokens);

    void parseChunk(PUConcreteNodeList& nodes, const PUScriptTokenList& tokens);

private:
    PUScriptToken* getToken(PUScriptTokenList::iterator i, PUScriptTokenList::iterator end, int offset);
    PUScriptTokenList::const_iterator skipNewlines(PUScriptTokenList::const_iterator i,
                                                   PUScriptTokenList::const_iterator end);
};

}  // namespace ax
