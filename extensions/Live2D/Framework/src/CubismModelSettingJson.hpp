/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ICubismModelSetting.hpp"
#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief Model3Jsonパーサー.
 *
 * model3.jsonファイルをパースして値を取得する。
 *
 */
class CubismModelSettingJson : public ICubismModelSetting, public CubismJsonHolder
{
public:

    /**
     *
     * @brief 引数付きコンストラクタ
     *
     * 引数付きコンストラクタ。
     *
     * @param[in] buffer     Model3Jsonをバイト配列として読み込んだデータバッファ
     * @param[in] size       Model3Jsonのデータサイズ
     *
     */
    CubismModelSettingJson(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismModelSettingJson();

    /**
     * @brief   CubismJsonオブジェクトのポインタを取得する
     *
     * @return  CubismJsonのポインタ
     */
    Utils::CubismJson* GetJsonPointer() const;

    const csmChar* GetModelFileName();

    csmInt32 GetTextureCount();

    const csmChar* GetTextureDirectory();

    const csmChar* GetTextureFileName(csmInt32 index);

    csmInt32 GetHitAreasCount();

    CubismIdHandle GetHitAreaId(csmInt32 index);

    const csmChar* GetHitAreaName(csmInt32 index);

    const csmChar* GetPhysicsFileName();

    const csmChar* GetPoseFileName();

    const csmChar* GetDisplayInfoFileName();

    csmInt32 GetExpressionCount();

    const csmChar* GetExpressionName(csmInt32 index);

    const csmChar* GetExpressionFileName(csmInt32 index);

    csmInt32 GetMotionGroupCount();

    const csmChar* GetMotionGroupName(csmInt32 index);

    csmInt32 GetMotionCount(const csmChar* groupName);

    const csmChar* GetMotionFileName(const csmChar* groupName, csmInt32 index);

    const csmChar* GetMotionSoundFileName(const csmChar* groupName, csmInt32 index);

    csmFloat32 GetMotionFadeInTimeValue(const csmChar* groupName, csmInt32 index);

    csmFloat32 GetMotionFadeOutTimeValue(const csmChar* groupName, csmInt32 index);

    const csmChar* GetUserDataFile();

    csmBool GetLayoutMap(csmMap<csmString, csmFloat32>& outLayoutMap);

    csmInt32 GetEyeBlinkParameterCount();

    CubismIdHandle GetEyeBlinkParameterId(csmInt32 index);

    csmInt32 GetLipSyncParameterCount();

    CubismIdHandle GetLipSyncParameterId(csmInt32 index);

private:

    enum FrequentNode
    {
        FrequentNode_Groups,        ///< GetRoot()[Groups]
        FrequentNode_Moc,           ///< GetRoot()[FileReferences][Moc]
        FrequentNode_Motions,       ///< GetRoot()[FileReferences][Motions]
        FrequentNode_DisplayInfo,   ///< GetRoot()[FileReferences][DisplayInfo]
        FrequentNode_Expressions,   ///< GetRoot()[FileReferences][Expressions]
        FrequentNode_Textures,      ///< GetRoot()[FileReferences][Textures]
        FrequentNode_Physics,       ///< GetRoot()[FileReferences][Physics]
        FrequentNode_Pose,          ///< GetRoot()[FileReferences][Pose]
        FrequentNode_HitAreas,      ///< GetRoot()[HitAreas]
    };

    /**
     * @brief        モデルファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistModelFile() const;

    /**
     * @brief        テクスチャファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistTextureFiles() const;

    /**
     * @brief        当たり判定のキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistHitAreas() const;

    /**
     * @brief        物理演算ファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistPhysicsFile() const;

    /**
     * @brief        ポーズ設定ファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistPoseFile() const;

    /**
     * @brief        表示名称設定ファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistDisplayInfoFile() const;

    /**
     * @brief        表情設定ファイルのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistExpressionFile() const;

    /**
     * @brief        モーショングループのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistMotionGroups() const;

    /**
     * @brief        引数で指定したモーショングループのキーが存在するかどうかを確認する
     *
     * @param[in]    groupName  グループ名
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistMotionGroupName(const csmChar* groupName) const;

    /**
     * @brief        引数で指定したモーションに対応するサウンドファイルのキーが存在するかどうかを確認する
     *
     * @param[in]    groupName  グループ名
     * @param[in]    index   配列のインデックス値
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistMotionSoundFile(const csmChar* groupName, csmInt32 index) const;

    /**
     * @brief        引数で指定したモーションに対応するフェードイン時間のキーが存在するかどうかを確認する
     *
     * @param[in]    groupName  グループ名
     * @param[in]    index   配列のインデックス値
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistMotionFadeIn(const csmChar* groupName, csmInt32 index) const;

    /**
     * @brief        引数で指定したモーションに対応するフェードアウト時間のキーが存在するかどうかを確認する
     *
     * @param[in]    groupName  グループ名
     * @param[in]    index   配列のインデックス値
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistMotionFadeOut(const csmChar* groupName, csmInt32 index) const;

    /**
    * @brief        UserDataのファイル名が存在するか確認
    *
    * @retval       true   キーが存在する
    * @retval       false  キーが存在しない
    */
    csmBool IsExistUserDataFile() const;

    /**
     * @brief        目パチに対応付けられたパラメータが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistEyeBlinkParameters() const;

    /**
     * @brief        リップシンクに対応付けられたパラメータが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistLipSyncParameters() const;

    csmVector<Utils::Value*>    _jsonValue;  ///< モデルデータjsonの頻出ノード
};
}}}
