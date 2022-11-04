
#pragma once

#include "../LLGI.Texture.h"
#include "LLGI.GraphicsMetal.h"

namespace LLGI
{

class TextureMetal : public Texture
{
private:
	ReferenceObject* owner_ = nullptr;
	std::vector<uint8_t> data;

	id<MTLTexture> texture_ = nullptr;
	Vec3I size_;
	bool fromExternal_ = false;

	TextureParameter parameter_;

	bool Initialize(id<MTLDevice> device, const TextureParameter& parameter);
	void Write(const uint8_t* data);

public:
	TextureMetal();
	~TextureMetal() override;

	bool Initialize(GraphicsMetal* owner, const TextureParameter& parameter);
	bool Initialize(GraphicsMetal* owner, id<MTLTexture> externalTexture);
	void Reset(id<MTLTexture> nativeTexture);
	void* Lock() override;
	void Unlock() override;
	Vec2I GetSizeAs2D() const override;

	const TextureParameter& GetParameter() const { return parameter_; }

	id<MTLTexture>& GetTexture() { return texture_; }
};

} // namespace LLGI
