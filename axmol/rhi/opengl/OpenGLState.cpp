#include "axmol/rhi/opengl/OpenGLState.h"
#include <unordered_set>

namespace ax::rhi::gl
{

static std::unique_ptr<OpenGLState> g_defaultOpenGLState;

AX_DLL OpenGLState* __state{nullptr};

void OpenGLState::reset()
{
    g_defaultOpenGLState = std::make_unique<OpenGLState>();
    __state              = g_defaultOpenGLState.get();
}

OpenGLState::OpenGLState() {}

OpenGLState::~OpenGLState() {}

}  // namespace ax::rhi::gl
