
set(lib_name box2d-optimized)
set(target_name ${lib_name})

project(${lib_name})

set(BOX2D_SOURCE_FILES
	src/collision/b2_broad_phase.cpp
	src/collision/b2_circle_shape.cpp
	src/collision/b2_collide_circle.cpp
	src/collision/b2_collide_edge.cpp
	src/collision/b2_collide_polygon.cpp
	src/collision/b2_collision.cpp
	src/collision/b2_distance.cpp
	src/collision/b2_edge_shape.cpp
	src/collision/b2_polygon_shape.cpp
	src/collision/b2_time_of_impact.cpp
	src/common/b2_block_allocator.cpp
	src/common/b2_draw.cpp
	src/common/b2_math.cpp
	src/common/b2_settings.cpp
	src/common/b2_geometry.cpp
	src/common/b2_stack_allocator.cpp
	src/common/b2_timer.cpp
	src/dynamics/b2_body.cpp
	src/dynamics/b2_contact.cpp
	src/dynamics/b2_contact_manager.cpp
	src/dynamics/b2_contact_solver.cpp
	src/dynamics/b2_contact_solver.h
	src/dynamics/b2_distance_joint.cpp
	src/dynamics/b2_fixture.cpp
	src/dynamics/b2_friction_joint.cpp
	src/dynamics/b2_gear_joint.cpp
	src/dynamics/b2_island.cpp
	src/dynamics/b2_island.h
	src/dynamics/b2_joint.cpp
	src/dynamics/b2_motor_joint.cpp
	src/dynamics/b2_mouse_joint.cpp
	src/dynamics/b2_prismatic_joint.cpp
	src/dynamics/b2_pulley_joint.cpp
	src/dynamics/b2_revolute_joint.cpp
	src/dynamics/b2_weld_joint.cpp
	src/dynamics/b2_wheel_joint.cpp
	src/dynamics/b2_world.cpp
	src/dynamics/b2_world_callbacks.cpp
	src/rope/b2_rope.cpp
	src/particle/b2_particle.cpp
	src/particle/b2_particle_group.cpp
	src/particle/b2_particle_system.cpp
	src/particle/b2_stack_queue.h
	src/particle/b2_free_list.cpp
	src/particle/b2_voronoi_diagram.cpp
	src/particle/b2_voronoi_diagram.h
	src/particle/b2_tracked_block.cpp)

set(BOX2D_HEADER_FILES
	"include/box2d/b2_api.h"
	"include/box2d/b2_block_allocator.h"
	"include/box2d/b2_body.h"
	"include/box2d/b2_broad_phase.h"
	"include/box2d/b2_circle_shape.h"
	"include/box2d/b2_collision.h"
	"include/box2d/b2_common.h"
	"include/box2d/b2_contact.h"
	"include/box2d/b2_contact_manager.h"
	"include/box2d/b2_distance.h"
	"include/box2d/b2_distance_joint.h"
	"include/box2d/b2_draw.h"
	"include/box2d/b2_edge_shape.h"
	"include/box2d/b2_fixture.h"
	"include/box2d/b2_friction_joint.h"
	"include/box2d/b2_gear_joint.h"
	"include/box2d/b2_growable_stack.h"
	"include/box2d/b2_joint.h"
	"include/box2d/b2_math.h"
	"include/box2d/b2_motor_joint.h"
	"include/box2d/b2_mouse_joint.h"
	"include/box2d/b2_polygon_shape.h"
	"include/box2d/b2_prismatic_joint.h"
	"include/box2d/b2_pulley_joint.h"
	"include/box2d/b2_revolute_joint.h"
	"include/box2d/b2_rope.h"
	"include/box2d/b2_settings.h"
	"include/box2d/b2_geometry.h"
	"include/box2d/b2_shape.h"
	"include/box2d/b2_stack_allocator.h"
	"include/box2d/b2_time_of_impact.h"
	"include/box2d/b2_timer.h"
	"include/box2d/b2_time_step.h"
	"include/box2d/b2_types.h"
	"include/box2d/b2_weld_joint.h"
	"include/box2d/b2_wheel_joint.h"
	"include/box2d/b2_world.h"
	"include/box2d/b2_world_callbacks.h"
	"include/box2d/b2_growable_buffer.h"
	"include/box2d/b2_intrusive_list.h"
	"include/box2d/b2_slab_allocator.h"
	"include/box2d/b2_free_list.h"
	"include/box2d/b2_tracked_block.h"
	"include/box2d/b2_particle_group.h"
	"include/box2d/b2_particle.h"
	"include/box2d/b2_particle_system.h"
	"include/box2d/box2d.h")

add_library(${target_name} STATIC ${BOX2D_SOURCE_FILES} ${BOX2D_HEADER_FILES})

target_include_directories(${target_name} 
    PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
