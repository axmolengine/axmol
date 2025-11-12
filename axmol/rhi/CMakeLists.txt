set(_AX_RHI_HEADER
  rhi/axslc-spec.h
  rhi/axmol-rhi.h
  rhi/RHITypes.h
  rhi/Buffer.h
  rhi/RenderContext.h
  rhi/DepthStencilState.h
  rhi/DriverBase.h
  rhi/PixelBufferDesc.h
  rhi/RHIUtils.h
  rhi/Program.h
  rhi/ProgramState.h
  rhi/RenderPassDesc.h
  rhi/RenderPipeline.h
  rhi/RenderTarget.h
  rhi/ShaderCache.h
  rhi/ShaderModule.h
  rhi/Texture.h
  rhi/VertexLayout.h
  renderer/VertexLayoutManager.h
  rhi/SamplerCache.h
)

set(_AX_RHI_SRC
  rhi/RenderContext.cpp
  rhi/DepthStencilState.cpp
  rhi/DriverBase.cpp
  rhi/ShaderModule.cpp
  rhi/Texture.cpp
  rhi/RHIUtils.cpp
  rhi/VertexLayout.cpp
  renderer/VertexLayoutManager.cpp
  rhi/Program.cpp
  rhi/ProgramState.cpp
  rhi/ShaderCache.cpp
  rhi/RenderPassDesc.cpp
  rhi/SamplerCache.cpp
)

if(AX_RENDER_API STREQUAL "gl")
  list(APPEND _AX_RHI_HEADER
    rhi/opengl/OpenGLState.h
    rhi/opengl/BufferGL.h
    rhi/opengl/RenderContextGL.h
    rhi/opengl/DepthStencilStateGL.h
    rhi/opengl/DriverGL.h
    rhi/opengl/MacrosGL.h
    rhi/opengl/ProgramGL.h
    rhi/opengl/RenderPipelineGL.h
    rhi/opengl/RenderTargetGL.h
    rhi/opengl/ShaderModuleGL.h
    rhi/opengl/TextureGL.h
    rhi/opengl/UtilsGL.h
    rhi/opengl/VertexLayoutGL.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/opengl/OpenGLState.cpp
    rhi/opengl/BufferGL.cpp
    rhi/opengl/RenderContextGL.cpp
    rhi/opengl/DepthStencilStateGL.cpp
    rhi/opengl/DriverGL.cpp
    rhi/opengl/ProgramGL.cpp
    rhi/opengl/RenderPipelineGL.cpp
    rhi/opengl/ShaderModuleGL.cpp
    rhi/opengl/TextureGL.cpp
    rhi/opengl/UtilsGL.cpp
    rhi/opengl/RenderTargetGL.cpp
    rhi/opengl/VertexLayoutGL.cpp
  )
elseif(AX_RENDER_API STREQUAL "mtl")
  list(APPEND _AX_RHI_HEADER
    rhi/metal/BufferMTL.h
    rhi/metal/BufferManager.h
    rhi/metal/RenderContextMTL.h
    rhi/metal/RenderTargetMTL.h
    rhi/metal/DepthStencilStateMTL.h
    rhi/metal/DriverMTL.h
    rhi/metal/RenderPipelineMTL.h
    rhi/metal/ShaderModuleMTL.h
    rhi/metal/TextureMTL.h
    rhi/metal/UtilsMTL.h
    rhi/metal/ProgramMTL.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/metal/BufferMTL.mm
    rhi/metal/BufferManager.mm
    rhi/metal/RenderContextMTL.mm
    rhi/metal/RenderTargetMTL.mm
    rhi/metal/DepthStencilStateMTL.mm
    rhi/metal/DriverMTL.mm
    rhi/metal/RenderPipelineMTL.mm
    rhi/metal/ShaderModuleMTL.mm
    rhi/metal/TextureMTL.mm
    rhi/metal/UtilsMTL.mm
    rhi/metal/ProgramMTL.mm
  )
elseif(AX_RENDER_API STREQUAL "d3d")
    list(APPEND _AX_RHI_HEADER
    rhi/d3d/BufferD3D.h
    rhi/d3d/RenderContextD3D.h
    rhi/d3d/RenderTargetD3D.h
    rhi/d3d/DepthStencilStateD3D.h
    rhi/d3d/DriverD3D.h
    rhi/d3d/RenderPipelineD3D.h
    rhi/d3d/ShaderModuleD3D.h
    rhi/d3d/TextureD3D.h
    rhi/d3d/UtilsD3D.h
    rhi/d3d/ProgramD3D.h
    rhi/d3d/VertexLayoutD3D.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/d3d/BufferD3D.cpp
    rhi/d3d/RenderContextD3D.cpp
    rhi/d3d/RenderTargetD3D.cpp
    rhi/d3d/DepthStencilStateD3D.cpp
    rhi/d3d/DriverD3D.cpp
    rhi/d3d/RenderPipelineD3D.cpp
    rhi/d3d/ShaderModuleD3D.cpp
    rhi/d3d/TextureD3D.cpp
    rhi/d3d/UtilsD3D.cpp
    rhi/d3d/ProgramD3D.cpp
    rhi/d3d/VertexLayoutD3D.cpp
  )
elseif(AX_RENDER_API STREQUAL "vk")
    list(APPEND _AX_RHI_HEADER
    rhi/vulkan/BufferVK.h
    rhi/vulkan/RenderContextVK.h
    rhi/vulkan/RenderTargetVK.h
    rhi/vulkan/DepthStencilStateVK.h
    rhi/vulkan/DriverVK.h
    rhi/vulkan/RenderPipelineVK.h
    rhi/vulkan/ShaderModuleVK.h
    rhi/vulkan/TextureVK.h
    rhi/vulkan/UtilsVK.h
    rhi/vulkan/ProgramVK.h
    rhi/vulkan/VertexLayoutVK.h
    rhi/vulkan/SemaphorePoolVK.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/vulkan/BufferVK.cpp
    rhi/vulkan/RenderContextVK.cpp
    rhi/vulkan/RenderTargetVK.cpp
    rhi/vulkan/DepthStencilStateVK.cpp
    rhi/vulkan/DriverVK.cpp
    rhi/vulkan/RenderPipelineVK.cpp
    rhi/vulkan/ShaderModuleVK.cpp
    rhi/vulkan/TextureVK.cpp
    rhi/vulkan/UtilsVK.cpp
    rhi/vulkan/ProgramVK.cpp
    rhi/vulkan/VertexLayoutVK.cpp
    rhi/vulkan/SemaphorePoolVK.cpp
  )
endif()
