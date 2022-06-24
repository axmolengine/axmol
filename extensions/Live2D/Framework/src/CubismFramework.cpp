/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismFramework.hpp"
#include "Utils/CubismDebug.hpp"
#include "Utils/CubismJson.hpp"
#include "Id/CubismIdManager.hpp"
#include "Rendering/CubismRenderer.hpp"

#ifdef CSM_DEBUG_MEMORY_LEAKING

#include <vector>

#endif


//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

// Framework内で使う定数の宣言
namespace Constant {
    const csmInt32 VertexOffset = 0;
    const csmInt32 VertexStep = 2;
}

// ファイルスコープの変数を初期化
namespace {
csmBool                           s_isStarted = false;
csmBool                           s_isInitialized = false;
ICubismAllocator*                 s_allocator = NULL;
const CubismFramework::Option*    s_option = NULL;
CubismIdManager*                  s_cubismIdManager = NULL;

}

inline ICubismAllocator* GetAllocator()
{
    return s_allocator;
}

#ifdef CSM_DEBUG_MEMORY_LEAKING

namespace {
std::vector<void*>*    s_allocationList = NULL;
}

#endif

csmBool CubismFramework::StartUp(ICubismAllocator* allocator, const Option* option)
{
    if(s_isStarted)
    {
        CubismLogInfo("CubismFramework::StartUp() is already done.");
        return s_isStarted;
    }

    s_option = option;
    if (s_option != NULL)
    {
        Core::csmSetLogFunction(s_option->LogFunction);
    }

    if (allocator == NULL)
    {
        CubismLogWarning("CubismFramework::StartUp() failed, need allocator instance.");
        s_isStarted = false;
    }
    else
    {
        s_allocator = allocator;
        s_isStarted = true;
    }

    // Live2D Cubism Coreバージョン情報を表示
    if(s_isStarted)
    {
        const Core::csmVersion version = Core::csmGetVersion();

        const csmUint32 major = static_cast<csmUint32>((version & 0xFF000000) >> 24);
        const csmUint32 minor = static_cast<csmUint32>((version & 0x00FF0000) >> 16);
        const csmUint32 patch = static_cast<csmUint32>((version & 0x0000FFFF));
        const csmUint32 vesionNumber = version;

        CubismLogInfo("Live2D Cubism Core version: %02d.%02d.%04d (%d)", major, minor, patch, vesionNumber);
    }

    CubismLogInfo("CubismFramework::StartUp() is complete.");

    return s_isStarted;
}

void CubismFramework::CleanUp()
{
    s_isStarted = false;
    s_isInitialized = false;
    s_allocator = NULL;
    s_option = NULL;
    s_cubismIdManager = NULL;
#ifdef CSM_DEBUG_MEMORY_LEAKING
    s_allocationList = NULL;
#endif
}

csmBool CubismFramework::IsStarted()
{
    return s_isStarted;
}

void CubismFramework::Initialize()
{
    CSM_ASSERT(s_isStarted);
    if(!s_isStarted)
    {
        CubismLogWarning("CubismFramework is not started.");
        return;
    }

    // --- s_isInitializedによる連続初期化ガード ---
    // 連続してリソース確保が行われないようにする。
    // 再度Initialize()するには先にDispose()を実行する必要がある。
    if (s_isInitialized)
    {
        CubismLogWarning("CubismFramework::Initialize() skipped, already initialized.");
        return;
    }

#ifdef CSM_DEBUG_MEMORY_LEAKING
    s_allocationList = CSM_NEW std::vector<void*>();
#endif

    //---- static 初期化 ----
    Utils::Value::StaticInitializeNotForClientCall();

    s_cubismIdManager = CSM_NEW CubismIdManager();

    s_isInitialized = true;

    CubismLogInfo("CubismFramework::Initialize() is complete.");
}

void CubismFramework::Dispose()
{
    CSM_ASSERT(s_isStarted);
    if (!s_isStarted)
    {
        CubismLogWarning("CubismFramework is not started.");
        return;
    }

    // --- s_isInitializedによる未初期化解放ガード ---
    // Dispose()するには先にInitialize()を実行する必要がある。
    if (!s_isInitialized) // false...リソース未確保の場合
    {
        CubismLogWarning("CubismFramework::Dispose() skipped, not initialized.");
        return;
    }

    //---- static 解放 ----
    Utils::Value::StaticReleaseNotForClientCall();

    CSM_DELETE(s_cubismIdManager);

    //レンダラの静的リソース（シェーダプログラム他）を解放する
    Rendering::CubismRenderer::StaticRelease();

#ifdef CSM_DEBUG_MEMORY_LEAKING

    for (csmUint32 i = 0; i < s_allocationList->size(); ++i)
    {
        CubismLogInfo("Memory leaking: 0x%p", s_allocationList->at(i));
    }

    if (s_allocationList->size() == 0)
    {
        CubismLogInfo("No memory leaking");
    }

    CSM_DELETE(s_allocationList);

#endif

    s_isInitialized = false;

    CubismLogInfo("CubismFramework::Dispose() is complete.");
}

csmBool CubismFramework::IsInitialized()
{
    return s_isInitialized;
}

void CubismFramework::CoreLogFunction(const csmChar* message)
{
    // Return if logging not possible.
    if (!Core::csmGetLogFunction())
    {
        return;
    }

    Core::csmGetLogFunction()(message);
}

CubismFramework::Option::LogLevel CubismFramework::GetLoggingLevel()
{
    if (s_option != NULL)
    {
        return s_option->LoggingLevel;
    }
    return Option::LogLevel_Off;
}

CubismIdManager* CubismFramework::GetIdManager()
{
    return s_cubismIdManager;
}

#ifdef CSM_DEBUG_MEMORY_LEAKING

void* CubismFramework::Allocate(csmSizeType size, const csmChar* fileName, csmInt32 lineNumber)
{
    void* address = GetAllocator()->Allocate(size);

    CubismLogVerbose("CubismFramework::Allocate(0x%p, %dbytes) %s(%d)", address, size, fileName, lineNumber);

    if (s_allocationList)
    {
        s_allocationList->push_back(address);
    }

    return address;
}

void* CubismFramework::AllocateAligned(csmSizeType size, csmUint32 alignment, const csmChar* fileName, csmInt32 lineNumber)
{
    void* address = GetAllocator()->AllocateAligned(size, alignment);

    CubismLogVerbose("CubismFramework::AllocateAligned(0x%p, a:%d, %dbytes) %s(%d)", address, alignment, size, fileName, lineNumber);

    if (s_allocationList)
    {
        s_allocationList->push_back(address);
    }

    return address;
}

void CubismFramework::Deallocate(void* address, const csmChar* fileName, csmInt32 lineNumber)
{
    if (!address)
    {
        return;
    }

    CubismLogVerbose("CubismFramework::Deallocate(0x%p) %s(%d)", address, fileName, lineNumber);

    if (s_allocationList)
    {
        for (std::vector<void*>::iterator iter = s_allocationList->begin(); iter != s_allocationList->end(); ++iter)
        {
            if (*iter != address)
            {
                continue;
            }

            s_allocationList->erase(iter);
            break;
        }
    }

    GetAllocator()->Deallocate(address);
}

void CubismFramework::DeallocateAligned(void* address, const csmChar* fileName, csmInt32 lineNumber)
{
    if (!address)
    {
        return;
    }

    CubismLogVerbose("CubismFramework::DeallocateAligned(0x%p) %s(%d)", address, fileName, lineNumber);

    if (s_allocationList)
    {
        for (std::vector<void*>::iterator iter = s_allocationList->begin(); iter != s_allocationList->end(); ++iter)
        {
            if (*iter != address)
            {
                continue;
            }

            s_allocationList->erase(iter);
            break;
        }
    }

    GetAllocator()->DeallocateAligned(address);
}

#else

void* CubismFramework::Allocate(csmSizeType size)
{
    return GetAllocator()->Allocate(size);
}

void* CubismFramework::AllocateAligned(csmSizeType size, csmUint32 alignment)
{
    return GetAllocator()->AllocateAligned(size, alignment);
}

void CubismFramework::Deallocate(void* address)
{
    if (!address)
    {
        return;
    }

    GetAllocator()->Deallocate(address);
}

void CubismFramework::DeallocateAligned(void* address)
{
    if (!address)
    {
        return;
    }

    GetAllocator()->DeallocateAligned(address);
}

#endif

}}}
//--------- LIVE2D NAMESPACE ------------

#ifdef CSM_DEBUG_MEMORY_LEAKING

void* operator new(Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::CubismAllocationTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber)
{
    return Live2D::Cubism::Framework::CubismFramework::Allocate(size, fileName, lineNumber);
}

void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::csmUint32 alignment, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber)
{
    return Live2D::Cubism::Framework::CubismFramework::AllocateAligned(size, alignment, fileName, lineNumber);
}

void operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber)
{
    Live2D::Cubism::Framework::CubismFramework::Deallocate(address, fileName, lineNumber);
}

void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag, const Live2D::Cubism::Framework::csmChar* fileName, Live2D::Cubism::Framework::csmInt32 lineNumber)
{
    Live2D::Cubism::Framework::CubismFramework::DeallocateAligned(address, fileName, lineNumber);
}

#else

void* operator new(Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::CubismAllocationTag tag)
{
    return Live2D::Cubism::Framework::CubismFramework::Allocate(size);
}

void* operator new (Live2D::Cubism::Framework::csmSizeType size, Live2D::Cubism::Framework::csmUint32 alignment, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag)
{
    return Live2D::Cubism::Framework::CubismFramework::AllocateAligned(size, alignment);
}

void operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationTag tag)
{
    Live2D::Cubism::Framework::CubismFramework::Deallocate(address);
}

void  operator delete(void* address, Live2D::Cubism::Framework::CubismAllocationAlignedTag tag)
{
    Live2D::Cubism::Framework::CubismFramework::DeallocateAligned(address);
}

#endif
