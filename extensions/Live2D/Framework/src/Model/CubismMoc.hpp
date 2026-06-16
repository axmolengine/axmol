/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismModel;

/**
 * Handles management of MOC data
 */
class CubismMoc
{
    friend class CubismModel;
public:
    /**
     * Makes an instance.
     *
     * @param mocBytes Buffer containing the loaded MOC file
     * @param size Size of the buffer in bytes
     *
     * @return Created instance
     */
    static CubismMoc* Create(const csmByte* mocBytes, csmSizeInt size, csmBool shouldCheckMocConsistency = false);

    /**
     * Destroys an instance.
     *
     * @param moc `CubismMoc` instance to be destroyed
     */
    static void Delete(CubismMoc* moc);

    /**
     * Makes a model instance.
     *
     * @return Created model instance
     */
    CubismModel* CreateModel();

    /**
     * Destroys a model instance.
     *
     * @param model `CubismModel` instance to be destroyed
     */
    void DeleteModel(CubismModel* model);

    /**
     * Returns the latest MOC file version.
     *
     * @return Version
     */
    static Core::csmMocVersion GetLatestMocVersion();

    /**
     * Returns the version of the loaded MOC file.
     *
     * @return Version
     */
    Core::csmMocVersion GetMocVersion();

    /**
     * Returns the version of the unload MOC file.
     *
     * @param mocBytes Buffer containing the loaded MOC file
     * @param size Size of the buffer in bytes
     *
     * @return Version
     */
    static Core::csmMocVersion GetMocVersionFromBuffer(const csmByte* mocBytes, csmSizeInt size);

    /**
     * Checks the consistency of the MOC file.
     *
     * @param address Address of the un-restored MOC file. The address must be aligned to 'csmAlignofMoc'.
     * @param size Size of the MOC file in bytes
     *
     * @return true if the file is consistent; otherwise false
     */
    static csmBool HasMocConsistency(void* address, const csmUint32 size);

    /**
     * Checks the consistency of the MOC file.
     *
     * @param mocBytes Buffer of the MOC file
     * @param size Size of the buffer
     *
     * @return true if the file is consistent; otherwise false
     */
    static csmBool HasMocConsistencyFromUnrevivedMoc(const csmByte* mocBytes, csmSizeInt size);

    /**
     * Delete copy constructor.
     */
    CubismMoc(const CubismMoc&) = delete;

    /**
     * Delete copy assignment operator.
     */
    CubismMoc& operator=(const CubismMoc&) = delete;

protected:
    CubismMoc(Core::csmMoc* moc);

    virtual ~CubismMoc();

    Core::csmMoc*     _moc;
    csmInt32          _modelCount;
    csmUint32         _mocVersion;
};

}}}
