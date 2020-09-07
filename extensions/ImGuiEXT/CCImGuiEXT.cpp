#include "CCImGuiEXT.h"
#include "imgui_impl_cocos2dx.h"

NS_CC_EXT_BEGIN

static uint32_t fourccValue(const std::string& str) {
	if (str.empty() || str[0] != '#') return (uint32_t)-1;
	uint32_t value = 0;
	memcpy(&value, str.c_str() + 1, std::min(sizeof(value), str.size() - 1));
	return value;
}

// TODO: Rename to ImGuiEXTLayerBlock
// Because it's only used for block event when ImGui::IsAnyWindowHovered() == 'true'
class ImGuiEXTRenderer : public Layer
{
CC_CONSTRUCTOR_ACCESS:
    bool initWithImGuiEXT()
	{
		if (!Layer::init())
			return false;

#ifdef CC_PLATFORM_PC
		// note: when at the first click to focus the window, this will not take effect
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);
		listener->onTouchBegan = [this](Touch* touch, Event*) -> bool {
			if (!_visible)
				return false;
			return ImGui::IsAnyWindowHovered();
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		// add by halx99
		auto stopAnyMouse = [=](EventMouse* event) {
			if (ImGui::IsAnyWindowHovered()) {
				event->stopPropagation();
			}
		};
		auto mouseListener = EventListenerMouse::create();
		mouseListener->onMouseDown = mouseListener->onMouseUp = stopAnyMouse;
		_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
#endif
		// add an empty sprite to avoid render problem
		// const auto sp = Sprite::create();
		// sp->setGlobalZOrder(1);
		// sp->setOpacity(0);
		// addChild(sp, 1);

		/*
		* There a 3 choice for schedule frame for ImGui render loop
		* a. at visit/draw to call beginFrame/endFrame, but at ImGui loop, we can't game object and add to Scene directly, will cause damage iterator
		* b. scheduleUpdate at onEnter to call beginFrame, at visit/draw to call endFrame, it's solve iterator damage problem, but when director is paused
		*    the director will stop call 'update' function of Scheduler
		*    And need modify engine code to call _scheduler->update(_deltaTime) even director is paused, pass 0 for update
		* c. Director::EVENT_BEFORE_DRAW call beginFrame, EVENT_AFTER_VISIT call endFrame
		*/
		return true;
	}

	~ImGuiEXTRenderer() 
	{
	}
};

static ImGuiEXT* _instance = nullptr;
std::function<void(ImGuiEXT*)> ImGuiEXT::_onInit;

void ImGuiEXT::init()
{
	ImGui_ImplCocos2dx_Init(true);

	auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addCustomEventListener(Director::EVENT_BEFORE_DRAW, [=](EventCustom*) { beginFrame(); });
	eventDispatcher->addCustomEventListener(Director::EVENT_AFTER_VISIT, [=](EventCustom*) { endFrame(); });
}

ImGuiEXT* ImGuiEXT::getInstance()
{
	if(_instance == nullptr)
	{
		_instance = new ImGuiEXT();
		_instance->init();
		if (_onInit)
			_onInit(_instance);
	}
	return _instance;
}

void ImGuiEXT::destroyInstance()
{
	if (_instance)
	{
		auto eventDispatcher = Director::getInstance()->getEventDispatcher();
		eventDispatcher->removeCustomEventListeners(Director::EVENT_AFTER_VISIT);
		eventDispatcher->removeCustomEventListeners(Director::EVENT_BEFORE_DRAW);

		ImGui_ImplCocos2dx_Shutdown();
		delete _instance;
		_instance = nullptr;
	}
}

void ImGuiEXT::setOnInit(const std::function<void(ImGuiEXT*)>& callBack)
{
	_onInit = callBack;
}

/*
	* begin ImGui frame and draw ImGui stubs
	*/
void ImGuiEXT::beginFrame()
{
	// create frame
	ImGui_ImplCocos2dx_NewFrame();

	// draw all gui
	this->update();

	// render
	ImGui::Render();
}

/*
* flush ImGui draw data to engine
*/
void ImGuiEXT::endFrame() {
	ImGui_ImplCocos2dx_RenderDrawData(ImGui::GetDrawData());
	ImGui_ImplCocos2dx_RenderPlatform();
}

void ImGuiEXT::update()
{ // drived by ImGuiEXTRenderer

	// clear things from last frame
	usedCCRefIdMap.clear();
	usedCCRef.clear();
	// drawing commands
	for (auto& pipline : _renderPiplines)
		pipline.second.frame();

	// commands will be processed after update
}

bool ImGuiEXT::addRenderLoop(const std::string& id, Scene* scene, std::function<void()> onFrame)
{
	// TODO: check whether exist
	auto fourccId = fourccValue(id);
	if (_renderPiplines.find(fourccId) != _renderPiplines.end())
	{
		return false;
	}

	auto renderer = utils::newInstance<ImGuiEXTRenderer>(&ImGuiEXTRenderer::initWithImGuiEXT);
	scene->addChild(renderer, INT_MAX, fourccId);
	_renderPiplines.emplace(fourccId, RenderPipline{ renderer, std::move(onFrame) });

    return true;
}

void ImGuiEXT::removeRenderLoop(const std::string& id)
{
	auto fourccId = fourccValue(id);
	 const auto iter = _renderPiplines.find(fourccId);
	 if (iter != _renderPiplines.end()) {
		 auto renderer = iter->second.renderer;
		 if (renderer->getParent())
			 renderer->removeFromParent();
		 renderer->release();
		 _renderPiplines.erase(iter);
	 }
}

static std::tuple<ImVec2, ImVec2> getTextureUV(Sprite* sp)
{
	ImVec2 uv0, uv1;
	if (!sp || !sp->getTexture())
		return { uv0,uv1 };
	const auto rect = sp->getTextureRect();
	const auto tex = sp->getTexture();
	const float atlasWidth = (float)tex->getPixelsWide();
	const float atlasHeight = (float)tex->getPixelsHigh();
	uv0.x = rect.origin.x / atlasWidth;
	uv0.y = rect.origin.y / atlasHeight;
	uv1.x = (rect.origin.x + rect.size.width) / atlasWidth;
	uv1.y = (rect.origin.y + rect.size.height) / atlasHeight;
	return { uv0,uv1 };
}

void ImGuiEXT::image(Texture2D* tex, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
	const ImVec4& tint_col, const ImVec4& border_col)
{
	if (!tex)
		return;
	auto size_ = size;
	if (size_.x <= 0.f) size_.x = tex->getPixelsWide();
	if (size_.y <= 0.f) size_.y = tex->getPixelsHigh();
	ImGui::PushID(getCCRefId(tex));
	ImGui::Image((ImTextureID)tex, size_, uv0, uv1, tint_col, border_col);
	ImGui::PopID();
}

void ImGuiEXT::image(Sprite* sprite, const ImVec2& size, const ImVec4& tint_col, const ImVec4& border_col)
{
	if (!sprite || !sprite->getTexture())
		return;
	auto size_ = size;
	const auto rect = sprite->getTextureRect();
	if (size_.x <= 0.f) size_.x = rect.size.width;
	if (size_.y <= 0.f) size_.y = rect.size.height;
	ImVec2 uv0, uv1;
	std::tie(uv0, uv1) = getTextureUV(sprite);
	ImGui::PushID(getCCRefId(sprite));
	ImGui::Image((ImTextureID)sprite->getTexture(), size_, uv0, uv1, tint_col, border_col);
	ImGui::PopID();
}

bool ImGuiEXT::imageButton(Texture2D* tex, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1,
	int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	if (!tex)
		return false;
	auto size_ = size;
	if (size_.x <= 0.f) size_.x = tex->getPixelsWide();
	if (size_.y <= 0.f) size_.y = tex->getPixelsHigh();
	ImGui::PushID(getCCRefId(tex));
	const auto ret = ImGui::ImageButton((ImTextureID)tex,
		size_, uv0, uv1, frame_padding, bg_col, tint_col);
	ImGui::PopID();
	return ret;
}

bool ImGuiEXT::imageButton(Sprite* sprite, const ImVec2& size, int frame_padding, const ImVec4& bg_col,
	const ImVec4& tint_col)
{
	if (!sprite || !sprite->getTexture())
		return false;
	auto size_ = size;
	const auto rect = sprite->getTextureRect();
	if (size_.x <= 0.f) size_.x = rect.size.width;
	if (size_.y <= 0.f) size_.y = rect.size.height;
	ImVec2 uv0, uv1;
	std::tie(uv0, uv1) = getTextureUV(sprite);
	ImGui::PushID(getCCRefId(sprite));
	const auto ret = ImGui::ImageButton((ImTextureID)sprite->getTexture(),
		size_, uv0, uv1, frame_padding, bg_col, tint_col);
	ImGui::PopID();
	return ret;
}

void ImGuiEXT::node(Node* node, const ImVec4& tint_col, const ImVec4& border_col)
{
	if (!node)
		return;
	const auto size = node->getContentSize();
	const auto pos = ImGui::GetCursorScreenPos();
	Mat4 tr;
	tr.m[5] = -1;
	tr.m[12] = pos.x;
	tr.m[13] = pos.y + size.height;
	if (border_col.w > 0.f)
	{
		tr.m[12] += 1;
		tr.m[13] += 1;
	}
	node->setNodeToParentTransform(tr);
	ImGui::PushID(getCCRefId(node));
	ImGui::Image((ImTextureID)node,
		ImVec2(size.width, size.height), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
	ImGui::PopID();
}

bool ImGuiEXT::nodeButton(Node* node, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	if (!node)
		return false;
	const auto size = node->getContentSize();
	const auto pos = ImGui::GetCursorScreenPos();
	Mat4 tr;
	tr.m[5] = -1;
	tr.m[12] = pos.x;
	tr.m[13] = pos.y + size.height;
	if (frame_padding >= 0)
	{
		tr.m[12] += (float)frame_padding;
		tr.m[13] += (float)frame_padding;
	}
	else
	{
		tr.m[12] += ImGui::GetStyle().FramePadding.x;
		tr.m[13] += ImGui::GetStyle().FramePadding.y;
	}
	node->setNodeToParentTransform(tr);
	ImGui::PushID(getCCRefId(node));
	const auto ret = ImGui::ImageButton((ImTextureID)node,
		ImVec2(size.width, size.height), ImVec2(0, 0), ImVec2(1, 1), frame_padding, bg_col, tint_col);
	ImGui::PopID();
	return ret;
}

std::tuple<ImTextureID, int> ImGuiEXT::useTexture(Texture2D* texture)
{
	if (!texture)
		return { nullptr,0 };
	return { (ImTextureID)texture,getCCRefId(texture) };
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiEXT::useSprite(Sprite* sprite)
{
	if (!sprite || !sprite->getTexture())
		return { nullptr,{},{},0 };
	ImVec2 uv0, uv1;
	std::tie(uv0, uv1) = getTextureUV(sprite);
	return { (ImTextureID)sprite->getTexture(),uv0,uv1,getCCRefId(sprite) };
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiEXT::useNode(Node* node, const ImVec2& pos)
{
	if (!node)
		return { nullptr,{},{},0 };
	const auto size = node->getContentSize();
	Mat4 tr;
	tr.m[5] = -1;
	tr.m[12] = pos.x;
	tr.m[13] = pos.y + size.height;
	node->setNodeToParentTransform(tr);
	return { (ImTextureID)node,pos,ImVec2(pos.x + size.width,pos.y + size.height),getCCRefId(node) };
}

void ImGuiEXT::setNodeColor(Node* node, const ImVec4& col)
{
	if (node)
	{
		node->setColor({ uint8_t(col.x * 255),uint8_t(col.y * 255),uint8_t(col.z * 255) });
		node->setOpacity(uint8_t(col.w * 255));
	}
}

void ImGuiEXT::setNodeColor(Node* node, ImGuiCol col)
{
	if (node && 0 <= col && col < ImGuiCol_COUNT)
		setNodeColor(node, ImGui::GetStyleColorVec4(col));
}

void ImGuiEXT::setLabelColor(Label* label, const ImVec4& col)
{
	if (label)
	{
		label->setTextColor(
			{ uint8_t(col.x * 255),uint8_t(col.y * 255),uint8_t(col.z * 255),uint8_t(col.w * 255) });
	}
}

void ImGuiEXT::setLabelColor(Label* label, bool disabled)
{
	if (label)
		setLabelColor(label, ImGui::GetStyleColorVec4(disabled ? ImGuiCol_TextDisabled : ImGuiCol_Text));
}

void ImGuiEXT::setLabelColor(Label* label, ImGuiCol col)
{
	if (label && 0 <= col && col < ImGuiCol_COUNT)
		setLabelColor(label, ImGui::GetStyleColorVec4(col));
}

ImWchar* ImGuiEXT::addGlyphRanges(const std::string& key, const std::vector<ImWchar>& ranges)
{
	auto it = glyphRanges.find(key);
	// the pointer must be persistant, do not replace
	if (it != glyphRanges.end())
		return it->second.data();
	glyphRanges[key] = ranges;
	if (ranges.empty())
		glyphRanges[key].push_back(0);
	return glyphRanges[key].data();
}

void ImGuiEXT::mergeFontGlyphs(ImFont* dst, ImFont* src, ImWchar start, ImWchar end)
{
	if (!dst || !src || start > end)
		return;
	for (auto i = start; i <= end; ++i)
	{
		const auto g = src->FindGlyphNoFallback(i);
		if (g)
		{
			// TODO
			// dst->AddGlyph(g->Codepoint, g->X0, g->Y0, g->X1, g->Y1, g->U0, g->V0, g->U1, g->V1, g->AdvanceX);
		}
	}
	dst->BuildLookupTable();
}

int ImGuiEXT::getCCRefId(Ref* p)
{
	int id = 0;
	const auto it = usedCCRefIdMap.find(p);
	if (it == usedCCRefIdMap.end())
	{
		usedCCRefIdMap[p] = 0;
		usedCCRef.pushBack(p);
	}
	else
		id = ++it->second;
	// BKDR hash
	constexpr unsigned int seed = 131;
	unsigned int hash = 0;
	for (auto i = 0u; i < sizeof(void*); ++i)
		hash = hash * seed + ((const char*)&p)[i];
	for (auto i = 0u; i < sizeof(int); ++i)
		hash = hash * seed + ((const char*)&id)[i];
	return (int)hash;
}

#if defined(HAVE_IMGUI_MARKDOWN)
#include "imgui_markdown/imgui_markdown.h"

static ImGuiEXT::MdLinkCallback ImGuiMarkdownLinkCallback = nullptr;
static ImGuiEXT::MdImageCallback ImGuiMarkdownImageCallback = nullptr;
static ImGui::MarkdownImageData ImGuiMarkdownInvalidImageData = { false, false, nullptr, {0.f, 0.f} };

void MarkdownLinkCallback(ImGui::MarkdownLinkCallbackData data)
{
	if (ImGuiMarkdownLinkCallback)
	{
		ImGuiMarkdownLinkCallback(
			{ data.text, (size_t)data.textLength }, { data.link, (size_t)data.linkLength }, data.isImage);
	}
}

ImGui::MarkdownImageData MarkdownImageCallback(ImGui::MarkdownLinkCallbackData data)
{
	if (!data.isImage || !ImGuiMarkdownImageCallback)
		return ImGuiMarkdownInvalidImageData;
	Sprite* sp; ImVec2 size; ImVec4 tint_col; ImVec4 border_col;
	std::tie(sp, size, tint_col, border_col) = ImGuiMarkdownImageCallback(
		{ data.text, (size_t)data.textLength },
		{ data.link, (size_t)data.linkLength });
	if(!sp || !sp->getTexture())
		return ImGuiMarkdownInvalidImageData;
	auto size_ = size;
	const auto rect = sp->getTextureRect();
	if (size_.x <= 0.f) size_.x = rect.size.width;
	if (size_.y <= 0.f) size_.y = rect.size.height;
	ImVec2 uv0, uv1;
	std::tie(uv0, uv1) = getTextureUV(sp);
	ImGuiEXT::getInstance()->getCCRefId(sp);
	return { true, true, (ImTextureID)sp->getTexture(), size_,uv0, uv1, tint_col, border_col };
}

static std::string ImGuiMarkdownLinkIcon;
static ImGui::MarkdownConfig ImGuiMarkdownConfig = {
	MarkdownLinkCallback, MarkdownImageCallback, "" };

void ImGuiEXT::setMarkdownLinkCallback(const MdLinkCallback& f)
{
	ImGuiMarkdownLinkCallback = f;
}

void ImGuiEXT::setMarkdownImageCallback(const MdImageCallback& f)
{
	ImGuiMarkdownImageCallback = f;
}

void ImGuiEXT::setMarkdownFont(int index, ImFont* font, bool seperator, float scale)
{
	if (index < 0 || index >= ImGui::MarkdownConfig::NUMHEADINGS)
		return;
	ImGuiMarkdownConfig.headingFormats[index] = { font,seperator };
	ImGuiMarkdownConfig.headingScales[index] = scale;
}

void ImGuiEXT::setMarkdownLinkIcon(const std::string& icon)
{
	ImGuiMarkdownLinkIcon = icon;
	ImGuiMarkdownConfig.linkIcon = ImGuiMarkdownLinkIcon.c_str();
}

void ImGuiEXT::markdown(const std::string& content)
{
	ImGui::Markdown(content.c_str(), content.size(), ImGuiMarkdownConfig);
}

#endif

NS_CC_EXT_END
