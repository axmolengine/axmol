#include "UIConfig.h"

NS_FGUI_BEGIN
using namespace ax;

std::string UIConfig::defaultFont = "";
std::string UIConfig::buttonSound = "";
float UIConfig::buttonSoundVolumeScale = 1;
int UIConfig::defaultScrollStep = 25;
float UIConfig::defaultScrollDecelerationRate = 0.967f;
bool UIConfig::defaultScrollTouchEffect = true;
bool UIConfig::defaultScrollBounceEffect = true;
ScrollBarDisplayType UIConfig::defaultScrollBarDisplay = ScrollBarDisplayType::DEFAULT;
std::string UIConfig::verticalScrollBar = "";
std::string UIConfig::horizontalScrollBar = "";
int UIConfig::touchDragSensitivity = 10;
int UIConfig::clickDragSensitivity = 2;
int UIConfig::touchScrollSensitivity = 20;
int UIConfig::defaultComboBoxVisibleItemCount = 10;
std::string UIConfig::globalModalWaiting = "";
std::string UIConfig::tooltipsWin = "";
Color4F UIConfig::modalLayerColor = Color4F(0, 0, 0, 0.4f);
bool UIConfig::bringWindowToFrontOnClick = true;
std::string UIConfig::windowModalWaiting = "";
std::string UIConfig::popupMenu = "";
std::string UIConfig::popupMenu_seperator = "";

std::unordered_map<std::string, UIConfig::FontNameItem> UIConfig::_fontNames;

void UIConfig::registerFont(const std::string& aliasName, const std::string& realName)
{
    FontNameItem fi;
    fi.name = realName;
    bool tmp = FileUtils::getInstance()->isPopupNotify();
    FileUtils::getInstance()->setPopupNotify(false);
    fi.ttf = FileUtils::getInstance()->isFileExist(realName);
    FileUtils::getInstance()->setPopupNotify(tmp);
    _fontNames[aliasName] = fi;
}

const std::string& UIConfig::getRealFontName(const std::string& aliasName, bool* isTTF)
{
    std::unordered_map<std::string, UIConfig::FontNameItem>::const_iterator it;
    if (aliasName.empty())
        it = _fontNames.find(UIConfig::defaultFont);
    else
        it = _fontNames.find(aliasName);
    if (it != _fontNames.end())
    {
        if(isTTF)
            *isTTF = it->second.ttf;
        return it->second.name;
    }
    else
    {
        if (isTTF)
            *isTTF = false;
        return aliasName;
    }
}

NS_FGUI_END

