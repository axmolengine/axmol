-- enable log


local function doTest()
    cclog("********************** init value ***********************")

    -- set default value

    ax.UserDefault:getInstance():setStringForKey("string", "value1")
    ax.UserDefault:getInstance():setIntegerForKey("integer", 10)
    ax.UserDefault:getInstance():setFloatForKey("float", 2.3)
    ax.UserDefault:getInstance():setDoubleForKey("double", 2.4)
    ax.UserDefault:getInstance():setBoolForKey("bool", true)

    -- print value

    local ret = ax.UserDefault:getInstance():getStringForKey("string")
    cclog("string is %s", ret)

    local d = ax.UserDefault:getInstance():getDoubleForKey("double")
    cclog("double is %f", d)

    local i = ax.UserDefault:getInstance():getIntegerForKey("integer")
    cclog("integer is %d", i)

    local f = ax.UserDefault:getInstance():getFloatForKey("float")
    cclog("float is %f", f)

    local b = ax.UserDefault:getInstance():getBoolForKey("bool")
    if b == true then
        cclog("bool is true")
    else
        cclog("bool is false")
    end

    --ax.UserDefault:getInstance():flush()

    cclog("********************** after change value ***********************")

    -- change the value

    ax.UserDefault:getInstance():setStringForKey("string", "value2")
    ax.UserDefault:getInstance():setIntegerForKey("integer", 11)
    ax.UserDefault:getInstance():setFloatForKey("float", 2.5)
    ax.UserDefault:getInstance():setDoubleForKey("double", 2.6)
    ax.UserDefault:getInstance():setBoolForKey("bool", false)

    ax.UserDefault:getInstance():flush()

    -- print value

    ret = ax.UserDefault:getInstance():getStringForKey("string")
    cclog("string is %s", ret)

    d = ax.UserDefault:getInstance():getDoubleForKey("double")
    cclog("double is %f", d)

    i = ax.UserDefault:getInstance():getIntegerForKey("integer")
    cclog("integer is %d", i)

    f = ax.UserDefault:getInstance():getFloatForKey("float")
    cclog("float is %f", f)

    b = ax.UserDefault:getInstance():getBoolForKey("bool")
    if b == true then
        cclog("bool is true")
    else
        cclog("bool is false")
    end
end

function UserDefaultTestMain()
    local ret = ax.Scene:create()
    local s = ax.Director:getInstance():getLogicalSize()
    local  label = ax.Label:createWithTTF("UserDefault test see log", s_arialPath, 28)
    ret:addChild(label, 0)
    label:setAnchorPoint(ax.p(0.5, 0.5))
    label:setPosition( ax.p(s.width/2, s.height-50) )
    ret:addChild(CreateBackMenuItem())
    doTest()
    return ret
end
