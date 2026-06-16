/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Utils/CubismJson.hpp"

namespace Live2D { namespace Cubism { namespace Framework {
/**
 * Handles the management of model part opacities using pose settings
 */
class CubismPose
{
public:
    /**
     * Handles data related to parts
     */
    struct PartData
    {
        /**
         * Constructor
         */
        PartData();

        /**
         * Copy constructor
         */
        PartData(const PartData& v);

        /**
         * Destructor
         */
        virtual ~PartData();

        /**
         * Overloads assignment
         */
        PartData&   operator=(const PartData& v);

        /**
         * Initializes.
         *
         * @param model Model used for initialization
         */
        void                Initialize(CubismModel* model);

        CubismIdHandle                     PartId;                  ///< Part ID
        csmInt32                            ParameterIndex;         ///< Parameter index
        csmInt32                            PartIndex;              ///< Part index
        csmVector<PartData>                 Link;                   ///< Attached parameter
    };

    /**
     * Makes an instance of CubismPose.
     *
     * @param pose3json Buffer where the pose settings file is loaded
     * @param size Size of the buffer in bytes
     *
     * @return Maked instance of CubismPose
     */
    static CubismPose*  Create(const csmByte* pose3json, csmSizeInt size);

    /**
     * Destroys an instance of CubismPose.
     *
     * @param pose Instance of CubismPose to destroy
     */
    static void         Delete(CubismPose* pose);

    /**
     * Updates the parameters of the model.
     *
     * @param model Model to update
     * @param deltaTimeSeconds Current time in seconds
     */
    void                UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

    /**
     * Resets the state.
     *
     * @param model Model to reset
     *
     * @note Parameters with an initial opacity value other than 0 will have their opacity set to 1.
     */
    void                Reset(CubismModel* model);

private:
    CubismPose();

    virtual ~CubismPose();

    void                CopyPartOpacities(CubismModel* model);

    void                DoFade(CubismModel* model, csmFloat32 deltaTimeSeconds, csmInt32 beginIndex, csmInt32 partGroupCount);

    csmVector<PartData>             _partGroups;
    csmVector<csmInt32>             _partGroupCounts;
    csmFloat32                      _fadeTimeSeconds;
    CubismModel*                    _lastModel;
};

}}}
