#include "Vec2.hpp"

#include <cmath>

Vec2::Vec2(const float xIn, const float yIn):
    x(xIn), y(yIn) {}

bool Vec2::operator==(const Vec2& rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
    return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator-(float val) const
{
    return {x - val, y - val};
}

Vec2 Vec2::operator/(float val) const
{
    return {x / val, y / val};
}

Vec2 Vec2::operator*(float val) const
{
    return {x * val, y * val};
}

bool Vec2::operator+=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return true; // why bool??
}

bool Vec2::operator-=(const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return true; // why bool??
}

bool Vec2::operator/=(float val)
{
    if (val == 0)
    {
        return false;
    }

    x /= val;
    y /= val;
    return true;
}

bool Vec2::operator*=(float val)
{
    x *= val;
    y *= val;
    return true; // why bool??
}

bool Vec2::operator+=(const float val)
{
    x += val;
    y += val;
    return true;
}

bool Vec2::operator-=(const float val)
{
    x -= val;
    y -= val;
    return true;
}

bool Vec2::operator<(const Vec2& rhs) const
{
    return (x < rhs.x) || (x == rhs.x && y < rhs.y);
}

float Vec2::cross2d(const Vec2& rhs) const
{
    return x * rhs.y - y * rhs.x;
}

float Vec2::length() const
{
    return sqrt(x * x + y * y);
}

Vec2 Vec2::normalize()
{
    *this = (length() == 0.0f) ? Vec2(0.0f, 0.0f) : *this / length();
    return *this;
}

Vec2& Vec2::magnitude(const float length)
{
    normalize();
    *this *= length;
    return *this;
}

float Vec2::angle(const Vec2& point) const
{
    return atan2(y - point.y, x - point.x);
}

float Vec2::dist(const Vec2& rhs) const
{
    const float dX = abs(x - rhs.x);
    const float dY = abs(y - rhs.y);
    return sqrtf(dX * dX + dY * dY);
}