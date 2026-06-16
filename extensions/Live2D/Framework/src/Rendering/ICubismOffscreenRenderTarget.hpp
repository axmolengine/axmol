/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/**
 * @brief  オフスクリーン用のレンダーターゲットを管理するクラス
 * 　　　　テンプレートはそれぞれ継承先の子クラス、レンダーターゲットの型を指定する
 */
template<class T, class U>
class ICubismOffscreenRenderTarget
{
public:
    /**
     * @brief   デストラクタ
     */
    virtual ~ICubismOffscreenRenderTarget();

    /**
     * @brief   オフスクリーンのIndexを設定
     */
    void SetOffscreenIndex(csmInt32 offscreenIndex);

    /**
     * @brief   オフスクリーンのIndexを取得
     */
    csmInt32 GetOffscreenIndex() const;

    /**
     * @brief 以前のオフスクリーンレンダリングターゲットを設定
     *
     * @param oldOffscreen 以前のオフスクリーンレンダリングターゲット
     */
    void SetOldOffscreen(T* oldOffscreen);

    /**
     * @brief 以前のオフスクリーンレンダリングターゲットを取得
     *
     * @return 以前のオフスクリーンレンダリングターゲット
     */
    T* GetOldOffscreen() const;

    /**
     * @brief 親のオフスクリーンレンダリングターゲットを設定
     *
     * @param parentRenderTarget 親のオフスクリーンレンダリングターゲット
     */
    void SetParentPartOffscreen(T* parentRenderTarget);

    /**
     * @brief 親のオフスクリーンレンダリングターゲットを取得
     *
     * @return 親のオフスクリーンレンダリングターゲット
     */
    T* GetParentPartOffscreen() const;

    /**
     * @brief レンダーターゲットを取得する。
     *
     * @return レンダーターゲットを返す。
     */
    U* GetRenderTarget() const;

protected:
    /**
     * @brief   コンストラクタ
     */
    ICubismOffscreenRenderTarget();

    U* _renderTarget;            ///< レンダーターゲット

private:
    csmInt32 _offscreenIndex;   ///< _offscreensのIndex
    T* _parentRenderTarget;     ///< 親のオフスクリーンレンダリングターゲット
    T* _oldOffscreen;           ///< 以前のオフスクリーンレンダリングターゲット
};

template<class T, class U>
ICubismOffscreenRenderTarget<T, U>::~ICubismOffscreenRenderTarget()
{
}

template<class T, class U>
void ICubismOffscreenRenderTarget<T, U>::SetOffscreenIndex(csmInt32 const offscreenIndex)
{
    _offscreenIndex = offscreenIndex;
}

template<class T, class U>
csmInt32 ICubismOffscreenRenderTarget<T, U>::GetOffscreenIndex() const
{
    return _offscreenIndex;
}

template<class T, class U>
void ICubismOffscreenRenderTarget<T, U>::SetOldOffscreen(T* oldOffscreen)
{
    _oldOffscreen = oldOffscreen;
}

template<class T, class U>
T* ICubismOffscreenRenderTarget<T, U>::GetOldOffscreen() const
{
    return _oldOffscreen;
}

template<class T, class U>
void ICubismOffscreenRenderTarget<T, U>::SetParentPartOffscreen(T* parentRenderTarget)
{
    _parentRenderTarget = parentRenderTarget;
}

template<class T, class U>
T* ICubismOffscreenRenderTarget<T, U>::GetParentPartOffscreen() const
{
    return _parentRenderTarget;
}

template<class T, class U>
U* ICubismOffscreenRenderTarget<T, U>::GetRenderTarget() const
{
    return _renderTarget;
}

template<class T, class U>
ICubismOffscreenRenderTarget<T, U>::ICubismOffscreenRenderTarget()
    : _renderTarget(nullptr)
    , _offscreenIndex(-1)
    , _parentRenderTarget(nullptr)
    , _oldOffscreen(nullptr)
{
}

}}}}

//------------ LIVE2D NAMESPACE ------------
