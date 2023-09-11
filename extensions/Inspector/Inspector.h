#pragma once

#include "ExtensionMacros.h"
#include "base/Config.h"
#include <string>

NS_AX_BEGIN

class Node;
class Scene;

NS_AX_END

NS_AX_EXT_BEGIN

class Inspector
{
  public:
    static Inspector* getInstance();
    static void destroyInstance();
    static std::string getNodeName(Node*);
    static std::string demangle(const char* mangled);
    void openForScene(Scene*);
    void close();

  private:
    void init();
    void cleanup();
    void mainLoop();
    void drawTreeRecusrive(Node*, int index = 0);
    void drawProperties();

    ax::Node* _selected_node = nullptr;
    ax::Scene* _target = nullptr;
};

NS_AX_EXT_END
