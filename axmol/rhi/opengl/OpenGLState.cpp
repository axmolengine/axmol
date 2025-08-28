#include "axmol/rhi/opengl/OpenGLState.h"

namespace ax::rhi::gl
{

namespace
{
std::unique_ptr<OpenGLState> g_defaultOpenGLState;
}

AX_DLL OpenGLState* __state{nullptr};

OpenGLState::OpenGLState()
{
    // NOT GLES2.0, need generate VAO clearly
    glGenVertexArrays(1, &_defaultVAO);
    glBindVertexArray(_defaultVAO);
}

void OpenGLState::reset()
{
    g_defaultOpenGLState = std::make_unique<OpenGLState>();
    __state              = g_defaultOpenGLState.get();
}

}  // namespace ax::rhi::gl
