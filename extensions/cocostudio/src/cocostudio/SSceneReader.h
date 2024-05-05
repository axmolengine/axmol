/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#ifndef __CCSSCENEREADER_H__
#define __CCSSCENEREADER_H__

#include "DictionaryHelper.h"
#include "CocosStudioExport.h"

namespace cocostudio
{

class CCS_DLL SceneReader
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
     *  @js purge
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

    ax::Node* createObject(const rapidjson::Value& dict,
                                ax::Node* parent,
                                AttachComponentType attachComponent);
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

}  // namespace cocostudio

#endif
