/** Thoughts of changing jump mechanics */
#ifndef TEST_RAIN_HPP
#define TEST_RAIN_HPP

#include <iostream>

#include <SFML/Graphics.hpp>

#include "GameEngine.hpp"
#include "Vec2.hpp"

class RainDrop
{
public:
    Vec2 position;
    Vec2 velocity;

    RainDrop(Vec2 pos, Vec2 vel) :
        position(pos), velocity(vel) {}

    void update(float dt)
    {
        position += velocity * dt;
    }
};

std::vector<RainDrop> rainDrops;
void generateRain(int count)
{
    for (int i = 0; i < count; ++i)
    {
        Vec2 pos(rand() % game::kWinWidth, rand() % game::kWinHeight);
        // Vec2 vel(-170, rand() % 200 + 300);
        Vec2 vel(-170, static_cast<float>(rand() % 200 + 100));
        rainDrops.emplace_back(pos, vel);
    }
}

void updateRain(float dt)
{
    for (auto& drop: rainDrops)
    {
        drop.update(dt);
        if (drop.position.y > game::kWinHeight)
        {
            drop.position.y = 0;
            drop.position.x = rand() % game::kWinWidth;
        }
    }
}

void renderRain(sf::RenderWindow& window)
{
    for (const auto& drop: rainDrops)
    {
        sf::RectangleShape rainDropShape(sf::Vector2f(1, 25)); // drops form
        rainDropShape.setPosition(drop.position.x, drop.position.y);
        rainDropShape.setRotation(15);
        rainDropShape.setFillColor(game::Silver);
        window.draw(rainDropShape);
    }
}

void test_rain()
{
    sf::RenderWindow window(sf::VideoMode(1200, 760), "Kinda rain");
    window.setFramerateLimit(60);

    // So called char
    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setPosition(100.0f, 500.0f);
    player.setFillColor(sf::Color::Blue);

    float velocityX = 0.0f;
    generateRain(200);

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Left)
                {
                    velocityX = -5.0f;
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    velocityX = 5.0f;
                }
            }
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
                {
                    velocityX = 0.f;
                }
            }
        }
        player.move(velocityX, 0.f);
        window.clear();

        updateRain(0.0167f);
        renderRain(window);

        window.draw(player);
        window.display();
    }
}

#endif //TEST_RAIN_HPP
