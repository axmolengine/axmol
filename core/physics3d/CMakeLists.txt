add_definitions(-DAX_USE_3D_PHYSICS=1)

set(_AX_PHYSICS3D_HEADER
    physics3d/Physics3DConstraint.h
    physics3d/Physics3DShape.h
    physics3d/Physics3D.h
    physics3d/Physics3DWorld.h
    physics3d/PhysicsMeshRenderer.h
    physics3d/Physics3DObject.h
    physics3d/Physics3DComponent.h
    physics3d/Physics3DDebugDrawer.h
    )

set(_AX_PHYSICS3D_SRC
    physics3d/Physics3D.cpp
    physics3d/Physics3DComponent.cpp
    physics3d/Physics3DConstraint.cpp
    physics3d/Physics3DDebugDrawer.cpp
    physics3d/Physics3DObject.cpp
    physics3d/Physics3DShape.cpp
    physics3d/Physics3DWorld.cpp
    physics3d/PhysicsMeshRenderer.cpp
    )
