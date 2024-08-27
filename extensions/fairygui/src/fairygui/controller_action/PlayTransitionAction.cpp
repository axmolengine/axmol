#include "PlayTransitionAction.h"
#include "Controller.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

PlayTransitionAction::PlayTransitionAction() :
    playTimes(1), delay(0), stopOnExit(false), _currentTransition(nullptr)
{
}

void PlayTransitionAction::setup(ByteBuffer * buffer)
{
    ControllerAction::setup(buffer);

    transitionName = buffer->readS();
    playTimes = buffer->readInt();
    delay = buffer->readFloat();
    stopOnExit = buffer->readBool();
}

void PlayTransitionAction::enter(GController * controller)
{
    Transition* trans = controller->getParent()->getTransition(transitionName);
    if (trans != nullptr)
    {
        if (_currentTransition != nullptr && _currentTransition->isPlaying())
            trans->changePlayTimes(playTimes);
        else
            trans->play(playTimes, delay, nullptr);
        _currentTransition = trans;
    }
}

void PlayTransitionAction::leave(GController * controller)
{
    if (stopOnExit && _currentTransition != nullptr)
    {
        _currentTransition->stop();
        _currentTransition = nullptr;
    }
}

NS_FGUI_END