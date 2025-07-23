local function CurrentLanguageTest()
    local ret = ax.Layer:create()
    local  label = ax.Label:createWithTTF("Current language Test", s_arialPath, 28)
    ret:addChild(label, 0)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition( ax.p(VisibleRect:center().x, VisibleRect:top().y-50) )

    local labelLanguage = ax.Label:createWithTTF("", s_arialPath, 20)
    labelLanguage:setAnchorPoint(ax.p(0.5, 0.5))
    labelLanguage:setPosition(VisibleRect:center())

    local currentLanguageType = ax.Application:getInstance():getCurrentLanguage()

    if currentLanguageType == ax.LANGUAGE_ENGLISH then
        labelLanguage:setString("current language is English")
    elseif currentLanguageType == ax.LANGUAGE_CHINESE then
        labelLanguage:setString("current language is Chinese")
    elseif currentLanguageType == ax.LANGUAGE_FRENCH then
        labelLanguage:setString("current language is French")
    elseif currentLanguageType == ax.LANGUAGE_GERMAN then
        labelLanguage:setString("current language is German")
    elseif currentLanguageType == ax.LANGUAGE_ITALIAN then
        labelLanguage:setString("current language is Italian")
    elseif currentLanguageType == ax.LANGUAGE_RUSSIAN then
        labelLanguage:setString("current language is Russian")
    elseif currentLanguageType == ax.LANGUAGE_SPANISH then
        labelLanguage:setString("current language is Spanish")
    elseif currentLanguageType == ax.LANGUAGE_KOREAN then
        labelLanguage:setString("current language is Korean")
    elseif currentLanguageType == ax.LANGUAGE_JAPANESE then
        labelLanguage:setString("current language is Japanese")
    elseif currentLanguageType == ax.LANGUAGE_HUNGARIAN then
        labelLanguage:setString("current language is Hungarian")
    elseif currentLanguageType == ax.LANGUAGE_PORTUGUESE then
        labelLanguage:setString("current language is Portuguese")
    elseif currentLanguageType == ax.LANGUAGE_ARABIC then
        labelLanguage:setString("current language is Arabic")
    end
    ret:addChild(labelLanguage)
    return ret
end

function CurrentLanguageTestMain()
    local scene = ax.Scene:create()
    local pLayer = CurrentLanguageTest()
    scene:addChild(pLayer)
    scene:addChild(CreateBackMenuItem())
    return scene
end
