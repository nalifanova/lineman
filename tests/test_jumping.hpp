/** Thoughts of changing jump mechanics */
#ifndef TEST_JUMPING_HPP
#define TEST_JUMPING_HPP

#include <iostream>

#include <SFML/Graphics.hpp>

void test_jumping()
{
    sf::RenderWindow window(sf::VideoMode(1200, 760), "Fixed Time Step");
    window.setFramerateLimit(60);

    // So called char
    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setPosition(100.0f, 100.0f);
    player.setFillColor(sf::Color::Green);

    float velocityY = 0.0f;
    float velocityX = 0.0f;
    float gravity = 980.0f;
    bool isJumping = false;

    const float fixedTimeStep = 1.0f / 60.0f; // update 60 times per second
    float accumulator = 0.0f;

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space && !isJumping)
                {
                    velocityY = -500.0f; // jump speed ~ 442.94
                    isJumping = true;
                }
                else if (event.key.code == sf::Keyboard::Left)
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
        // deltaTime: 0.017655 - 0.016995
        float deltaTime = clock.restart().asSeconds();
        // 0.0220313 - 0.00536458
        accumulator += deltaTime;

        // fixed update
        while (accumulator >= fixedTimeStep)
        {
            // check gravity and position: -483 .. 316
            velocityY += gravity * fixedTimeStep; // 0.0167
            std::cout << "Velocity.y + grav " << velocityY << "\n";
            player.move(velocityX, velocityY * fixedTimeStep);

            // any collision check!
            // collision with ground
            if (player.getPosition().y >= 500.0f)
            {
                player.setPosition(player.getPosition().x, 500.0f);
                velocityY = 0.0f;
                isJumping = false;
            }

            accumulator -= fixedTimeStep;
            std::cout << "Accumulator: " << accumulator << "\n";
        }
        window.clear();

        window.draw(player);
        window.display();
    }
}

#endif //TEST_JUMPING_HPP
