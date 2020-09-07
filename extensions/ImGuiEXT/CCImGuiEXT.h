#pragma once

#include "cocos2d.h"
#include "ExtensionMacros.h"

#include "imgui/imgui.h"
#include <tuple>

// #define HAVE_IMGUI_MARKDOWN 1

NS_CC_EXT_BEGIN

class ImGuiEXTRenderer;
class ImGuiEXT
{
	friend class ImGuiEXTRenderer;
	void init();
public:

	enum class CHS_GLYPH_RANGE {
		NONE,
		GENERAL,
		FULL
	};

	enum {
		DEFAULT_FONT_SIZE = 13 // see imgui.cpp
	};

	static ImGuiEXT* getInstance();
	static void destroyInstance();
	static void setOnInit(const std::function<void(ImGuiEXT*)>& callBack);

	/// <summary>
	/// Scale ImGui with majorMoniter DPI scaling
	/// </summary>
	/// <param name="userScale">Usually is 1.0</param>
	/// <param name="fontFile">The full path of .ttc/.ttf file</param>
	/// <returns>finalScale = userScale * dpiScale</returns>
	float scaleAllByDPI(float userScale);

	/// <summary>
	/// Add ImGui font
	/// </summary>
	/// <param name="fontFile"></param>
	/// <param name="glyphRange"></param>
	void addFont(const std::string& fontFile, float fontSize = DEFAULT_FONT_SIZE, CHS_GLYPH_RANGE glyphRange = CHS_GLYPH_RANGE::NONE);
	void clearFonts();

	/// <summary>
	/// Add a ImGui render loop to specific scene
	/// </summary>
	/// <param name="id">FOURCC starts with '#', such as "#abcd"</id>
	/// <param name="scene">the scene to render ImGui</param>
	/// <param name="onFrame">the ImGui render loop</param>
	bool addRenderLoop(const std::string& id, Scene* scene, std::function<void()> onFrame);

	/// <summary>
	/// Remove ImGui render loop
	/// </summary>
	/// <param name="id">FOURCC starts with '#', such as "#abcd"</id>
	void removeRenderLoop(const std::string& id);

    // imgui helper
    void image(
		Texture2D* tex,
		const ImVec2& size,
		const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    void image(
		Sprite* sprite,
		const ImVec2& size,
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	bool imageButton(
		Texture2D* tex,
		const ImVec2& size,
		const ImVec2& uv0 = ImVec2(0, 0),
		const ImVec2& uv1 = ImVec2(1, 1),
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));
	bool imageButton(
		Sprite* sprite,
		const ImVec2& size,
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	void node(
		Node* node,
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1),
		const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	bool nodeButton(
		Node* node,
		int frame_padding = -1,
		const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
		const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	std::tuple<ImTextureID, int> useTexture(Texture2D* texture);
	std::tuple<ImTextureID, ImVec2, ImVec2, int> useSprite(Sprite* sprite);
	std::tuple<ImTextureID, ImVec2, ImVec2, int> useNode(Node* node, const ImVec2& pos);

	static void setNodeColor(Node* node, const ImVec4& col);
	static void setNodeColor(Node* node, ImGuiCol col);
	static void setLabelColor(Label* label, const ImVec4& col);
	static void setLabelColor(Label* label, bool disabled = false);
	static void setLabelColor(Label* label, ImGuiCol col);

	ImWchar* addGlyphRanges(const std::string& key, const std::vector<ImWchar>& ranges);
	static void mergeFontGlyphs(ImFont* dst, ImFont* src, ImWchar start, ImWchar end);
	int getCCRefId(Ref* p);

#if defined(HAVE_IMGUI_MARKDOWN)
	// markdown

	using MdLinkCallback = std::function<void(const std::string&, const std::string&, bool)>;
	using MdImageCallback = std::function<std::tuple<Sprite*, ImVec2, ImVec4, ImVec4>(const std::string&, const std::string&)>;

	void setMarkdownLinkCallback(const MdLinkCallback& f);
	void setMarkdownImageCallback(const MdImageCallback& f);
	void setMarkdownFont(int index, ImFont* font, bool seperator, float scale = 1.f);
	void setMarkdownLinkIcon(const std::string& icon);
	void markdown(const std::string& content);
#endif

private:
	// perform draw ImGui stubs
	void beginFrame();
	void update();
	void endFrame();

private:
	static std::function<void(ImGuiEXT*)> _onInit;
	
	struct RenderPipline {
		ImGuiEXTRenderer* renderer;
		std::function<void()> frame;
	};

    std::unordered_map<uint32_t, RenderPipline> _renderPiplines;

    std::unordered_map<Ref*, int> usedCCRefIdMap;
	// cocos objects should be retained until next frame
    Vector<Ref*> usedCCRef;
	std::unordered_map<std::string, std::vector<ImWchar>> glyphRanges;

	float _contentZoomFactor = 1.0f;
};

NS_CC_EXT_END
