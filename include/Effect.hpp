#ifndef EFFECT_HPP
#define EFFECT_HPP

#include "Vec2.hpp"

class DustParticle
{
public:
    DustParticle(Vec2 pos, Vec2 vel, float size, size_t lifetime, size_t& currentFrame);
    // No copy, no deletion
    DustParticle(const DustParticle&) = delete;
    DustParticle& operator=(const DustParticle&) = delete;

    DustParticle(DustParticle&& other) noexcept :
        m_pos(std::move(other.m_pos)),
        m_vel(std::move(other.m_vel)),
        m_size(other.m_size),
        m_lifetime(other.m_lifetime),
        m_currentFrame(other.m_currentFrame),
        m_frameCreated(other.m_frameCreated) {}

    DustParticle& operator=(DustParticle&& other) noexcept
    {
        if (this != &other)
        {
            m_pos = std::move(other.m_pos);
            m_vel = std::move(other.m_vel);
            m_size = other.m_size;
            m_lifetime = other.m_lifetime;
            m_currentFrame = other.m_currentFrame;
            m_frameCreated = other.m_frameCreated;
        }
        return *this;
    }

    void update(float dt);

    [[nodiscard]] bool isAlive() const;
    [[nodiscard]] Vec2 pos() const;
    [[nodiscard]] float size() const;
    [[nodiscard]] size_t frameCreated() const;
    [[nodiscard]] size_t lifetime() const;
    [[nodiscard]] float getOpacity() const;

private:
    Vec2 m_pos;
    Vec2 m_vel;
    float m_size;
    size_t m_lifetime;
    size_t m_frameCreated = 0;
    size_t& m_currentFrame;
    bool m_isAlive = true;
};

#endif //EFFECT_HPP
