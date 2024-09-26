/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "Node3DReader.h"

#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/CSParse3DBinary_generated.h"

#include "cocostudio/FlatBuffersSerialize.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

using namespace ax;
using namespace flatbuffers;

namespace cocostudio
{
IMPLEMENT_CLASS_NODE_READER_INFO(Node3DReader)

Node3DReader::Node3DReader() {}

Node3DReader::~Node3DReader() {}

static Node3DReader* _instanceNode3DReader = nullptr;

Node3DReader* Node3DReader::getInstance()
{
    if (!_instanceNode3DReader)
    {
        _instanceNode3DReader = new Node3DReader();
    }

    return _instanceNode3DReader;
}

void Node3DReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceNode3DReader);
}

Vec3 Node3DReader::getVec3Attribute(pugi::xml_attribute attribute) const
{
    if (!attribute)
        return Vec3::ZERO;

    Vec3 ret;
    std::string attriname;

    while (attribute)
    {
        attriname         = attribute.name();
        std::string_view value = attribute.value();

        if (attriname == "X")
        {
            ret.x = atof(value.data());
        }
        else if (attriname == "Y")
        {
            ret.y = atof(value.data());
        }
        else if (attriname == "Z")
        {
            ret.z = atof(value.data());
        }

        attribute = attribute.next_attribute();
    }

    return ret;
}

Offset<Table> Node3DReader::createOptionsWithFlatBuffersForNode(pugi::xml_node objectData,
                                                                flatbuffers::FlatBufferBuilder* builder)
{
    std::string name;
    long actionTag    = 0;
    Vec2 rotationSkew = Vec2::ZERO;
    int zOrder        = 0;
    bool visible      = true;
    uint8_t alpha     = 255;
    int tag           = 0;
    Vec2 position     = Vec2::ZERO;
    Vec2 scale        = Vec2(1.0f, 1.0f);
    Vec2 anchorPoint  = Vec2::ZERO;
    Color4B color(255, 255, 255, 255);

    Vec2 size         = Vec2::ZERO;
    bool flipX        = false;
    bool flipY        = false;
    bool ignoreSize   = false;
    bool touchEnabled = false;
    std::string frameEvent;
    std::string customProperty;

    bool positionXPercentEnabled  = false;
    bool positionYPercentEnabled  = false;
    float positionXPercent        = 0;
    float positionYPercent        = 0;
    bool sizeXPercentEnable       = false;
    bool sizeYPercentEnable       = false;
    float sizeXPercent            = 0;
    float sizeYPercent            = 0;
    bool stretchHorizontalEnabled = false;
    bool stretchVerticalEnabled   = false;
    std::string horizontalEdge;
    std::string verticalEdge;
    float leftMargin   = 0;
    float rightMargin  = 0;
    float topMargin    = 0;
    float bottomMargin = 0;

    // attributes
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        std::string_view attriname = attribute.name();
        std::string_view value     = attribute.value();

        if (attriname == "Name")
        {
            name = value;
        }
        else if (attriname == "ActionTag")
        {
            actionTag = atol(value.data());
        }
        else if (attriname == "VisibleForFrame")
        {
            visible = (value == "True") ? true : false;
        }
        else if (attriname == "Alpha")
        {
            alpha = atoi(value.data());
        }
        else if (attriname == "Tag")
        {
            tag = atoi(value.data());
        }
        else if (attriname == "UserData")
        {
            customProperty = value;
        }
        else if (attriname == "FrameEvent")
        {
            frameEvent = value;
        }

        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();
    while (child)
    {
        std::string_view attriname = child.name();
        if (attriname == "Position3D")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname         = attribute.name();
                std::string_view value = attribute.value();

                if (attriname == "X")
                {
                    position.x = atof(value.data());
                }
                else if (attriname == "Y")
                {
                    position.y = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (attriname == "Scale3D")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname         = attribute.name();
                std::string_view value = attribute.value();

                if (attriname == "X")
                {
                    scale.x = atof(value.data());
                }
                else if (attriname == "Y")
                {
                    scale.y = atof(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        else if (attriname == "CColor")
        {
            attribute = child.first_attribute();

            while (attribute)
            {
                attriname         = attribute.name();
                std::string_view value = attribute.value();

                if (attriname == "A")
                {
                    color.a = atoi(value.data());
                }
                else if (attriname == "R")
                {
                    color.r = atoi(value.data());
                }
                else if (attriname == "G")
                {
                    color.g = atoi(value.data());
                }
                else if (attriname == "B")
                {
                    color.b = atoi(value.data());
                }

                attribute = attribute.next_attribute();
            }
        }
        child = child.next_sibling();
    }

    FVec2 f_rotationskew(rotationSkew.x, rotationSkew.y);
    FVec2 f_position(position.x, position.y);
    FVec2 f_scale(scale.x, scale.y);
    FVec2 f_anchortpoint(anchorPoint.x, anchorPoint.y);
    Color f_color(color.a, color.r, color.g, color.b);
    FlatSize f_size(size.x, size.y);
    auto f_layoutComponent = CreateLayoutComponentTable(
        *builder, positionXPercentEnabled, positionYPercentEnabled, positionXPercent, positionYPercent,
        sizeXPercentEnable, sizeYPercentEnable, sizeXPercent, sizeYPercent, stretchHorizontalEnabled,
        stretchVerticalEnabled, builder->CreateString(horizontalEdge), builder->CreateString(verticalEdge), leftMargin,
        rightMargin, topMargin, bottomMargin);

    auto options = CreateWidgetOptions(*builder, builder->CreateString(name), (int)actionTag, &f_rotationskew, zOrder,
                                       visible, alpha, tag, &f_position, &f_scale, &f_anchortpoint, &f_color, &f_size,
                                       flipX, flipY, ignoreSize, touchEnabled, builder->CreateString(frameEvent),
                                       builder->CreateString(customProperty), 0, 0, f_layoutComponent,
                                       false,  // 3D Node currently: fixed: cascadeColorEnabled = false
                                       false   // 3D Node currently: fixed: cascadeOpacityEnabled = false
    );

    return *(Offset<Table>*)(&options);
}

Offset<Table> Node3DReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                         flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = createOptionsWithFlatBuffersForNode(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
    int cameraMask = 0;

    std::string attriname;
    auto attribute = objectData.first_attribute();
    while (attribute)
    {
        attriname         = attribute.name();
        std::string_view value = attribute.value();

        if (attriname == "CameraFlagMode")
        {
            cameraMask = atoi(value.data());
        }

        attribute = attribute.next_attribute();
    }

    auto child = objectData.first_child();

    while (child)
    {
        std::string_view name = child.name();

        if (name == "Position3D")
        {
            position = getVec3Attribute(child.first_attribute());
        }
        else if (name == "Rotation3D")
        {
            rotation = getVec3Attribute(child.first_attribute());
        }
        else if (name == "Scale3D")
        {
            scale = getVec3Attribute(child.first_attribute());
        }

        child = child.next_sibling();
    }

    FVec3 position3D(position.x, position.y, position.z);
    FVec3 rotation3D(rotation.x, rotation.y, rotation.z);
    FVec3 scale3D(scale.x, scale.y, scale.z);

    auto options = CreateNode3DOption(*builder, nodeOptions, &position3D, &rotation3D, &scale3D, cameraMask);

    return *(Offset<Table>*)(&options);
}

void Node3DReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* node3DOptions)
{
    auto options = (Node3DOption*)node3DOptions;

    const FVec3* position = options->position3D();
    const FVec3* rotation = options->rotation3D();
    const FVec3* scale    = options->scale3D();
    int cameraMask        = options->cameramask();

    if (position)
    {
        node->setPosition3D(Vec3(position->x(), position->y(), position->z()));
    }

    if (rotation)
    {
        node->setRotation3D(Vec3(rotation->x(), rotation->y(), rotation->z()));
    }

    if (scale)
    {
        node->setScaleX(scale->x());
        node->setScaleY(scale->y());
        node->setScaleZ(scale->z());
    }

    node->setCameraMask(cameraMask, true);

    auto nodeReader = NodeReader::getInstance();
    nodeReader->setPropsWithFlatBuffers(node, (Table*)(options->nodeOptions()));
}

Node* Node3DReader::createNodeWithFlatBuffers(const flatbuffers::Table* node3DOptions)
{
    Node* node = Node::create();

    setPropsWithFlatBuffers(node, (Table*)node3DOptions);

    return node;
}
}  // namespace cocostudio
