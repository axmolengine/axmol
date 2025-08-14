#pragma once

#include "axmol/axmol.h"
#include "extensions/ExtensionMacros.h"

NS_AX_EXT_BEGIN

struct FontAtlasGenParams;

namespace xasset
{
class FontAtlas;
}
class SDFGen
{
public:
    static SDFGen* getInstance();
    static void destroyInstance();

    void open(ax::Scene* = nullptr);
    void close();

protected:
    void refreshFontList();

    void onImGuiDraw();

protected:
    FontAtlasGenParams* _atlasParams{nullptr};

    ax::Sprite* _atlasViewer{nullptr};
    xasset::FontAtlas* _fontAtlas{nullptr};

    std::vector<std::string> _fontList;
};

NS_AX_EXT_END
