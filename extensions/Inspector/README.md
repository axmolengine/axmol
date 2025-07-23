# Inspector

(Experimental)

- Visualize node tree in real time
- Edit node properties

## Usage

call `openForScene(ax::Scene*)` in your `onEnter` overload and `close` in your `onExit` overload. Dont forget to call Scene methods.
```cpp
#include "Inspector/Inspector.h"

class MyScene : public ax::Scene {
        void onEnter() override {
        ax::Scene::onEnter();
        ax::extension::Inspector::getInstance()->openForScene(this);
    }
    void onExit() override {
        ax::extension::Inspector::getInstance()->close();
        ax::Scene::onExit();
    }
};
```
