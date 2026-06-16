/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once


//========================================================
//  Include the Live2D Cubism Core header.
//========================================================
#include "Live2DCubismCore.hpp"


//========================================================
//  Include the Framework Configurations header.
//========================================================
#include "CubismFrameworkConfig.hpp"


//========================================================
//  Include the Custom Allocator header.
//========================================================
#include <new>
#include "ICubismAllocator.hpp"

#ifdef __linux__
#include <cstdlib>
#endif

#include <string>

//========================================================
//  Configurations of Memory Allocator.
//========================================================
namespace Live2D { namespace Cubism { namespace Framework {

class CubismAllocationTag
{ };

class CubismAllocationAlignedTag
{ };

static CubismAllocationTag GlobalTag;
static CubismAllocationAlignedTag GlobalAlignedTag;

}}}

// Macros for memory allocation.
#ifdef CSM_DEBUG_MEMORY_LEAKING

// For debugging.
// Tracks / Detects the memory leaking.

void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::CubismAllocationTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber);
void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::csmUint32 alignment, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber);
void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber);
void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber);

template<typename T>
void CsmDelete(T* address, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber)
{
    if (!address)
    {
        return;
    }

    address->~T();

    operator delete(reinterpret_cast<void*>(address), Live2D::Cubism::Framework::GlobalTag, fileName, lineNumber);
}

#define CSM_NEW                          new(Live2D::Cubism::Framework::GlobalTag, __FILE__, __LINE__)
#define CSM_DELETE_SELF(type, obj)       do { if (!obj){ break; } obj->~type(); operator delete(obj, Live2D::Cubism::Framework::GlobalTag, __FILE__, __LINE__); } while(0)
#define CSM_DELETE(obj)                  CsmDelete(obj, __FILE__, __LINE__)
#define CSM_MALLOC(size)                 Live2D::Cubism::Framework::CubismFramework::Allocate(size, __FILE__, __LINE__)
#define CSM_MALLOC_ALLIGNED(size, align) Live2D::Cubism::Framework::CubismFramework::AllocateAligned(size, align, __FILE__, __LINE__)
#define CSM_FREE(ptr)                    Live2D::Cubism::Framework::CubismFramework::Deallocate(ptr, __FILE__, __LINE__)
#define CSM_FREE_ALLIGNED(ptr)           Live2D::Cubism::Framework::CubismFramework::DeallocateAligned(ptr, __FILE__, __LINE__)

#else

// For releasing.
// Without Tracking / Detecting.

void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::CubismAllocationTag tag);
void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::csmUint32 alignment, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag);
void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationTag tag);
void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag);

template<typename T>
void CsmDelete(T* address)
{
    if (!address)
    {
        return;
    }

    address->~T();

    operator delete(reinterpret_cast<void*>(address), Live2D::Cubism::Framework::GlobalTag);
}

#define CSM_NEW                          new(Live2D::Cubism::Framework::GlobalTag)
#define CSM_DELETE_SELF(type, obj)       do { if (!obj){ break; } obj->~type(); operator delete(obj, Live2D::Cubism::Framework::GlobalTag); } while(0)
#define CSM_DELETE(obj)                  CsmDelete(obj)
#define CSM_MALLOC(size)                 Live2D::Cubism::Framework::CubismFramework::Allocate(size)
#define CSM_MALLOC_ALIGNED(size, align) Live2D::Cubism::Framework::CubismFramework::AllocateAligned(size, align)
#define CSM_FREE(ptr)                    Live2D::Cubism::Framework::CubismFramework::Deallocate(ptr)
#define CSM_FREE_ALIGNED(ptr)           Live2D::Cubism::Framework::CubismFramework::DeallocateAligned(ptr)

#endif

#define CSM_PLACEMENT_NEW(addrs)         new((addrs))


//========================================================
//  Include the definition of types for the each architectures.
//========================================================
#include "Type/CubismBasicType.hpp"


//========================================================
//  Forward Declaration of CubismIdManager.
//========================================================
namespace Live2D { namespace Cubism { namespace Framework {

class CubismIdManager;

}}}


//========================================================
//  Compiler Setttings for buiding the Framework.
//========================================================
#ifdef _MSC_VER
#pragma warning (disable : 4100)
#endif

#ifndef NULL
#define NULL  0
#endif


//========================================================
//  Macros for validation.
//========================================================
/**
 * Ensures that the expression is valid.
 *
 * @param expression Expression to be ensured
 * @param message Message if the expression is invalid
 * @param body Handler if the expression is invalid
 */
#define CubismEnsure(expression, message, body)       \
do                                              \
{                                               \
  if (!(expression))                            \
  {                                             \
    CubismFramework::CoreLogFunction("[Cubism Framework] " message); \
    body;                                       \
  }                                             \
}                                               \
while (0);



//========================================================
//  Ailias of the Framework.
//========================================================
namespace Csm = Live2D::Cubism::Framework;


//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/** Typedef for file loader */
typedef csmByte* (*csmLoadFileFunction)(const std::string filePath, csmSizeInt* outSize);
typedef void (*csmReleaseBytesFunction)(Csm::csmByte* byteData);

/**
 * Constants.
 */
namespace Constant {
/** Vertex offset of Drawable */
extern const csmInt32 VertexOffset;

/** Number of vertices in Drawable */
extern const csmInt32 VertexStep;
}

/**
 * Entrypoint for the Live2D Cubism SDK
 *
 * @see #StartUp()
 * @see #Initialize()
 * @see #Dispose()
 * @see #CleanUp()
 *
 * @note At the start of use, call CubismFramework::StartUp() before calling CubismFramework::Initialize(). <br>
 * At the end, call CubismFramework::Dispose() before calling CubismFramework::CleanUp().
 */
class CubismFramework
{
public:
    /**
     * Framework configurations.
     */
    class Option
    {
    public:
        /**
         * Enumerator for Logging level
         */
        enum LogLevel
        {
            /** Indicates that the logging of all is enabled. */
            LogLevel_Verbose = 0,

            /** Indicates that up to the logging level of Debug. */
            LogLevel_Debug,

            /** Indicates that up to the logging level of Info. */
            LogLevel_Info,

            /** Indicates that up to the logging level of Warning. */
            LogLevel_Warning,

            /** Indicates that up to the logging level of Error. */
            LogLevel_Error,

            /**  Indicates that the logging of all is disabled. */
            LogLevel_Off
        };

        /** Logging function */
        Core::csmLogFunction LogFunction;

        /** Logging level */
        LogLevel LoggingLevel;

        /** File reading function */
       csmLoadFileFunction LoadFileFunction;

       /** Release bytes function */
       csmReleaseBytesFunction ReleaseBytesFunction;
    };

    /**
     *
     * Enables the Cubism Framework API.
     *
     * @param allocator Instance of memory allocator
     * @param option Instance of Framework configuration option.<br>
     * If an empty instance is set, the Framework configuration option is not used.
     *
     * @return true if Cubism Framework is available; otherwise false.
     *
     * @note Make sure to execute this function before executing the API of the Framework, <br>
     * and be sure to pass the memory allocator to the argument.<br>
     * Once the preparation is complete, the process will be skipped if executed again afterwards.
     */
    static csmBool StartUp(ICubismAllocator* allocator, const Option* option = NULL);

    /**
     * Enables the Cubism Framework API to reuse.
     *
     * @note Clear each parameter of CubismFramework initialized by StartUp().<br>
     * Use this method when reusing Cubism Framework that has been Dispose().
     */
    static void CleanUp();

    /**
     * Returns whether the API of Cubism Framework is available.
     *
     * @return true if the API is available; otherwise false.
     */
    static csmBool IsStarted();

    /**
     * Initializes the resources in the Cubism Framework and makes the Model ready for display.
     */
    static void Initialize();

    /**
     * Releases all resources in the Cubism Framework.
     *
     * @note Resources allocated externally will not be released.<br>
     * Discard them externally as appropriate.
     */
    static void Dispose();

    /**
     * Returns whether the Cubism Framework resources have been initialized.
     *
     * @return true if the resource has been initialized; otherwise false.
     */
    static csmBool IsInitialized();

    /**
     * Executes the logging function of Cubism Core API.
     *
     * @param message Message for logging
     *
     * @note Logging using Option::logFunction.
     */
    static void CoreLogFunction(const csmChar* message);

    /**
     * Returns the logging level setting.
     *
     * @return Logging level setting
     */
    static Option::LogLevel GetLoggingLevel();

    /**
     * Returns the loading file function.
     *
     * @return Function to read the file as csmByte*.
     */
    static csmLoadFileFunction GetLoadFileFunction();

    /**
     * Returns the memory release function.
     *
     * @return Function to free memory allocated by csmByte*.
     *
     * @note Memory allocated by GetLoadFileFunction() must be released with this function.
     */
    static csmReleaseBytesFunction GetReleaseBytesFunction();

    /**
     * Returns the instance of CubismIdManager.
     *
     * @note Please use `GetId()` through this function to get `CubismId`.<br>
     *           ex) CubismFramework::GetIdManager()->GetId(id_str);
     *
     * @return Instance of CubismIdManager.
     */
    static CubismIdManager* GetIdManager();

#ifdef CSM_DEBUG_MEMORY_LEAKING

    /**
     * (For debugging) Allocates the memory.
     *
     * @param size Desired amount of memory in bytes
     * @param fileName Name of source code that called
     * @param lineNumber Number of line of source code that called
     *
     * @return Pointer to the allocated memory if succeeded; otherwise `0`
     */
    static void* Allocate(csmSizeType size, const csmChar* fileName, csmInt32 lineNumber);

    /**
     * (For debugging) Allocates the memory with specified alignment.
     *
     * @param size Desired amount of memory in bytes
     * @param alignment Desired alignment of memory in bytes
     * @param fileName Name of source code that called
     * @param lineNumber Number of line of source code that called
     *
     * @return Pointer to the allocated memory if succeeded; otherwise `0`
     */
    static void* AllocateAligned(csmSizeType size, csmUint32 alignment, const csmChar* fileName, csmInt32 lineNumber);

    /**
     * (For debugging) Deallocates the aligned memory.
     *
     * @param address Pointer to allocated memory to be deallocated
     * @param fileName Name of source code that called
     * @param lineNumber Number of line of source code that called
     */
    static void  Deallocate(void* address, const csmChar* fileName, csmInt32 lineNumber);

    /**
     * (For debugging) Deallocates the aligned memory.
     *
     * @param address Pointer to allocated memory to be deallocated
     * @param fileName Name of source code that called
     * @param lineNumber Number of line of source code that called
     */
    static void  DeallocateAligned(void* address, const csmChar* fileName, csmInt32 lineNumber);

#else

    /**
     * Allocates the memory with specified alignment.
     *
     * @param size Desired amount of memory in bytes
     * @param alignment Desired alignment of memory in bytes
     *
     * @return PPointer to the allocated memory if succeeded; otherwise `0`
     */
    static void* Allocate(csmSizeType size);

    /**
     * Allocates the memory with specified alignment.
     *
     * @param size Desired amount of memory in bytes
     * @param alignment Desired alignment of memory in bytes
     *
     * @return Pointer to the allocated memory if succeeded; otherwise `0`
     */
    static void* AllocateAligned(csmSizeType size, csmUint32 alignment);

    /**
     * Deallocates the memory.
     *
     * @param address Pointer to allocated memory to be deallocated
     */
    static void  Deallocate(void* address);

    /**
     * Deallocates the aligned memory.
     *
     * @param address Pointer to allocated memory to be deallocated
     */
    static void  DeallocateAligned(void* address);

#endif

private:
    /**
     * Constructor
     *
     * @note Prevent instantiating and be used as a static class.
     */
    CubismFramework(){}

};

}}}
//--------- LIVE2D NAMESPACE ------------
