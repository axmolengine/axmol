
#include <TargetConditionals.h>

#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
#import <Cocoa/Cocoa.h>
#endif

#import <MetalKit/MetalKit.h>

#import "LLGI.Metal_Impl.h"

namespace LLGI
{

MTLPixelFormat ConvertFormat(TextureFormatType format)
{
	if (format == TextureFormatType::R8G8B8A8_UNORM)
	{
		return MTLPixelFormatRGBA8Unorm;
	}
	else if (format == TextureFormatType::R8G8B8A8_UNORM_SRGB)
	{
		return MTLPixelFormatRGBA8Unorm_sRGB;
	}
	else if (format == TextureFormatType::B8G8R8A8_UNORM)
	{
		return MTLPixelFormatBGRA8Unorm;
	}
	else if (format == TextureFormatType::R8_UNORM)
	{
		return MTLPixelFormatR8Unorm;
	}
	else if (format == TextureFormatType::R16G16B16A16_FLOAT)
	{
		return MTLPixelFormatRGBA16Float;
	}
	else if (format == TextureFormatType::R32G32B32A32_FLOAT)
	{
		return MTLPixelFormatRGBA32Float;
	}
	else if (format == TextureFormatType::R16G16_FLOAT)
	{
		return MTLPixelFormatRG32Float;
	}
#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
	else if (format == TextureFormatType::BC1)
	{
		return MTLPixelFormatBC1_RGBA;
	}
	else if (format == TextureFormatType::BC1_SRGB)
	{
		return MTLPixelFormatBC1_RGBA_sRGB;
	}
	else if (format == TextureFormatType::BC2)
	{
		return MTLPixelFormatBC2_RGBA;
	}
	else if (format == TextureFormatType::BC2_SRGB)
	{
		return MTLPixelFormatBC2_RGBA_sRGB;
	}
	else if (format == TextureFormatType::BC3)
	{
		return MTLPixelFormatBC3_RGBA;
	}
	else if (format == TextureFormatType::BC3_SRGB)
	{
		return MTLPixelFormatBC3_RGBA_sRGB;
	}
	else if (format == TextureFormatType::D24S8)
	{
		return MTLPixelFormatDepth24Unorm_Stencil8;
	}
#endif
	else if (format == TextureFormatType::D32)
	{
		return MTLPixelFormatDepth32Float;
	}
	else if (format == TextureFormatType::D32S8)
	{
		return MTLPixelFormatDepth32Float_Stencil8;
	}
	else if (format == TextureFormatType::Unknown)
	{
		return MTLPixelFormatInvalid;
	}

	return MTLPixelFormatInvalid;
}

TextureFormatType ConvertFormat(MTLPixelFormat format)
{
	if (format == MTLPixelFormatRGBA8Unorm)
	{
		return TextureFormatType::R8G8B8A8_UNORM;
	}
	else if (format == MTLPixelFormatRGBA8Unorm_sRGB)
	{
		return TextureFormatType::R8G8B8A8_UNORM_SRGB;
	}
	else if (format == MTLPixelFormatBGRA8Unorm)
	{
		return TextureFormatType::B8G8R8A8_UNORM;
	}
	else if (format == MTLPixelFormatR8Unorm)
	{
		return TextureFormatType::R8_UNORM;
	}
	else if (format == MTLPixelFormatRGBA16Float)
	{
		return TextureFormatType::R16G16B16A16_FLOAT;
	}
	else if (format == MTLPixelFormatRGBA32Float)
	{
		return TextureFormatType::R32G32B32A32_FLOAT;
	}
	else if (format == MTLPixelFormatRG32Float)
	{
		return TextureFormatType::R16G16_FLOAT;
	}
#if !(TARGET_OS_IPHONE) && !(TARGET_OS_SIMULATOR)
	else if (format == MTLPixelFormatBC1_RGBA)
	{
		return TextureFormatType::BC1;
	}
	else if (format == MTLPixelFormatBC1_RGBA_sRGB)
	{
		return TextureFormatType::BC1_SRGB;
	}
	else if (format == MTLPixelFormatBC2_RGBA)
	{
		return TextureFormatType::BC2;
	}
	else if (format == MTLPixelFormatBC2_RGBA_sRGB)
	{
		return TextureFormatType::BC2_SRGB;
	}
	else if (format == MTLPixelFormatBC3_RGBA)
	{
		return TextureFormatType::BC3;
	}
	else if (format == MTLPixelFormatBC3_RGBA_sRGB)
	{
		return TextureFormatType::BC3_SRGB;
	}
	else if (format == MTLPixelFormatDepth24Unorm_Stencil8)
	{
		return TextureFormatType::D24S8;
	}
#endif
	else if (format == MTLPixelFormatDepth32Float)
	{
		return TextureFormatType::D32;
	}
	else if (format == MTLPixelFormatDepth32Float_Stencil8)
	{
		return TextureFormatType::D32S8;
	}
	else if (format == MTLPixelFormatInvalid)
	{
		return TextureFormatType::Unknown;
	}

	return TextureFormatType::Unknown;
}

}
