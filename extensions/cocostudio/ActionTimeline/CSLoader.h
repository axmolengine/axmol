/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org

 https://adxeproject.github.io/

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

NS_CC_BEGIN

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

    static cocos2d::Node* createNode(std::string_view filename);
    static cocos2d::Node* createNode(std::string_view filename, const ccNodeLoadCallback& callback);
    static cocos2d::Node* createNode(const Data& data);
    static cocos2d::Node* createNode(const Data& data, const ccNodeLoadCallback& callback);
    static cocos2d::Node* createNodeWithVisibleSize(std::string_view filename);
    static cocos2d::Node* createNodeWithVisibleSize(std::string_view filename, const ccNodeLoadCallback& callback);

    static cocostudio::timeline::ActionTimeline* createTimeline(std::string_view filename);
    static cocostudio::timeline::ActionTimeline* createTimeline(const Data& data, std::string_view filename);

    /*
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename);
    static cocostudio::timeline::ActionTimelineNode* createActionTimelineNode(std::string_view filename, int
    startIndex, int endIndex, bool loop);
     */

    cocos2d::Node* createNodeFromJson(std::string_view filename);
    cocos2d::Node* loadNodeWithFile(std::string_view fileName);
    cocos2d::Node* loadNodeWithContent(std::string_view content);

    void setRecordJsonPath(bool record) { _recordJsonPath = record; }
    bool isRecordJsonPath() const { return _recordJsonPath; }

    void setJsonPath(std::string jsonPath) { _jsonPath = jsonPath; }
    std::string getJsonPath() const { return _jsonPath; }

    cocos2d::Node* createNodeWithFlatBuffersFile(std::string_view filename);
    cocos2d::Node* nodeWithFlatBuffersFile(std::string_view fileName);
    cocos2d::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree);

    bool bindCallback(std::string_view callbackName,
                      std::string_view callbackType,
                      cocos2d::ui::Widget* sender,
                      cocos2d::Node* handler);

    void registReaderObject(std::string_view className, ObjectFactory::Instance ins);

    cocos2d::Node* createNodeWithFlatBuffersForSimulator(std::string_view filename);
    cocos2d::Node* nodeWithFlatBuffersForSimulator(const flatbuffers::NodeTree* nodetree);

protected:
    cocos2d::Node* createNodeWithFlatBuffersFile(std::string_view filename, const ccNodeLoadCallback& callback);
    cocos2d::Node* nodeWithFlatBuffersFile(std::string_view fileName, const ccNodeLoadCallback& callback);
    cocos2d::Node* nodeWithFlatBuffers(const flatbuffers::NodeTree* nodetree, const ccNodeLoadCallback& callback);

    cocos2d::Node* loadNode(const rapidjson::Value& json);

    void locateNodeWithMulresPosition(cocos2d::Node* node, const rapidjson::Value& json);

    void initNode(cocos2d::Node* node, const rapidjson::Value& json);

    // load nodes
    cocos2d::Node* loadSimpleNode(const rapidjson::Value& json);
    cocos2d::Node* loadSubGraph(const rapidjson::Value& json);
    cocos2d::Node* loadSprite(const rapidjson::Value& json);
    cocos2d::Node* loadParticle(const rapidjson::Value& json);
    cocos2d::Node* loadTMXTiledMap(const rapidjson::Value& json);

    // load gui
    cocos2d::Node* loadWidget(const rapidjson::Value& json);

    // load component
    cocos2d::Component* loadComponent(const rapidjson::Value& json);
    cocos2d::Component* loadComAudio(const rapidjson::Value& json);

    bool isWidget(std::string_view type);
    bool isCustomWidget(std::string_view type);

    std::string_view getGUIClassName(std::string_view name);
    std::string_view getWidgetReaderClassName(cocos2d::ui::Widget* widget);

    inline void reconstructNestNode(cocos2d::Node* node);
    static inline std::string_view getExtentionName(std::string_view name);

    typedef std::function<cocos2d::Node*(const rapidjson::Value& json)> NodeCreateFunc;
    typedef std::pair<std::string, NodeCreateFunc> Pair;

    std::unordered_map<std::string, NodeCreateFunc> _funcs;

    typedef std::function<cocos2d::Component*(const rapidjson::Value& json)> ComponentCreateFunc;
    typedef std::pair<std::string, ComponentCreateFunc> ComponentPair;

    std::unordered_map<std::string, ComponentCreateFunc> _componentFuncs;

    bool _recordJsonPath;

    std::string _jsonPath;

    std::string _monoCocos2dxVersion;

    Node* _rootNode;
    cocos2d::Vector<cocos2d::Node*> _callbackHandlers;

    std::string _csBuildID;
};

NS_CC_END

#endif /* defined(__cocos2d_libs__CSLoader__) */
