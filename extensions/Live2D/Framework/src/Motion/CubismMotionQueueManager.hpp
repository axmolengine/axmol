/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Model/CubismModel.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMotionQueueEntry;
class CubismMotionQueueManager;

/**
* @brief イベントのコールバック関数定義
*
* イベントのコールバックに登録できる関数の型情報
*
* @param[in]   caller           発火したイベントを再生させたCubismMotionQueueManager
* @param[in]   eventValue       発火したイベントの文字列データ
* @param[in]   customData       コールバックに返される登録時に指定されたデータ
*/
typedef void(*CubismMotionEventFunction)(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData);

/**
 * @brief モーションの識別番号
 *
 * モーションの識別番号の定義
 */
typedef void* CubismMotionQueueEntryHandle;

extern const CubismMotionQueueEntryHandle InvalidMotionQueueEntryHandleValue;       ///< 無効なモーションの識別番号の定義

/**
 * @brief モーション再生の管理
 *
 * モーション再生の管理用クラス。CubismMotionモーションなどACubismMotionのサブクラスを再生するために使用する。
 *
 * @note 再生中に別のモーションが StartMotion()された場合は、新しいモーションに滑らかに変化し旧モーションは中断する。
 *       表情用モーション、体用モーションなどを分けてモーション化した場合など、
 *       複数のモーションを同時に再生させる場合は、複数のCubismMotionQueueManagerインスタンスを使用する。
 */
class CubismMotionQueueManager
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMotionQueueManager();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMotionQueueManager();

    /**
     * @brief 指定したモーションの開始
     *
     * 指定したモーションを開始する。同じタイプのモーションが既にある場合は、既存のモーションに終了フラグを立て、フェードアウトを開始させる。
     *
     * @param[in]   motion          開始するモーション
     * @param[in]   autoDelete      再生が終了したモーションのインスタンスを削除するなら true
     * @param[in]   userTimeSeconds デルタ時間の積算値[秒]
     * @return                      開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    CubismMotionQueueEntryHandle    StartMotion(ACubismMotion* motion, csmBool autoDelete, csmFloat32 userTimeSeconds);

    /**
     * @brief すべてのモーションの終了の確認
     *
     * すべてのモーションが終了しているかどうか。
     *
     * @retval  true    すべて終了している
     * @retval  false   終了していない
     */
    csmBool     IsFinished();

    /**
     * @brief 指定したモーションの終了の確認
     *
     * 指定したモーションが終了しているかどうか。
     *
     * @param[in]   motionQueueEntryNumber  モーションの識別番号
     * @retval  true    指定したモーションは終了している
     * @retval  false   終了していない
     */
    csmBool     IsFinished(CubismMotionQueueEntryHandle motionQueueEntryNumber);

    /**
     * @brief すべてのモーションの停止
     *
     * すべてのモーションを停止する。
     */
    void        StopAllMotions();

    /**
     * @brief 指定したCubismMotionQueueEntryの取得
     *
     * 指定したCubismMotionQueueEntryを取得する。
     *
     * @param[in]   motionQueueEntryNumber  モーションの識別番号
     * @return  指定したCubismMotionQueueEntryへのポインタ
     * @retval  NULL   見つからなかった
     */
    CubismMotionQueueEntry* GetCubismMotionQueueEntry(CubismMotionQueueEntryHandle motionQueueEntryNumber);

    /**
    * @brief イベントを受け取るCallbackの登録
    *
    * イベントを受け取るCallbackの登録をする。
    *
    * @param[in]   callback     コールバック関数
    * @param[in]   customData   コールバックに返されるデータ
    */
    void SetEventCallback(CubismMotionEventFunction callback, void* customData = NULL);

protected:
    /**
    * @brief モーションの更新
    *
    * モーションを更新して、モデルにパラメータ値を反映する。
    *
    * @param[in]   model   対象のモデル
    * @param[in]   userTimeSeconds   デルタ時間の積算値[秒]
    * @param[in][out]   opacity 透明度の値（Nullable）
    * @retval  true    モデルへパラメータ値の反映あり
    * @retval  false   モデルへパラメータ値の反映なし(モーションの変化なし)
    */
    virtual csmBool     DoUpdateMotion(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32* opacity = NULL);


    csmFloat32 _userTimeSeconds;        ///< デルタ時間の積算値[秒]

private:
    csmVector<CubismMotionQueueEntry*>      _motions;       ///< モーション

    CubismMotionEventFunction         _eventCallback;     ///< コールバック関数ポインタ
    void*                             _eventCustomData;   ///< コールバックに戻されるデータ
};

}}}
