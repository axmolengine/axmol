/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org

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

#ifndef __cocos2d_libs__CSLoader__
#define __cocos2d_libs__CSLoader__

#include "cocostudio/DictionaryHelper.h"
#include "cocostudio/CocosStudioExport.h"

#include "base/ObjectFactory.h"
#include "base/Data.h"
#include "ui/UIWidget.h"

#include "flatbuffers/flatbuffers.h"

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

namespace cocostudio
{
class ComAudio;
}

namespace cocostudio
{
namespace timeline
{
class ActionTimeline;
class ActionTimelineNode;
}  // namespace timeline
}  // namespace cocostudio

NS_AX_BEGIN

typedef std::function<void(Object*)> ccNodeLoadCallback;

class CCS_DLL CSLoader
{
public:
    static CSLoader* getInstance();
    static void destroyInstance();

    CSLoader();
    /** @deprecated Use method destroyInstance() instead */
    AX_DEPRECATED_ATTRIBUTE void purge();

    void init();

    static ax::Node* createNode(std::string_view filename);
    static ax::Node* createNode(std::string_view filename, const ccNodeLoadCallback& callback);
    static ax::Node* createNode(const Data& data);
    static ax::Node* createNode(const Data& data, const ccNodeLoadCallback& callback);
    static ax::Node* createNodeWithVisibleSize(std::string_view filename);
    static ax::Node* createNodeWithVisibleSize(std::string_view filename, const ccNodeLoadCallback& callback);

    static cocostudio::timeline::ActionTimeline* createTimeline(std::string_view filename);
    static cocostudio::timeline::ActionTimeline* createTimeline(const Data& data, std::string_view filename);

    /*
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename);
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename, int
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

NS_AX_END

#endif /* defined(__cocos2d_libs__CSLoader__) */
