/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

/**
 * @brief パラメータIDのデフォルト値を保持する定数<br>
 *         デフォルト値の仕様は以下のマニュアルに基づく<br>
 *         https://docs.live2d.com/cubism-editor-manual/standard-parametor-list/
 */

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace DefaultParameterId {

// パーツID
extern const csmChar* HitAreaPrefix;
extern const csmChar* HitAreaHead;
extern const csmChar* HitAreaBody;
extern const csmChar* PartsIdCore;
extern const csmChar* PartsArmPrefix;
extern const csmChar* PartsArmLPrefix;
extern const csmChar* PartsArmRPrefix;

// パラメータID
extern const csmChar* ParamAngleX;
extern const csmChar* ParamAngleY;
extern const csmChar* ParamAngleZ;
extern const csmChar* ParamEyeLOpen;
extern const csmChar* ParamEyeLSmile;
extern const csmChar* ParamEyeROpen;
extern const csmChar* ParamEyeRSmile;
extern const csmChar* ParamEyeBallX;
extern const csmChar* ParamEyeBallY;
extern const csmChar* ParamEyeBallForm;
extern const csmChar* ParamBrowLY;
extern const csmChar* ParamBrowRY;
extern const csmChar* ParamBrowLX;
extern const csmChar* ParamBrowRX;
extern const csmChar* ParamBrowLAngle;
extern const csmChar* ParamBrowRAngle;
extern const csmChar* ParamBrowLForm;
extern const csmChar* ParamBrowRForm;
extern const csmChar* ParamMouthForm;
extern const csmChar* ParamMouthOpenY;
extern const csmChar* ParamCheek;
extern const csmChar* ParamBodyAngleX;
extern const csmChar* ParamBodyAngleY;
extern const csmChar* ParamBodyAngleZ;
extern const csmChar* ParamBreath;
extern const csmChar* ParamArmLA;
extern const csmChar* ParamArmRA;
extern const csmChar* ParamArmLB;
extern const csmChar* ParamArmRB;
extern const csmChar* ParamHandL;
extern const csmChar* ParamHandR;
extern const csmChar* ParamHairFront;
extern const csmChar* ParamHairSide;
extern const csmChar* ParamHairBack;
extern const csmChar* ParamHairFluffy;
extern const csmChar* ParamShoulderY;
extern const csmChar* ParamBustX;
extern const csmChar* ParamBustY;
extern const csmChar* ParamBaseX;
extern const csmChar* ParamBaseY;
extern const csmChar* ParamNONE;
}}}}

//--------- LIVE2D NAMESPACE ------------
