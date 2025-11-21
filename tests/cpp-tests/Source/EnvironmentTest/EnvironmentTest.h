/****************************************************************************
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

#pragma once

#include "../BaseTest.h"

DEFINE_TEST_SUITE(EnvironmentTests);

class EnvironmentBase : public TestCase
{
protected:
public:
    virtual std::string title() const override;
};

class EnvironmentLoadConfig : public EnvironmentBase
{
public:
    CREATE_FUNC(EnvironmentLoadConfig);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class EnvironmentQuery : public EnvironmentBase
{
public:
    CREATE_FUNC(EnvironmentQuery);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class EnvironmentInvalid : public EnvironmentBase
{
public:
    CREATE_FUNC(EnvironmentInvalid);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class EnvironmentDefault : public EnvironmentBase
{
public:
    CREATE_FUNC(EnvironmentDefault);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class EnvironmentSet : public EnvironmentBase
{
public:
    CREATE_FUNC(EnvironmentSet);

    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};
