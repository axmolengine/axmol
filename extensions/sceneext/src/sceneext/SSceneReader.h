/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

namespace ax::ext
{

class SCNEXT_API SceneReader
{
public:
    enum class AttachComponentType
    {
        /// parent: Empty Node
        ///             ComRender(Sprite, Armature, TMXTiledMap, ParticleSystemQuad, GUIComponent)
        ///             ComAttribute
        ///             ComAudio
        ///             ....
        EMPTY_NODE,

        /// parent:   ComRender(Sprite, Armature, TMXTiledMap, ParticleSystemQuad, GUIComponent)
        ///           ComAttribute
        ///           ComAudio
        ///           .....
        RENDER_NODE,

        /// Default AttachComponentType is _EmptyNode
        DEFAULT = EMPTY_NODE,
    };

    static SceneReader* getInstance();
    /**
     *  @lua destroySceneReader
     */
    static void destroyInstance();
    static const char* sceneReaderVersion();
    ax::Node* createNodeWithSceneFile(std::string_view fileName,
                                      AttachComponentType attachComponent = AttachComponentType::EMPTY_NODE);
    void setTarget(const std::function<void(ax::Object* obj, void* doc)>& selector);
    ax::Node* getNodeByTag(int nTag);
    inline AttachComponentType getAttachComponentType() { return _attachComponent; }
    SceneReader();
    virtual ~SceneReader();

private:
    std::string getComponentClassName(std::string_view name);

    ax::Component* createComponent(std::string_view classname);

    ax::Node* createObject(const rapidjson::Value& dict, ax::Node* parent, AttachComponentType attachComponent);
    void setPropertyFromJsonDict(const rapidjson::Value& dict, ax::Node* node);
    bool readJson(std::string_view fileName, rapidjson::Document& doc);

    ax::Node* createObject(CocoLoader* cocoLoader,
                           stExpCocoNode* cocoNode,
                           ax::Node* parent,
                           AttachComponentType attachComponent);
    void setPropertyFromJsonDict(CocoLoader* cocoLoader, stExpCocoNode* cocoNode, ax::Node* node);

    ax::Node* nodeByTag(ax::Node* parent, int tag);

private:
    static SceneReader* s_sharedReader;
    std::function<void(ax::Object* obj, void* doc)> _fnSelector;
    ax::Node* _node;
    AttachComponentType _attachComponent;
};

}  // namespace ax::ext
