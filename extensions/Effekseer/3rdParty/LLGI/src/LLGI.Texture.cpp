#include "LLGI.Texture.h"

namespace LLGI
{

void* Texture::Lock() { return nullptr; }

void Texture::Unlock() {}

Vec2I Texture::GetSizeAs2D() const { return Vec2I(); }

bool Texture::IsRenderTexture() const { return false; }

bool Texture::IsDepthTexture() const { return false; }

TextureFormatType Texture::GetFormat() const { return format_; }

} // namespace LLGI
