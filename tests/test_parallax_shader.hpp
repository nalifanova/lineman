/** Playground to use shaders
 * https://thebookofshaders.com/
 * https://www.shadertoy.com/view/ldsGDn
 * https://gist.github.com/MarioLiebisch/c965e4caf4360ebd6def549d60423740
*/

#ifndef TEST_PARALLAX_SHADER_HPP
#define TEST_PARALLAX_SHADER_HPP

#include <SFML/Graphics.hpp>

int runParallax()
{
    sf::RenderWindow window(sf::VideoMode(200, 100), "Parallax Example",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    sf::Texture texture;
    if (!texture.loadFromFile("assets/images/tiles/intr_door.png"))
        return EXIT_FAILURE;
    texture.setRepeated(true);
    sf::Sprite sprite(texture);
    sprite.setPosition(0, 0);
    sprite.setColor(sf::Color(255, 255, 255, 200));

    sf::Shader parallaxShader;
    parallaxShader.loadFromMemory(
        "uniform float offset;"

        "void main() {"
        "    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;"
        "    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;"
        "    gl_TexCoord[0].x = gl_TexCoord[0].x + offset;" // magic
        "    gl_FrontColor = gl_Color;"
        "}"
        , sf::Shader::Vertex);

    float offset = 0.f;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        window.clear();

        // an arbitrary value as the offset, calculate this based on camera position
        parallaxShader.setUniform("offset", offset += clock.restart().asSeconds() / 10);

        window.clear();
        window.draw(sprite, &parallaxShader);
        window.display();
    }

    return EXIT_SUCCESS;
}

#endif //TEST_PARALLAX_SHADER_HPP
