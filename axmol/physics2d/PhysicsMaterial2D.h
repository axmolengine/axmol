#pragma once

namespace ax
{
struct PhysicsMaterial2D
{
    float density;      ///< The density of the object.
    float restitution;  ///< The bounciness of the physics body.
    float friction;     ///< The roughness of the surface of a shape.

    constexpr PhysicsMaterial2D() : density(0.0f), restitution(0.0f), friction(0.0f) {}

    constexpr PhysicsMaterial2D(float aDensity, float aRestitution, float aFriction)
        : density(aDensity), restitution(aRestitution), friction(aFriction)
    {}
};

inline constexpr PhysicsMaterial2D PHYSICS_MATERIAL_2D_DEFAULT(0.1f, 0.5f, 0.5f);

}  // namespace ax
