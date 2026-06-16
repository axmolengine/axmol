/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include "CubismModelMultiplyAndScreenColor.hpp"
#include "Rendering/csmBlendMode.hpp"
#include "Type/csmMap.hpp"
#include "Type/csmVector.hpp"
#include "Rendering/CubismRenderer.hpp"
#include "Id/CubismId.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismMoc;

/**
 * Information for a Cubism model object.
 */
struct CubismModelObjectInfo
{
    /**
     * type used in object information
     */
    enum ObjectType
    {
        ObjectType_Drawable = 0,
        ObjectType_Parts = 1
    };

    /**
     * Constructor.
     *
     * @param objectIndex index of the object
     * @param type type of the object (Drawable or Parts)
     */
    CubismModelObjectInfo(csmUint32 objectIndex, ObjectType type)
        : ObjectIndex(objectIndex)
        , ObjectType(type)
    {
    }

    /**
     * Destructor
     */
    ~CubismModelObjectInfo()
    {
    }

    ObjectType ObjectType;  ///< Type of the object (Drawable or Parts)
    csmUint32 ObjectIndex;  ///< Index of the object
};

/**
 * Information for part child draw objects.
 */
struct PartChildDrawObjects
{
    /**
     * Constructor
     */
    PartChildDrawObjects()
        : DrawableIndices()
        , OffscreenIndices()
    {
    }
    /**
     * Constructor
     *
     * @param drawableIndices collection of Drawable indices
     * @param offscreenIndices collection of Offscreen indices
     */
    PartChildDrawObjects(csmVector<csmUint32> drawableIndices, csmVector<csmUint32> offscreenIndices)
        : DrawableIndices(drawableIndices)
        , OffscreenIndices(offscreenIndices)
    {
    }

    /**
     * Destructor.
     */
    ~PartChildDrawObjects()
    {
    }

    csmVector<csmUint32> DrawableIndices;  ///< Collection of Drawable indices
    csmVector<csmUint32> OffscreenIndices; ///< Collection of Offscreen indices
};

/**
 * Information for a Cubism model part.
 */
struct CubismModelPartInfo
{
    /**
     * Constructor
     */
    CubismModelPartInfo()
        : Objects()
        , ChildDrawObjects()
    {
    }

    /**
     * Constructor
     *
     * @param objects collection of CubismModelObjectInfo
     * @param childDrawObjects information of part child draw objects
     */
    CubismModelPartInfo(csmVector<CubismModelObjectInfo> objects, PartChildDrawObjects childDrawObjects)
        : Objects(objects)
        , ChildDrawObjects(childDrawObjects)
    {
    }

    /**
     * Destructor
     */
    ~CubismModelPartInfo()
    {
    }

    csmVector<CubismModelObjectInfo> Objects; ///< Collection of object information
    PartChildDrawObjects ChildDrawObjects;    ///< Information of part child draw objects

    /**
     * Returns the number of child objects.
     *
     * @return Number of child objects
     */
    csmInt32 GetChildObjectCount() const
    {
        return static_cast<csmInt32>(Objects.GetSize());
    }
};

/**
 * Handles models created from MOC data.
 */
class CubismModel
{
    friend class CubismMoc;
public:
    enum CubismNoIndex
    {
        CubismNoIndex_Parent = -1,    ///< Index value when no parent exists.
        CubismNoIndex_Offscreen = -1  ///< Index value when no referenced offscreen exists.
    };

    /**
     * Structure to manage texture culling settings
     */
    struct CullingData
    {
        /**
         * Constructor
         */
        CullingData()
            : IsOverridden(false)
            , IsCulling(0)
        {
        }

        /**
         * Constructor
         *
         * @param isOverridden whether to be overridden
         * @param isCulling Culling information
         */
        CullingData(csmBool isOverridden, csmInt32 isCulling)
            : IsOverridden(isOverridden)
            , IsCulling(isCulling)
        {
        }

        /**
         * Destructor
         */
        ~CullingData()
        {
        }

        csmBool IsOverridden;      ///< Whether to be overridden
        csmInt32 IsCulling;         ///< Culling information
    };

    /**
     * Structure for managing the override of parameter repetition settings
     */
    struct ParameterRepeatData
    {
        /**
         * Constructor
         */
        ParameterRepeatData()
            : IsOverridden(false)
            , IsParameterRepeated(false)
        {
        }

        /**
         * Constructor
         *
         * @param isOverridden whether to be overriden
         * @param isParameterRepeated override flag for settings
         */
        ParameterRepeatData(csmBool isOverridden, csmBool isParameterRepeated)
            : IsOverridden(isOverridden)
            , IsParameterRepeated(isParameterRepeated)
        {
        }

        /**
         * Destructor
         */
        ~ParameterRepeatData()
        {
        }

        csmBool IsOverridden;            ///< Whether to be overridden
        csmBool IsParameterRepeated;     ///< Override flag for settings
    };

    /**
     * Calculates and updates the model state based on the set parameters.
     */
    void Update() const;

    /**
     * Returns the width of the canvas.
     *
     * @return Width of the canvas in pixels
     */
    csmFloat32 GetCanvasWidthPixel() const;

    /**
     * Returns the height of the canvas.
     *
     * @return Height of the canvas in pixels
     */
    csmFloat32 GetCanvasHeightPixel() const;

    /**
     * Returns the pixels per unit (PPU).
     *
     * @return Pixels per unit
     */
    csmFloat32 GetPixelsPerUnit() const;

    /**
     * Returns the width of the canvas.
     *
     * @return Width of the canvas in PPU (pixels per unit)
     */
    csmFloat32 GetCanvasWidth() const;

    /**
     * Returns the height of the canvas.
     *
     * @return Height of the canvas in PPU (pixels per unit)
     */
    csmFloat32 GetCanvasHeight() const;

    /**
     * Returns the list of object render orders.
     *
     * @return List of object render orders
     */
    const csmInt32* GetRenderOrders() const;

    //========================================================
    //  Part Functions.
    //========================================================

    /**
     * Returns the index of the part.
     *
     * @param partId Part ID
     *
     * @return Index of the part
     */
    csmInt32 GetPartIndex(CubismIdHandle partId);

    /**
     * Returns the ID of the part.
     *
     * @param partIndex Index of the part
     * @return Part ID
     */
    CubismIdHandle GetPartId(csmUint32 partIndex);

    /**
     * Returns the number of parts.
     *
     * @return Number of parts
     */
    csmInt32 GetPartCount() const;

    /**
     * Returns the index of the parent parts for the parts.
     *
     * @return Index of parent parts for the parts.
     */
    const csmInt32* GetPartParentPartIndices() const;

    /**
    * Returns the index of the offscreen sources for the part.
    *
    * @return Index of offscreen sources for the part.
    */
    const csmInt32* GetPartOffscreenIndices() const;

    /**
     * Sets the opacity of the part.
     *
     * @param partId Part ID
     * @param opacity Opacity
     */
    void SetPartOpacity(CubismIdHandle partId, csmFloat32 opacity);

    /**
     * Sets the opacity of the part.
     *
     * @param partIndex Part index
     * @param opacity Part opacity
     */
    void SetPartOpacity(csmInt32 partIndex, csmFloat32 opacity);

    /**
     * Returns the opacity of the part.
     *
     * @param partId Part ID
     *
     * @return Part opacity
     */
    csmFloat32 GetPartOpacity(CubismIdHandle partId);

    /**
     * Returns the opacity of the part.
     *
     * @param partIndex Part index
     *
     * @return Part opacity
     */
    csmFloat32 GetPartOpacity(csmInt32 partIndex);

    /**
     * Returns the index of the parent part of the part.
     *
     * @param partIndex Part index
     *
     * @return Index of the parent part of the part
     */
    csmInt32 GetPartParentPartIndex(csmUint32 partIndex) const;

    /**
     * Returns the child draw objects of the part.
     *
     * @param partInfoIndex Index of the part info
     */
    void GetPartChildDrawObjects(csmInt32 partInfoIndex);

    /**
     * Returns the parent-child hierarchy of the parts.
     *
     * @return Collection of parts hierarchy
     */
    csmVector<CubismModelPartInfo> GetPartsHierarchy() const;

    //========================================================
    //  Parameter Functions.
    //========================================================

    /**
     * Returns the index of the parameter.
     *
     * @param parameterId Parameter ID
     *
     * @return Parameter index
     */
    csmInt32 GetParameterIndex(CubismIdHandle parameterId);

    /**
     * Returns the ID of the parameter
     *
     * Retrieves the ID of the parameter.
     *
     * @param parameterIndex Index of the parameter
     * @return Parameter ID
     */
    CubismIdHandle GetParameterId(csmUint32 parameterIndex);

    /**
     * Returns the number of parameters.
     *
     * @return Number of parameters
     */
    csmInt32 GetParameterCount() const;

    /**
     * Returns the type of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return Parameter type
     */
    Core::csmParameterType GetParameterType(csmUint32 parameterIndex) const;

    /**
     * Returns the maximum value of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return Maximum value of the parameter
     */
    csmFloat32 GetParameterMaximumValue(csmUint32 parameterIndex) const;

    /**
     * Returns the minimum value of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return Minimum value of the parameter
     */
    csmFloat32 GetParameterMinimumValue(csmUint32 parameterIndex) const;

    /**
     * Returns the default value of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return Default value of the parameter
     */
    csmFloat32 GetParameterDefaultValue(csmUint32 parameterIndex) const;

    /**
     * Returns the value of the parameter.
     *
     * @param parameterId Parameter ID
     *
     * @return Parameter value
     */
    csmFloat32 GetParameterValue(CubismIdHandle parameterId);

    /**
     * Returns the value of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return Parameter value
     */
    csmFloat32 GetParameterValue(csmInt32 parameterIndex);

    /**
     * Sets the value of the parameter.
     *
     * @param parameterId Parameter ID
     * @param value Parameter value
     * @param weight Weight
     */
    void SetParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * Sets the value of the parameter.
     *
     * @param parameterIndex Parameter index
     * @param value Parameter value
     * @param weight Weight
     */
    void SetParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * Gets whether the parameter has the repeat setting.
     *
     * @param parameterIndex Parameter index
     *
     * @return true if it is set, otherwise returns false.
     */
    csmBool IsRepeat(csmInt32 parameterIndex) const;

    /**
     * Returns the calculated result ensuring the value falls within the parameter's range.
     *
     * @param parameterIndex Parameter index
     * @param value Parameter value
     *
     * @return a value that falls within the parameter’s range. If the parameter does not exist, returns it as is.
     */
    csmFloat32 GetParameterRepeatValue(csmInt32 parameterIndex, csmFloat32 value) const;

    /**
     * Returns the result of clamping the value to ensure it falls within the parameter's range.
     *
     * @param parameterIndex Parameter index
     * @param value Parameter value
     *
     * @return the clamped value. If the parameter does not exist, returns it as is.
     */
    csmFloat32 GetParameterClampValue(csmInt32 parameterIndex, csmFloat32 value) const;

    /**
     * Returns the repeat of the parameter.
     *
     * @param parameterIndex Parameter index
     *
     * @return the raw data parameter repeat from the Cubism Core.
     */
    csmBool GetParameterRepeats(csmUint32 parameterIndex) const;

    /**
     * Adds to the value of the parameter.
     *
     * @param parameterId Parameter ID
     * @param value Value to be added
     * @param weight Weight
     */
    void AddParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * Adds to the value of the parameter.
     *
     * @param parameterIndex Parameter index
     * @param value Value to be added
     * @param weight Weight
     */
    void AddParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * Multiplies the value of the parameter.
     *
     * @param parameterId Parameter ID
     * @param value Value to be multiplied
     * @param weight Weight
     */
    void MultiplyParameterValue(CubismIdHandle parameterId, csmFloat32 value, csmFloat32 weight = 1.0f);

    /**
     * Multiplies the value of the parameter.
     *
     * @param parameterIndex Parameter index
     * @param value Value to be multiplied
     * @param weight Weight
     */
    void MultiplyParameterValue(csmInt32 parameterIndex, csmFloat32 value, csmFloat32 weight = 1.0f);

    //========================================================
    //  Drawable Functions.
    //========================================================

    /**
     * Returns the index of the drawable.
     *
     * @param drawableId Drawable ID
     *
     * @return Index of the drawable
     */
    csmInt32 GetDrawableIndex(CubismIdHandle drawableId) const;

    /**
     * Returns the number of drawables.
     *
     * @return Number of drawables
     */
    csmInt32 GetDrawableCount() const;

    /**
     * Returns the ID of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Drawable ID
     */
    CubismIdHandle GetDrawableId(csmInt32 drawableIndex) const;

    /**
     * Returns the texture index attached to the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Texture index attached to the drawable
     */
    csmInt32 GetDrawableTextureIndex(csmInt32 drawableIndex) const;

    /**
     * Returns the number of vertex indices in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Number of vertex indices in the drawable
     */
    csmInt32 GetDrawableVertexIndexCount(csmInt32 drawableIndex) const;

    /**
     * Returns the number of vertices in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Number of vertices in the drawable
     */
    csmInt32 GetDrawableVertexCount(csmInt32 drawableIndex) const;

    /**
     * Returns the list of vertices in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return List of vertices in the drawable
     */
    const csmFloat32* GetDrawableVertices(csmInt32 drawableIndex) const;

    /**
     * Returns the list of vertex indices in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return List of vertex indices in the drawable
     */
    const csmUint16* GetDrawableVertexIndices(csmInt32 drawableIndex) const;

    /**
     * Returns the list of vertices in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return List of vertices in the drawable
     */
    const Core::csmVector2* GetDrawableVertexPositions(csmInt32 drawableIndex) const;

    /**
     * Returns the list of vertex UVs in the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return List of vertex UVs in the drawable
     */
    const Core::csmVector2* GetDrawableVertexUvs(csmInt32 drawableIndex) const;

    /**
     * Returns the opacity of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Opacity of the drawable
     */
    csmFloat32 GetDrawableOpacity(csmInt32 drawableIndex) const;

    /**
     * Returns the multiply color of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Multiply color of the drawable
     */
    Core::csmVector4 GetDrawableMultiplyColor(csmInt32 drawableIndex) const;

    /**
     * Returns the screen color of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Screen color of the drawable
     */
    Core::csmVector4 GetDrawableScreenColor(csmInt32 drawableIndex) const;

    /**
     * Returns the index of the parent part of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Index of the parent part of the drawable
     */
    csmInt32 GetDrawableParentPartIndex(csmUint32 drawableIndex) const;

    /**
     * Returns the blend mode of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Blend mode of the drawable
     */
    csmBlendMode GetDrawableBlendModeType(csmInt32 drawableIndex) const;

    /**
     * Returns the inverted mask setting for the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Inverted mask setting of the drawable. true if inverted.
     *
     * @note Ignored if the mask is not used.
     */
    csmBool GetDrawableInvertedMask(csmInt32 drawableIndex) const;

    /**
     * Returns the visibility information of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Visibility state of the drawable. true if visible.
     */
    csmBool GetDrawableDynamicFlagIsVisible(csmInt32 drawableIndex) const;

    /**
     * Returns whether the visibility state of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the visibility state of the drawable has changed.
     */
    csmBool GetDrawableDynamicFlagVisibilityDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns whether the opacity of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the opacity of the drawable has changed.
     */
    csmBool GetDrawableDynamicFlagOpacityDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns whether the draw order of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the draw order of the drawable has changed.
     *
     * @note Draw order is information specified from 0 to 1000 on the ArtMesh.
     */
    csmBool GetDrawableDynamicFlagDrawOrderDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns whether the render order of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the render order of the drawable has changed.
     */
    csmBool GetDrawableDynamicFlagRenderOrderDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns whether the vertex information of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the vertex information of the drawable has changed.
     */
    csmBool GetDrawableDynamicFlagVertexPositionsDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns whether the multiply or screen color of the drawable has changed from the dynamic flag.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the multiply or screen color of the drawable has changed.
     */
    csmBool GetDrawableDynamicFlagBlendColorDidChange(csmInt32 drawableIndex) const;

    /**
     * Returns the list of clipping masks of the drawables.
     *
     * @return List of clipping masks of the drawables
     */
    const csmInt32** GetDrawableMasks() const;

    /**
     * Returns the list of the number of clipping masks of the drawables.
     *
     * @return List of the number of clipping masks of the drawables
     */
    const csmInt32* GetDrawableMaskCounts() const;

    //========================================================
    //  Offscreen Functions.
    //========================================================

    /**
     * Returns the blend mode of the offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Blend mode of the offscreen
     */
    csmBlendMode GetOffscreenBlendModeType(csmInt32 offscreenIndex) const;

    /**
     * Returns the number of offscreens.
     *
     * @return Number of offscreens
     */
    csmInt32 GetOffscreenCount() const;

    /**
     * Returns the list of clipping masks of the offscreens.
     *
     * @return List of clipping masks of the offscreens
     */
    const csmInt32** GetOffscreenMasks() const;

    /**
     * Returns the list of the number of clipping masks of the offscreens.
     *
     * @return List of the number of clipping masks of the offscreens
     */
    const csmInt32* GetOffscreenMaskCounts() const;

    /**
     * Returns the list of owner indices for the offscreen.
     *
     * @return List of owner indices for the offscreen
     */
    const csmInt32* GetOffscreenOwnerIndices() const;

    /**
     * Returns the ID of the offscreen owner.
     *
     * @param offscreenIndex Index of the offscreen
     *
     * @return Owner ID
     */
    const CubismIdHandle GetOffscreenOwnerId(csmUint32 offscreenIndex) const;

    /**
     * Returns the multiply color of the offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Multiply color of the offscreen
     */
    Core::csmVector4 GetOffscreenMultiplyColor(csmInt32 offscreenIndex) const;

    /**
     * Returns the screen color of the offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Screen color of the offscreen
     */
    Core::csmVector4 GetOffscreenScreenColor(csmInt32 offscreenIndex) const;

    /**
     * Returns the inverted mask setting for the offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Inverted mask setting of the offscreen. true if inverted.
     *
     * @note Ignored if the mask is not used.
     */
    csmBool GetOffscreenInvertedMask(csmInt32 offscreenIndex) const;

    /**
     * Returns the opacity of the Offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Offscreen opacity
     */
    csmFloat32 GetOffscreenOpacity(csmInt32 offscreenIndex) const;

    //========================================================
    //  Other Functions.
    //========================================================

    /**
     * Checks whether the model uses clipping masks.
     *
     * @return true if the model uses clipping masks.
     */
    csmBool IsUsingMasking() const;

    /**
     * Checks whether the offscreen uses clipping masks.
     *
     * @return true if the offscreen uses clipping masks.
     */
    csmBool IsUsingMaskingForOffscreen() const;

    /**
     * Loads temporarily stored parameter values.
     */
    void LoadParameters();

    /**
     * Stores the value of the parameter temporarily.
     */
    void SaveParameters();

    /**
     * Checks whether parameter repetition is performed for the entire model.
     *
     * @return true if parameter repetition is performed for the entire model; otherwise returns false.
     */
    csmBool GetOverrideFlagForModelParameterRepeat() const;

    /**
     * Sets whether parameter repetition is performed for the entire model.
     * Use true to perform parameter repetition for the entire model, or false to not perform it.
     */
    void SetOverrideFlagForModelParameterRepeat(csmBool isRepeat);

    /**
     * Returns the flag indicating whether to override the parameter repeat.
     *
     * @param parameterIndex Parameter index
     *
     * @return true if the parameter repeat is overridden, false otherwise.
     */
    csmBool GetOverrideFlagForParameterRepeat(csmInt32 parameterIndex) const;

    /**
     * Sets the flag indicating whether to override the parameter repeat.
     *
     * @param parameterIndex Parameter index
     * @param value true if it is to be overridden; otherwise, false.
     */
    void SetOverrideFlagForParameterRepeat(csmInt32 parameterIndex, csmBool value);

    /**
     * Returns the repeat flag.
     *
     * @param parameterIndex Parameter index
     *
     * @return true if repeating, false otherwise.
     */
    csmBool GetRepeatFlagForParameterRepeat(csmInt32 parameterIndex) const;

    /**
     * Sets the repeat flag.
     *
     * @param parameterIndex Parameter index
     * @param value true to enable repeating, false otherwise.
     */
    void SetRepeatFlagForParameterRepeat(csmInt32 parameterIndex, csmBool value);

    /**
     * Returns the multiply and screen color settings.
     *
     * @return Multiply and screen color settings.
     */
    CubismModelMultiplyAndScreenColor& GetOverrideMultiplyAndScreenColor();

    /**
     * Returns the multiply and screen color settings.
     *
     * @return Multiply and screen color settings.
     */
    const CubismModelMultiplyAndScreenColor& GetOverrideMultiplyAndScreenColor() const;

    //========================================================
    //  Culling Functions.
    //========================================================

    /**
     * Returns the culling information of the drawable.
     *
     * @param drawableIndex Drawable index
     *
     * @return Culling information of the drawable
     */
    csmInt32 GetDrawableCulling(csmInt32 drawableIndex) const;

    /**
     * Sets the culling information of the drawable.
     *
     * @param drawableIndex Drawable index
     * @param isCulling True enable culling, false to disable
     */
    void SetDrawableCulling(csmInt32 drawableIndex, csmInt32 isCulling);

    /**
     * Returns the culling information of the offscreen.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return Culling information of the offscreen
     */
    csmInt32 GetOffscreenCulling(csmInt32 offscreenIndex) const;

    /**
     * Sets the culling information of the offscreen.
     *
     * @param offscreenIndex Offscreen index
     * @param isCulling True enable culling, false to disable
     */
    void SetOffscreenCulling(csmInt32 offscreenIndex, csmInt32 isCulling);

    /**
     * Checks whether the culling settings for the entire model are overridden by the SDK.
     *
     * @return true if the culling settings from the SDK are used; otherwise false.
     */
    csmBool GetOverrideFlagForModelCullings() const;

    /**
     * Sets whether the culling settings for the entire model are overridden by the SDK.
     * Use true to use the culling settings from the SDK, or false to use the culling settings from the model.
     *
     * @param value True to use the override, false to keep the model's own culling settings
     */
    void SetOverrideFlagForModelCullings(csmBool value);

    /**
     * Checks whether the culling settings for the drawable are overridden by the SDK.
     *
     * @param drawableIndex Drawable index
     *
     * @return true if the culling settings from the SDK are used; otherwise false.
     */
    csmBool GetOverrideFlagForDrawableCullings(csmInt32 drawableIndex) const;

    /**
     * Sets whether the culling settings for the drawable are overridden by the SDK.
     * Use true to use the culling settings from the SDK, or false to use the culling settings from the model.
     *
     * @param drawableIndex Drawable index
     * @param value True to use the override, false to keep the model's own culling settings
     */
    void SetOverrideFlagForDrawableCullings(csmUint32 drawableIndex, csmBool value);

    /**
     * Checks whether the culling settings for the offscreen are overridden by the SDK.
     *
     * @param offscreenIndex Offscreen index
     *
     * @return true if the culling settings from the SDK are used; otherwise false.
     */
    csmBool GetOverrideFlagForOffscreenCullings(csmInt32 offscreenIndex) const;

    /**
     * Sets whether the culling settings for the offscreen are overridden by the SDK.
     * Use true to use the culling settings from the SDK, or false to use the culling settings from the model.
     *
     * @param offscreenIndex Offscreen index
     * @param value True to use the override, false to keep the model's own culling settings
     */
    void SetOverrideFlagForOffscreenCullings(csmInt32 offscreenIndex, csmBool value);

    /**
     * Determines whether the drawable should be rendered with a blend mode.
     *
     * @return true if a blend mode is applied; otherwise, false.
     */
    csmBool IsBlendModeEnabled() const;

    /**
     * Returns the opacity of the model.
     *
     * @return Opacity value
     */
    csmFloat32 GetModelOpacity();

    /**
     * Sets the opacity of the model.
     *
     * @param value Opacity value
     */
    void SetModelOpacity(csmFloat32 value);

    Core::csmModel* GetModel() const;

    /**
     * Delete copy constructor.
     */
    CubismModel(const CubismModel&) = delete;

    /**
     * Delete copy assignment operator.
     */
    CubismModel& operator=(const CubismModel&) = delete;

protected:
    CubismModel(Core::csmModel* model);

    virtual ~CubismModel();

    void Initialize();

    void InitializeBlendMode();

    void SetupPartsHierarchy();

    csmMap<csmInt32, csmFloat32>        _notExistPartOpacities;
    csmMap<CubismIdHandle, csmInt32>   _notExistPartId;

    csmMap<csmInt32, csmFloat32>        _notExistParameterValues;
    csmMap<CubismIdHandle, csmInt32>   _notExistParameterId;

    csmVector<csmFloat32>   _savedParameters;

    Core::csmModel*     _model;

    csmFloat32*         _parameterValues;
    const csmFloat32*   _parameterMaximumValues;
    const csmFloat32*   _parameterMinimumValues;

    csmFloat32*         _partOpacities;

    csmFloat32 _modelOpacity;

    csmVector<CubismIdHandle> _parameterIds;
    csmVector<CubismIdHandle> _partIds;
    csmVector<CubismIdHandle> _drawableIds;
    csmVector<CubismModelPartInfo> _partsHierarchy;
    csmVector<ParameterRepeatData> _userParameterRepeatDataList;
    csmVector<CullingData> _userDrawableCullings;
    csmVector<CullingData> _userOffscreenCullings;
    CubismModelMultiplyAndScreenColor _overrideMultiplyAndScreenColors;
    csmBool _isOverriddenParameterRepeat;
    csmBool _isOverriddenCullings;
    csmBool _isBlendModeEnabled;
};

}}}
