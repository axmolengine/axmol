#include "OpenGLState.h"

NS_AX_BACKEND_BEGIN

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

NS_AX_BACKEND_END
