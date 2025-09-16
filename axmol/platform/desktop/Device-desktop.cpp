#include "axmol/platform/Device.h"
#include "GLFW/glfw3.h"

namespace ax
{
int Device::getDisplayRefreshRate()
{
    return glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
}
}  // namespace ax
