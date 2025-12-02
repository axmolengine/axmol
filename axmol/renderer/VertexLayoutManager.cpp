/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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
#include "axmol/renderer/VertexLayoutManager.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/tlx/singleton.hpp"

namespace ax
{

/*
 * shader vertex layout define functions
 */
struct BuiltinVertexDefine
{
    static void defineTexture(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(2);

        /// a_position
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, 3 * sizeof(float), false);

        desc.endLayout();
    }

    static void definePosUVColor(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(3);

        /// a_position
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4F, texCoord), false);

        /// a_color
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::FLOAT4, offsetof(V3F_T2F_C4F, color), false);

        desc.endLayout();
    }

    // posUVColor32
    static void defineSprite(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(3);

        /// a_position
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        /// a_texCoord
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4B, texCoord), false);

        /// a_color
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::UBYTE4, offsetof(V3F_T2F_C4B, color), true);

        desc.endLayout();
    }

    static void defineSprite2D(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(3);

        /// a_position
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT2, 0, false);
        /// a_texCoord
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, offsetof(V2F_T2F_C4B, texCoord), false);

        /// a_color
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::UBYTE4, offsetof(V2F_T2F_C4B, color), true);

        desc.endLayout();
    }

    static void defineDrawNode(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(3);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT2, 0, false);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, offsetof(V2F_T2F_C4F, texCoord), false);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::FLOAT4, offsetof(V2F_T2F_C4F, color), true);

        desc.endLayout();
    }

    static void defineDrawNode3D(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(3);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::FLOAT4, offsetof(V3F_C4F, color), true);

        desc.endLayout();
    }

    static void defineSkyBox(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(1);

        auto attrNameLoc = program->getVertexInputDesc(rhi::VERTEX_INPUT_NAME_POSITION);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, attrNameLoc, rhi::VertexFormat::FLOAT3, 0, false);

        desc.endLayout();
    }

    static void definePos(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(1);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        desc.endLayout();
    }

    static void definePosColor(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(1);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_COLOR, program->getVertexInputDesc(rhi::VertexInputKind::COLOR),
                       rhi::VertexFormat::FLOAT4, offsetof(V3F_C4F, color), false);
        desc.endLayout();
    }

    static void defineTerrain3D(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(1);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, offsetof(V3F_T2F_N3F, texcoord), false);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_NORMAL, program->getVertexInputDesc(rhi::VertexInputKind::NORMAL),
                       rhi::VertexFormat::FLOAT3, offsetof(V3F_T2F_N3F, normal), false);
        desc.endLayout();
    }

    static void defineInstanced(Program* program, VertexLayoutDesc& desc)
    {
        desc.startLayout(1);

        desc.addAttrib(rhi::VERTEX_INPUT_NAME_POSITION, program->getVertexInputDesc(rhi::VertexInputKind::POSITION),
                       rhi::VertexFormat::FLOAT3, 0, false);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_TEXCOORD, program->getVertexInputDesc(rhi::VertexInputKind::TEXCOORD),
                       rhi::VertexFormat::FLOAT2, 3 * sizeof(float), false);
        desc.addAttrib(rhi::VERTEX_INPUT_NAME_INSTANCE, program->getVertexInputDesc(rhi::VertexInputKind::INSTANCE),
                       rhi::VertexFormat::MAT4, 0, false, 1);

        desc.endLayout();
    }
};
std::function<void(Program*, VertexLayoutDesc& desc)> s_vertexLayoutDefineList[static_cast<int>(
    VertexLayoutKind::Count)] = {
    BuiltinVertexDefine::definePos,        BuiltinVertexDefine::defineTexture,  BuiltinVertexDefine::definePosUVColor,
    BuiltinVertexDefine::defineSprite,     BuiltinVertexDefine::defineSprite2D, BuiltinVertexDefine::defineDrawNode,
    BuiltinVertexDefine::defineDrawNode3D, BuiltinVertexDefine::defineSkyBox,   BuiltinVertexDefine::definePosColor,
    BuiltinVertexDefine::defineTerrain3D,  BuiltinVertexDefine::defineInstanced};

static constexpr size_t MAX_DESC_POOL_SIZE = 64;

VertexLayoutManager* VertexLayoutManager::getInstance()
{
    return tlx::singleton<VertexLayoutManager>::instance();
}

void VertexLayoutManager::destroyInstance()
{
    tlx::singleton<VertexLayoutManager>::destroy();
}

VertexLayoutManager::VertexLayoutManager()
{
    _builtinVertexLayouts.resize(static_cast<size_t>(VertexLayoutKind::Count), nullptr);
}

VertexLayoutManager::~VertexLayoutManager()
{
    _vertexLayoutDescPool.clear();
    _vertexLayoutDescPool.shrink_to_fit();

    for (auto layout : _builtinVertexLayouts)
        AX_SAFE_RELEASE(layout);
    _builtinVertexLayouts.clear();

    for (auto& [_, layout] : _customVertexLayouts)
        AX_SAFE_RELEASE(layout);

    _customVertexLayouts.clear();
}

VertexLayout* VertexLayoutManager::getBuiltinVertexLayout(VertexLayoutKind kind, Program* prog)
{
    if (kind < VertexLayoutKind::Count)
    {
        auto layout = _builtinVertexLayouts[static_cast<int>(kind)];
        if (!layout)
        {
            VertexLayoutDesc desc;
            s_vertexLayoutDefineList[static_cast<int>(kind)](prog, desc);
            layout = axdrv->createVertexLayout(std::forward<VertexLayoutDesc>(desc));
            _builtinVertexLayouts[static_cast<int>(kind)] = layout;

            layout->setBuiltinId((int)kind);
        }
        return layout;
    }

    return nullptr;
}

VertexLayout* VertexLayoutManager::getBuiltinVertexLayout(VertexLayoutKind kind)
{
    if (kind < VertexLayoutKind::Count)
    {
        auto layout = _builtinVertexLayouts[static_cast<int>(kind)];
        return layout;
    }
    return nullptr;
}

/// @brief acquire vertex layout by description
/// @return
VertexLayout* VertexLayoutManager::getVertexLayout(VertexLayoutDesc&& desc)
{

    auto key = desc.getHash();
    assert(key && "VertexLayoutDesc incomplete!");
    auto it = _customVertexLayouts.find(key);

    VertexLayout* layout{nullptr};
    if (it != _customVertexLayouts.end())
    {
        if (_vertexLayoutDescPool.size() < MAX_DESC_POOL_SIZE)
            _vertexLayoutDescPool.emplace_back(std::forward<VertexLayoutDesc>(desc));
        else
            desc.clear();
        layout = it->second;
    }
    else
    {
        layout = axdrv->createVertexLayout(std::forward<VertexLayoutDesc>(desc));
        _customVertexLayouts.emplace(key, layout);
    }
    return layout;
}

void VertexLayoutManager::removeUnusedVertexLayouts()
{
    for (auto it = _customVertexLayouts.cbegin(); it != _customVertexLayouts.cend(); /* nothing */)
    {
        auto vl = it->second;
        if (vl->getReferenceCount() == 1)
        {
            AXLOGD("VertexLayoutManager: removing unused vertex layout: {}", it->first);

            vl->release();
            it = _customVertexLayouts.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

VertexLayoutDesc VertexLayoutManager::allocateVertexLayoutDesc()
{
    if (!_vertexLayoutDescPool.empty())
    {
        VertexLayoutDesc desc = std::move(_vertexLayoutDescPool[_vertexLayoutDescPool.size() - 1]);
        _vertexLayoutDescPool.pop_back();
        return desc;
    }

    return VertexLayoutDesc{};
}

}  // namespace ax
