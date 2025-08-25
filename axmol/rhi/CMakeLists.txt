set(_AX_RHI_HEADER
  rhi/axslc-spec.h
  rhi/axmol-rhi.h
  rhi/RHITypes.h
  rhi/Buffer.h
  rhi/CommandBuffer.h
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
  rhi/CommandBuffer.cpp
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
    rhi/opengl/CommandBufferGL.h
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
    rhi/opengl/CommandBufferGL.cpp
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
    rhi/metal/CommandBufferMTL.h
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
    rhi/metal/CommandBufferMTL.mm
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
    rhi/d3d/CommandBufferD3D.h
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
    rhi/d3d/CommandBufferD3D.cpp
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
endif()
