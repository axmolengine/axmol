# cocos2d-x-imgui

This project is [ImGui](https://github.com/ocornut/imgui) binding for [cocos2d-x](https://github.com/cocos2d/cocos2d-x), with most things bind to lua.

Currently it works with [cocos2d-x 4.0](https://github.com/cocos2d/cocos2d-x/releases/tag/cocos2d-x-4.0) and ImGui docking branch. [Branch v3](https://github.com/Xrysnow/cocos2d-x-imgui/tree/v3) can work with cocos2d-x 3.x, but will not be maintained.

## How to use

* Include headers.
```cpp
#include "CCImGuiLayer.h"
```

* Add ImGui layer (usually on top).
```cpp
std::string layerName = "ImGUILayer";
auto order = INT_MAX;
auto layer = ImGuiLayer::create();
Director::getInstance()->getRunningScene()->addChild(layer, order, layerName);
```

* Keep the layer when scene changes.
```cpp
auto e = Director::getInstance()->getEventDispatcher();
auto detached = false;
e->addCustomEventListener(Director::EVENT_BEFORE_SET_NEXT_SCENE, [&](EventCustom*){
	layer = dynamic_cast<ImGuiLayer*>(Director::getInstance()->getRunningScene()->getChildByName(layerName));
	if (layer) {
		layer->retain();
		layer->removeFromParent();
		detached = true;
	}
});
e->addCustomEventListener(Director::EVENT_AFTER_SET_NEXT_SCENE, [&](EventCustom*){
	if (layer && detached) {
		Director::getInstance()->getRunningScene()->addChild(layer, order, layerName);
		layer->release();
		detached = false;
	}
});
```

* Use ImGui.
```cpp
// enable docking
auto& io = ImGui::GetIO();
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
auto sp = Sprite::create("filename.png");
layer->addChild(sp);
// add ui callbacks
CCIMGUI::getInstance()->addCallback([=](){
	ImGui::Text("Hello, world!");
	// create button with Sprite, auto pushID / popID with texture id
	CCIMGUI::getInstance()->imageButton(sp, ImVec2(0, 0));
}, "hello");
// remove ui callbacks to stop rendering
CCIMGUI::getInstance()->removeCallback("hello");
// add chinese font
io.Fonts->AddFontFromFileTTF("path/to/font.ttf", 16.0f, 0, io.Fonts->GetGlyphRangesChineseFull());
```

* Enable lua binding.
```cpp
#include "lua-bindings/imgui_lua.hpp"
auto L = LuaEngine::getInstance()->getLuaStack()->getLuaState();
luaopen_imgui(L);
```

* Use in lua. Note that all functions are start with a lower letter. The `lua_doc` folder contains docs for lua, just include it to your lua IDE (docs are in [EmmyLua](https://github.com/EmmyLua/IntelliJ-EmmyLua) format). The `lua_src` folder contains helpers and wrappers for lua development.
	* Without helper
	```lua
	local ret, buf, float = nil, '', 0
	imgui.draw = function()
		-- new window
		if imgui.begin("Window From Lua") then
			-- text
			imgui.text("Hello, World!")
			-- text button
			imgui.button("text button")
			-- input text
			ret, buf = imgui.inputText("input", buf)
			-- slider
			ret, float = imgui.sliderFloat("float", float, 0, 8)
			-- end window
			imgui.endToLua()
		end
	end
    -- check error somewhere
	if imgui.error then
		error(imgui.error)
	end
	```
	* With helper
	```lua
	require('imgui.__init__')
	local wi = require('imgui.Widget')
	local im = imgui
	local la = im.on()
	local sp = cc.Sprite:create('filename.png'):setVisible(false)
	la:addChild(sp) -- retain sprite
	local flags = im.WindowFlags.HorizontalScrollbar
	local content = wi.Widget(function()
		imgui.text("Hello, World!")
		im.image(sp)
	end)
	la:addChild(wi.Window('Window From Lua', nil, flags)
		:addChild(content)
		:addChild(wi.TreeNode('Tree')
			:addChild(wi.Text('text', cc.c3b(255, 0, 0), 'bullet'))
			:addChild(im.sameLine)
			:addChild(wi.Button('button'))
			:addChild(im.separator)
			:addChild(wi.Checkbox('checkbox'))
		)
	)
	```

## Thanks

Thanks to all previous works:

* https://github.com/namkazt/cocos2d-x-ImGui
* https://github.com/Mjarkiew/cocos2dx-imgui
* https://github.com/c0i/imguix
