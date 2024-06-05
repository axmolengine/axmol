#include "SDFGen.h"

#include <yasio/singleton.hpp>
#include <yasio/byte_buffer.hpp>
#include "base/format.h"

#include "base/ZipUtils.h"

#include "ImGuiPresenter.h"
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <zlib.h>
#include "base/JsonWriter.h"
#include "yasio/utils.hpp"

NS_AX_EXT_BEGIN

struct FontAtlasGenParams
{
    std::string sourceFont;  // font relative path? choose from developer machine?
    std::string fontAsset;   // fontAsset .xasset
    std::string glyphs;      // utf-8
    int faceSize    = 32;
    int atlasDim[2] = {512, 512};  // w,h
    bool useAscii   = true;

    bool saved = false;
    float cost = 0.0f;  // milliseconds
    std::string error;  // save error message if fail
};

/**
 * scan fonts in `Content/fonts`
 * kernings don't store
 * axmol .xasset format spec, sdf only
 * axmol .xasset binary format: \X\A\S
 * {
 *   "version": "2.1.0",
 *   "type": "fontatlas",
 *   "atlasName": "xxx",
 *   "sourceFont: "xxx",
 *   "spread": 6, // reserved
 *   "faceSize": 32,
 *   "atlasSize": [512, 512],
 *   "letters": [
 *     "30": {
 *         "page": 0,
 *         "xAdvance": 33,
 *         "width": 54,
 *         "height": 33,
 *         "offset": 5,
 *         "bearingY": 3,
 *     },
 *     "20": {
 *     }
 *   ]
 *   "pages: [ // zip + base64
 *     "",
 *     ""
 *   ]
 * }
 */

namespace xasset
{
class FontAtlas : public ax::FontAtlas
{
public:
    FontAtlas(Font* theFont, int atlasWidth, int atlasHeight) : ax::FontAtlas(theFont, atlasWidth, atlasHeight) {}
    static FontAtlas* newFontAtlas(FontAtlasGenParams* params)
    {
        auto font      = FontFreeType::create(params->sourceFont, params->faceSize,
                                         !params->useAscii ? ax::GlyphCollection::CUSTOM : ax::GlyphCollection::ASCII,
                                              params->glyphs, true);
        auto fontAtlas = new xasset::FontAtlas(font, params->atlasDim[0], params->atlasDim[1]);

        fontAtlas->generate(params);

        return fontAtlas;
    }

    bool save() { return this->saveAs(_params->fontAsset); }

    bool saveAs(std::string_view path)
    {
        std::string storePath;
        auto fu = FileUtils::getInstance();
        if (fu->isAbsolutePath(path))
        {
            storePath = path;
        }
        else
        {
            storePath = fu->getDefaultResourceRootPath();
            storePath += path;
        }

        auto start = yasio::highp_clock();

        JsonWriter<> xasset;

        xasset.writeStartObject();

        xasset.writeString("version"sv, AX_VERSION_STR_FULL);
        xasset.writeString("type"sv, "fontatlas"sv);
        xasset.writeString("sourceFont"sv, _params->sourceFont);
        xasset.writeString("atlasName"sv, _atlasName);
        xasset.writeNumber("spread"sv, 6);
        xasset.writeNumber("faceSize"sv, _params->faceSize);

        xasset.writeNumberArray("atlasDim"sv, _params->atlasDim);

        xasset.writeStartObject("letters"sv);
        std::string charCode;
        for (auto& letterInfo : getLetterDefinitions())
        {
            charCode.clear();
            fmt::format_to(charCode, "{}", (int32_t)letterInfo.first);

            xasset.writeStartObject(charCode);
            xasset.writeNumber("U", letterInfo.second.U);
            xasset.writeNumber("V", letterInfo.second.V);
            xasset.writeNumber("width", letterInfo.second.width);
            xasset.writeNumber("height", letterInfo.second.height);
            xasset.writeNumber("offsetX", letterInfo.second.offsetX);
            xasset.writeNumber("offsetY", letterInfo.second.offsetY);
            xasset.writeNumber("page", letterInfo.second.textureID);
            xasset.writeNumber("advance", letterInfo.second.xAdvance);
            xasset.writeEndObject();
        }
        xasset.writeEndObject();

        xasset.writeStartArray("pages");
        for (auto& data : _pageDatas)
        {
            auto compData = ZipUtils::compressGZ(std::span{data});
            auto pixels   = utils::base64Encode(std::span{compData});
            xasset.writeStringValue(pixels);
        }
        xasset.writeEndArray();

        xasset.writeNumber("pageX", _currentPageOrigX);
        xasset.writeNumber("pageY", _currentPageOrigY);

        xasset.writeEndObject();

        auto str = static_cast<std::string_view>(xasset);

        fu->writeStringToFile(str, storePath);

        _params->cost = (yasio::highp_clock() - start) / 1000.0;

        _params->error.clear();
        return true;
    }

    Texture2D* testTexture()
    {
        static Texture2D* texture = new Texture2D();
        static bool inited        = false;
        if (!inited && !_pageDatas.empty())
        {
            texture->initWithData(_pageDatas[0].data(), _pageDatas[0].size(), PixelFormat::R8, 512, 512, false);
            texture->retain();
            inited = true;
        }
        return inited ? texture : nullptr;
    }

protected:
    void generate(FontAtlasGenParams* params)
    {
        _params = params;

        // match with runtime
        _atlasName = fmt::format("df {} {}", params->faceSize, params->sourceFont);

        std::u32string utf32;
        if (StringUtils::UTF8ToUTF32(_fontFreeType->getGlyphCollection(), utf32))
            this->prepareLetterDefinitions(utf32);

        _pageDatas.emplace_back(_currentPageData, _currentPageData + _currentPageDataSize);
    }

    void addNewPage() override
    {
        if (_currentPage != -1)
            _pageDatas.emplace_back(_currentPageData, _currentPageData + _currentPageDataSize);
        ax::FontAtlas::addNewPage();
    }

    FontAtlasGenParams* _params{nullptr};  // weak ref
    std::string _atlasName;                // match with runtime
    std::vector<yasio::byte_buffer> _pageDatas;
};
};  // namespace xasset

SDFGen* SDFGen::getInstance()
{
    return yasio::singleton<SDFGen>::instance();
}
void SDFGen::destroyInstance()
{
    yasio::singleton<SDFGen>::destroy();
}

void SDFGen::open(ax::Scene* scene)
{
    refreshFontList();

    _atlasViewer = Sprite::create();
    _atlasViewer->setTexture(Director::getInstance()->getTextureCache()->getWhiteTexture("/black-texture", 0));
    _atlasViewer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    _atlasViewer->retain();

    auto defaultFontFile = FileUtils::getInstance()->fullPathForFilename(R"(fonts/arial.ttf)");

    _atlasParams             = new FontAtlasGenParams();
    _atlasParams->sourceFont = "fonts/arial.ttf";
    _atlasParams->fontAsset  = "fonts/arial-SDF.xasset";

    ImGuiPresenter::getInstance()->addFont(defaultFontFile);
    /* For Simplified Chinese support, please use:
    ImGuiPresenter::getInstance()->addFont(R"(C:\Windows\Fonts\msyh.ttc)", ImGuiPresenter::DEFAULT_FONT_SIZE,
                                       ImGuiPresenter::GLYPH_RANGES::CHINESE_GENERAL);
    */
    ImGuiPresenter::getInstance()->enableDPIScale();  // enable dpi scale for 4K display support, depends at least one
                                                      // valid ttf/ttc font was added.
    ImGuiPresenter::getInstance()->addRenderLoop("#sdfg", AX_CALLBACK_0(SDFGen::onImGuiDraw, this), scene);
}

void SDFGen::close()
{
    ImGuiPresenter::getInstance()->removeRenderLoop("#sdfg");

    delete _atlasParams;
    _atlasParams = nullptr;

    _atlasViewer->release();
}

void SDFGen::refreshFontList()
{
    _fontList.clear();

    auto fu = FileUtils::getInstance();

    std::vector<std::string> fileList;
    fu->listFilesRecursively("fonts", &fileList);

    auto& contentPath = fu->getDefaultResourceRootPath();
    for (auto& filePath : fileList)
    {
        if (!cxx20::ic::ends_with(filePath, ".ttf") && !cxx20::ic::ends_with(filePath, ".ttc"))
            continue;
        if (cxx20::starts_with(filePath, contentPath))
            _fontList.emplace_back(filePath.substr(contentPath.size()));
        else
            _fontList.emplace_back(std::move(filePath));
    }
}

void SDFGen::onImGuiDraw()
{
    ImGui::StyleColorsDark();

    auto& style                     = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.94f);

    ImGui::SetNextWindowSize(ImVec2{550, 660}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowContentSize(ImVec2{0, 0});

    static std::string title = fmt::format("Axmol SDF Font Creator {}", AX_VERSION_STR_FULL);
    if (ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginCombo("Source Font File", _atlasParams->sourceFont.c_str()))
        {
            for (int n = 0; n < _fontList.size(); n++)
            {
                bool is_selected = (_atlasParams->sourceFont == _fontList[n]);
                if (ImGui::Selectable(_fontList[n].c_str(), is_selected))
                    _atlasParams->sourceFont = _fontList[n];
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Refresh Font List"))
        {
            refreshFontList();
        }
        ImGui::DragInt("Sampling Point Size", &_atlasParams->faceSize, 1, 1, 144);
        ImGui::DragInt2("Atlas Resolution", _atlasParams->atlasDim, 32, 64, 4096);

        bool modified = ImGui::Checkbox("Use ASCII", &_atlasParams->useAscii);
        ImGui::SameLine();
        ImGui::Text("%s", "Character Set");
        if (!_atlasParams->useAscii)
        {
            ImGui::InputTextMultiline("glyphs", &_atlasParams->glyphs);
        }
        else
        {
            if (modified)
                _atlasParams->glyphs.clear();
        }

        if (ImGui::Button("Generate Font Atlas"))
        {
            AX_SAFE_RELEASE_NULL(_fontAtlas);
            _fontAtlas = xasset::FontAtlas::newFontAtlas(_atlasParams);

            // TODO: display multi-pages with listview?
            auto textureAtlasPage0 = _fontAtlas->getTexture(0);
            _atlasViewer->setTexture(textureAtlasPage0);

            Rect rect = Rect::ZERO;
            rect.size = textureAtlasPage0->getContentSize();
            _atlasViewer->setTextureRect(rect);
        }
        ImGui::InputText("Font Asset", &_atlasParams->fontAsset);
        if (ImGui::Button("Save"))
        {
            if (_fontAtlas)
                _fontAtlas->save();
            else
                _atlasParams->error = "Please generate first!";

            _atlasParams->saved = true;
        }

        if (_atlasParams->saved)
        {
            ImGui::SameLine();
            if (!_atlasParams->error.empty())
                ImGui::TextColored(ImVec4{1.0, 0.0, 0.0, 1.0}, "%s", _atlasParams->error.c_str());
            else
                ImGui::TextColored(ImVec4{0.0, 1.0, 0.0, 1.0}, "Save succeed, cost %.3f (ms)", _atlasParams->cost);
        }

        auto viewerSize = _atlasViewer->getContentSize();
        if (viewerSize.fuzzyEquals(Vec2::ZERO, 1e-3))
        {
            viewerSize.width  = 512;
            viewerSize.height = 512;
        }

        ImGui::Separator();
        ImGui::Text("Atals View:");
        ImGuiPresenter::getInstance()->image(_atlasViewer, ImVec2(viewerSize.width, viewerSize.height));
    }

    ImGui::End();
}

NS_AX_EXT_END
