#ifndef EFFECTMANAGER_HPP
#define EFFECTMANAGER_HPP

#include <SFML/Graphics.hpp>

#include "Effect.hpp"

class EffectManager
{
public:
    EffectManager() = default;

    void createDust(Vec2 position, int count, size_t& currentFrame);
    void update(float dt);
    void render(sf::RenderWindow& window);

private:
    std::vector<DustParticle> m_dust;
    std::vector<DustParticle> m_dustToAdd;
};

#endif //EFFECTMANAGER_HPP
