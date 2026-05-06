#pragma once
#include "axmol/scene/Component.h"
#include "axmol/physics/ContactEventBits.h"

namespace ax
{

class AX_DLL PhysicsActor : public Component
{
public:
    enum
    {
        kCollider,
        kRigidbody,
    };

    enum class SyncFlag
    {
        NONE             = 0,
        NODE_TO_PHYSICS  = 1,
        PHYSICS_TO_NODE  = 2,
        NODE_AND_PHYSICS = NODE_TO_PHYSICS | PHYSICS_TO_NODE
    };

    virtual void syncNodeToPhysics() = 0;
    virtual void syncPhysicsToNode() = 0;

    virtual Mat4 getWorldTransform() const = 0;

    void preSimulate()
    {
        if ((int)_syncFlag & (int)SyncFlag::NODE_TO_PHYSICS)
            syncNodeToPhysics();
    }

    void postSimulate()
    {
        if ((int)_syncFlag & (int)SyncFlag::PHYSICS_TO_NODE)
            syncPhysicsToNode();
    }

    int getActorType() const { return _actorType; }

    void setSyncFlag(SyncFlag flag) { _syncFlag = flag; }
    SyncFlag getSyncFlag() const { return _syncFlag; }

    void setTag(unsigned int tag) { _tag = tag; }
    unsigned int getTag() const { return _tag; }

    bool isEventEnabled(ContactEventBits events) const;
    void setEventEnabled(ContactEventBits events, bool enabled);

protected:
    SyncFlag _syncFlag{SyncFlag::NODE_AND_PHYSICS};
    int _actorType{kCollider};
    ContactEventBits _eventBits{ContactEventBits::AllBits};
    unsigned int _tag{0u};
};

}  // namespace ax
