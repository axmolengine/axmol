# ImGuiEXT of EGNX
Sync from https://github.com/Xrysnow/cocos2d-x-imgui and do a little changes

## Improvements
* Simple API, use add/remove renderLoop present ImGui GUI widgets
* Optimize call pipeline flow, support add/remove Node to Scene at ImGui render loop without container iterator damage
* Calculate deltaTime at ```ImGui_ImplCocos2dx_NewFrame``` to avoid error when ```cc.Director``` paused
* Refine ```Init/Shutdown```, Restore all callbacks for glfw to solve recreate ```ImGuiEXT``` instance support
* Use FOURCC for key of ImGui render loop

## How to use
1. Enable by cmake option -DBUILD_EXTENSION_IMGUIEXT=ON
2. 
```cpp
#include "ImGuiEXT/CCImGuiEXT.h"
USING_NS_CC;
USING_NS_CC_EXT;

class GameScene : public Scene {
public:
    void onEnter() override
    {
        ImGuiEXT::getInstance()->addRenderLoop("#im01", this, CC_CALLBACK_0(GameScene::onImGuiDraw, this));
    }
    void onExit() override
    {
        ImGuiEXT::getInstance()->removeRenderLoop("#im01");
    }
    void onImGuiDraw()
    {
        ImGui::Begin("window");
        ImGui::Text("FPS=%.1f", 1.f / ImGui::GetIO().DeltaTime);
        ImGui::End();
    }
}

```
More about use imgui widgets, please see: https://github.com/ocornut/imgui

## Tested devices
* win32
* macOS

## Known issues
* Can't enable muti-viewports on macOS, so we disable it for macOS
