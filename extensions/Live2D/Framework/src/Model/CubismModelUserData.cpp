/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismModelUserData.hpp"
#include "CubismModelUserDataJson.hpp"
#include "Utils/CubismString.hpp"

namespace Live2D {  namespace Cubism {  namespace Framework {

namespace
{
const Live2D::Cubism::Framework::csmChar*  ArtMesh = "ArtMesh";
}

CubismModelUserData::~CubismModelUserData()
{
    for (csmUint32 i = 0;i < _userDataNodes.GetSize(); ++i)
    {
        CSM_DELETE(const_cast<CubismModelUserDataNode*>(_userDataNodes[i]));
    }
}

const csmVector<const CubismModelUserData::CubismModelUserDataNode*>& CubismModelUserData::GetArtMeshUserDatas() const
{
    return _artMeshUserDataNodes;
}

CubismModelUserData* CubismModelUserData::Create(const csmByte* buffer, const csmSizeInt size)
{
    CubismModelUserData* ret = CSM_NEW CubismModelUserData();

    ret->ParseUserData(buffer, size);

    return ret;
}

void CubismModelUserData::Delete(CubismModelUserData* modelUserData)
{
    CSM_DELETE_SELF(CubismModelUserData, modelUserData);
}

void CubismModelUserData::ParseUserData(const csmByte* buffer, const csmSizeInt size)
{
    CubismModelUserDataJson* json = CSM_NEW CubismModelUserDataJson(buffer, size);

    const ModelUserDataType typeOfArtMesh = CubismFramework::GetIdManager()->GetId(ArtMesh);

    const csmUint32 nodeCount = json->GetUserDataCount();

    for (csmUint32 i = 0; i < nodeCount; i++)
    {
        CubismModelUserDataNode*    addNode = CSM_NEW CubismModelUserDataNode();
        addNode->TargetId = json->GetUserDataId(i);
        addNode->TargetType = CubismFramework::GetIdManager()->GetId(json->GetUserDataTargetType(i));
        addNode->Value = json->GetUserDataValue(i);
        _userDataNodes.PushBack(addNode);

        if (addNode->TargetType == typeOfArtMesh)
        {
            _artMeshUserDataNodes.PushBack(addNode);
        }
    }

    CSM_DELETE(json);
}
}}}
