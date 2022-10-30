
#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

class Texture : public ReferenceObject
{
private:
protected:
	TextureType type_ = TextureType::Unknown;
	TextureFormatType format_ = TextureFormatType::Unknown;
	int32_t samplingCount_ = 1;
	int32_t mipmapCount_ = 1;

public:
	Texture() = default;
	~Texture() override = default;

	/*[[deprecated("use CommandList::SetImageData2D.")]]*/ virtual void* Lock();

	/*[[deprecated("use CommandList::SetImageData2D.")]]*/ virtual void Unlock();

	/**
		@brief	Lock an image on mipmap
	*/
	virtual void* Lock(int32_t mipmapLevel) { return nullptr; }

	/**
		@brief	Generate mipmaps based on level zero.
	*/
	virtual void GenerateMipMaps() {}

	virtual Vec2I GetSizeAs2D() const;
	[[deprecated("use GetType.")]] virtual bool IsRenderTexture() const;
	[[deprecated("use GetType.")]] virtual bool IsDepthTexture() const;

	TextureType GetType() const { return type_; }

	virtual TextureFormatType GetFormat() const;

	int32_t GetSamplingCount() const { return samplingCount_; }

	int32_t GetMipmapCount() const { return mipmapCount_; }
};

} // namespace LLGI
