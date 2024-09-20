#include "Physics.hpp"

#include "Components.hpp"

Vec2 Physics::overlap(const Vec2& aPos, const Vec2& bPos, const Vec2& aSize, const Vec2& bSize)
{
    const Vec2 d = {abs(aPos.x - bPos.x), abs(aPos.y - bPos.y)};
    return {aSize.x + bSize.x - d.x, aSize.y + bSize.y - d.y};
}

Vec2 Physics::getOverlap(Entity& a, Entity& b)
{
    const auto& aPos = a.get<CTransform>().pos;
    const auto& bPos = b.get<CTransform>().pos;
    const auto& aSize = a.get<CBoundingBox>().halfSize;
    const auto& bSize = b.get<CBoundingBox>().halfSize;
    return overlap(aPos, bPos, aSize, bSize);
}

Vec2 Physics::getPreviousOverlap(Entity& a, Entity& b)
{
    const auto& aPos = a.get<CTransform>().prevPos;
    const auto& bPos = b.get<CTransform>().prevPos;
    const auto& aSize = a.get<CBoundingBox>().halfSize;
    const auto& bSize = b.get<CBoundingBox>().halfSize;
    return overlap(aPos, bPos, aSize, bSize);
}

bool Physics::isColliding(Entity& a, Entity& b)
{
    auto overlap = getOverlap(a, b);
    if (overlap.x > 0.0f && overlap.y > 0.0f) { return true; }
    return false;
}

Intersect Physics::lineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
    const Vec2 r = b - a;
    const Vec2 s = d - c;
    const float rxs = r.cross2d(s); // 2d cross product
    const Vec2 cma = c - a;
    const float t = cma.cross2d(s) / rxs; // scalar value for s
    const float u = cma.cross2d(r) / rxs; // scalar value for r

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
        return {true, a + r * t};

    return {false, Vec2(0.0f, 0.0f)};
}

bool Physics::entityIntersect(const Vec2& a, const Vec2& b, Entity& entity)
{
    // Position is a center point of an entity thus we should find position of each vertex
    // Then we have to check if line a-b intersects with each life of a BBox of the entity
    const auto& halfSize = entity.get<CBoundingBox>().halfSize;
    const auto& pos = entity.get<CTransform>().pos;

    Vec2 topLeft(pos.x - halfSize.x, pos.y - halfSize.y);
    Vec2 topRight(pos.x + halfSize.x, pos.y - halfSize.y);
    Vec2 bottomLeft(pos.x - halfSize.x, pos.y + halfSize.y);
    Vec2 bottomRight(pos.x + halfSize.x, pos.y + halfSize.y);

    bool intersects = lineIntersect(a, b, topLeft, topRight).intersect;
    intersects = intersects || lineIntersect(a, b, topRight, bottomRight).intersect;
    intersects = intersects || lineIntersect(a, b, bottomRight, bottomLeft).intersect;
    intersects = intersects || lineIntersect(a, b, bottomLeft, topLeft).intersect;

    return intersects;
}
