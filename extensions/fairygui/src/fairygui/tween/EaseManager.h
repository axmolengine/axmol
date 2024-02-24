#ifndef __EASE_MANAGER_H__
#define __EASE_MANAGER_H__

#include "FairyGUIMacros.h"
#include "EaseType.h"

NS_FGUI_BEGIN

class EaseManager
{
public:
    static float evaluate(EaseType easeType, float time, float duration, float overshootOrAmplitude, float period);
};

NS_FGUI_END

#endif
