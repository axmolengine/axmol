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

#include "ProjectNodeReader.h"

#include "cocostudio/CSParseBinary_generated.h"
#include "cocostudio/WidgetReader/NodeReader/NodeReader.h"

#include "flatbuffers/flatbuffers.h"

USING_NS_AX;
using namespace flatbuffers;

namespace cocostudio
{
ProjectNodeReader::ProjectNodeReader() {}

ProjectNodeReader::~ProjectNodeReader() {}

static ProjectNodeReader* _instanceProjectNodeReader = nullptr;

ProjectNodeReader* ProjectNodeReader::getInstance()
{
    if (!_instanceProjectNodeReader)
    {
        _instanceProjectNodeReader = new ProjectNodeReader();
    }

    return _instanceProjectNodeReader;
}

void ProjectNodeReader::purge()
{
    AX_SAFE_DELETE(_instanceProjectNodeReader);
}

void ProjectNodeReader::destroyInstance()
{
    AX_SAFE_DELETE(_instanceProjectNodeReader);
}

Offset<Table> ProjectNodeReader::createOptionsWithFlatBuffers(pugi::xml_node objectData,
                                                              flatbuffers::FlatBufferBuilder* builder)
{
    auto temp        = NodeReader::getInstance()->createOptionsWithFlatBuffers(objectData, builder);
    auto nodeOptions = *(Offset<WidgetOptions>*)(&temp);

    std::string filename;
    float innerspeed = 1.0f;

    pugi::xml_attribute objattri = objectData.first_attribute();
    // inneraction speed
    while (objattri)
    {
        std::string_view name  = objattri.name();
        std::string_view value = objattri.value();
        if (name == "InnerActionSpeed")
        {
            innerspeed = atof(value.data());
            break;
        }
        objattri = objattri.next_attribute();
    }

    // FileData
    auto child = objectData.first_child();
    while (child)
    {
        std::string_view name = child.name();

        if (name == "FileData")
        {
            auto attribute = child.first_attribute();

            while (attribute)
            {
                name              = attribute.name();
                std::string_view value = attribute.value();

                if (name == "Path")
                {
                    size_t pos          = value.find_last_of('.');
                    std::string convert = std::string{value.substr(0, pos)}.append(".csb");
                    filename            = convert;
                }

                attribute = attribute.next_attribute();
            }
        }

        child = child.next_sibling();
    }

    auto options = CreateProjectNodeOptions(*builder, nodeOptions, builder->CreateString(filename), innerspeed);

    return *(Offset<Table>*)(&options);
}

void ProjectNodeReader::setPropsWithFlatBuffers(ax::Node* node, const flatbuffers::Table* projectNodeOptions)
{
    auto options = (ProjectNodeOptions*)projectNodeOptions;

    auto nodeReader = NodeReader::getInstance();

    nodeReader->setPropsWithFlatBuffers(node, (Table*)options->nodeOptions());
}

Node* ProjectNodeReader::createNodeWithFlatBuffers(const flatbuffers::Table* /*nodeOptions*/)
{
    return nullptr;
}
}  // namespace cocostudio
