#include "ChatScene.h"
#include "EmojiParser.h"

using namespace ax;

ChatScene::ChatScene() :_emojiSelectUI(nullptr)
{
}

ChatScene::~ChatScene()
{
    AX_SAFE_RELEASE(_emojiSelectUI);
}

void ChatScene::continueInit()
{
    UIPackage::addPackage("UI/Emoji");
    UIConfig::verticalScrollBar = "";

    _view = UIPackage::createObject("Emoji", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _list = _view->getChild("list")->as<GList>();
    _list->setVirtual();
    _list->itemProvider = AX_CALLBACK_1(ChatScene::getListItemResource, this);
    _list->itemRenderer = AX_CALLBACK_2(ChatScene::renderListItem, this);

    _input = _view->getChild("input")->as<GTextInput>();
    _input->addEventListener(UIEventType::Submit, AX_CALLBACK_1(ChatScene::onSubmit, this));

    _view->getChild("btnSend")->addClickListener(AX_CALLBACK_1(ChatScene::onClickSendBtn, this));
    _view->getChild("btnEmoji")->addClickListener(AX_CALLBACK_1(ChatScene::onClickEmojiBtn, this));

    _emojiSelectUI = UIPackage::createObject("Emoji", "EmojiSelectUI")->as<GComponent>();
    _emojiSelectUI->retain();
    _emojiSelectUI->getChild("list")->addEventListener(UIEventType::ClickItem, AX_CALLBACK_1(ChatScene::onClickEmoji, this));
}

void ChatScene::onClickSendBtn(EventContext * context)
{
    const std::string& msg = _input->getText();
    if (msg.empty())
        return;

    addMsg("Unity", "r0", msg, true);
    _input->setText("");
}

void ChatScene::onClickEmojiBtn(EventContext * context)
{
    _groot->showPopup(_emojiSelectUI, (GObject*)context->getSender(), PopupDirection::UP);
}

void ChatScene::onClickEmoji(EventContext * context)
{
    GObject* item = (GObject*)context->getData();
    _input->setText(_input->getText() + "[:" + item->getText() + "]");
}

void ChatScene::onSubmit(EventContext * context)
{
    onClickSendBtn(nullptr);
}

void ChatScene::renderListItem(int index, GObject * obj)
{
    GButton* item = obj->as<GButton>();
    Message msg = _messages[index];
    if (!msg.fromMe)
        item->getChild("name")->setText(msg.sender);
    item->setIcon("ui://Emoji/" + msg.senderIcon);

    GRichTextField* tf = item->getChild("msg")->as<GRichTextField>();
    tf->setText("");
    tf->setWidth(tf->initSize.width);
    tf->setText(EmojiParser::getInstance()->parse(msg.msg.c_str()));
    tf->setWidth(tf->getTextSize().width);
}

std::string ChatScene::getListItemResource(int index)
{
    Message msg = _messages[index];
    if (msg.fromMe)
        return "ui://Emoji/chatRight";
    else
        return "ui://Emoji/chatLeft";
}

void ChatScene::addMsg(const std::string& sender, const std::string& senderIcon, const std::string& msg, bool fromMe)
{
    bool isScrollBottom = _list->getScrollPane()->isBottomMost();
    Message newMessage;
    newMessage.sender = sender;
    newMessage.senderIcon = senderIcon;
    newMessage.msg = msg;
    newMessage.fromMe = fromMe;
    _messages.push_back(newMessage);

    if (newMessage.fromMe)
    {
        if (_messages.size() == 1 || rand_0_1() < 0.5f)
        {
            Message replyMessage;
            replyMessage.sender = "FairyGUI";
            replyMessage.senderIcon = "r1";
            replyMessage.msg = "Today is a good day. [:cool]";
            replyMessage.fromMe = false;
            _messages.push_back(replyMessage);
        }
    }

    if (_messages.size() > 100)
        _messages.erase(_messages.cbegin(), _messages.cend() - 100);

    _list->setNumItems(_messages.size());

    if (isScrollBottom)
        _list->getScrollPane()->scrollBottom(true);
}
