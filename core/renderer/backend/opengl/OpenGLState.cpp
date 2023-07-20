#include "OpenGLState.h"

NS_AX_BACKEND_BEGIN

static auto defaultOpenGLState = std::make_unique<OpenGLState>();

OpenGLState* __gl = defaultOpenGLState.get();

NS_AX_BACKEND_END
