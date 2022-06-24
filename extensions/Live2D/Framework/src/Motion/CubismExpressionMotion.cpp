/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismExpressionMotion.hpp"
#include "Id/CubismIdManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

namespace {
// exp3.jsonのキーとデフォルト値
const csmChar* ExpressionKeyFadeIn = "FadeInTime";
const csmChar* ExpressionKeyFadeOut = "FadeOutTime";
const csmChar* ExpressionKeyParameters = "Parameters";
const csmChar* ExpressionKeyId = "Id";
const csmChar* ExpressionKeyValue = "Value";
const csmChar* ExpressionKeyBlend = "Blend";
const csmChar* BlendValueAdd = "Add";
const csmChar* BlendValueMultiply = "Multiply";
const csmChar* BlendValueOverwrite = "Overwrite";
const csmFloat32 DefaultFadeTime = 1.0f;
}

CubismExpressionMotion::CubismExpressionMotion()
{ }

CubismExpressionMotion::~CubismExpressionMotion()
{ }

CubismExpressionMotion* CubismExpressionMotion::Create(const csmByte* buffer, csmSizeInt size)
{
    CubismExpressionMotion* expression = CSM_NEW CubismExpressionMotion();

    Utils::CubismJson* json = Utils::CubismJson::Create(buffer, size);
    Utils::Value& root = json->GetRoot();

    expression->SetFadeInTime(root[ExpressionKeyFadeIn].ToFloat(DefaultFadeTime));   // フェードイン
    expression->SetFadeOutTime(root[ExpressionKeyFadeOut].ToFloat(DefaultFadeTime)); // フェードアウト

    // 各パラメータについて
    const csmInt32 parameterCount = root[ExpressionKeyParameters].GetSize();
    expression->_parameters.PrepareCapacity(parameterCount);

    for (csmInt32 i = 0; i < parameterCount; ++i)
    {
        Utils::Value& param = root[ExpressionKeyParameters][i];
        const CubismIdHandle parameterId = CubismFramework::GetIdManager()->GetId(param[ExpressionKeyId].GetRawString()); // パラメータID
        const csmFloat32 value = static_cast<csmFloat32>(param[ExpressionKeyValue].ToFloat());   // 値

        // 計算方法の設定
        ExpressionBlendType blendType;

        if (param[ExpressionKeyBlend].IsNull() || param[ExpressionKeyBlend].GetString() == BlendValueAdd)
        {
            blendType = ExpressionBlendType_Add;
        }
        else if (param[ExpressionKeyBlend].GetString() == BlendValueMultiply)
        {
            blendType = ExpressionBlendType_Multiply;
        }
        else if (param[ExpressionKeyBlend].GetString() == BlendValueOverwrite)
        {
            blendType = ExpressionBlendType_Overwrite;
        }
        else
        {
            // その他 仕様にない値を設定したときは加算モードにすることで復旧
            blendType = ExpressionBlendType_Add;
        }

        // 設定オブジェクトを作成してリストに追加する
        ExpressionParameter item;

        item.ParameterId = parameterId;
        item.BlendType   = blendType;
        item.Value       = value;

        expression->_parameters.PushBack(item);
    }

    Utils::CubismJson::Delete(json); // JSONデータは不要になったら削除する

    return expression;
}

void CubismExpressionMotion::DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 weight, CubismMotionQueueEntry* motionQueueEntry)
{
    for (csmUint32 i = 0; i < _parameters.GetSize(); ++i)
    {
        ExpressionParameter& parameter = _parameters[i];

        switch (parameter.BlendType)
        {
        case ExpressionBlendType_Add: {
            model->AddParameterValue(parameter.ParameterId, parameter.Value, weight);            // 相対変化 加算
            break;
        }
        case ExpressionBlendType_Multiply: {
            model->MultiplyParameterValue(parameter.ParameterId, parameter.Value, weight);       // 相対変化 乗算
            break;
        }
        case ExpressionBlendType_Overwrite: {
            model->SetParameterValue(parameter.ParameterId, parameter.Value, weight);            // 絶対変化 上書き
            break;
        }
        default:
            // 仕様にない値を設定したときは既に加算モードになっている
            break;
        }
    }
}

}}}
