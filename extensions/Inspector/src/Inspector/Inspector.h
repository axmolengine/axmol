/****************************************************************************
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

#pragma once

#include <memory>
#include "extensions/ExtensionMacros.h"
#include "base/Config.h"
#include <string>
#include <unordered_map>
#include "EventListenerCustom.h"
#include "RefPtr.h"
#include "2d/Node.h"

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
    void openForCurrentScene();
    void close();

    bool addPropertyHandler(std::string_view handlerId, std::unique_ptr<InspectPropertyHandler> handler);
    void removePropertyHandler(const std::string& handlerId);

    void setAutoAddToScenes(bool autoAdd);

    std::string_view getFontPath() { return _fontPath; }
    void setFontPath(std::string_view fontPath);

  private:
    void init();
    void cleanup();
    void mainLoop();
    void drawTreeRecursive(Node*, int index = 0);
    void drawProperties();

    ax::RefPtr<ax::Node> _selected_node = nullptr;
    ax::Scene* _target = nullptr;

    std::unordered_map<std::string, std::unique_ptr<InspectPropertyHandler>> _propertyHandlers;
    RefPtr<EventListenerCustom> _beforeNewSceneEventListener;
    RefPtr<EventListenerCustom> _afterNewSceneEventListener;

    bool _autoAddToScenes = false;
    std::string _fontPath;
};

NS_AX_EXT_END
