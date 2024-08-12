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

#pragma once

#include <stdint.h>
#include <tuple>

#include "axmol.h"
#include "extensions/ExtensionMacros.h"

#include "imgui/imgui.h"

// #define HAVE_IMGUI_MARKDOWN 1

NS_AX_EXT_BEGIN

class ImGuiEventTracker;
class ImGuiPresenter
{
    friend class ImGuiRenderer;
    void init();
    void cleanup();

public:
    inline static const std::string_view GLYPH_RANGES_DEFAULT_ID         = "__DEFAULT_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_GREEK_ID           = "__GREEK_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_KOREAN_ID          = "__KOREAN_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_CHINESE_GENERAL_ID = "__CHINESE_SIMPLIFIED_COMMON_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_CHINESE_FULL_ID    = "__CHINESE_FULL_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_JAPANESE_ID        = "__JAPANESE_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_CYRILLIC_ID        = "__CYRILLIC_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_THAI_ID            = "__THAI_GLYPH__";
    inline static const std::string_view GLYPH_RANGES_VIETNAMESE_ID      = "__VIETNAMESE_GLYPH__";

    // predefined glyph ranges by imgui
    enum class GLYPH_RANGES
    {
        NONE,
        DEFAULT,
        GREEK,
        KOREAN,
        CHINESE_GENERAL,
        CHINESE_FULL,
        JAPANESE,
        CYRILLIC,
        THAI,
        VIETNAMESE
    };

    static std::string_view getGlyphRangesId(GLYPH_RANGES glyphRanges);

    enum
    {
        DEFAULT_FONT_SIZE = 13  // see imgui.cpp
    };

    static ImGuiPresenter* getInstance();
    static void destroyInstance();
    static void setOnInit(const std::function<void(ImGuiPresenter*)>& callBack);

    /// deprecated use enableDPIScale instead
    float scaleAllByDPI(float userScale = 1.0f) { return enableDPIScale(userScale); }

    /// <summary>
    /// Scale ImGui with majorMoniter DPI scaling
    /// </summary>
    /// <param name="userScale">Usually is 1.0</param>
    /// <param name="fontFile">The full path of .ttc/.ttf file</param>
    /// <returns>The final contentZoomFactor = userScale * dpiScale</returns>
    float enableDPIScale(float userScale = 1.0f);

    float getContentZoomFactor() const { return _contentZoomFactor; }

    void setViewResolution(float width, float height);

    /// <summary>
    /// Add ImGui font with contentZoomFactor
    /// </summary>
    /// <param name="fontFile"></param>
    /// <param name="fontSize"></param>
    /// <param name="glyphRange"></param>
    /// <param name="fontConfig"></param>
    void addFont(std::string_view fontFile,
                 float fontSize          = DEFAULT_FONT_SIZE,
                 GLYPH_RANGES glyphRange = GLYPH_RANGES::NONE,
                 const ImFontConfig& fontConfig = ImFontConfig());
    /// <summary>
    /// Add ImGui font with contentZoomFactor and use pre-existing glyph range for the specified font
    /// </summary>
    /// <param name="fontFile"></param>
    /// <param name="fontSize"></param>
    /// <param name="glyphRangesId"></param>
    /// <param name="fontConfig"></param>
    void addFont(std::string_view fontFile,
                 float fontSize,
                 std::string_view glyphRangesId,
                 const ImFontConfig& fontConfig = ImFontConfig());
    /// <summary>
    /// Add ImGui font with contentZoomFactor and use specified custom glyph range for the specified font
    /// </summary>
    /// <param name="fontFile"></param>
    /// <param name="fontSize"></param>
    /// <param name="glyphRanges">The glyph range vector must end with 0 and it should be included in the size</param>
    /// <param name="fontConfig"></param>
    void addFont(std::string_view fontFile,
                 float fontSize,
                 const std::vector<ImWchar>& glyphRanges,
                 const ImFontConfig& fontConfig = ImFontConfig());
    /// <summary>
    /// Add ImGui font with contentZoomFactor and use custom glyph range and specify a custom id
    /// </summary>
    /// <param name="fontFile"></param>
    /// <param name="fontSize"></param>
    /// <param name="glyphRangesId">Custom Lookup Id</param>
    /// <param name="glyphRanges">The glyph range vector must end with 0 and it should be included in the size</param>
    /// <param name="fontConfig"></param>
    void addFont(std::string_view fontFile,
                 float fontSize,
                 std::string_view glyphRangesId,
                 const std::vector<ImWchar>& glyphRanges,
                 const ImFontConfig& fontConfig = ImFontConfig());
    void removeFont(std::string_view fontFile);
    void clearFonts();

    /// <summary>
    /// Add a ImGui render loop to specific scene
    /// </summary>
    /// <param name="id">The FOURCC id of render loop, starts with '#', such as "#abcd"</id>
    /// <param name="func">the ImGui render loop</param>
    /// <param name="target">The target scene to track event, nullptr for global, useful for global GM tools</param>
    bool addRenderLoop(std::string_view id, std::function<void()> func, Scene* target);

    /// <summary>
    /// Remove ImGui render loop
    /// </summary>
    /// <param name="id">FOURCC starts with '#', such as "#abcd"</id>
    void removeRenderLoop(std::string_view id);

    void end();

    // imgui helper
    void image(Texture2D* tex,
               const ImVec2& size,
               const ImVec2& uv0        = ImVec2(0, 0),
               const ImVec2& uv1        = ImVec2(1, 1),
               const ImVec4& tint_col   = ImVec4(1, 1, 1, 1),
               const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    void image(Sprite* sprite,
               const ImVec2& size,
               bool keepAspectRatio = true,
               const ImVec4& tint_col   = ImVec4(1, 1, 1, 1),
               const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    void image(SpriteFrame* spriteFrame,
               const ImVec2& size,
               bool keepAspectRatio     = true,
               const ImVec4& tint_col   = ImVec4(1, 1, 1, 1),
               const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    bool imageButton(Texture2D* tex,
                     const ImVec2& size,
                     const ImVec2& uv0      = ImVec2(0, 0),
                     const ImVec2& uv1      = ImVec2(1, 1),
                     int frame_padding      = -1,
                     const ImVec4& bg_col   = ImVec4(0, 0, 0, 0),
                     const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
    bool imageButton(Sprite* sprite,
                     const ImVec2& size,
                     int frame_padding      = -1,
                     const ImVec4& bg_col   = ImVec4(0, 0, 0, 0),
                     const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

    void node(Node* node, const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    bool nodeButton(Node* node,
                    int frame_padding      = -1,
                    const ImVec4& bg_col   = ImVec4(0, 0, 0, 0),
                    const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

    std::tuple<ImTextureID, int> useTexture(Texture2D* texture);
    std::tuple<ImTextureID, ImVec2, ImVec2, int> useSprite(Sprite* sprite);
    std::tuple<ImTextureID, ImVec2, ImVec2, int> useNode(Node* node, const ImVec2& pos);

    static void setNodeColor(Node* node, const ImVec4& col);
    static void setNodeColor(Node* node, ImGuiCol col);
    static void setLabelColor(Label* label, const ImVec4& col);
    static void setLabelColor(Label* label, bool disabled = false);
    static void setLabelColor(Label* label, ImGuiCol col);

    ImWchar* addGlyphRanges(GLYPH_RANGES glyphRange);
    ImWchar* addGlyphRanges(std::string_view key, const std::vector<ImWchar>& ranges);
    void removeGlyphRanges(std::string_view key);
    void clearGlyphRanges();
    static void mergeFontGlyphs(ImFont* dst, ImFont* src, ImWchar start, ImWchar end);
    int getCCRefId(Object* p);

private:
    static void loadCustomFonts(void*);

    // perform draw ImGui stubs
    void beginFrame();
    void update();
    void endFrame();

private:
    static std::function<void(ImGuiPresenter*)> _onInit;

    struct ImGuiLoop
    {
        ImGuiEventTracker* tracker;
        std::function<void()> func;
        bool removing = false;
    };

    std::unordered_map<uint32_t, ImGuiLoop> _renderLoops;

    std::unordered_map<Object*, int> usedCCRefIdMap;
    // cocos objects should be retained until next frame
    Vector<Object*> usedCCRef;

    hlookup::string_map<std::vector<ImWchar>> _glyphRanges;
    std::unordered_set<uintptr_t> _usedGlyphRanges; // there should be one intance of "each glyph ranges"
    // temporarily stores the current erased/replaced ranges, gets cleared in the next `loadCustomFonts` interation
    std::vector<std::vector<ImWchar>> _eraseGlyphRanges;

    float _contentZoomFactor = 1.0f;

    int64_t _beginFrames = 0;

    Texture2D* _fontsTexture = nullptr;

    struct FontInfo
    {
        float fontSize;
        ImWchar* glyphRanges;
        std::string glyphRangesId;
        ImFontConfig fontConfig;
    };

    hlookup::string_map<FontInfo> _fontsInfoMap;

    bool _purgeNextLoop = false;
};

NS_AX_EXT_END
