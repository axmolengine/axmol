#include "OpenGLState.h"

namespace ax::backend {

namespace
{
auto g_defaultOpenGLState = std::make_unique<OpenGLState>();
}

OpenGLState* __gl = g_defaultOpenGLState.get();

void OpenGLState::reset()
{
    g_defaultOpenGLState = std::make_unique<OpenGLState>();
    __gl = g_defaultOpenGLState.get();
}

}
