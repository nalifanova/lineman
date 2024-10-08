#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Entity.hpp"
#include "Vec2.hpp"

struct Intersect
{
    bool intersect = false;
    Vec2 position = {0.0f, 0.0f};
};

class Physics
{
public:
    static Vec2 overlap(const Vec2& aPos, const Vec2& bPos, const Vec2& aSize, const Vec2& bSize);

    static Vec2 getOverlap(Entity& a, Entity& b);
    static Vec2 getPreviousOverlap(Entity& a, Entity& b);
    static bool isColliding(Entity& a, Entity& b);
    static bool isInteractableColliding(Entity& player, Entity& intr);

    static Intersect lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);
    static bool entityIntersect(const Vec2& a, const Vec2& b, Entity& entity);
};

#endif //PHYSICS_HPP
