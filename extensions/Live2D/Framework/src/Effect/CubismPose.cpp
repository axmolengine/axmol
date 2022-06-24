/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismPose.hpp"
#include "Id/CubismIdManager.hpp"

using namespace Live2D::Cubism::Framework;

namespace Live2D { namespace Cubism { namespace Framework {

namespace {
const csmFloat32 Epsilon= 0.001f;
const csmFloat32 DefaultFadeInSeconds = 0.5f;

// Pose.jsonのタグ
const csmChar*   FadeIn = "FadeInTime";
const csmChar*   Link   = "Link";
const csmChar*   Groups = "Groups";
const csmChar*   Id     = "Id";
}

CubismPose::PartData::PartData()
{ }

CubismPose::PartData::~PartData()
{ }

CubismPose::PartData::PartData(const PartData& v)
                                            : ParameterIndex(0)
                                            , PartIndex(0)
{
    PartId = v.PartId;

    for (csmVector<PartData>::const_iterator ite = v.Link.Begin(); ite != v.Link.End(); ++ite)
    {
        Link.PushBack(*ite);
    }
}

CubismPose::PartData& CubismPose::PartData::operator=(const PartData& v)
{
    PartId = v.PartId;

    for (csmVector<PartData>::const_iterator ite = v.Link.Begin(); ite != v.Link.End(); ++ite)
    {
        Link.PushBack(*ite);
    }

    return (*this);
}

void CubismPose::PartData::Initialize(CubismModel* model)
{
    ParameterIndex = model->GetParameterIndex(PartId);
    PartIndex = model->GetPartIndex(PartId);

    model->SetParameterValue(ParameterIndex, 1);
}

CubismPose::CubismPose() : _fadeTimeSeconds(DefaultFadeInSeconds)
                         , _lastModel(NULL)
{ }

CubismPose::~CubismPose()
{ }

CubismPose* CubismPose::Create(const csmByte* pose3json, csmSizeInt size)
{
    CubismPose*         ret = CSM_NEW CubismPose();
    Utils::CubismJson*  json = Utils::CubismJson::Create(pose3json, size);
    Utils::Value&       root = json->GetRoot();

    // フェード時間の指定
    if (!root[FadeIn].IsNull())
    {
        ret->_fadeTimeSeconds = root[FadeIn].ToFloat(DefaultFadeInSeconds);

        if (ret->_fadeTimeSeconds < 0.0f)
        {
            ret->_fadeTimeSeconds = DefaultFadeInSeconds;
        }
    }

    // パーツグループ
    Utils::Value&      poseListInfo = root[Groups];
    const csmInt32     poseCount = poseListInfo.GetSize();

    for (csmInt32 poseIndex = 0; poseIndex < poseCount; ++poseIndex)
    {
        Utils::Value&      idListInfo = poseListInfo[poseIndex];
        const csmInt32    idCount = idListInfo.GetSize();
        csmInt32    groupCount = 0;

        for (csmInt32 groupIndex = 0; groupIndex < idCount; ++groupIndex)
        {
            Utils::Value&   partInfo = idListInfo[groupIndex];
            PartData        partData;
            const CubismIdHandle parameterId = CubismFramework::GetIdManager()->GetId(partInfo[Id].GetRawString());

            partData.PartId = parameterId;

            // リンクするパーツの設定
            if (!partInfo[Link].IsNull())
            {
                Utils::Value&   linkListInfo = partInfo[Link];
                const csmInt32        linkCount = linkListInfo.GetSize();

                for (csmInt32 linkIndex = 0; linkIndex < linkCount; ++linkIndex)
                {
                    PartData             linkPart;
                    const CubismIdHandle linkId = CubismFramework::GetIdManager()->GetId(linkListInfo[linkIndex].GetString());

                    linkPart.PartId = linkId;

                    partData.Link.PushBack(linkPart);
                }
            }

            ret->_partGroups.PushBack(partData);

            ++groupCount;
        }

        ret->_partGroupCounts.PushBack(groupCount);

    }

    Utils::CubismJson::Delete(json);

    return ret;
}

void CubismPose::Delete(CubismPose* pose)
{
    CSM_DELETE_SELF(CubismPose, pose);
}

void CubismPose::Reset(CubismModel* model)
{
    csmInt32 beginIndex = 0;

    for (csmUint32 i = 0; i < _partGroupCounts.GetSize(); ++i)
    {
        const csmInt32 groupCount = _partGroupCounts[i];

        for (csmInt32 j = beginIndex; j < beginIndex + groupCount; ++j)
        {
            _partGroups[j].Initialize(model);

            const csmInt32 partsIndex = _partGroups[j].PartIndex;
            const csmInt32 paramIndex = _partGroups[j].ParameterIndex;

            if (partsIndex < 0)
            {
                continue;
            }

            model->SetPartOpacity( partsIndex, (j == beginIndex ? 1.0f : 0.0f));
            model->SetParameterValue(   paramIndex, (j == beginIndex ? 1.0f : 0.0f));

            for (csmUint32 k = 0; k < _partGroups[j].Link.GetSize(); ++k)
            {
                _partGroups[j].Link[k].Initialize(model);
            }
        }

        beginIndex += groupCount;

    }

}

void CubismPose::CopyPartOpacities(CubismModel* model)
{
    for (csmUint32 groupIndex = 0; groupIndex < _partGroups.GetSize(); ++groupIndex)
    {
        PartData& partData = _partGroups[groupIndex];

        if (partData.Link.GetSize() == 0)
        {
            continue; // 連動するパラメータはない
        }

        const csmInt32    partIndex = _partGroups[groupIndex].PartIndex;
        const csmFloat32  opacity = model->GetPartOpacity(partIndex);

        for (csmUint32 linkIndex = 0; linkIndex < partData.Link.GetSize(); ++linkIndex)
        {
            PartData&   linkPart = partData.Link[linkIndex];
            const csmInt32    linkPartIndex = linkPart.PartIndex;

            if (linkPartIndex < 0)
            {
                continue;
            }

            model->SetPartOpacity(linkPartIndex, opacity);
        }
    }
}

void CubismPose::DoFade(CubismModel* model, csmFloat32 deltaTimeSeconds, csmInt32 beginIndex, csmInt32 partGroupCount)
{
    csmInt32    visiblePartIndex = -1;
    csmFloat32  newOpacity = 1.0f;

    const csmFloat32 Phi = 0.5f;
    const csmFloat32 BackOpacityThreshold = 0.15f;

    // 現在、表示状態になっているパーツを取得
    for (csmInt32 i = beginIndex; i < beginIndex + partGroupCount; ++i)
    {
        const csmInt32 partIndex = _partGroups[i].PartIndex;
        const csmInt32 paramIndex = _partGroups[i].ParameterIndex;

        if (model->GetParameterValue(paramIndex) > Epsilon)
        {
            if (visiblePartIndex >= 0)
            {
                break;
            }

            visiblePartIndex = i;
            newOpacity = model->GetPartOpacity(partIndex);

            // 新しい不透明度を計算
            newOpacity += (deltaTimeSeconds / _fadeTimeSeconds);

            if (newOpacity > 1.0f)
            {
                newOpacity = 1.0f;
            }
        }
    }

    if (visiblePartIndex < 0)
    {
        visiblePartIndex = 0;
        newOpacity = 1.0f;
    }

    //  表示パーツ、非表示パーツの不透明度を設定する
    for (csmInt32 i = beginIndex; i < beginIndex + partGroupCount; ++i)
    {
        const csmInt32 partsIndex = _partGroups[i].PartIndex;

        //  表示パーツの設定
        if (visiblePartIndex == i)
        {
            model->SetPartOpacity(partsIndex, newOpacity); // 先に設定
        }
        // 非表示パーツの設定
        else
        {
            csmFloat32 opacity = model->GetPartOpacity(partsIndex);
            csmFloat32 a1;          // 計算によって求められる不透明度

            if (newOpacity < Phi)
            {
                a1 = newOpacity * (Phi - 1) / Phi + 1.0f; // (0,1),(phi,phi)を通る直線式
            }
            else
            {
                a1 = (1 - newOpacity) * Phi / (1.0f - Phi); // (1,0),(phi,phi)を通る直線式
            }

            // 背景の見える割合を制限する場合
            const csmFloat32 backOpacity = (1.0f - a1) * (1.0f - newOpacity);

            if (backOpacity > BackOpacityThreshold)
            {
                a1 = 1.0f - BackOpacityThreshold / (1.0f - newOpacity);
            }

            if (opacity > a1)
            {
                opacity = a1; // 計算の不透明度よりも大きければ（濃ければ）不透明度を上げる
            }

            model->SetPartOpacity(partsIndex, opacity);
        }
    }
}

void CubismPose::UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds)
{
    // 前回のモデルと同じではないときは初期化が必要
    if (model != _lastModel)
    {
        // パラメータインデックスの初期化
        Reset(model);
    }

    _lastModel = model;

    // 設定から時間を変更すると、経過時間がマイナスになることがあるので、経過時間0として対応。
    if (deltaTimeSeconds < 0.0f)
    {
        deltaTimeSeconds = 0.0f;
    }

    csmInt32 beginIndex = 0;

    for (csmUint32 i = 0; i < _partGroupCounts.GetSize(); i++)
    {
        const csmInt32 partGroupCount = _partGroupCounts[i];

        DoFade(model, deltaTimeSeconds, beginIndex, partGroupCount);

        beginIndex += partGroupCount;
    }

    CopyPartOpacities(model);
}

}}}
