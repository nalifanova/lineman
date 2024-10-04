#include  "EffectManager.hpp"

void EffectManager::createDust(Vec2 position, int count, size_t& currentFrame)
{
    for (int i = 0; i < count; ++i)
    {
        Vec2 velocity(rand() % 80 - 50, -abs(rand() % 30 - 25)); // Different directions
        float lifetime = 1.0f + static_cast<float>(rand() % 50) / 100.0f + 72.f; // + 1.2sec
        auto size = static_cast<float>(rand() % 3 + 0.5); // .5-3.5 pixels
        m_dust.emplace_back(position, velocity, size, lifetime, currentFrame);
    }
}

void EffectManager::update(float dt)
{
    for (DustParticle& particle: m_dust)
    {
        particle.update(dt);
    }

    std::erase_if(m_dust, [](const DustParticle& particle) { return !particle.isAlive(); });
}

void EffectManager::render(sf::RenderWindow& window)
{
    for (const auto& particle: m_dust)
    {
        if (particle.isAlive())
        {
            sf::CircleShape dustShape(particle.size());
            dustShape.setPosition(particle.pos().x, particle.pos().y);
            dustShape.setFillColor(sf::Color(100, 100, 100, static_cast<sf::Uint8>(particle.getOpacity())));
            window.draw(dustShape);
        }
    }
}
