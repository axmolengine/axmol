/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once


//========================================================
//  Live2D Cubism Coreライブラリをインクルード
//========================================================
#include "Live2DCubismCore.hpp"


//========================================================
//  設定用ヘッダをインクルード
//========================================================
#include "CubismFrameworkConfig.hpp"


//========================================================
//  カスタムアロケータをインクルード
//========================================================
#include <new>
#include "ICubismAllocator.hpp"

#ifdef __linux__
#include <cstdlib>
#endif

namespace Live2D { namespace Cubism { namespace Framework {

class CubismAllocationTag
{ };

class CubismAllocationAlignedTag
{ };

static CubismAllocationTag GlobalTag;
static CubismAllocationAlignedTag GloabalAlignedTag;

}}}

#ifdef CSM_DEBUG_MEMORY_LEAKING

// デバッグ用
// メモリリークの検出や確保・解放の追跡を行う。

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

// リリース用
// 何も追跡しない。

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
#define CSM_MALLOC_ALLIGNED(size, align) Live2D::Cubism::Framework::CubismFramework::AllocateAligned(size, align)
#define CSM_FREE(ptr)                    Live2D::Cubism::Framework::CubismFramework::Deallocate(ptr)
#define CSM_FREE_ALLIGNED(ptr)           Live2D::Cubism::Framework::CubismFramework::DeallocateAligned(ptr)

#endif

#define CSM_PLACEMENT_NEW(addrs)         new((addrs))


//========================================================
//  アーキテクチャ用型定義をインクルード
//========================================================
#include "Type/CubismBasicType.hpp"


//========================================================
//  IDマネージャの前方宣言
//========================================================
namespace Live2D { namespace Cubism { namespace Framework {

class CubismIdManager;

}}}


//========================================================
//  コンパイラに関する設定
//========================================================
#ifdef _MSC_VER
#pragma warning (disable : 4100)
#endif

#ifndef NULL
#define NULL  0
#endif


//========================================================
//  検証マクロ
//========================================================
/*
 * @brief  式が有効であることを保証する。
 *
 * @param  expression  検証する式。
 * @param  message     検証結果がfalseだった場合のログメッセージ。
 * @param  body        検証結果がfalseだった場合に実行する処理。
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
//  名前空間のエイリアス
//========================================================
namespace Csm = Live2D::Cubism::Framework;


//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief Framework内で使う定数の宣言
 *
 */
namespace Constant {
extern const csmInt32 VertexOffset; ///< メッシュ頂点のオフセット値
extern const csmInt32 VertexStep;   ///< メッシュ頂点のステップ値
}

/**
 * @brief Live2D Cubism Original Workflow SDKのエントリポイント<br>
 *         利用開始時はCubismFramework::Initialize()を呼び、CubismFramework::Dispose()で終了する。
 *
 */
class CubismFramework
{
public:

    /*
     * @brief CubismFrameworkに設定するオプション要素を定義するクラス
     *
     */
    class Option
    {
    public:
        /**
         * @brief   ログ出力のレベル
         */
        enum LogLevel
        {
            LogLevel_Verbose = 0,   ///<  詳細ログ
            LogLevel_Debug,         ///<  デバッグログ
            LogLevel_Info,          ///<  Infoログ
            LogLevel_Warning,       ///<  警告ログ
            LogLevel_Error,         ///<  エラーログ
            LogLevel_Off            ///<  ログ出力無効
        };

        Core::csmLogFunction LogFunction;       ///< ログ出力の関数ポインタ
        LogLevel LoggingLevel;                  ///< ログ出力レベル設定
    };

   /**
    * @brief    Cubism FrameworkのAPIを使用可能にする。<br>
    *            APIを実行する前に必ずこの関数を実行すること。<br>
    *            引数に必ずメモリアロケータを渡してください。<br>
    *            一度準備が完了して以降は、再び実行しても内部処理がスキップされます。
    *
    * @param    allocator   ICubismAllocatorクラスのインスタンス
    * @param    option      Optionクラスのインスタンス
    *
    * @return   準備処理が完了したらtrueが返ります。
    */
    static csmBool StartUp(ICubismAllocator* allocator, const Option* option = NULL);

    /**
    * @brief    StartUp()で初期化したCubismFrameworkの各パラメータをクリアします。<br>
    *            Dispose()したCubismFrameworkを再利用する際に利用してください。<br>
    *
    */
    static void CleanUp();

    /**
     * @brief   Cubism FrameworkのAPIを使用する準備が完了したかどうか？
     *
     * @return  APIを使用する準備が完了していればtrueが返ります。
     */
    static csmBool IsStarted();

    /**
     * @brief  Cubism Framework内のリソースを初期化してモデルを表示可能な状態にします。<br>
     *         再度Initialize()するには先にDispose()を実行する必要があります。
     */
    static void Initialize();

    /**
     *@brief Cubism Framework内の全てのリソースを解放します。<br>
     *        ただし、外部で確保されたリソースについては解放しません。<br>
     *        外部で適切に破棄する必要があります。
     */
    static void Dispose();

    /**
     * @brief   Cubism Frameworkのリソース初期化が既に行われているかどうか？
     *
     * @return  リソース確保が完了していればtrueが返ります。
     */
    static csmBool IsInitialized();

    /**
     * @brief   Core APIにバインドしたログ関数を実行する
     *
     * @param message   ->  ログメッセージ
     */
    static void CoreLogFunction(const csmChar* message);

    /**
     * @biref   現在のログ出力レベル設定の値を返す。
     *
     * @return  現在のログ出力レベル設定の値
     */
    static Option::LogLevel GetLoggingLevel();

    /**
     * @brief IDマネージャのインスタンスを取得する。
     *
     * @return CubismIdManagerクラスのインスタンス
     */
    static CubismIdManager* GetIdManager();

#ifdef CSM_DEBUG_MEMORY_LEAKING

    static void* Allocate(csmSizeType size, const csmChar* fileName, csmInt32 lineNumber);
    static void* AllocateAligned(csmSizeType size, csmUint32 alignment, const csmChar* fileName, csmInt32 lineNumber);
    static void  Deallocate(void* address, const csmChar* fileName, csmInt32 lineNumber);
    static void  DeallocateAligned(void* address, const csmChar* fileName, csmInt32 lineNumber);

#else

    static void* Allocate(csmSizeType size);
    static void* AllocateAligned(csmSizeType size, csmUint32 alignment);
    static void  Deallocate(void* address);
    static void  DeallocateAligned(void* address);

#endif

private:
    /**
     *@brief  コンストラクタ<br>
     *         静的クラスとして使用する<br>
     *         インスタンス化させない
     */
    CubismFramework(){}

};

}}}
//--------- LIVE2D NAMESPACE ------------
