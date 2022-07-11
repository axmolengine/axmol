/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org

 https://axis-project.github.io/

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

#include "DictionaryHelper.h"
#include "CocosStudioExport.h"

#include "base/ObjectFactory.h"
#include "base/CCData.h"
#include "ui/UIWidget.h"

namespace flatbuffers
{
class FlatBufferBuilder;

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

typedef std::function<void(Ref*)> ccNodeLoadCallback;

class CCS_DLL CSLoader
{
public:
    static CSLoader* getInstance();
    static void destroyInstance();

    CSLoader();
    /** @deprecated Use method destroyInstance() instead */
    CC_DEPRECATED_ATTRIBUTE void purge();

    void init();

    static axis::Node* createNode(std::string_view filename);
    static axis::Node* createNode(std::string_view filename, const ccNodeLoadCallback& callback);
    static axis::Node* createNode(const Data& data);
    static axis::Node* createNode(const Data& data, const ccNodeLoadCallback& callback);
    static axis::Node* createNodeWithVisibleSize(std::string_view filename);
    static axis::Node* createNodeWithVisibleSize(std::string_view filename, const ccNodeLoadCallback& callback);

    static cocostudio::timeline::ActionTimeline* createTimeline(std::string_view filename);
    static cocostudio::timeline::ActionTimeline* createTimeline(const Data& data, std::string_view filename);

    /*
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename);
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename, int
    startIndex, int endIndex, bool loop);
     */

    axis::Node* createNodeFromJson(std::string_view filename);
    axis::Node* loadNodeWithFile(std::string_view fileName);
    axis::Node* loadNodeWithContent(std::string_view content);

    void setRecordJsonPath(bool record) { _recordJsonPath = record; }
    bool isRecordJsonPath() const { return _recordJsonPath; }

    void setJsonPath(std::string jsonPath) { _jsonPath = jsonPath; }
    std::string getJsonPath() const { return _jsonPath; }

    axis::Node* createNodeWithFlatBuffersFile(std::string_view filename);
    axis::Node* nodeWithFlatBuffersFile(std::string_view fileName);
    axis::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree);

    bool bindCallback(std::string_view callbackName,
                      std::string_view callbackType,
                      axis::ui::Widget* sender,
                      axis::Node* handler);

    void registReaderObject(std::string_view className, ObjectFactory::Instance ins);

    axis::Node* createNodeWithFlatBuffersForSimulator(std::string_view filename);
    axis::Node* nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree);

protected:
    axis::Node* createNodeWithFlatBuffersFile(std::string_view filename, const ccNodeLoadCallback& callback);
    axis::Node* nodeWithFlatBuffersFile(std::string_view fileName, const ccNodeLoadCallback& callback);
    axis::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree, const ccNodeLoadCallback& callback);

    axis::Node* loadNode(const rapidjson::Value& json);

    void locateNodeWithMulresPosition(axis::Node* node, const rapidjson::Value& json);

    void initNode(axis::Node* node, const rapidjson::Value& json);

    // load nodes
    axis::Node* loadSimpleNode(const rapidjson::Value& json);
    axis::Node* loadSubGraph(const rapidjson::Value& json);
    axis::Node* loadSprite(const rapidjson::Value& json);
    axis::Node* loadParticle(const rapidjson::Value& json);
    axis::Node* loadTMXTiledMap(const rapidjson::Value& json);

    // load gui
    axis::Node* loadWidget(const rapidjson::Value& json);

    // load component
    axis::Component* loadComponent(const rapidjson::Value& json);
    axis::Component* loadComAudio(const rapidjson::Value& json);

    bool isWidget(std::string_view type);
    bool isCustomWidget(std::string_view type);

    std::string_view getGUIClassName(std::string_view name);
    std::string_view getWidgetReaderClassName(axis::ui::Widget* widget);

    inline void reconstructNestNode(axis::Node* node);
    static inline std::string_view getExtentionName(std::string_view name);

    typedef std::function<axis::Node*(const rapidjson::Value& json)> NodeCreateFunc;
    typedef std::pair<std::string, NodeCreateFunc> Pair;

    std::unordered_map<std::string, NodeCreateFunc> _funcs;

    typedef std::function<axis::Component*(const rapidjson::Value& json)> ComponentCreateFunc;
    typedef std::pair<std::string, ComponentCreateFunc> ComponentPair;

    std::unordered_map<std::string, ComponentCreateFunc> _componentFuncs;

    bool _recordJsonPath;

    std::string _jsonPath;

    std::string _monoCocos2dxVersion;

    Node* _rootNode;
    axis::Vector<axis::Node*> _callbackHandlers;

    std::string _csBuildID;
};

NS_AX_END

#endif /* defined(__cocos2d_libs__CSLoader__) */
