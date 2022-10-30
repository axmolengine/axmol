#include "LLGI.CompilerMetal.h"

#import <MetalKit/MetalKit.h>

namespace LLGI
{

void CompilerMetal::Initialize() {}

void CompilerMetal::Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage)
{
	@autoreleasepool
	{
		// Metal doesn't support to save a library as binary file (with external tool, it can)
		NSString* codeStr = [[[NSString alloc] initWithUTF8String:code] autorelease];

		id<MTLDevice> device = MTLCreateSystemDefaultDevice();

		NSError* libraryError = nil;
		id<MTLLibrary> lib = [[device newLibraryWithSource:codeStr options:NULL error:&libraryError] autorelease];
		if (libraryError)
		{
			result.Message = libraryError.localizedDescription.UTF8String;
		}

		if (lib == NULL)
		{
			return;
		}

		std::vector<char> buffer;

		// header
		buffer.push_back('m');
		buffer.push_back('t');
		buffer.push_back('l');
		buffer.push_back('c');
		buffer.push_back('o');
		buffer.push_back('d');
		buffer.push_back('e');

		auto len = strlen(code) + 1;
		for (int i = 0; i < len; i++)
		{
			buffer.push_back(code[i]);
		}

		result.Binary.resize(1);
		result.Binary[0].resize(buffer.size());
		memcpy(result.Binary[0].data(), buffer.data(), buffer.size());
	}
}

}
