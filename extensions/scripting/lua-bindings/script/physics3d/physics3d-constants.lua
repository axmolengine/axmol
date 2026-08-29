if nil == ax.Rigidbody3D then
    return
end

ax.ContactEventBits =
{
    None = 0,
    PreSolve = 1,
    Hit = 2,
    Contact = 4,
    Sensor = 8,
    Persisted = 16,
    AllBits = 31,
}

ax.Rigidbody3D.SyncFlag =
{
    NONE = 0,
    NODE_TO_PHYSICS = 1,
    PHYSICS_TO_NODE = 2,
    NODE_AND_PHYSICS = 3,
}

ax.Collider3D.SyncFlag = ax.Rigidbody3D.SyncFlag

ax.Joint3D.Type =
{
    UNKNOWN = 0,
    POINT_TO_POINT = 1,
    HINGE = 2,
    SLIDER = 3,
    CONE_TWIST = 4,
    SIX_DOF = 5,
}
