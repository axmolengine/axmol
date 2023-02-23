#ifndef __MAILITEM_H__
#define __MAILITEM_H__

#include "FairyGUI.h"

USING_NS_FGUI;

class MailItem : public GButton
{
public:
    CREATE_FUNC(MailItem);

    void setTime(const std::string& value);
    void setRead(bool value);
    void setFetched(bool value);
    void playEffect(float delay);

protected:
    virtual void onConstruct() override;

    GTextField* _timeText;
    GController* _readController;
    GController* _fetchController;
    Transition* _trans;
};

#endif