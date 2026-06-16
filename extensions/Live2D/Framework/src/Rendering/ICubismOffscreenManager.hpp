/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/csmVector.hpp"
#include "Math/CubismMath.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/**
 * @brief  オフスクリーン用のレンダーターゲットを管理するクラス
 */
template<class T>
class ICubismOffscreenManager
{
public:
    /**
     * @brief   デストラクタ
     */
    virtual ~ICubismOffscreenManager();

    /**
     * @brief モデルを描画する前に呼び出すフレーム開始時の処理を行う
     */
    void BeginFrameProcess();

    /**
     * @brief モデルの描画が終わった後に呼び出すフレーム終了時の処理
     */
    void EndFrameProcess();

    /**
     * @brief レンダーターゲットのリストサイズの取得する。
     *
     * @return  描画オブジェクトのレンダーテクスチャの枚数を返す。
     */
    csmUint32 GetOffscreenRenderTargetListSize() const;

    /**
     * @brief レンダーターゲットの使用状態を取得する。
     *
     * @param[in]  renderTarget  -> レンダーターゲット
     *
     * @return 使用中はtrue、未使用の場合はfalseを返す。
     */
    csmBool GetUsingRenderTextureState(T* renderTarget);

    /**
     * @brief レンダーターゲットの使用を終了する。
     *
     * @param[in]  renderTarget  -> レンダーターゲット
     */
    void StopUsingRenderTexture(T* renderTarget);

    /**
     * @brief レンダーターゲットの使用を全て終了する。
     */
    void StopUsingAllRenderTextures();

    /**
     * @brief 使用されていないレンダーターゲットを開放する。
     */
    void ReleaseStaleRenderTextures();

    /**
     * @brief レンダーターゲットを全て開放する。
     */
    void ReleaseAllRenderTextures();

    /**
     * @brief 直前のアクティブなレンダーターゲットの最大数を取得する。
     *
     * @return  直前のアクティブなレンダーターゲットの最大数を返す。
     */
    csmUint32 GetPreviousActiveRenderTextureCount() const;

    /**
     * @brief 現在のアクティブなレンダーターゲットの数を取得する。
     *
     * @return  現在のアクティブなレンダーターゲットの数を返す。
     */
    csmUint32 GetCurrentActiveRenderTextureCount() const;

protected:
    /**
     * @brief   コンストラクタ
     */
    ICubismOffscreenManager();

    /**
     * @brief レンダーターゲットの最大数の更新
     */
    void UpdateRenderTargetCount();

    /**
     * @brief 未使用のレンダーターゲットを取得する
     *
     * @return  未使用のレンダーターゲットを返す。なければnullptrを返す。
     */
    T* GetUnusedOffscreenRenderTarget();

    /**
     * @brief レンダーターゲットを作成する
     *
     * @return  レンダーターゲットを返す。
     */
    T* CreateOffscreenRenderTarget();

private:
    /**
     * @brief  オフスクリーン用のレンダーターゲットのコンテナ構造体
     */
    struct CubismRenderTargetContainer
    {
        /**
         * @brief   コンストラクタ
         *
         */
        CubismRenderTargetContainer()
            : RenderTarget(nullptr)
            , IsUsed(true)
        {
        }

        T* RenderTarget; ///< レンダーターゲット本体
        csmBool IsUsed; ///< 使用中かどうか
    };

    csmVector<CubismRenderTargetContainer> _offscreenRenderTargetList;   ///< オフスクリーン描画用レンダーターゲットのコンテナリスト
    csmUint32 _previousActiveRenderTextureMaxCount; ///< 直前のアクティブなレンダーターゲットの最大数
    csmUint32 _currentActiveRenderTextureCount; ///< 現在のアクティブなレンダーターゲットの数
    csmBool _hasResetThisFrame; ///< 今フレームでリセットされたかどうか
};

template<class T>
ICubismOffscreenManager<T>::~ICubismOffscreenManager()
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        _offscreenRenderTargetList[i].RenderTarget->DestroyRenderTarget();
        CSM_DELETE_SELF(T, _offscreenRenderTargetList[i].RenderTarget);
    }
}

template<class T>
void ICubismOffscreenManager<T>::BeginFrameProcess()
{
    if (_hasResetThisFrame)
    {
        return;
    }
    _previousActiveRenderTextureMaxCount = 0;
    _hasResetThisFrame = true;
}

template<class T>
void ICubismOffscreenManager<T>::EndFrameProcess()
{
    _hasResetThisFrame = false;
}

template<class T>
csmUint32 ICubismOffscreenManager<T>::GetOffscreenRenderTargetListSize() const
{
    return _offscreenRenderTargetList.GetSize();
}

template<class T>
csmBool ICubismOffscreenManager<T>::GetUsingRenderTextureState(T* renderTarget)
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        if (_offscreenRenderTargetList[i].RenderTarget == renderTarget)
        {
            return _offscreenRenderTargetList[i].IsUsed;
        }
    }
    return false;
}

template<class T>
void ICubismOffscreenManager<T>::StopUsingRenderTexture(T* renderTarget)
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        if (_offscreenRenderTargetList[i].RenderTarget == renderTarget)
        {
            if (!_offscreenRenderTargetList[i].IsUsed)
            {
                break;
            }

            _offscreenRenderTargetList[i].IsUsed = false;

            if (0 < _currentActiveRenderTextureCount)
            {
                --_currentActiveRenderTextureCount;
            }

            break;
        }
    }
}

template<class T>
void ICubismOffscreenManager<T>::StopUsingAllRenderTextures()
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        _offscreenRenderTargetList[i].IsUsed = false;
    }

    _currentActiveRenderTextureCount = 0;
}

template<class T>
void ICubismOffscreenManager<T>::ReleaseStaleRenderTextures()
{
    const csmUint32 listSize = _offscreenRenderTargetList.GetSize();
    if (_hasResetThisFrame || listSize == 0)
    {
        // 使用する量が変化する場合は開放しない
        return;
    }

    // 未使用な場所を開放して直前の最大数までリサイズする
    csmUint32 findPos = 0;
    csmUint32 resize = _previousActiveRenderTextureMaxCount;
    for (csmUint32 i = listSize; _previousActiveRenderTextureMaxCount < i; --i)
    {
        const csmUint32 index = i - 1;
        if (_offscreenRenderTargetList[index].IsUsed)
        {
            // 空いている場所探して移動させる
            csmBool isFind = false;
            for (; findPos < _previousActiveRenderTextureMaxCount; ++findPos)
            {
                if (!_offscreenRenderTargetList[findPos].IsUsed)
                {
                    T* RenderTarget = _offscreenRenderTargetList[findPos].RenderTarget;
                    _offscreenRenderTargetList[findPos].RenderTarget = _offscreenRenderTargetList[index].RenderTarget;
                    _offscreenRenderTargetList[findPos].IsUsed = true;
                    _offscreenRenderTargetList[index].RenderTarget = RenderTarget;
                    _offscreenRenderTargetList[index].IsUsed = false;
                    isFind = true;
                    break;
                }
            }
            if (!isFind)
            {
                // 空いている場所が見つからなかったら現状のサイズでリサイズする
                resize = i;
                break;
            }
        }
        _offscreenRenderTargetList[index].RenderTarget->DestroyRenderTarget();
        CSM_DELETE_SELF(T, _offscreenRenderTargetList[index].RenderTarget);
    }
    _offscreenRenderTargetList.Resize(resize);
}

template<class T>
void ICubismOffscreenManager<T>::ReleaseAllRenderTextures()
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        _offscreenRenderTargetList[i].RenderTarget->DestroyRenderTarget();
        CSM_DELETE_SELF(T, _offscreenRenderTargetList[i].RenderTarget);
    }
    _offscreenRenderTargetList.Clear();

    _previousActiveRenderTextureMaxCount = 0;
    _currentActiveRenderTextureCount = 0;
}

template<class T>
csmUint32 ICubismOffscreenManager<T>::GetPreviousActiveRenderTextureCount() const
{
    return _previousActiveRenderTextureMaxCount;
}

template<class T>
csmUint32 ICubismOffscreenManager<T>::GetCurrentActiveRenderTextureCount() const
{
    return _currentActiveRenderTextureCount;
}

template<class T>
ICubismOffscreenManager<T>::ICubismOffscreenManager()
    : _previousActiveRenderTextureMaxCount(0)
    , _currentActiveRenderTextureCount(0)
    , _hasResetThisFrame(false)
{
}

template<class T>
void ICubismOffscreenManager<T>::UpdateRenderTargetCount()
{
    ++_currentActiveRenderTextureCount;

    // 最大数更新
    _previousActiveRenderTextureMaxCount = _currentActiveRenderTextureCount > _previousActiveRenderTextureMaxCount ?
        _currentActiveRenderTextureCount :
        _previousActiveRenderTextureMaxCount;
}

template<class T>
T* ICubismOffscreenManager<T>::GetUnusedOffscreenRenderTarget()
{
    for (csmUint32 i = 0; i < _offscreenRenderTargetList.GetSize(); ++i)
    {
        if (!_offscreenRenderTargetList[i].IsUsed)
        {
            _offscreenRenderTargetList[i].IsUsed = true;
            return _offscreenRenderTargetList[i].RenderTarget;
        }
    }

    return nullptr;
}

template<class T>
T* ICubismOffscreenManager<T>::CreateOffscreenRenderTarget()
{
    _offscreenRenderTargetList.PushBack(CubismRenderTargetContainer());
    _offscreenRenderTargetList.Back().RenderTarget = CSM_NEW T();
    return _offscreenRenderTargetList.Back().RenderTarget;
}

}}}}

//------------ LIVE2D NAMESPACE ------------
