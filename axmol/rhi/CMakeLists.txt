set(_AX_RHI_HEADER
  rhi/axslc-spec.h
  rhi/axmol-rhi.h
  rhi/RHITypes.h
  rhi/Buffer.h
  rhi/GraphicsContext.h
  rhi/DepthStencilState.h
  rhi/GraphicsDevice.h
  rhi/PixelBufferDesc.h
  rhi/RHIUtils.h
  rhi/Program.h
  rhi/ProgramState.h
  rhi/RenderPassDesc.h
  rhi/GraphicsPipeline.h
  rhi/ComputePipeline.h
  rhi/RenderTarget.h
  rhi/ShaderCache.h
  rhi/ShaderModule.h
  rhi/Texture.h
  rhi/VertexLayout.h
  rhi/SamplerRegistry.h
  rhi/GraphicsDeviceFactory.h
  rhi/GraphicsCore.h
  rhi/VulkanInterop.h
  rhi/PipelineDesc.h
)

set(_AX_RHI_SRC
  rhi/GraphicsContext.cpp
  rhi/ComputePipeline.cpp
  rhi/DepthStencilState.cpp
  rhi/GraphicsDevice.cpp
  rhi/ShaderModule.cpp
  rhi/Texture.cpp
  rhi/RHIUtils.cpp
  rhi/VertexLayout.cpp
  rhi/Program.cpp
  rhi/ProgramState.cpp
  rhi/RenderTarget.cpp
  rhi/ShaderCache.cpp
  rhi/RenderPassDesc.cpp
  rhi/SamplerRegistry.cpp
  rhi/GraphicsCore.cpp
)

if(AX_ENABLE_GL)
  list(APPEND _AX_RHI_HEADER
    rhi/opengl/OpenGLState.h
    rhi/opengl/BufferGL.h
    rhi/opengl/GraphicsContextGL.h
    rhi/opengl/DepthStencilStateGL.h
    rhi/opengl/GraphicsDeviceGL.h
    rhi/opengl/MacrosGL.h
    rhi/opengl/ProgramGL.h
    rhi/opengl/GraphicsPipelineGL.h
    rhi/opengl/ComputePipelineGL.h
    rhi/opengl/RenderTargetGL.h
    rhi/opengl/ShaderModuleGL.h
    rhi/opengl/TextureGL.h
    rhi/opengl/UtilsGL.h
    rhi/opengl/VertexLayoutGL.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/opengl/OpenGLState.cpp
    rhi/opengl/BufferGL.cpp
    rhi/opengl/GraphicsContextGL.cpp
    rhi/opengl/DepthStencilStateGL.cpp
    rhi/opengl/GraphicsDeviceGL.cpp
    rhi/opengl/ProgramGL.cpp
    rhi/opengl/GraphicsPipelineGL.cpp
    rhi/opengl/ShaderModuleGL.cpp
    rhi/opengl/TextureGL.cpp
    rhi/opengl/UtilsGL.cpp
    rhi/opengl/RenderTargetGL.cpp
    rhi/opengl/VertexLayoutGL.cpp
  )
endif()

if(AX_ENABLE_MTL)
  list(APPEND _AX_RHI_HEADER
    rhi/metal/BufferMTL.h
    rhi/metal/BufferManager.h
    rhi/metal/GraphicsContextMTL.h
    rhi/metal/RenderTargetMTL.h
    rhi/metal/DepthStencilStateMTL.h
    rhi/metal/GraphicsDeviceMTL.h
    rhi/metal/GraphicsPipelineMTL.h
    rhi/metal/ComputePipelineMTL.h
    rhi/metal/ShaderModuleMTL.h
    rhi/metal/TextureMTL.h
    rhi/metal/UtilsMTL.h
    rhi/metal/ProgramMTL.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/metal/BufferMTL.mm
    rhi/metal/BufferManager.mm
    rhi/metal/GraphicsContextMTL.mm
    rhi/metal/RenderTargetMTL.mm
    rhi/metal/DepthStencilStateMTL.mm
    rhi/metal/GraphicsDeviceMTL.mm
    rhi/metal/GraphicsPipelineMTL.mm
    rhi/metal/ComputePipelineMTL.mm
    rhi/metal/ShaderModuleMTL.mm
    rhi/metal/TextureMTL.mm
    rhi/metal/UtilsMTL.mm
    rhi/metal/ProgramMTL.mm
  )
endif()

if(AX_ENABLE_D3D11)
    list(APPEND _AX_RHI_HEADER
    rhi/d3d11/Buffer11.h
    rhi/d3d11/GraphicsContext11.h
    rhi/d3d11/RenderTarget11.h
    rhi/d3d11/DepthStencilState11.h
    rhi/d3d11/GraphicsDevice11.h
    rhi/d3d11/GraphicsPipeline11.h
    rhi/d3d11/ComputePipeline11.h
    rhi/d3d11/ShaderModule11.h
    rhi/d3d11/Texture11.h
    rhi/d3d11/Program11.h
    rhi/d3d11/VertexLayout11.h
    rhi/DXUtils.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/d3d11/Buffer11.cpp
    rhi/d3d11/GraphicsContext11.cpp
    rhi/d3d11/RenderTarget11.cpp
    rhi/d3d11/DepthStencilState11.cpp
    rhi/d3d11/GraphicsDevice11.cpp
    rhi/d3d11/GraphicsPipeline11.cpp
    rhi/d3d11/ShaderModule11.cpp
    rhi/d3d11/Texture11.cpp
    rhi/d3d11/Program11.cpp
    rhi/d3d11/VertexLayout11.cpp
    rhi/DXUtils.cpp
  )
endif()

if(AX_ENABLE_D3D12)
    list(APPEND _AX_RHI_HEADER
    rhi/d3d12/Buffer12.h
    rhi/d3d12/GraphicsContext12.h
    rhi/d3d12/RenderTarget12.h
    rhi/d3d12/DepthStencilState12.h
    rhi/d3d12/GraphicsDevice12.h
    rhi/d3d12/GraphicsPipeline12.h
    rhi/d3d12/ShaderModule12.h
    rhi/d3d12/Texture12.h
    rhi/d3d12/Program12.h
    rhi/d3d12/VertexLayout12.h
rhi/d3d12/DescriptorHeapAllocator12.h
rhi/d3d12/UploadBufferAllocator12.h
rhi/d3d12/ComputePipeline12.h
    rhi/DXUtils.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/d3d12/Buffer12.cpp
    rhi/d3d12/GraphicsContext12.cpp
    rhi/d3d12/RenderTarget12.cpp
    rhi/d3d12/DepthStencilState12.cpp
    rhi/d3d12/GraphicsDevice12.cpp
    rhi/d3d12/GraphicsPipeline12.cpp
    rhi/d3d12/ShaderModule12.cpp
    rhi/d3d12/Texture12.cpp
    rhi/d3d12/Program12.cpp
    rhi/d3d12/VertexLayout12.cpp
rhi/d3d12/DescriptorHeapAllocator12.cpp
rhi/d3d12/UploadBufferAllocator12.cpp
rhi/d3d12/ComputePipeline12.cpp
    rhi/DXUtils.cpp
  )
endif()

if(AX_ENABLE_VK)
    list(APPEND _AX_RHI_HEADER
    rhi/vulkan/BufferVK.h
    rhi/vulkan/GraphicsContextVK.h
    rhi/vulkan/RenderTargetVK.h
    rhi/vulkan/DepthStencilStateVK.h
    rhi/vulkan/GraphicsDeviceVK.h
    rhi/vulkan/GraphicsPipelineVK.h
    rhi/vulkan/ShaderModuleVK.h
    rhi/vulkan/TextureVK.h
    rhi/vulkan/UtilsVK.h
    rhi/vulkan/ProgramVK.h
    rhi/vulkan/VertexLayoutVK.h
    rhi/vulkan/SemaphorePoolVK.h
    rhi/vulkan/ComputePipelineVK.h
  )

  list(APPEND _AX_RHI_SRC
    rhi/vulkan/BufferVK.cpp
    rhi/vulkan/GraphicsContextVK.cpp
    rhi/vulkan/RenderTargetVK.cpp
    rhi/vulkan/DepthStencilStateVK.cpp
    rhi/vulkan/GraphicsDeviceVK.cpp
    rhi/vulkan/GraphicsPipelineVK.cpp
    rhi/vulkan/ShaderModuleVK.cpp
    rhi/vulkan/TextureVK.cpp
    rhi/vulkan/UtilsVK.cpp
    rhi/vulkan/ProgramVK.cpp
    rhi/vulkan/VertexLayoutVK.cpp
    rhi/vulkan/SemaphorePoolVK.cpp
    rhi/vulkan/ComputePipelineVK.cpp
  )
endif()

if(APPLE)
  set_source_files_properties(rhi/GraphicsCore.cpp PROPERTIES LANGUAGE OBJCXX)
endif()
