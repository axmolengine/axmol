
#pragma once

#include "../LLGI.Shader.h"
#import <MetalKit/MetalKit.h>

namespace LLGI
{

class GraphicsMetal;

class ShaderMetal : public Shader
{
private:
	GraphicsMetal* graphics_ = nullptr;
	id<MTLLibrary> library_ = nullptr;

public:
	ShaderMetal();
	~ShaderMetal() override;
	bool Initialize(GraphicsMetal* graphics, DataStructure* data, int32_t count);

	id<MTLLibrary>& GetLibrary() { return library_; }
};

} // namespace LLGI
