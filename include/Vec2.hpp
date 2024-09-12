#ifndef VEC2_H
#define VEC2_H

#include <iostream>

class Vec2
{
public:
    Vec2() = default;
    Vec2(float xIn, float yIn);

    bool operator ==(const Vec2& rhs) const;
    bool operator !=(const Vec2& rhs) const;

    Vec2 operator +(const Vec2& rhs) const;
    Vec2 operator -(const Vec2& rhs) const;
    Vec2 operator /(float val) const;
    Vec2 operator *(float val) const;

    bool operator +=(const Vec2& rhs);
    bool operator -=(const Vec2& rhs);
    bool operator /=(float val);
    bool operator *=(float val);
    bool operator +=(float val);
    bool operator -=(float val);

    bool operator<(const Vec2& rhs) const;

    friend std::ostream&
    operator<<(std::ostream& os, const Vec2& vec)
    {
        return os << "(" << vec.x << ", " << vec.y << ")";
    }

    /**
     * Line segment intersection
     * 2D Cross
     *          a x b = (a.x * b.y) - (a.y * b.x)
     *
     * Properties:
     *          a x a = 0
     *          (a + r) x s = (a x s) + (r x s)
     *          (r x s) = - (s x r)
     */
    [[nodiscard]] float cross2d(const Vec2& rhs) const;

    /**
     * Vector length is a distance it travels from origin (0.0, 0.0) to
     * desired destination        ___________
     *                      L = \/ x^2 + y^2
     */
    [[nodiscard]] float length() const;

    /**
     * Normalizing a vector maintains its angle but changes its length to 1
     * (unit vector)
     *      N = Vec2(V.x / L, V.y / L)
     */
    [[nodiscard]] Vec2 normalize();
    [[nodiscard]] float magnitude() const;
    [[nodiscard]] float angle(const Vec2& point) const;

    float x = 0.0f;
    float y = 0.0f;
};

#endif //VEC2_H
