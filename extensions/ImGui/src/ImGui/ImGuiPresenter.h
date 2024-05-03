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
    enum class CHS_GLYPH_RANGE
    {
        NONE,
        GENERAL,
        FULL
    };

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
    /// <param name="glyphRange"></param>
    void addFont(std::string_view fontFile,
                 float fontSize             = DEFAULT_FONT_SIZE,
                 CHS_GLYPH_RANGE glyphRange = CHS_GLYPH_RANGE::NONE);
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

    ImWchar* addGlyphRanges(std::string_view key, const std::vector<ImWchar>& ranges);
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
    hlookup::string_map<std::vector<ImWchar>> glyphRanges;

    float _contentZoomFactor = 1.0f;

    int64_t _beginFrames = 0;

    Texture2D* _fontsTexture = nullptr;

    struct FontInfo
    {
        float fontSize;
        CHS_GLYPH_RANGE glyphRange;
    };

    hlookup::string_map<FontInfo> _fontsInfoMap;

    bool _purgeNextLoop = false;
};

NS_AX_EXT_END
