/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/ComBase.h"
#include "axmol/scene/Component.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class SCNEXT_API ComRender : public ax::Component
{
    DECLARE_CLASS_COMPONENT_INFO

public:
    const static std::string COMPONENT_NAME;

    static ComRender* create();
    static ComRender* create(ax::Node* node, const char* comName);

    /**
     * @lua NA
     */
    void onEnter() override;
    /**
     * @lua NA
     */
    void onExit() override;
    /**
     * @lua NA
     */
    void onAdd() override;
    /**
     * @lua NA
     */
    void onRemove() override;
    bool serialize(void* r) override;
    virtual ax::Node* getNode();
    virtual void setNode(ax::Node* node);
    /**
     */
    ComRender();
    ComRender(ax::Node* node, const char* comName);
    /**
     * @lua NA
     */
    virtual ~ComRender();

private:
    bool readJson(std::string_view fileName, rapidjson::Document& doc);

    ax::Node* _render;
};

}  // namespace ax::ext
