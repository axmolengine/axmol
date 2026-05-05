#include "axmol/physics/PhysicsActor.h"

namespace ax
{
bool PhysicsActor::isEventEnabled(ContactEventBits events) const
{
    return bitmask::only(_eventBits, events);
}

void PhysicsActor::setEventEnabled(ContactEventBits events, bool enabled)
{
    if (enabled)
        _eventBits |= events;
    else
        _eventBits &= ~events;
}

}  // namespace ax
