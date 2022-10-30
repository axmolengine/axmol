#include "LLGI.ShaderMetal.h"
#include "LLGI.GraphicsMetal.h"
#include "LLGI.Metal_Impl.h"
#import <MetalKit/MetalKit.h>

#define SUPPRESS_COMPILE_WARNINGS

namespace LLGI
{

ShaderMetal::ShaderMetal() {}

ShaderMetal::~ShaderMetal()
{
	if (library_ != nullptr)
	{
		[library_ release];
		library_ = nullptr;
	}
	SafeRelease(graphics_);
}

bool ShaderMetal::Initialize(GraphicsMetal* graphics, DataStructure* data, int32_t count)
{
	@autoreleasepool
	{
		SafeAddRef(graphics);
		SafeRelease(graphics_);
		graphics_ = graphics;
		auto g = static_cast<GraphicsMetal*>(graphics);

		auto device = g->GetDevice();

		if (data[0].Size < 7)
			return false;

		// check whether binary or code
		bool isCode = false;
		const char* code = static_cast<const char*>(data[0].Data);

		if (code[0] == 'm' || code[1] == 't' || code[2] == 'l' || code[3] == 'c' || code[4] == 'o' || code[5] == 'd' || code[6] == 'e')
		{
			isCode = true;
		}

		if (isCode)
		{
			code += 7;
			NSString* codeStr = [[[NSString alloc] initWithUTF8String:code] autorelease];

			id<MTLDevice> device = MTLCreateSystemDefaultDevice();

			NSError* libraryError = nil;
			id<MTLLibrary> lib = [device newLibraryWithSource:codeStr options:NULL error:&libraryError];
			if (libraryError
#ifdef SUPPRESS_COMPILE_WARNINGS
				&& [libraryError.localizedDescription rangeOfString:@"succeeded"].location == NSNotFound
#endif
			)
			{
				Log(LogType::Error, libraryError.localizedDescription.UTF8String);
				return false;
			}

			this->library_ = lib;
		}
		else
		{
			NSError* libraryError = nil;
			id<MTLLibrary> lib = [device newLibraryWithData:(dispatch_data_t)data error:&libraryError];

			if (libraryError
#ifdef SUPPRESS_COMPILE_WARNINGS
				&& [libraryError.localizedDescription rangeOfString:@"succeeded"].location == NSNotFound
#endif
			)
			{
				Log(LogType::Error, libraryError.localizedDescription.UTF8String);
				return false;
			}

			this->library_ = lib;
		}

		return true;
	}
}

}
