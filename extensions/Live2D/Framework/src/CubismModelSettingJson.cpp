/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismModelSettingJson.hpp"
#include "CubismFramework.hpp"
#include "Type/csmMap.hpp"
#include "Id/CubismId.hpp"
#include "Id/CubismIdManager.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief   Model3Jsonのキー文字列
 *
 */
namespace {

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#endif

// JSON keys
const csmChar* Version = "Version";
const csmChar* FileReferences = "FileReferences";
const csmChar* Groups = "Groups";
const csmChar* Layout = "Layout";
const csmChar* HitAreas = "HitAreas";

const csmChar* Moc = "Moc";
const csmChar* Textures = "Textures";
const csmChar* Physics = "Physics";
const csmChar* DisplayInfo = "DisplayInfo";
const csmChar* Pose = "Pose";
const csmChar* Expressions = "Expressions";
const csmChar* Motions = "Motions";

const csmChar* UserData = "UserData";
const csmChar* Name = "Name";
const csmChar* FilePath = "File";
const csmChar* Id = "Id";
const csmChar* Ids = "Ids";
const csmChar* Target = "Target";

// Motions
const csmChar* Idle = "Idle";
const csmChar* TapBody = "TapBody";
const csmChar* PinchIn = "PinchIn";
const csmChar* PinchOut = "PinchOut";
const csmChar* Shake = "Shake";
const csmChar* FlickHead = "FlickHead";
const csmChar* Parameter = "Parameter";

const csmChar* SoundPath = "Sound";
const csmChar* FadeInTime = "FadeInTime";
const csmChar* FadeOutTime = "FadeOutTime";

// Layout
const csmChar* CenterX = "CenterX";
const csmChar* CenterY = "CenterY";
const csmChar* X = "X";
const csmChar* Y = "Y";
const csmChar* Width = "Width";
const csmChar* Height = "Height";

const csmChar* LipSync = "LipSync";
const csmChar* EyeBlink = "EyeBlink";

const csmChar* InitParameter = "init_param";
const csmChar* InitPartsVisible = "init_parts_visible";
const csmChar* Val = "val";

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

// キーが存在するかどうかのチェック
csmBool CubismModelSettingJson::IsExistModelFile() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Moc]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistTextureFiles() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Textures]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistHitAreas() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_HitAreas]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistPhysicsFile() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Physics]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistPoseFile() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Pose]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistDisplayInfoFile() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_DisplayInfo]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistExpressionFile() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Expressions]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistMotionGroups() const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Motions]);
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistMotionGroupName(const csmChar* groupName) const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Motions])[groupName];
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistMotionSoundFile(const csmChar* groupName, csmInt32 index) const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Motions])[groupName][index][SoundPath];
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistMotionFadeIn(const csmChar* groupName, csmInt32 index) const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Motions])[groupName][index][FadeInTime];
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistMotionFadeOut(const csmChar* groupName, csmInt32 index) const
{
    Utils::Value& node = (*_jsonValue[FrequentNode_Motions])[groupName][index][FadeOutTime];
    return !node.IsNull() && !node.IsError();
}
csmBool CubismModelSettingJson::IsExistUserDataFile() const { return !_json->GetRoot()[FileReferences][UserData].IsNull(); }


csmBool CubismModelSettingJson::IsExistEyeBlinkParameters() const
{
    if (_jsonValue[FrequentNode_Groups]->IsNull() || _jsonValue[FrequentNode_Groups]->IsError())
    {
        return false;
    }

    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); ++i)
    {
        if (strcmp((*_jsonValue[FrequentNode_Groups])[i][Name].GetRawString(), EyeBlink) == 0)
        {
            return true;
        }
    }
    return false;
}

csmBool CubismModelSettingJson::IsExistLipSyncParameters() const
{
    if (_jsonValue[FrequentNode_Groups]->IsNull() || _jsonValue[FrequentNode_Groups]->IsError())
    {
        return false;
    }

    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); ++i)
    {
        if (strcmp((*_jsonValue[FrequentNode_Groups])[i][Name].GetRawString(), LipSync) == 0)
        {
            return true;
        }
    }
    return false;
}

CubismModelSettingJson::CubismModelSettingJson(const csmByte* buffer, csmSizeInt size)
{
    CreateCubismJson(buffer, size);

    if (_json)
    {
        _jsonValue.Clear();

        // 順番はenum FrequentNodeと一致させる
        _jsonValue.PushBack(&(_json->GetRoot()[Groups]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Moc]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Motions]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][DisplayInfo]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Expressions]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Textures]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Physics]));
        _jsonValue.PushBack(&(_json->GetRoot()[FileReferences][Pose]));
        _jsonValue.PushBack(&(_json->GetRoot()[HitAreas]));
    }
}

CubismModelSettingJson::~CubismModelSettingJson()
{
    DeleteCubismJson();
}

Utils::CubismJson* CubismModelSettingJson::GetJsonPointer() const
{
    return _json;
}

const csmChar* CubismModelSettingJson::GetModelFileName()
{
    if (!IsExistModelFile())return "";
    return (*_jsonValue[FrequentNode_Moc]).GetRawString();
}

// テクスチャについて
csmInt32 CubismModelSettingJson::GetTextureCount()
{
    if (!IsExistTextureFiles())return 0;
    return (*_jsonValue[FrequentNode_Textures]).GetSize();
}

const csmChar* CubismModelSettingJson::GetTextureDirectory()
{
    if (!IsExistTextureFiles())
    {
        return "";
    }

    csmVector<csmString> splitPathBuffer;
    csmVector<csmChar> charBuffer;
    const csmChar* rawString = (*_jsonValue[FrequentNode_Textures])[0].GetRawString();
    csmInt32 rawStringSize = (*_jsonValue[FrequentNode_Textures])[0].GetString().GetLength();
    for (csmInt32 i = 0; i < rawStringSize; i++)
    {
        // /を含んでいる場合splitする
        if (rawString[i] == '/')
        {
            csmString str = csmString(charBuffer.GetPtr());
            splitPathBuffer.PushBack(str);
            charBuffer.Clear();
        }
        // 一文字ずつVector配列に格納する
        else
        {
            charBuffer.PushBack(static_cast<csmChar>(rawString[i] & 0xFF));
        }
    }

    csmInt32 arrayLength = splitPathBuffer.GetSize();
    csmString textureDirectoryStr = csmString();
    for(csmInt32 i = 0; i < arrayLength; i++)
    {
        textureDirectoryStr = textureDirectoryStr + splitPathBuffer[i];
        if(i < arrayLength - 1)
        {
            textureDirectoryStr = textureDirectoryStr + "/";
        }
    }

    return textureDirectoryStr.GetRawString();
}

const csmChar* CubismModelSettingJson::GetTextureFileName(csmInt32 index)
{
    return (*_jsonValue[FrequentNode_Textures])[index].GetRawString();
}

// あたり判定について
csmInt32 CubismModelSettingJson::GetHitAreasCount()
{
    if (!IsExistHitAreas())return 0;
    return (*_jsonValue[FrequentNode_HitAreas]).GetSize();
}

CubismIdHandle CubismModelSettingJson::GetHitAreaId(csmInt32 index)
{
    return CubismFramework::GetIdManager()->GetId((*_jsonValue[FrequentNode_HitAreas])[index][Id].GetRawString());
}

const csmChar* CubismModelSettingJson::GetHitAreaName(csmInt32 index)
{
    return (*_jsonValue[FrequentNode_HitAreas])[index][Name].GetRawString();
}

// 物理演算、表示名称、パーツ切り替え、表情ファイルについて
const csmChar* CubismModelSettingJson::GetPhysicsFileName()
{
    if (!IsExistPhysicsFile())return "";
    return (*_jsonValue[FrequentNode_Physics]).GetRawString();
}

const csmChar* CubismModelSettingJson::GetPoseFileName()
{
    if (!IsExistPoseFile())return "";
    return (*_jsonValue[FrequentNode_Pose]).GetRawString();
}

const csmChar* CubismModelSettingJson::GetDisplayInfoFileName()
{
    if (!IsExistDisplayInfoFile())return "";
    return (*_jsonValue[FrequentNode_DisplayInfo]).GetRawString();
}

csmInt32 CubismModelSettingJson::GetExpressionCount()
{
    if (!IsExistExpressionFile())return 0;
    return (*_jsonValue[FrequentNode_Expressions]).GetSize();
}

const csmChar* CubismModelSettingJson::GetExpressionName(csmInt32 index)
{
    return (*_jsonValue[FrequentNode_Expressions])[index][Name].GetRawString();
}

const csmChar* CubismModelSettingJson::GetExpressionFileName(csmInt32 index)
{
    return (*_jsonValue[FrequentNode_Expressions])[index][FilePath].GetRawString();
}

// モーションについて
csmInt32 CubismModelSettingJson::GetMotionGroupCount()
{
    if (!IsExistMotionGroups())
    {
        return 0;
    }
    return (*_jsonValue[FrequentNode_Motions]).GetKeys().GetSize();
}

const csmChar* CubismModelSettingJson::GetMotionGroupName(csmInt32 index)
{
    if (!IsExistMotionGroups())
    {
        return NULL;
    }
    return (*_jsonValue[FrequentNode_Motions]).GetKeys()[index].GetRawString();
}

csmInt32 CubismModelSettingJson::GetMotionCount(const csmChar* groupName)
{
    if (!IsExistMotionGroupName(groupName))return 0;
    return (*_jsonValue[FrequentNode_Motions])[groupName].GetSize();
}

const csmChar* CubismModelSettingJson::GetMotionFileName(const csmChar* groupName, csmInt32 index)
{
    if (!IsExistMotionGroupName(groupName))return "";
    return (*_jsonValue[FrequentNode_Motions])[groupName][index][FilePath].GetRawString();
}

const csmChar* CubismModelSettingJson::GetMotionSoundFileName(const csmChar* groupName, csmInt32 index)
{
    if (!IsExistMotionSoundFile(groupName, index))return "";
    return (*_jsonValue[FrequentNode_Motions])[groupName][index][SoundPath].GetRawString();
}

csmFloat32 CubismModelSettingJson::GetMotionFadeInTimeValue(const csmChar* groupName, csmInt32 index)
{
    if (!IsExistMotionFadeIn(groupName, index))return -1.0f;
    return (*_jsonValue[FrequentNode_Motions])[groupName][index][FadeInTime].ToFloat();
}

csmFloat32 CubismModelSettingJson::GetMotionFadeOutTimeValue(const csmChar* groupName, csmInt32 index)
{
    if (!IsExistMotionFadeOut(groupName, index))return -1.0f;
    return (*_jsonValue[FrequentNode_Motions])[groupName][index][FadeOutTime].ToFloat();
}


const csmChar* CubismModelSettingJson::GetUserDataFile()
{
    if (!IsExistUserDataFile())
    {
        return "";
    }
    return _json->GetRoot()[FileReferences][UserData].GetRawString();
}

csmBool CubismModelSettingJson::GetLayoutMap(csmMap<csmString, csmFloat32>& outLayoutMap)
{
    csmMap<csmString, Utils::Value*>* map = _json->GetRoot()[Layout].GetMap();
    if (map == NULL)
    {
        return false;
    }
    csmMap<csmString, Utils::Value*>::const_iterator map_ite;
    csmBool ret = false;
    for (map_ite = map->Begin(); map_ite != map->End(); ++map_ite)
    {
        outLayoutMap[map_ite->First] = map_ite->Second->ToFloat();
        ret = true;
    }
    return ret;
}

csmInt32 CubismModelSettingJson::GetEyeBlinkParameterCount()
{
    if (!IsExistEyeBlinkParameters())
    {
        return 0;
    }

    csmInt32 num = 0;
    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); i++)
    {
        Utils::Value& refI = (*_jsonValue[FrequentNode_Groups])[i];
        if(refI.IsNull() || refI.IsError())
        {
            continue;
        }

        if (strcmp(refI[Name].GetRawString(), EyeBlink) == 0)
        {
            num = refI[Ids].GetVector()->GetSize();
            break;
        }
    }

    return num;
}

CubismIdHandle CubismModelSettingJson::GetEyeBlinkParameterId(csmInt32 index)
{
    if (!IsExistEyeBlinkParameters())
    {
        return NULL;
    }

    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); i++)
    {
        Utils::Value& refI = (*_jsonValue[FrequentNode_Groups])[i];
        if (refI.IsNull() || refI.IsError())
        {
            continue;
        }

        if (strcmp(refI[Name].GetRawString(), EyeBlink) == 0)
        {
            return CubismFramework::GetIdManager()->GetId(refI[Ids][index].GetRawString());
        }
    }
    return NULL;
}

csmInt32 CubismModelSettingJson::GetLipSyncParameterCount()
{
    if (!IsExistLipSyncParameters())
    {
        return 0;
    }

    csmInt32 num = 0;
    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); i++)
    {
        Utils::Value& refI = (*_jsonValue[FrequentNode_Groups])[i];
        if (refI.IsNull() || refI.IsError())
        {
            continue;
        }

        if (strcmp(refI[Name].GetRawString(), LipSync) == 0)
        {
            num = refI[Ids].GetVector()->GetSize();
            break;
        }
    }

    return num;
}

CubismIdHandle CubismModelSettingJson::GetLipSyncParameterId(csmInt32 index)
{
    if (!IsExistLipSyncParameters())
    {
        return NULL;
    }

    for (csmInt32 i = 0; i < _jsonValue[FrequentNode_Groups]->GetSize(); i++)
    {
        Utils::Value& refI = (*_jsonValue[FrequentNode_Groups])[i];
        if (refI.IsNull() || refI.IsError())
        {
            continue;
        }

        if (strcmp(refI[Name].GetRawString(), LipSync) == 0)
        {
            return CubismFramework::GetIdManager()->GetId(refI[Ids][index].GetRawString());
        }
    }
    return NULL;
}

}}}
