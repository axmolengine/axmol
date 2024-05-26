#include "Inspector.h"
#include "ImGuiPresenter.h"
#include "axmol.h"

#if __has_include(<cxxabi.h>)
#    define AX_HAS_CXXABI 1
#    include <cxxabi.h>
#endif

#include "fmt/format.h"
#include <memory>
#include <imgui/misc/cpp/imgui_stdlib.h>

NS_AX_EXT_BEGIN

namespace
{
Inspector* g_instance = nullptr;
}

bool InspectorNodePropertyHandler::isSupportedType(Node* node)
{
    return true;
}

void InspectorNodePropertyHandler::drawProperties(Node* node)
{
    if (auto userData = node->getUserData(); userData)
    {
        ImGui::SameLine();
        ImGui::Text("User data: %p", userData);
    }

    auto pos      = node->getPosition();
    float _pos[2] = {pos.x, pos.y};
    ImGui::DragFloat2("Position", _pos);
    node->setPosition({_pos[0], _pos[1]});

    // need to use getScaleX() because of assert
    float _scale[3] = {node->getScaleX(), node->getScaleX(), node->getScaleY()};
    ImGui::DragFloat3("Scale", _scale, 0.025f);

    if (auto scale_0 = node->getScaleX(); scale_0 != _scale[0])
    {
        node->setScale(_scale[0]);
    }
    else
    {
        node->setScaleX(_scale[1]);
        node->setScaleY(_scale[2]);
    }

    float rotation[3] = {node->getRotationSkewX(), node->getRotationSkewX(),
                         node->getRotationSkewY()};
    if (ImGui::DragFloat3("Rotation", rotation, 1.0f))
    {
        if (node->getRotationSkewX() != rotation[0])
        {
            node->setRotation(rotation[0]);
        }
        else
        {
            node->setRotationSkewX(rotation[1]);
            node->setRotationSkewY(rotation[2]);
        }
    }

    auto contentSize = node->getContentSize();
    float _cont[2]   = {contentSize.x, contentSize.y};
    ImGui::DragFloat2("Content Size", _cont);
    node->setContentSize({_cont[0], _cont[1]});

    auto anchor    = node->getAnchorPoint();
    float _anch[2] = {anchor.x, anchor.y};
    ImGui::DragFloat2("Anchor Point", _anch);
    node->setAnchorPoint({_anch[0], _anch[1]});

    int localZOrder = node->getLocalZOrder();
    ImGui::InputInt("Local Z", &localZOrder);
    if (node->getLocalZOrder() != localZOrder)
    {
        node->setLocalZOrder(localZOrder);
    }

    float globalZOrder = node->getGlobalZOrder();
    ImGui::InputFloat("Global Z", &globalZOrder);
    if (node->getGlobalZOrder() != globalZOrder)
    {
        node->setGlobalZOrder(globalZOrder);
    }

    auto visible = node->isVisible();
    ImGui::Checkbox("Visible", &visible);
    if (visible != node->isVisible())
    {
        node->setVisible(visible);
    }

    auto color      = node->getColor();
    float _color[4] = {color.r / 255.f, color.g / 255.f, color.b / 255.f, node->getOpacity() / 255.f};
    ImGui::ColorEdit4("Color", _color);
    node->setColor({static_cast<GLubyte>(_color[0] * 255), static_cast<GLubyte>(_color[1] * 255),
                              static_cast<GLubyte>(_color[2] * 255)});
    node->setOpacity(static_cast<int>(_color[3] * 255.f));
}

bool InspectorSpritePropertyHandler::isSupportedType(Node* node)
{
    return dynamic_cast<Sprite*>(node) != nullptr;
}

void InspectorSpritePropertyHandler::drawProperties(Node* node)
{
    auto* sprite = dynamic_cast<ax::Sprite*>(node);

    ImGui::SameLine();
    bool flipx = sprite->isFlippedX();
    bool flipy = sprite->isFlippedY();
    ImGui::Checkbox("FlipX", &flipx);
    ImGui::SameLine();
    ImGui::Checkbox("FlipY", &flipy);
    sprite->setFlippedX(flipx);
    sprite->setFlippedY(flipy);

    auto texture = sprite->getTexture();
    ImGui::TextWrapped("Texture: %s", texture->getPath().c_str());

    ImGuiPresenter::getInstance()->image(sprite, ImVec2(256, 256));
}

bool InspectorLabelProtocolPropertyHandler::isSupportedType(Node* node)
{
    return dynamic_cast<LabelProtocol*>(node) != nullptr;
}

void InspectorLabelProtocolPropertyHandler::drawProperties(Node* node)
{
    auto label_node = dynamic_cast<LabelProtocol*>(node);

    std::string_view label = label_node->getString();
    std::string label_str(label);

    if (ImGui::InputTextMultiline("Text", &label_str, {0, 50}))
    {
        label_node->setString(label_str);
    }
}

Inspector* Inspector::getInstance()
{
    if (g_instance == nullptr)
    {
        g_instance = new Inspector();
        g_instance->init();
    }
    return g_instance;
}

void Inspector::destroyInstance()
{
    if (g_instance)
    {
        g_instance->close();
        g_instance->cleanup();
        delete g_instance;
        g_instance = nullptr;
    }
}

void Inspector::setFontPath(std::string_view fontPath)
{
    _fontPath = std::string(fontPath);
}

void Inspector::setFontSize(float fontSize)
{
    _fontSize = fontSize;
}

void Inspector::init()
{
    _fontPath = "fonts/arial.ttf";

    addPropertyHandler("__NODE__", std::make_unique<InspectorNodePropertyHandler>());
    addPropertyHandler("__SPRITE__", std::make_unique<InspectorSpritePropertyHandler>());
    addPropertyHandler("__LABEL_PROTOCOL__", std::make_unique<InspectorLabelProtocolPropertyHandler>());

    _beforeNewSceneEventListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_BEFORE_SET_NEXT_SCENE, [this](EventCustom*) {
            if (!_autoAddToScenes)
                return;

        getInstance()->close();
    });
    _afterNewSceneEventListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        Director::EVENT_AFTER_SET_NEXT_SCENE, [this](EventCustom*) {
            if (!_autoAddToScenes)
                return;

        getInstance()->openForCurrentScene();
    });
}

void Inspector::cleanup()
{
    _propertyHandlers.clear();
    auto* eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->removeEventListener(_beforeNewSceneEventListener);
    eventDispatcher->removeEventListener(_afterNewSceneEventListener);

    _beforeNewSceneEventListener = nullptr;
    _afterNewSceneEventListener = nullptr;
}

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32

std::string Inspector::demangle(const char* name)
{
    // works because msvc's typeid().name() returns undecorated name
    // typeid(Node).name() == "class ax::Node"
    // the + 6 gets rid of the class prefix
    // "class ax::Node" + 6 == "ax::Node"
    return { name + 6 };
}

#elif AX_HAS_CXXABI

std::string Inspector::demangle(const char* mangled_name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status), std::free};
    return (status == 0) ? res.get() : mangled_name;
}

#else

std::string Inspector::demangle(const char* name)
{
    return { name };
}

#endif

std::string Inspector::getNodeTypeName(Node* node)
{
    return demangle(typeid(*node).name());
}

void Inspector::drawTreeRecursive(Node* node, int index)
{
    std::string str = fmt::format("[{}] {}", index, getNodeTypeName(node));

    if (node->getTag() != -1)
    {
        fmt::format_to(std::back_inserter(str), " ({})", node->getTag());
    }

    const auto nodeName = node->getName();
    if (!nodeName.empty())
    {
        fmt::format_to(std::back_inserter(str), " \"{}\"", nodeName);
    }

    const auto childrenCount = node->getChildrenCount();
    if (childrenCount != 0)
    {
        fmt::format_to(std::back_inserter(str), " {{{}}}", childrenCount);
    }

    auto flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;
    if (_selected_node == node)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (node->getChildrenCount() == 0)
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    const bool is_open = ImGui::TreeNodeEx(node, flags, "%s", str.c_str());

    if (ImGui::IsItemClicked())
    {
        if (node == _selected_node && ImGui::GetIO().KeyAlt)
        {
            _selected_node = nullptr;
        }
        else
        {
            _selected_node = node;
        }
    }

    if (is_open)
    {
        const auto &children = node->getChildren();
        for (int i = 0; auto* child : children)
        {
            if(!child)
            {
                continue;
            }

            drawTreeRecursive(child, i);
            i++;
        }
        ImGui::TreePop();
    }
}

void Inspector::drawProperties()
{
    if (_selected_node == nullptr)
    {
        ImGui::Text("Select a node to edit its properties :-)");
        return;
    }

    if (_selected_node->getReferenceCount() <= 1)
    {
        // Node no longer exists in the scene, and we're holding the only reference to it, so release it
        _selected_node = nullptr;
        return;
    }

    if (ImGui::Button("Delete"))
    {
        _selected_node->removeFromParentAndCleanup(true);
        _selected_node = nullptr;
        return;
    }

    // ImGui::SameLine();
    //
    // if (ImGui::Button("Add Child"))
    //{
    //     ImGui::OpenPopup("Add Child");
    // }
    //
    // if (ImGui::BeginPopupModal("Add Child"))
    //{
    //     static int item = 0;
    //     ImGui::Combo("Node", &item, "Node\0LabelBMFont\0LabelTTF\0Sprite\0MenuItemSpriteExtra\0");
    //
    //     static int tag = -1;
    //     ImGui::InputInt("Tag", &tag);
    //
    //     static char text[256]{0};
    //     if (item == 1)
    //     {
    //         static char labelFont[256]{0};
    //         ImGui::InputText("Text", text, 256);
    //         ImGui::InputText("Font", labelFont, 256);
    //     }
    //     static int fontSize = 20;
    //     if (item == 2)
    //     {
    //         ImGui::InputText("Text", text, 256);
    //         ImGui::InputInt("Font Size", &fontSize);
    //     }
    //     static bool frame = false;
    //     if (item == 3 || item == 4)
    //     {
    //         ImGui::InputText("Texture", text, 256);
    //         ImGui::Checkbox("Frame", &frame);
    //     }
    //
    //     ImGui::Separator();
    //     ImGui::SameLine();
    //     if (ImGui::Button("Cancel"))
    //     {
    //         ImGui::CloseCurrentPopup();
    //     }
    //     ImGui::EndPopup();
    // }

    ImGui::Text("Addr: %p", _selected_node.get());

    for (auto&& propertyHandler : _propertyHandlers)
    {
        if (propertyHandler.second->isSupportedType(_selected_node))
        {
            propertyHandler.second->drawProperties(_selected_node);
        }
    }
}

void Inspector::openForScene(Scene* target)
{
    if (_target == target)
        return;

    _target = target;

    if (_target == nullptr)
    {
        close();
        return;
    }

    auto* presenter = ImGuiPresenter::getInstance();
    presenter->addFont(FileUtils::getInstance()->fullPathForFilename(_fontPath), _fontSize);
    presenter->enableDPIScale();
    presenter->addRenderLoop("#insp", AX_CALLBACK_0(Inspector::mainLoop , this), target);
}

void Inspector::openForCurrentScene()
{
    openForScene(Director::getInstance()->getRunningScene());
}

void Inspector::close()
{
    _selected_node = nullptr;
    _target = nullptr;

    auto presenter = ImGuiPresenter::getInstance();
    presenter->removeRenderLoop("#insp");
    presenter->clearFonts();
}

bool Inspector::addPropertyHandler(std::string_view handlerId, std::unique_ptr<InspectPropertyHandler> handler)
{
    auto result = _propertyHandlers.try_emplace(std::string(handlerId), std::move(handler));
    return result.second;
}

void Inspector::removePropertyHandler(const std::string& handlerId)
{
    _propertyHandlers.erase(handlerId);
}

void Inspector::setAutoAddToScenes(bool autoAdd)
{
    if (_autoAddToScenes == autoAdd)
        return;

    _autoAddToScenes = autoAdd;
    close();

    if (_autoAddToScenes)
    {
        openForCurrentScene();
    }
}

void Inspector::mainLoop()
{
    if(!_target)
    {
        close();
        return;
    }

    if (ImGui::Begin("Inspector"))
    {
        const auto avail = ImGui::GetContentRegionAvail();
        if (ImGui::BeginChild("node.explorer.tree", ImVec2(avail.x * 0.5f, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            drawTreeRecursive(_target);
        }
        ImGui::EndChild();

        ImGui::SameLine();

        if (ImGui::BeginChild("node.explorer.options"))
        {
            drawProperties();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

NS_AX_EXT_END
