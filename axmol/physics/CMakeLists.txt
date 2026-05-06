set(_AX_PHYSICS_HEADER
  physics/PhysicsConsts.h
  physics/PhysicsActor.h
  physics/ContactEventBits.h
  )

set(_AX_PHYSICS_SRC
  physics/PhysicsActor.cpp
  )

if(AX_ENABLE_PHYSICS_2D)
  include(physics/2d/CMakeLists.txt)
endif()

if(AX_ENABLE_PHYSICS_3D)
  include(physics/3d/CMakeLists.txt)
endif()
