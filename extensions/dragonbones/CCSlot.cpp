#include "CCSlot.h"
#include "CCTextureAtlasData.h"
#include "CCArmatureDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

void CCSlot::_onClear()
{
    Slot::_onClear();

    _textureScale = 1.0f;
    _renderDisplay = nullptr;
}

void CCSlot::_initDisplay(void* value, bool isRetain)
{
    const auto renderDisplay = static_cast<cocos2d::Node*>(value);
    renderDisplay->retain();
}

void CCSlot::_disposeDisplay(void* value, bool isRelease)
{
    const auto renderDisplay = static_cast<cocos2d::Node*>(value);
    renderDisplay->release();
}

void CCSlot::_onUpdateDisplay()
{
    _renderDisplay = static_cast<cocos2d::Node*>(_display != nullptr ? _display : _rawDisplay);
}

void CCSlot::_addDisplay()
{
    const auto container = static_cast<CCArmatureDisplay*>(_armature->getDisplay());
    container->addChild(_renderDisplay);
}

void CCSlot::_replaceDisplay(void* value, bool isArmatureDisplay)
{
    const auto container = static_cast<CCArmatureDisplay*>(_armature->getDisplay());
    const auto prevDisplay = isArmatureDisplay ? static_cast<cocos2d::Node*>(value) : static_cast<cocos2d::Node*>(value);
    container->addChild(_renderDisplay, prevDisplay->getLocalZOrder());
    container->removeChild(prevDisplay);
    _textureScale = 1.0f;
}

void CCSlot::_removeDisplay()
{
    _renderDisplay->removeFromParent();
}

void CCSlot::_updateZOrder()
{
    if (_renderDisplay->getLocalZOrder() == _zOrder) 
    {
        return;
    }

    _renderDisplay->setLocalZOrder(_zOrder);
}

void CCSlot::_updateFrame()
{
    const auto currentVerticesData = (_deformVertices != nullptr && _display == _meshDisplay) ? _deformVertices->verticesData : nullptr;
    const auto currentTextureData = static_cast<CCTextureData*>(_textureData);
    const auto frameDisplay = static_cast<DBCCSprite*>(_renderDisplay); // In cocos2dx render meshDisplay and frameDisplay are the same display

    if (_displayIndex >= 0 && _display != nullptr && currentTextureData != nullptr)
    {
        if (currentTextureData->spriteFrame != nullptr)
        {
            if (currentVerticesData != nullptr) // Mesh.
            {
                const auto data = currentVerticesData->data;
                const auto intArray = data->intArray;
                const auto floatArray = data->floatArray;
                const unsigned vertexCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
                const unsigned triangleCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshTriangleCount];
                int vertexOffset = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshFloatOffset];

                if (vertexOffset < 0)
                {
                    vertexOffset += 65536; // Fixed out of bouds bug. 
                }

                const unsigned uvOffset = vertexOffset + vertexCount * 2;

                const auto& region = currentTextureData->region;
                const auto& textureAtlasSize = currentTextureData->spriteFrame->getTexture()->getContentSizeInPixels();
                auto vertices = new cocos2d::V3F_C4B_T2F[vertexCount]; // does cocos2dx release it?
                auto vertexIndices = new unsigned short[triangleCount * 3]; // does cocos2dx release it?
                cocos2d::Rect boundsRect(999999.0f, 999999.0f, -999999.0f, -999999.0f);

                for (std::size_t i = 0, l = vertexCount * 2; i < l; i += 2)
                {
                    const auto iH = i / 2;
                    const auto x = floatArray[vertexOffset + i];
                    const auto y = floatArray[vertexOffset + i + 1];
                    auto u = floatArray[uvOffset + i];
                    auto v = floatArray[uvOffset + i + 1];
                    cocos2d::V3F_C4B_T2F vertexData;
                    vertexData.vertices.set(x, -y, 0.0f);

                    if (currentTextureData->rotated)
                    {
                        vertexData.texCoords.u = (region.x + (1.0f - v) * region.width) / textureAtlasSize.width;
                        vertexData.texCoords.v = (region.y + u * region.height) / textureAtlasSize.height;
                    }
                    else
                    {
                        vertexData.texCoords.u = (region.x + u * region.width) / textureAtlasSize.width;
                        vertexData.texCoords.v = (region.y + v * region.height) / textureAtlasSize.height;
                    }

                    vertexData.colors = cocos2d::Color4B::WHITE;
                    vertices[iH] = vertexData;

                    if (boundsRect.origin.x > x)
                    {
                        boundsRect.origin.x = x;
                    }

                    if (boundsRect.size.width < x)
                    {
                        boundsRect.size.width = x;
                    }

                    if (boundsRect.origin.y > -y)
                    {
                        boundsRect.origin.y = -y;
                    }

                    if (boundsRect.size.height < -y)
                    {
                        boundsRect.size.height = -y;
                    }
                }

                boundsRect.size.width -= boundsRect.origin.x;
                boundsRect.size.height -= boundsRect.origin.y;

                for (std::size_t i = 0; i < triangleCount * 3; ++i)
                {
                    vertexIndices[i] = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshVertexIndices + i];
                }

                _textureScale = 1.0f;
                frameDisplay->setSpriteFrame(currentTextureData->spriteFrame); // polygonInfo will be override

                cocos2d::PolygonInfo polygonInfo;
                auto& triangles = polygonInfo.triangles;
                triangles.verts = vertices;
                triangles.indices = vertexIndices;
                triangles.vertCount = vertexCount;
                triangles.indexCount = triangleCount * 3;
                const auto& transform = frameDisplay->getNodeToParentTransform(); // Backup transform. (Set rect and polygon will override transform).

#if COCOS2D_VERSION >= 0x00031400
                polygonInfo.setRect(boundsRect);
#else
                polygonInfo.rect = boundsRect; // Copy
                frameDisplay->setContentSize(boundsRect.size);
#endif
                frameDisplay->setPolygonInfo(polygonInfo);

                const auto isSkinned = currentVerticesData->weight != nullptr;
                if (isSkinned) 
                {
                    _identityTransform();
                }
                else 
                {
                    frameDisplay->setNodeToParentTransform(transform);
                }
            }
            else // Normal texture.
            {
                const auto scale = currentTextureData->parent->scale * _armature->_armatureData->scale;
                _textureScale = scale * cocos2d::Director::getInstance()->getContentScaleFactor();
                frameDisplay->setSpriteFrame(currentTextureData->spriteFrame); // polygonInfo will be override
            }

            _visibleDirty = true;
            _blendModeDirty = true; // Relpace texture will override blendMode and color.
            _colorDirty = true;

            return;
        }
    }

    frameDisplay->setTexture(nullptr);
    frameDisplay->setTextureRect(cocos2d::Rect::ZERO);
    frameDisplay->setPosition(0.0f, 0.0f);
    frameDisplay->setVisible(false);
}

void CCSlot::_updateMesh() 
{
    const auto scale = _armature->_armatureData->scale;
    const auto& deformVertices = _deformVertices->vertices;
    const auto& bones = _deformVertices->bones;
    const auto verticesData = _deformVertices->verticesData;
    const auto weightData = verticesData->weight;

    const auto hasFFD = !deformVertices.empty();
    const auto textureData = static_cast<CCTextureData*>(_textureData);
    const auto meshDisplay = static_cast<DBCCSprite*>(_renderDisplay);
    const auto vertices = meshDisplay->getPolygonInfoModify().triangles.verts;
    cocos2d::Rect boundsRect(999999.0f, 999999.0f, -999999.0f, -999999.0f);

    if (!textureData || meshDisplay->getSpriteFrame() != textureData->spriteFrame)
    {
        return;
    }

    if (weightData != nullptr)
    {
        const auto data = verticesData->data;
        const auto intArray = data->intArray;
        const auto floatArray = data->floatArray;
        const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
        int weightFloatOffset = intArray[weightData->offset + (unsigned)BinaryOffset::WeigthFloatOffset];

        if (weightFloatOffset < 0)
        {
            weightFloatOffset += 65536; // Fixed out of bouds bug. 
        }

        for (
            std::size_t i = 0, iB = weightData->offset + (unsigned)BinaryOffset::WeigthBoneIndices + bones.size(), iV = (std::size_t)weightFloatOffset, iF = 0;
            i < vertexCount;
            ++i
        )
        {
            const auto boneCount = (std::size_t)intArray[iB++];
            auto xG = 0.0f, yG = 0.0f;
            for (std::size_t j = 0; j < boneCount; ++j)
            {
                const auto boneIndex = (unsigned)intArray[iB++];
                const auto bone = bones[boneIndex];
                if (bone != nullptr) 
                {
                    const auto& matrix = bone->globalTransformMatrix;
                    const auto weight = floatArray[iV++];
                    auto xL = floatArray[iV++] * scale;
                    auto yL = floatArray[iV++] * scale;

                    if (hasFFD) 
                    {
                        xL += deformVertices[iF++];
                        yL += deformVertices[iF++];
                    }

                    xG += (matrix.a * xL + matrix.c * yL + matrix.tx) * weight;
                    yG += (matrix.b * xL + matrix.d * yL + matrix.ty) * weight;
                }
            }

            auto& vertex = vertices[i];
            auto& vertexPosition = vertex.vertices;

            vertexPosition.set(xG, -yG, 0.0f);

            if (boundsRect.origin.x > xG)
            {
                boundsRect.origin.x = xG;
            }

            if (boundsRect.size.width < xG)
            {
                boundsRect.size.width = xG;
            }

            if (boundsRect.origin.y > -yG)
            {
                boundsRect.origin.y = -yG;
            }

            if (boundsRect.size.height < -yG)
            {
                boundsRect.size.height = -yG;
            }
        }
    }
    else if (hasFFD)
    {
        const auto data = verticesData->data;
        const auto intArray = data->intArray;
        const auto floatArray = data->floatArray;
        const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
        int vertexOffset = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshFloatOffset];

        if (vertexOffset < 0)
        {
            vertexOffset += 65536; // Fixed out of bouds bug. 
        }

        for (std::size_t i = 0, l = vertexCount * 2; i < l; i += 2)
        {
            const auto iH = i / 2;
            const auto xG = floatArray[vertexOffset + i] * scale + deformVertices[i];
            const auto yG = floatArray[vertexOffset + i + 1] * scale + deformVertices[i + 1];

            auto& vertex = vertices[iH];
            auto& vertexPosition = vertex.vertices;

            vertexPosition.set(xG, -yG, 0.0f);

            if (boundsRect.origin.x > xG)
            {
                boundsRect.origin.x = xG;
            }

            if (boundsRect.size.width < xG)
            {
                boundsRect.size.width = xG;
            }

            if (boundsRect.origin.y > -yG)
            {
                boundsRect.origin.y = -yG;
            }

            if (boundsRect.size.height < -yG)
            {
                boundsRect.size.height = -yG;
            }
        }
    }

    boundsRect.size.width -= boundsRect.origin.x;
    boundsRect.size.height -= boundsRect.origin.y;

    const auto& transform = meshDisplay->getNodeToParentTransform(); // Backup transform. (Set rect and polygon will override transform).
    auto polygonInfo = meshDisplay->getPolygonInfo();

#if COCOS2D_VERSION >= 0x00031400
    polygonInfo.setRect(boundsRect);
#else
    polygonInfo.rect = boundsRect; // Copy
    meshDisplay->setContentSize(boundsRect.size);
#endif

    meshDisplay->setPolygonInfo(polygonInfo);

    if (weightData != nullptr) 
    {
        _identityTransform(); // ?!?!?!?!?!?!?!?!?!?!?!?!?!?! WTF
    }
    else 
    {
        meshDisplay->setNodeToParentTransform(transform);
    }
}

void CCSlot::_updateTransform()
{
    static cocos2d::Mat4 transform; 
    transform.m[0] = globalTransformMatrix.a;
    transform.m[1] = globalTransformMatrix.b;
    transform.m[4] = -globalTransformMatrix.c;
    transform.m[5] = -globalTransformMatrix.d;

    if (_renderDisplay == _rawDisplay || _renderDisplay == _meshDisplay)
    {
        if (_textureScale != 1.0f)
        {
            transform.m[0] *= _textureScale;
            transform.m[1] *= _textureScale;
            transform.m[4] *= _textureScale;
            transform.m[5] *= _textureScale;
        }

        transform.m[12] = globalTransformMatrix.tx - (globalTransformMatrix.a * _pivotX - globalTransformMatrix.c * _pivotY);
        transform.m[13] = globalTransformMatrix.ty - (globalTransformMatrix.b * _pivotX - globalTransformMatrix.d * _pivotY);
    }
    else if (_childArmature)
    {
        transform.m[12] = globalTransformMatrix.tx;
        transform.m[13] = globalTransformMatrix.ty;
    }
    else 
    {
        const auto& anchorPoint = _renderDisplay->getAnchorPoint();
        transform.m[12] = globalTransformMatrix.tx - (globalTransformMatrix.a * anchorPoint.x - globalTransformMatrix.c * anchorPoint.y);
        transform.m[13] = globalTransformMatrix.ty - (globalTransformMatrix.b * anchorPoint.x - globalTransformMatrix.d * anchorPoint.y);
    }

    _renderDisplay->setNodeToParentTransform(transform);
}

void CCSlot::_identityTransform()
{
    static cocos2d::Mat4 transform;
    transform.m[0] = 1.0f;
    transform.m[1] = 0.0f;
    transform.m[4] = -0.0f;
    transform.m[5] = -1.0f;
    transform.m[12] = 0.0f;
    transform.m[13] = 0.0f;

    _renderDisplay->setNodeToParentTransform(transform);
}

void CCSlot::_updateVisible()
{
    _renderDisplay->setVisible(_parent->getVisible());
}

void CCSlot::_updateBlendMode()
{
    cocos2d::Sprite* spriteDisplay = dynamic_cast<cocos2d::Sprite*>(_renderDisplay);
    if (spriteDisplay)
    {
        switch (_blendMode)
        {
        case BlendMode::Normal:
            // spriteDisplay->setBlendFunc(cocos2d::BlendFunc::DISABLE);
            break;

        case BlendMode::Add:
        {
            const auto texture = spriteDisplay->getTexture();
            if (texture && texture->hasPremultipliedAlpha())
            {
                cocos2d::BlendFunc blendFunc = { GL_ONE, GL_ONE };
                spriteDisplay->setBlendFunc(blendFunc);
            }
            else
            {
                spriteDisplay->setBlendFunc(cocos2d::BlendFunc::ADDITIVE);
            }
            break;
        }

        default:
            break;
        }
    }
    else if (_childArmature != nullptr)
    {
        for (const auto slot : _childArmature->getSlots())
        {
            slot->_blendMode = _blendMode;
            slot->_updateBlendMode();
        }
    }
}

void CCSlot::_updateColor()
{
    _renderDisplay->setOpacity(_colorTransform.alphaMultiplier * 255.0f);

    static cocos2d::Color3B helpColor;
    helpColor.r = _colorTransform.redMultiplier * 255.0f;
    helpColor.g = _colorTransform.greenMultiplier * 255.0f;
    helpColor.b = _colorTransform.blueMultiplier * 255.0f;

    _renderDisplay->setColor(helpColor);
}

DRAGONBONES_NAMESPACE_END
