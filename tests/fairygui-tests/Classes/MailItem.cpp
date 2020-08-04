#include "MailItem.h"

void MailItem::onConstruct()
{
    _timeText = getChild("timeText")->as<GTextField>();
    _readController = getController("IsRead");
    _fetchController = getController("c1");
    _trans = getTransition("t0");
}

void MailItem::setTime(const std::string& value)
{
    _timeText->setText(value);
}

void MailItem::setRead(bool value)
{
    _readController->setSelectedIndex(value ? 1 : 0);
}

void MailItem::setFetched(bool value)
{
    _fetchController->setSelectedIndex(value ? 1 : 0);
}

void MailItem::playEffect(float delay)
{
    setVisible(false);
    _trans->play(1, delay, nullptr);
}
