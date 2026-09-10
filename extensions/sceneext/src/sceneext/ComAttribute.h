/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ComBase.h"
#include "axmol/scene/Component.h"
#include "axmol/base/Value.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class SCNEXT_API ComAttribute : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO
public:
    ComAttribute(void);
    /**
     * @lua NA
     */
    virtual ~ComAttribute(void);

public:
    const static std::string COMPONENT_NAME;

    static ComAttribute* create(void);
    bool init() override;
    bool serialize(void* r) override;

    void setInt(std::string_view key, int value);
    void setFloat(std::string_view key, float value);
    void setBool(std::string_view key, bool value);
    void setString(std::string_view key, std::string_view value);
    int getInt(std::string_view key, int def = 0) const;
    float getFloat(std::string_view key, float def = 0.0f) const;
    bool getBool(std::string_view key, bool def = false) const;
    std::string getString(std::string_view key, std::string_view def = "") const;
    bool parse(std::string_view jsonFile);

private:
    ax::ValueMap _dict;
    rapidjson::Document _doc;
};

}  // namespace ax::ext
