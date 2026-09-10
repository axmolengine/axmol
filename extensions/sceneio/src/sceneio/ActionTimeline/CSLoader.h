/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "sceneext/DictionaryHelper.h"
#include "sceneext/SceneExtMacros.h"

#include "axmol/base/ObjectFactory.h"
#include "axmol/base/Data.h"
#include "axmol/ui/Widget.h"
#include "flatbuffers/flatbuffer_builder.h"

namespace flatbuffers
{
struct NodeTree;

struct WidgetOptions;
struct SingleNodeOptions;
struct SpriteOptions;
struct ParticleSystemOptions;
struct TMXTiledMapOptions;
struct ProjectNodeOptions;

struct ComponentOptions;
struct ComAudioOptions;
}  // namespace flatbuffers

namespace ax::ext
{
class ComAudio;
namespace timeline
{
class ActionTimeline;
class ActionTimelineNode;
}  // namespace timeline
}  // namespace ax::ext

namespace ax
{

typedef std::function<void(Object*)> ccNodeLoadCallback;

class SCNIO_API CSLoader
{
public:
    static CSLoader* getInstance();
    static void destroyInstance();

    CSLoader();

    void init();

    static ax::Node* createNode(std::string_view filename);
    static ax::Node* createNode(std::string_view filename, const ccNodeLoadCallback& callback);
    static ax::Node* createNode(const Data& data);
    static ax::Node* createNode(const Data& data, const ccNodeLoadCallback& callback);
    static ax::Node* createNodeWithVisibleSize(std::string_view filename);
    static ax::Node* createNodeWithVisibleSize(std::string_view filename, const ccNodeLoadCallback& callback);

    static ext::timeline::ActionTimeline* createTimeline(std::string_view filename);
    static ext::timeline::ActionTimeline* createTimeline(const Data& data, std::string_view filename);

    /*
    static ext::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename);
    static ext::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename, int
    startIndex, int endIndex, bool loop);
     */

    ax::Node* createNodeFromJson(std::string_view filename);
    ax::Node* loadNodeWithFile(std::string_view fileName);
    ax::Node* loadNodeWithContent(std::string_view content);

    void setRecordJsonPath(bool record) { _recordJsonPath = record; }
    bool isRecordJsonPath() const { return _recordJsonPath; }

    void setJsonPath(std::string jsonPath) { _jsonPath = jsonPath; }
    std::string getJsonPath() const { return _jsonPath; }

    ax::Node* createNodeWithFlatBuffersFile(std::string_view filename);
    ax::Node* nodeWithFlatBuffersFile(std::string_view fileName);
    ax::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree);

    bool bindCallback(std::string_view callbackName,
                      std::string_view callbackType,
                      ax::ui::Widget* sender,
                      ax::Node* handler);

    void registReaderObject(std::string_view className, ObjectFactory::Instance ins);

    ax::Node* createNodeWithFlatBuffersForSimulator(std::string_view filename);
    ax::Node* nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree);

protected:
    ax::Node* createNodeWithFlatBuffersFile(std::string_view filename, const ccNodeLoadCallback& callback);
    ax::Node* nodeWithFlatBuffersFile(std::string_view fileName, const ccNodeLoadCallback& callback);
    ax::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree, const ccNodeLoadCallback& callback);

    ax::Node* loadNode(const rapidjson::Value& json);

    void locateNodeWithMulresPosition(ax::Node* node, const rapidjson::Value& json);

    void initNode(ax::Node* node, const rapidjson::Value& json);

    // load nodes
    ax::Node* loadSimpleNode(const rapidjson::Value& json);
    ax::Node* loadSubGraph(const rapidjson::Value& json);
    ax::Node* loadSprite(const rapidjson::Value& json);
    ax::Node* loadParticle(const rapidjson::Value& json);
    ax::Node* loadTMXTiledMap(const rapidjson::Value& json);

    // load gui
    ax::Node* loadWidget(const rapidjson::Value& json);

    // load component
    ax::Component* loadComponent(const rapidjson::Value& json);
    ax::Component* loadComAudio(const rapidjson::Value& json);

    bool isWidget(std::string_view type);
    bool isCustomWidget(std::string_view type);

    std::string_view getGUIClassName(std::string_view name);
    std::string_view getWidgetReaderClassName(ax::ui::Widget* widget);

    inline void reconstructNestNode(ax::Node* node);
    static inline std::string_view getExtentionName(std::string_view name);

    typedef std::function<ax::Node*(const rapidjson::Value& json)> NodeCreateFunc;
    typedef std::pair<std::string, NodeCreateFunc> Pair;

    std::unordered_map<std::string, NodeCreateFunc> _funcs;

    typedef std::function<ax::Component*(const rapidjson::Value& json)> ComponentCreateFunc;
    typedef std::pair<std::string, ComponentCreateFunc> ComponentPair;

    std::unordered_map<std::string, ComponentCreateFunc> _componentFuncs;

    bool _recordJsonPath;

    std::string _jsonPath;

    std::string _monoCocos2dxVersion;

    Node* _rootNode;
    ax::Vector<ax::Node*> _callbackHandlers;

    std::string _csBuildID;
};

}  // namespace ax
