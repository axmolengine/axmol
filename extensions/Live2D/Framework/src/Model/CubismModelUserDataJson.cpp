/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismModelUserDataJson.hpp"
#include "CubismModelUserData.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D {  namespace Cubism {  namespace Framework {

namespace {
const csmChar* Meta = "Meta";
const csmChar* UserDataCount = "UserDataCount";
const csmChar* TotalUserDataSize = "TotalUserDataSize";
const csmChar* UserData = "UserData";
const csmChar* Target = "Target";
const csmChar* Id = "Id";
const csmChar* Value = "Value";
}
CubismModelUserDataJson::CubismModelUserDataJson(const csmByte* buffer, csmSizeInt size)
{
    CreateCubismJson(buffer, size);
}

CubismModelUserDataJson::~CubismModelUserDataJson()
{
    DeleteCubismJson();
}

csmInt32 CubismModelUserDataJson::GetUserDataCount() const
{
    return _json->GetRoot()[Meta][UserDataCount].ToInt();
}

csmInt32 CubismModelUserDataJson::GetTotalUserDataSize() const
{
    return _json->GetRoot()[Meta][TotalUserDataSize].ToInt();
}

csmString CubismModelUserDataJson::GetUserDataTargetType(const csmInt32 i) const
{
    return  _json->GetRoot()[UserData][i][Target].GetRawString();
}

CubismIdHandle CubismModelUserDataJson::GetUserDataId(const csmInt32 i) const
{
    return CubismFramework::GetIdManager()->GetId(_json->GetRoot()[UserData][i][Id].GetRawString());
}

const csmChar* CubismModelUserDataJson::GetUserDataValue(const csmInt32 i) const
{
    return _json->GetRoot()[UserData][i][Value].GetRawString();
}

}}}
