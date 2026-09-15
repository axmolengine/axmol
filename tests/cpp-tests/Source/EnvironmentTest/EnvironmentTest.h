/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
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
