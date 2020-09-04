#include "CCImGuiLayer.h"
#include "imgui/imgui.h"
#include "imgui_impl_cocos2dx.h"
#include "CCImGuiEXT.h"

NS_CC_EXT_BEGIN

bool ImGuiLayer::init()
{
	if (!Layer::init() || !ImGuiEXT::getInstance())
		return false;

#ifdef CC_PLATFORM_PC
	// note: when at the first click to focus the window, this will not take effect
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch* touch, Event*) -> bool {
		if (!_visible)
			return false;
		return ImGui::IsAnyWindowHovered();
	};
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	// add by halx99
	auto stopAnyMouse = [=](EventMouse* event) {
		if (ImGui::IsAnyWindowHovered()) {
			event->stopPropagation();
			cocos2d::log("!!!EventMouse should stop by ImGuiLayer");
		}
	};
	auto mouseListener = EventListenerMouse::create();
	mouseListener->onMouseDown = mouseListener->onMouseUp = stopAnyMouse;
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);
#endif
	// add an empty sprite to avoid render problem
	const auto sp = Sprite::create();
	sp->setGlobalZOrder(1);
	sp->setOpacity(0);
	addChild(sp, 1);

	return true;
}

void ImGuiLayer::visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
	Layer::visit(renderer, parentTransform, parentFlags);
	if(_visible) frame();
}

void ImGuiLayer::frame()
{
	// create frame
	ImGui_ImplCocos2dx_NewFrame();

	// draw all gui
	ImGuiEXT::getInstance()->onDraw();

	// render
	ImGui::Render();

	ImGui_ImplCocos2dx_RenderDrawData(ImGui::GetDrawData());
	ImGui_ImplCocos2dx_RenderPlatform();
}

NS_CC_EXT_END
