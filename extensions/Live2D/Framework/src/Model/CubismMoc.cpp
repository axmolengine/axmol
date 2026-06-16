/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismMoc.hpp"
#include "CubismModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

CubismMoc* CubismMoc::Create(const csmByte* mocBytes, csmSizeInt size, csmBool shouldCheckMocConsistency)
{
    CubismMoc* cubismMoc = NULL;

    void* alignedBuffer = CSM_MALLOC_ALIGNED(size, Core::csmAlignofMoc);
    memcpy(alignedBuffer, mocBytes, size);

    if (shouldCheckMocConsistency)
    {
        // .moc3の整合性を確認
        csmBool consistency = HasMocConsistency(alignedBuffer, size);
        if (!consistency)
        {
            CSM_FREE_ALIGNED(alignedBuffer);

            // 整合性が確認できなければ処理しない
            CubismLogError("Inconsistent MOC3.");
            return cubismMoc;
        }
    }

    Core::csmMoc* moc = Core::csmReviveMocInPlace(alignedBuffer, size);
    const Core::csmMocVersion version = Core::csmGetMocVersion(alignedBuffer, size);

    if (moc)
    {
        cubismMoc = CSM_NEW CubismMoc(moc);
        cubismMoc->_mocVersion = version;
    }

    return cubismMoc;
}

void CubismMoc::Delete(CubismMoc* moc)
{
    CSM_DELETE_SELF(CubismMoc, moc);
}

CubismMoc::CubismMoc(Core::csmMoc* moc)
                        : _moc(moc)
                        , _modelCount(0)
                        , _mocVersion(0)
{ }

CubismMoc::~CubismMoc()
{
    CSM_ASSERT(_modelCount == 0);

    CSM_FREE_ALIGNED(_moc);
}

CubismModel* CubismMoc::CreateModel()
{
    CubismModel*     cubismModel = NULL;
    const csmUint32  modelSize = Core::csmGetSizeofModel(_moc);
    void*            modelMemory = CSM_MALLOC_ALIGNED(modelSize, Core::csmAlignofModel);

    Core::csmModel* model = Core::csmInitializeModelInPlace(_moc, modelMemory, modelSize);

    if (model)
    {
        cubismModel = CSM_NEW CubismModel(model);
        cubismModel->Initialize();

        ++_modelCount;
    }

    return cubismModel;
}

void CubismMoc::DeleteModel(CubismModel* model)
{
    CSM_DELETE_SELF(CubismModel, model);
    --_modelCount;
}

Core::csmMocVersion CubismMoc::GetLatestMocVersion()
{
    return Core::csmGetLatestMocVersion();
}

Core::csmMocVersion CubismMoc::GetMocVersion()
{
    return _mocVersion;
}

Core::csmMocVersion CubismMoc::GetMocVersionFromBuffer(const csmByte* mocBytes, csmSizeInt size)
{
    if (mocBytes == nullptr || size == 0)
    {
        CubismLogError("Invalid mocBytes or size.");
        return 0;
    }
    void* alignedBuffer = CSM_MALLOC_ALIGNED(size, Core::csmAlignofMoc);
    if (alignedBuffer == nullptr)
    {
        CubismLogError("Failed to allocate aligned memory.");
        return 0;
    }
    memcpy(alignedBuffer, mocBytes, size);
    const Core::csmMocVersion version = Core::csmGetMocVersion(alignedBuffer, size);
    CSM_FREE_ALIGNED(alignedBuffer);
    return version;
}

csmBool CubismMoc::HasMocConsistency(void* address, const csmUint32 size)
{
    csmInt32 isConsistent = Core::csmHasMocConsistency(address, size);
    return isConsistent != 0 ? true : false;
}

csmBool CubismMoc::HasMocConsistencyFromUnrevivedMoc(const csmByte* mocBytes, csmSizeInt size)
{
    void* alignedBuffer = CSM_MALLOC_ALIGNED(size, Core::csmAlignofMoc);
    memcpy(alignedBuffer, mocBytes, size);

    csmBool consistency = CubismMoc::HasMocConsistency(alignedBuffer, size);

    CSM_FREE_ALIGNED(alignedBuffer);

    return consistency;
}

}}}
