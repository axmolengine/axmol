/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

 Ideas taken from:
 - GamePlay3D: http://gameplay3d.org/
 - OGRE3D: http://www.ogre3d.org/
 - Qt3D: http://qt-project.org/
 ****************************************************************************/

#include "renderer/Technique.h"
#include "renderer/Material.h"
#include "renderer/Pass.h"

namespace ax
{

Technique* Technique::createWithProgramState(Material* parent, backend::ProgramState* state)
{
    auto technique = new Technique();
    if (technique->init(parent))
    {
        auto pass = Pass::createWithProgramState(technique, state);
        technique->addPass(pass);

        technique->autorelease();
        return technique;
    }
    delete technique;
    return nullptr;
}

Technique* Technique::create(Material* material)
{
    auto technique = new Technique();
    if (technique->init(material))
    {
        technique->autorelease();
        return technique;
    }
    delete technique;
    return nullptr;
}

Technique::Technique() : _name("") {}

Technique::~Technique() {}

bool Technique::init(Material* parent)
{
    _material = parent;
    return true;
}

Technique* Technique::clone() const
{
    auto technique = new Technique();

    technique->_name        = _name;
    technique->_renderState = _renderState;
    for (const auto pass : _passes)
    {
        auto p        = pass->clone();
        p->_technique = technique;
        technique->_passes.pushBack(p);
    }

    technique->autorelease();
    return technique;
}

void Technique::addPass(Pass* pass)
{
    _passes.pushBack(pass);
}

std::string Technique::getName() const
{
    return _name;
}

void Technique::setName(std::string_view name)
{
    _name = name;
}

Pass* Technique::getPassByIndex(ssize_t index) const
{
    AX_ASSERT(index >= 0 && index < _passes.size() && "Invalid index");
    return _passes.at(index);
}

ssize_t Technique::getPassCount() const
{
    return _passes.size();
}

const Vector<Pass*>& Technique::getPasses() const
{
    return _passes;
}

}
