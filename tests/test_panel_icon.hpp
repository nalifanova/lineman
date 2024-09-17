/** I know that this is a weird way of testing, I know.
*/

#ifndef TEST_PANEL_ICON
#define TEST_PANEL_ICON

#include <SFML/Graphics.hpp>
#include <vector>
#include "Assets.hpp"
#include "EntityManager.hpp"
#include "Tags.hpp"

void game_panel_icon()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Icon SFML");
    window.setFramerateLimit(60);
    sf::Clock clock; // Таймер

    EntityManager m_entityManager;
    std::vector<Entity> m_entityPanel;
    std::vector<std::string> panelEntitiesNames{"PanelInk", "PanelShield", "PanelBoom"};
    m_entityPanel.reserve(panelEntitiesNames.size());
    auto assets = Assets();
    assets.loadFromFile("config/assets.txt");

    size_t i = 0;
    for (const auto& name: panelEntitiesNames)
    {
        auto panelEntity = m_entityManager.addEntity(ePanel);
        panelEntity.add<CTransform>(Vec2(200 + i, 300));
        auto& pos = panelEntity.get<CTransform>().pos;
        auto& anim = assets.getAnimation(name);
        panelEntity.add<CAnimation>(anim, true);
        panelEntity.get<CAnimation>().animation.getSprite().setPosition(pos.x, pos.y);
        panelEntity.add<CBoundingBox>(Vec2(anim.getSize().x, anim.getSize().y), true, false);
        panelEntity.add<CConsumable>(i + 60); // cooldown is the same for all items 5 sec
        m_entityPanel.push_back(panelEntity);
        i += 80;
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        window.clear();

        for (auto e: m_entityPanel)
        {
            sf::RectangleShape grayOverlay(sf::Vector2f(56, 56));
            auto pos = e.get<CAnimation>().animation.getSprite().getPosition();
            auto c = e.get<CConsumable>();
            grayOverlay.setFillColor(sf::Color(128, 128, 128, 100));
            grayOverlay.setPosition(pos.x - 28, pos.y - 28);

            float elapsedTime = clock.getElapsedTime().asSeconds();

            if (elapsedTime <= c.cooldown / 60)
            {
                float uncoverPercentage = elapsedTime / (c.cooldown / 60);
                float newHeight = 56 * (1.0f - uncoverPercentage);
                grayOverlay.setSize(sf::Vector2f(56, newHeight));
                grayOverlay.setPosition(pos.x - 28, pos.y - 28 + (56 - newHeight));
            } else { grayOverlay.setFillColor(sf::Color(128, 128, 128, 0)); }

            window.draw(e.get<CAnimation>().animation.getSprite());
            window.draw(grayOverlay);
        }
        window.display();
    }
}

#endif // TEST_PANEL_ICON
