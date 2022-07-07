/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismIdManager.hpp"
#include "CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismIdManager::CubismIdManager()
{ }

CubismIdManager::~CubismIdManager()
{
    for (csmUint32 i = 0; i < _ids.GetSize(); ++i)
    {
        CSM_DELETE_SELF(CubismId, _ids[i]);
    }
}

void CubismIdManager::RegisterIds(const csmChar** ids, csmInt32 count)
{
    for (csmInt32 i = 0; i < count; ++i)
    {
        RegisterId(ids[i]);
    }
}

void CubismIdManager::RegisterIds(const csmVector<csmString>& ids)
{
    for (csmUint32 i = 0; i < ids.GetSize(); ++i)
    {
        RegisterId(ids[i]);
    }
}

const CubismId* CubismIdManager::GetId(const csmString& id)
{
    return RegisterId(id.GetRawString());
}

const CubismId* CubismIdManager::GetId(const csmChar* id)
{
    return RegisterId(id);
}

csmBool CubismIdManager::IsExist(const csmString& id) const
{
    return IsExist(id.GetRawString());
}
csmBool CubismIdManager::IsExist(const csmChar* id) const
{
    return (FindId(id) != NULL);
}

const CubismId* CubismIdManager::RegisterId(const csmChar* id)
{
    CubismId* result = NULL;

    if ((result = FindId(id)) != NULL)
    {
        return result;
    }

    result = CSM_NEW CubismId(id);
    _ids.PushBack(result);

    return result;
}

const CubismId* CubismIdManager::RegisterId(const csmString& id)
{
    return RegisterId(id.GetRawString());
}

CubismId* CubismIdManager::FindId(const csmChar* id) const
{
    for (csmUint32 i = 0; i < _ids.GetSize(); ++i)
    {
        if (_ids[i]->GetString() == id)
        {
            return _ids[i];
        }
    }

    return NULL;
}

}}}
