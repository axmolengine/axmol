#include "ApplicationBase.h"
#include "base/Director.h"

NS_AX_BEGIN

ApplicationBase::~ApplicationBase()
{
    Director::destroyInstance();
}

NS_AX_END
