#include "TweenManager.h"
#include "GTweener.h"

NS_FGUI_BEGIN
USING_NS_AX;

GTweener** TweenManager::_activeTweens = nullptr;
std::vector<GTweener*> TweenManager::_tweenerPool;
int TweenManager::_totalActiveTweens = 0;
int TweenManager::_arrayLength = 0;
bool TweenManager::_inited = false;

class TweenEngine
{
public:
    void update(float dt)
    {
        TweenManager::update(dt);
    }
};
static TweenEngine tweenEngine;

GTweener* TweenManager::createTween()
{
    if (!_inited)
        init();

    GTweener* tweener;
    int cnt = (int)_tweenerPool.size();
    if (cnt > 0)
    {
        tweener = _tweenerPool[cnt - 1];
        _tweenerPool.pop_back();
    }
    else
        tweener = new GTweener();
    tweener->_init();
    _activeTweens[_totalActiveTweens++] = tweener;

    if (_totalActiveTweens == _arrayLength)
    {
        int newLen = _arrayLength + ceil(_arrayLength * 0.5f);
        GTweener** newArray = new GTweener*[newLen];
        memcpy(newArray, _activeTweens, _arrayLength * sizeof(GTweener*));
        delete _activeTweens;
        _activeTweens = newArray;
        _arrayLength = newLen;
    }

    return tweener;
}

bool TweenManager::isTweening(ax::Object* target, TweenPropType propType)
{
    if (target == nullptr)
        return false;

    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < _totalActiveTweens; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed && (anyType || tweener->_propType == propType))
            return true;
    }

    return false;
}

bool TweenManager::killTweens(ax::Object* target, TweenPropType propType, bool completed)
{
    if (target == nullptr)
        return false;

    bool flag = false;
    int cnt = _totalActiveTweens;
    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed && (anyType || tweener->_propType == propType))
        {
            tweener->kill(completed);
            flag = true;
        }
    }

    return flag;
}

GTweener* TweenManager::getTween(ax::Object* target, TweenPropType propType)
{
    if (target == nullptr)
        return nullptr;

    int cnt = _totalActiveTweens;
    bool anyType = propType == TweenPropType::None;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr && tweener->_target == target && !tweener->_killed && (anyType || tweener->_propType == propType))
        {
            return tweener;
        }
    }

    return nullptr;
}

void TweenManager::update(float dt)
{
    int cnt = _totalActiveTweens;
    int freePosStart = -1;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener == nullptr)
        {
            if (freePosStart == -1)
                freePosStart = i;
        }
        else if (tweener->_killed)
        {
            tweener->_reset();
            _tweenerPool.push_back(tweener);
            _activeTweens[i] = nullptr;

            if (freePosStart == -1)
                freePosStart = i;
        }
        else
        {
            if (tweener->_refTarget != nullptr && tweener->_refTarget->getReferenceCount() == 1)
                tweener->_killed = true;
            else if (!tweener->_paused)
                tweener->_update(dt);

            if (freePosStart != -1)
            {
                _activeTweens[freePosStart] = tweener;
                _activeTweens[i] = nullptr;
                freePosStart++;
            }
        }
    }

    if (freePosStart >= 0)
    {
        if (_totalActiveTweens != cnt) //new tweens added
        {
            int j = cnt;
            cnt = _totalActiveTweens - cnt;
            for (int i = 0; i < cnt; i++)
                _activeTweens[freePosStart++] = _activeTweens[j++];
        }
        _totalActiveTweens = freePosStart;
    }
}

void TweenManager::clean()
{
    for (auto it = _tweenerPool.begin(); it != _tweenerPool.end(); it++)
        (*it)->release();
    _tweenerPool.clear();
}

void TweenManager::init()
{
    _inited = true;

    if (_activeTweens == nullptr)
    {
        _arrayLength = 30;
        _activeTweens = new GTweener*[_arrayLength];
    }

    ax::Director::getInstance()->getScheduler()->scheduleUpdate(&tweenEngine, INT_MIN + 10, false);
    ax::Director::getInstance()->getEventDispatcher()->addCustomEventListener(ax::Director::EVENT_RESET, &reset);
}

void TweenManager::reset(ax::EventCustom*)
{
    int cnt = _totalActiveTweens;
    for (int i = 0; i < cnt; i++)
    {
        GTweener* tweener = _activeTweens[i];
        if (tweener != nullptr)
        {
            tweener->_reset();
            _tweenerPool.push_back(tweener);
            _activeTweens[i] = nullptr;
        }
    }

    _totalActiveTweens = 0;
    _inited = false;
}

NS_FGUI_END