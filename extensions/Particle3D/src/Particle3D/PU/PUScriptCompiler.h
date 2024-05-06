/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#ifndef __AX_PU_SCRIPT_COMPILER_H__
#define __AX_PU_SCRIPT_COMPILER_H__
#include "base/Object.h"
#include "Particle3D/PU/PUScriptParser.h"

#include <list>
#include <unordered_map>

NS_AX_BEGIN

/** This enum holds the types of the possible abstract nodes */
enum PUAbstractNodeType
{
    ANT_UNKNOWN,
    ANT_ATOM,
    ANT_OBJECT,
    ANT_PROPERTY,
    ANT_IMPORT,
    ANT_VARIABLE_SET,
    ANT_VARIABLE_ACCESS
};
class AX_EX_DLL PUAbstractNode;
typedef std::list<PUAbstractNode*> PUAbstractNodeList;

class AX_EX_DLL PUAbstractNode
{
public:
    std::string file;
    unsigned int line;
    PUAbstractNodeType type;
    PUAbstractNode* parent;
    Object* context;

    // contextd//
    //    Any context; // A holder for translation context data
public:
    PUAbstractNode(PUAbstractNode* ptr) : line(0), type(ANT_UNKNOWN), parent(ptr), context(nullptr) {}
    virtual ~PUAbstractNode() {}
    /// Returns a new AbstractNode which is a replica of this one.
    virtual PUAbstractNode* clone() const = 0;
    /// Returns a string value depending on the type of the AbstractNode.
    virtual std::string getValue() const = 0;
};

/** This specific abstract node represents a script object */
class AX_EX_DLL PUObjectAbstractNode : public PUAbstractNode
{
private:
    hlookup::string_map<std::string> _env;

public:
    std::string name, cls;
    std::vector<std::string> bases;
    unsigned int id;
    bool abstract;
    PUAbstractNodeList children;
    PUAbstractNodeList values;
    PUAbstractNodeList overrides;  // For use when processing object inheritance and overriding
public:
    PUObjectAbstractNode(PUAbstractNode* ptr);
    virtual ~PUObjectAbstractNode();
    PUAbstractNode* clone() const;
    std::string getValue() const;

    void addVariable(std::string_view name);
    void setVariable(std::string_view name, std::string_view value);
    std::pair<bool, std::string> getVariable(std::string_view name) const;
    const hlookup::string_map<std::string>& getVariables() const;
};

/** This abstract node represents a script property */
class AX_EX_DLL PUPropertyAbstractNode : public PUAbstractNode
{
public:
    std::string name;
    unsigned int id;
    PUAbstractNodeList values;

public:
    PUPropertyAbstractNode(PUAbstractNode* ptr);
    virtual ~PUPropertyAbstractNode();
    PUAbstractNode* clone() const;
    std::string getValue() const;
};

/** This is an abstract node which cannot be broken down further */
class AX_EX_DLL PUAtomAbstractNode : public PUAbstractNode
{
public:
    std::string value;
    unsigned int id;

public:
    PUAtomAbstractNode(PUAbstractNode* ptr);
    PUAbstractNode* clone() const;
    std::string getValue() const;

private:
    void parseNumber() const;
};

class AX_EX_DLL PUParticleSystem3D;
class AX_EX_DLL PUScriptCompiler
{

private:
    hlookup::string_map<PUAbstractNodeList>::iterator compile(const PUConcreteNodeList& nodes, std::string_view file);
    // is it excluded?//
    bool isNameExcluded(std::string_view cls, PUAbstractNode* parent);

public:
    typedef std::unordered_map<std::string, unsigned int> IdMap;

    static PUScriptCompiler* Instance();

    void setParticleSystem3D(PUParticleSystem3D* pu);

    const PUAbstractNodeList* compile(std::string_view file, bool& isFirstCompile);

    void convertToAST(const PUConcreteNodeList& nodes, PUAbstractNodeList& aNodes);

    std::unordered_map<std::string, std::string> env;

private:
    PUScriptCompiler();
    virtual ~PUScriptCompiler();

    void visitList(const PUConcreteNodeList& nodes);
    void visit(PUConcreteNode* node);

private:
    hlookup::string_map<PUAbstractNodeList> _compiledScripts;
    PUAbstractNode* _current;
    PUAbstractNodeList* _nodes;
    PUParticleSystem3D* _PUParticleSystem3D;
};

NS_AX_END

#endif /* defined(__ssaafsdf__CCScriptCompile__) */
