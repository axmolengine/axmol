#pragma once

#include <map>
#include <memory>

#include "ExtensionMacros.h"
#include "base/Config.h"
#include <string>
#include <unordered_map>
#include <vector>

NS_AX_BEGIN
class Node;
class Scene;

NS_AX_END

NS_AX_EXT_BEGIN

class InspectPropertyHandler
{
public:
    virtual ~InspectPropertyHandler() = default;
    virtual bool isSupportedType(Node* node) = 0;
    virtual void drawProperties(Node* node) = 0;
};

class InspectorNodePropertyHandler : public InspectPropertyHandler
{
public:
    ~InspectorNodePropertyHandler() override = default;
    bool isSupportedType(Node* node) override;
    void drawProperties(Node* node) override;
};

class InspectorSpritePropertyHandler : public InspectPropertyHandler
{
public:
    ~InspectorSpritePropertyHandler() override = default;
    bool isSupportedType(Node* node) override;
    void drawProperties(Node* node) override;
};

class InspectorLabelProtocolPropertyHandler : public InspectPropertyHandler
{
public:
    ~InspectorLabelProtocolPropertyHandler() override = default;
    bool isSupportedType(Node* node) override;
    void drawProperties(Node* node) override;
};

class Inspector
{
  public:
    static Inspector* getInstance();
    static void destroyInstance();
    static std::string getNodeTypeName(Node*);
    static std::string demangle(const char* mangled);
    void openForScene(Scene*);
    void close();

    bool addPropertyHandler(std::string_view handlerId, std::unique_ptr<InspectPropertyHandler> handler);
    void removePropertyHandler(const std::string& handlerId);

  private:
    void init();
    void cleanup();
    void mainLoop();
    void drawTreeRecursive(Node*, int index = 0);
    void drawProperties();

    ax::Node* _selected_node = nullptr;
    ax::Scene* _target = nullptr;

    std::unordered_map<std::string, std::unique_ptr<InspectPropertyHandler>> _propertyHandlers;
};

NS_AX_EXT_END
