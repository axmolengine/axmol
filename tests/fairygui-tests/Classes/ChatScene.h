#ifndef __CHAT_SCENE_H__
#define __CHAT_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class ChatScene : public DemoScene
{
public:
    ChatScene();
    ~ChatScene();

    // implement the "static create()" method manually
    CREATE_FUNC(ChatScene);

protected:
    virtual void continueInit() override;

private:
    void onClickSendBtn(EventContext* context);
    void onClickEmojiBtn(EventContext* context);
    void onClickEmoji(EventContext* context);
    void onSubmit(EventContext* context);
    void renderListItem(int index, GObject* obj);
    std::string getListItemResource(int index);

    void addMsg(const std::string& sender, const std::string& senderIcon, const std::string& msg, bool fromMe);

    GComponent* _view;
    GList* _list;
    GTextInput* _input;
    GComponent* _emojiSelectUI;
    
    struct Message
    {
        std::string sender;
        std::string senderIcon;
        std::string msg;
        bool fromMe;
    };
    std::vector<Message> _messages;
};

#endif
