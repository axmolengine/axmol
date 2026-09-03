if nil == axui then
    return
end

axui.TextField = axui.InputField

--tip
local function deprecatedTip(old_name,new_name)
    print("\n********** \n"..old_name.." was deprecated please use ".. new_name .. " instead.\n**********")
end

--functions of axui.Text will be deprecated begin
local TextDeprecated = { }
function TextDeprecated.setText(self, str)
    deprecatedTip("axui.Text:setText","axui.Text:setString")
    return self:setString(str)
end
axui.Text.setText = TextDeprecated.setText

function TextDeprecated.getStringValue(self)
    deprecatedTip("axui.Text:getStringValue","axui.Text:getString")
    return self:getString()
end
axui.Text.getStringValue = TextDeprecated.getStringValue

--functions of axui.Text will be deprecated begin

--functions of axui.TextAtlas will be deprecated begin
local TextAtlasDeprecated = { }
function TextAtlasDeprecated.setStringValue(self, str)
    deprecatedTip("axui.TextAtlas:setStringValue","axui.TextAtlas:setString")
    return self:setString(str)
end
axui.TextAtlas.setStringValue = TextAtlasDeprecated.setStringValue

function TextAtlasDeprecated.getStringValue(self)
    deprecatedTip("axui.TextAtlas:getStringValue","axui.TextAtlas:getString")
    return self:getString()
end
axui.TextAtlas.getStringValue = TextAtlasDeprecated.getStringValue
--functions of axui.TextAtlas will be deprecated begin


--functions of axui.TextBMFont will be deprecated begin
local TextBMFontDeprecated = { }
function TextBMFontDeprecated.setText(self, str)
    deprecatedTip("axui.TextBMFont:setText","axui.TextBMFont:setString")
    return self:setString(str)
end
axui.TextBMFont.setText =  TextBMFontDeprecated.setText

function TextBMFontDeprecated.getStringValue(self)
    deprecatedTip("axui.Text:getStringValue","axui.TextBMFont:getString")
    return self:getString()
end
axui.Text.getStringValue = TextBMFontDeprecated.getStringValue
--functions of axui.TextBMFont will be deprecated begin

--functions of axui.Widget will be deprecated begin
local UIWidgetDeprecated = { }
function UIWidgetDeprecated.getLeftInParent(self)
    deprecatedTip("axui.Widget:getLeftInParent","axui.Widget:getLeftBoundary")
    return self:getLeftBoundary()
end
axui.Widget.getLeftInParent = UIWidgetDeprecated.getLeftInParent

function UIWidgetDeprecated.getBottomInParent(self)
    deprecatedTip("axui.Widget:getBottomInParent","axui.Widget:getBottomBoundary")
    return self:getBottomBoundary()
end
axui.Widget.getBottomInParent = UIWidgetDeprecated.getBottomInParent

function UIWidgetDeprecated.getRightInParent(self)
    deprecatedTip("axui.Widget:getRightInParent","axui.Widget:getRightBoundary")
    return self:getRightBoundary()
end
axui.Widget.getRightInParent = UIWidgetDeprecated.getRightInParent

function UIWidgetDeprecated.getTopInParent(self)
    deprecatedTip("axui.Widget:getTopInParent","axui.Widget:getTopBoundary")
    return self:getTopBoundary()
end
axui.Widget.getTopInParent = UIWidgetDeprecated.getTopInParent

function UIWidgetDeprecated.getSize(self)
    deprecatedTip("axui.Widget:getSize","axui.Widget:getContentSize")
    return self:getContentSize()
end
axui.Widget.getSize = UIWidgetDeprecated.getSize

function UIWidgetDeprecated.setSize(self, ...)
    deprecatedTip("axui.Widget:setSize","axui.Widget:setContentSize")
    return self:setContentSize(...)
end
axui.Widget.setSize = UIWidgetDeprecated.setSize

--functions of axui.Widget will be deprecated end

--functions of axui.CheckBox will be deprecated begin
local UICheckBoxDeprecated = { }
function UICheckBoxDeprecated.addEventListenerCheckBox(self,handler)
    deprecatedTip("axui.CheckBox:addEventListenerCheckBox","axui.CheckBox:addEventListener")
    return self:addEventListener(handler)
end
axui.CheckBox.addEventListenerCheckBox = UICheckBoxDeprecated.addEventListenerCheckBox

function UICheckBoxDeprecated.setSelectedState(self,flag)
   deprecatedTip("axui.CheckBox:setSelectedState", "axui.CheckBox:setSelected")
   return self:setSelected(flag)
end
axui.CheckBox.setSelectedState = UICheckBoxDeprecated.setSelectedState

function UICheckBoxDeprecated.getSelectedState(self)
   deprecatedTip("axui.CheckBox:getSelectedState", "axui.CheckBox:getSelected")
   return self:getSelected()
end
axui.CheckBox.getSelectedState = UICheckBoxDeprecated.setSelectedState

--functions of axui.CheckBox will be deprecated end

--functions of axui.Slider will be deprecated begin
local UISliderDeprecated = { }
function UISliderDeprecated.addEventListenerSlider(self,handler)
    deprecatedTip("axui.Slider:addEventListenerSlider","axui.Slider:addEventListener")
    return self:addEventListener(handler)
end
axui.Slider.addEventListenerSlider = UISliderDeprecated.addEventListenerSlider
--functions of axui.Slider will be deprecated end

--functions of axui.TextField will be deprecated begin
local UITextFieldDeprecated = { }
function UITextFieldDeprecated.addEventListenerTextField(self,handler)
    deprecatedTip("axui.TextField:addEventListenerTextField","axui.TextField:addEventListener")
    return self:addEventListener(handler)
end
axui.TextField.addEventListenerTextField = UITextFieldDeprecated.addEventListenerTextField

function UITextFieldDeprecated.setText(self, str)
    deprecatedTip("axui.TextField:setText","axui.TextField:setString")
    return self:setString(str)
end
axui.TextField.setText = UITextFieldDeprecated.setText

function UITextFieldDeprecated.getStringValue(self)
    deprecatedTip("axui.TextField:getStringValue","axui.TextField:getString")
    return self:getString()
end
axui.TextField.getStringValue = UITextFieldDeprecated.getStringValue
--functions of axui.TextField will be deprecated end

--functions of axui.PageView will be deprecated begin
local UIPageViewDeprecated = { }
function UIPageViewDeprecated.addEventListenerPageView(self,handler)
    deprecatedTip("axui.PageView:addEventListenerPageView","axui.PageView:addEventListener")
    return self:addEventListener(handler)
end
axui.PageView.addEventListenerPageView = UIPageViewDeprecated.addEventListenerPageView

function UIPageViewDeprecated.addWidgetToPage(self, widget, pageIdx)
    deprecatedTip("axui.PageView:addWidgetToPage","axui.PageView:insertPage")
    return self:insertPage(widget, pageIdx)
end
axui.PageView.addWidgetToPage = UIPageViewDeprecated.addWidgetToPage

function UIPageViewDeprecated.getCurPageIndex(self)
    deprecatedTip("axui.PageView:getCurPageIndex","axui.PageView:getCurrentPageIndex")
    return self:getCurrentPageIndex()
end
axui.PageView.getCurPageIndex = UIPageViewDeprecated.getCurPageIndex

function UIPageViewDeprecated.setCurPageIndex(self, index)
    deprecatedTip("axui.PageView:setCurPageIndex","axui.PageView:setCurrentPageIndex")
    return self:setCurrentPageIndex(index)
end
axui.PageView.setCurPageIndex = UIPageViewDeprecated.setCurPageIndex

function UIPageViewDeprecated.getPages(self)
    deprecatedTip("axui.PageView:getPages","axui.PageView:getItems")
    return self:getItems()
end
axui.PageView.getPages = UIPageViewDeprecated.getPages

function UIPageViewDeprecated.getPage(self, index)
    deprecatedTip("axui.PageView:getPage","axui.PageView:getItem")
    return self:getItem(index)
end
axui.PageView.getPage = UIPageViewDeprecated.getPage

function UIPageViewDeprecated.setCustomScrollThreshold(self)
    print("Since v3.9, this method has no effect.")
end
axui.PageView.setCustomScrollThreshold = UIPageViewDeprecated.setCustomScrollThreshold

function UIPageViewDeprecated.getCustomScrollThreshold(self)
    print("Since v3.9, this method has no effect.")
end
axui.PageView.getCustomScrollThreshold = UIPageViewDeprecated.getCustomScrollThreshold

function UIPageViewDeprecated.isUsingCustomScrollThreshold(self)
    print("Since v3.9, this method has no effect.")
end
axui.PageView.isUsingCustomScrollThreshold = UIPageViewDeprecated.isUsingCustomScrollThreshold

function UIPageViewDeprecated.setUsingCustomScrollThreshold(self)
    print("Since v3.9, this method has no effect.")
end
axui.PageView.setUsingCustomScrollThreshold = UIPageViewDeprecated.setUsingCustomScrollThreshold
--functions of axui.PageView will be deprecated end

--functions of axui.ScrollView will be deprecated begin
local UIScrollViewDeprecated = { }
function UIScrollViewDeprecated.addEventListenerScrollView(self,handler)
    deprecatedTip("axui.ScrollView:addEventListenerScrollView","axui.ScrollView:addEventListener")
    return self:addEventListener(handler)
end
axui.ScrollView.addEventListenerScrollView = UIScrollViewDeprecated.addEventListenerScrollView
--functions of axui.ScrollView will be deprecated end

--functions of axui.ListView will be deprecated begin
local UIListViewDeprecated = { }
function UIListViewDeprecated.addEventListenerListView(self,handler)
    deprecatedTip("axui.ListView:addEventListenerListView","axui.ListView:addEventListener")
    return self:addEventListener(handler)
end
axui.ListView.addEventListenerListView = UIListViewDeprecated.addEventListenerListView

function UIListViewDeprecated.requestRefreshView(self)
    deprecatedTip("axui.ListView:requestRefreshView","axui.ListView:forceDoLayout")
    return self:forceDoLayout()
end
axui.ListView.requestRefreshView = UIListViewDeprecated.requestRefreshView

function UIListViewDeprecated.refreshView(self)
    deprecatedTip("axui.ListView:refreshView","axui.ListView:refreshView")
    return self:forceDoLayout()
end
axui.ListView.refreshView = UIListViewDeprecated.refreshView
--functions of axui.ListView will be deprecated end
