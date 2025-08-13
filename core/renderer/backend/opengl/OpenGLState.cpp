#include "OpenGLState.h"

namespace ax::backend {

namespace
{
std::unique_ptr<OpenGLState> g_defaultOpenGLState;
}

AX_DLL OpenGLState* __gl{nullptr};

OpenGLState::OpenGLState()
{
    // NOT GLES2.0, need generate VAO clearly
    glGenVertexArrays(1, &_defaultVAO);
    glBindVertexArray(_defaultVAO);
}

void OpenGLState::reset()
{
    g_defaultOpenGLState = std::make_unique<OpenGLState>();
    __gl = g_defaultOpenGLState.get();
}

}
