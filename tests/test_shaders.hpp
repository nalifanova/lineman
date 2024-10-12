#ifndef TEST_SHADERS_HPP
#define TEST_SHADERS_HPP

#include <iostream>

#include <SFML/Graphics.hpp>

void test_shaders()
{
    std::vector<std::string> shaderFiles = {
        "shaders/effects.frag",
        "shaders/red_fade.frag",
    };
    size_t currentShaderIndex = 0;
    sf::RenderWindow window(sf::VideoMode(1200, 760), "SFML Shader");
    window.setFramerateLimit(60);

    // So called char
    sf::RectangleShape player(sf::Vector2f(50.0f, 50.0f));
    player.setPosition(100.0f, 500.0f);
    player.setFillColor(sf::Color::Blue);
    float velocityX = 0.0f;

    // from png
    sf::Texture texture;
    if (!texture.loadFromFile("assets/images/tiles/tile_door.png"))
    {
        std::cerr << "Error while loading tile_door.png\n";
        return;
    }
    // Enable the smooth filter. The texture appears smoother so that pixels are less noticable.
    texture.setSmooth(true);

    // Create a sprite and apply to texture
    sf::Sprite sprite;
    sprite.setTexture(texture);
    sf::FloatRect spriteSize = sprite.getGlobalBounds();
    // Set origin in the middle of the sprite
    sprite.setOrigin(spriteSize.width / 2.f, spriteSize.height / 2.f);
    sprite.setPosition(300.f, 200.f);

    // shader part
    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shaders are not available.\n";
        return;
    }

    sf::Shader shader;
    bool drawShader = false;
    sf::Clock clock;

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
                    drawShader = true;
                    currentShaderIndex = 0;
                }
                else if (event.key.code == sf::Keyboard::Right)
                {
                    velocityX = 5.0f;
                    drawShader = true;
                    currentShaderIndex = 1;
                }
            }
            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right)
                {
                    velocityX = 0.f;
                    drawShader = false;
                }
            }
        }
        player.move(velocityX, 0.f);
        if (!shader.loadFromFile(shaderFiles[currentShaderIndex], sf::Shader::Fragment))
        {
            std::cerr << "Error while loading shaders\n";
            return;
        }

        // set shader parameters
        shader.setUniform("time", clock.getElapsedTime().asSeconds());
        if (currentShaderIndex == 0)
            shader.setUniform("resolution", sf::Vector2f(window.getSize()));

        window.clear();

        if (drawShader)
        {
            window.draw(player);
            window.draw(sprite, &shader);
        }
        else
        {
            window.draw(player);
            window.draw(sprite);
        }
        window.display();
    }
}

#endif //TEST_SHADERS_HPP
