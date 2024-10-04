#include "Effect.hpp"

DustParticle::DustParticle(Vec2 pos, Vec2 vel, float size, size_t lifetime, size_t& currentFrame):
    m_pos(pos), m_vel(vel), m_size(size), m_lifetime(lifetime), m_currentFrame(currentFrame),
    m_frameCreated(currentFrame) {}

void DustParticle::update(float dt) { m_pos += m_vel * dt; }

bool DustParticle::isAlive() const { return m_currentFrame - m_frameCreated <= m_lifetime; }

Vec2 DustParticle::pos() const { return m_pos; }

float DustParticle::size() const { return m_size; }

size_t DustParticle::lifetime() const { return m_lifetime; }

size_t DustParticle::frameCreated() const { return m_frameCreated; }

float DustParticle::getOpacity() const
{
    const auto remainingTime = m_currentFrame - m_frameCreated;
    return 130 - 128 * (static_cast<float>(remainingTime) / static_cast<float>(m_lifetime));
}
