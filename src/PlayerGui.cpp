#include "PlayerGui.hpp"

#include "Data.hpp"
#include "Tags.hpp"

PlayerGui::PlayerGui(sf::RenderWindow& window, EntityManager& entityManager, const sf::Font& font):
    m_window(window), m_entityManager(entityManager), m_font(font) {};

void PlayerGui::setBottomPanel()
{
    sf::View view = m_window.getView();
    m_panel.setSize(sf::Vector2f(game::kGridSize * 6, game::kGridSize * 1.5f));
    m_panel.setFillColor(sf::Color::Transparent);
    // m_panel.setOutlineColor(sf::Color(game::Gray));
    m_panel.setOutlineThickness(0);
    m_panel.setPosition(
        view.getCenter().x - game::kGridSize * 3,
        static_cast<float>(m_window.getSize().y) - game::kGridSize * 2.0f
        );

    auto winPos = m_panel.getPosition(); // 7.10

    setPanelIcons(winPos);
    setPanelIconTitles();
    m_window.draw(m_panel);
}

void PlayerGui::setTopPanel()
{
    sf::View view = m_window.getView();
    m_panel.setSize(sf::Vector2f(game::kGridSize * 6, game::kGridSize * 1.5f));
    m_panel.setFillColor(sf::Color::Transparent);
    m_panel.setOutlineColor(sf::Color(game::Gray));
    m_panel.setOutlineThickness(0);
    m_panel.setPosition(view.getCenter().x - game::kGridSize * 9.0f,
        static_cast<float>(m_window.getSize().y) - game::kGridSize * 12.0f
        );

    auto winPos = m_panel.getPosition(); // 7.10

    setPanelIcons(winPos);
    setPanelIconTitles();
    m_window.draw(m_panel);
}

void PlayerGui::setPanelIcons(const sf::Vector2f& winPos)
{
    size_t delta = 0; // delta shift
    for (auto& entity: m_entityManager.getEntities(ePanel))
    {
        auto& transform = entity.get<CTransform>();
        if (!entity.has<CAnimation>()) { continue; }

        auto iconColor = game::Gray;
        auto& animation = entity.get<CAnimation>().animation;

        animation.getSprite().setRotation(transform.angle);
        animation.getSprite().setPosition(
            winPos.x + game::kGridSize / 2.0f + static_cast<float>(delta) * game::kGridSize + 2.0f,
            winPos.y + game::kGridSize / 2.0f
            );
        animation.getSprite().setScale(transform.scale.x, transform.scale.y);
        if (entity.get<CConsumable>().amount > 0) { iconColor = game::Silver; }
        animation.getSprite().setColor(iconColor);

        // Bars to upgrade
        barsUpgrade(entity);
        // showCoolDownProgress(entity);

        m_window.draw(animation.getSprite());
        delta++;
    }
}

void PlayerGui::setPanelIconTitles() const
{
    sf::Text title;
    unsigned int charSize = 12;
    title.setFont(m_font);
    title.setCharacterSize(charSize);


    int keyNumber = 1;
    for (auto& entity: m_entityManager.getEntities(ePanel))
    {
        if (!entity.has<CAnimation>()) { continue; }
        auto iconColor = (entity.get<CConsumable>().amount > 0) ? game::Silver : game::Gray;
        title.setFillColor(sf::Color(iconColor));
        auto& animation = entity.get<CAnimation>().animation;
        auto name = animation.getName();
        // name.replace(name.find(entity.tag()), 5, ""); .. tooltip?
        title.setPosition(
            animation.getSprite().getPosition().x - 26,
            animation.getSprite().getPosition().y - game::kGridSize / 2.0f + 4
            );
        title.setString(std::to_string(entity.get<CConsumable>().amount));
        title.setFillColor(iconColor);
        m_window.draw(title);

        auto& pos = title.getPosition();
        title.setString(std::to_string(keyNumber));
        title.setPosition(pos.x + 20.0f, pos.y + 74.0f);
        m_window.draw(title);
        keyNumber++;
    }
}

void PlayerGui::barsUpgrade(Entity& entity)
{
    sf::Color color = game::DarkGray;

    auto& item = entity.get<CConsumable>();
    auto& pos = entity.get<CAnimation>().animation.getSprite().getPosition();
    Vec2 size(game::kGridSize - 8.0f, 6); // bar outside size
    sf::RectangleShape rect({size.x, size.y}); // bar inside

    auto rect2 = rect;
    rect.setPosition(pos.x - 28, pos.y + 36);
    rect.setFillColor(sf::Color::Black);
    rect.setOutlineColor(sf::Color(game::DarkGray));
    rect.setOutlineThickness(1);
    m_window.draw(rect);

    float ratio = static_cast<float>(item.amount) / static_cast<float>(game::maxAmountToChange);
    size.x *= ratio <= 1.0f ? ratio : 1.0f; // limitation to max amount
    rect.setSize({size.x, size.y});
    if (item.amount >= game::maxAmountToChange) { color = game::LightGray; }
    rect.setFillColor(sf::Color(color));
    rect.setOutlineThickness(0);
    m_window.draw(rect);
}

void PlayerGui::showCoolDownProgress(Entity& entity)
{
    // TODO: finish it
    auto& c = entity.get<CConsumable>();
    if (!c.frameCreated) { return; }

    sf::RectangleShape grayOverlay(sf::Vector2f(56, 56));
    auto pos = entity.get<CAnimation>().animation.getSprite().getPosition();
    grayOverlay.setFillColor(sf::Color(128, 128, 128, 100));
    grayOverlay.setPosition(pos.x - 28, pos.y - 28);

    float elapsedTime = m_clock.getElapsedTime().asSeconds();
    if (elapsedTime <= c.cooldown / 60)
    {
        float uncoverPercentage = elapsedTime / static_cast<float>(c.cooldown / 60);
        float newHeight = 56 * (1.0f - uncoverPercentage);
        grayOverlay.setSize(sf::Vector2f(56, newHeight));
        grayOverlay.setPosition(pos.x - 28, pos.y - 28 + (56 - newHeight));
    }
    else
    {
        grayOverlay.setFillColor(sf::Color(128, 128, 128, 0));
    }

    m_window.draw(grayOverlay);
}
