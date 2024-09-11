#include "Animation.hpp"

#include <cmath>

#include "SFML/Graphics/Texture.hpp"

Animation::Animation() = default;

Animation::Animation(const std::string& name, const sf::Texture& t) :
    Animation(name, t, 1, 0) {}

Animation::Animation(std::string name, const sf::Texture& t, const size_t frameCount, const size_t speed) :
    m_name(std::move(name)), m_sprite(t), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed)
{
    m_size = Vec2(
        static_cast<float>(t.getSize().x) / static_cast<float>(frameCount),
        static_cast<float>(t.getSize().y) / 1.0f
        );
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(sf::IntRect(
            std::floor(static_cast<float>(m_currentFrame) * m_size.x),
            0,
            static_cast<int>(m_size.x),
            static_cast<int>(m_size.y)
            )
        );
}

void Animation::update()
{
    m_currentFrame++; // add the speed variable to the current frame

    const size_t animationFrame = (m_currentFrame / m_speed) % m_frameCount;
    m_sprite.setTextureRect(sf::IntRect(
            static_cast<int>(animationFrame) * static_cast<int>(m_size.x),
            0,
            static_cast<int>(m_size.x),
            static_cast<int>(m_size.y))
        );
}

bool Animation::hasEnded() const
{
    return (m_currentFrame / m_speed) % m_frameCount == m_frameCount - 1;
}

const Vec2& Animation::getSize() const
{
    return m_size;
}

const std::string& Animation::getName() const
{
    return m_name;
}

const sf::Sprite& Animation::getSprite() const
{
    return m_sprite;
}

sf::Sprite& Animation::getSprite()
{
    return m_sprite;
}
