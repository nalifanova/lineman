#include "PlayerGui.hpp"

#include "Data.hpp"
#include "Tags.hpp"

PlayerGui::PlayerGui(sf::RenderWindow& window, EntityManager& entityManager, const sf::Font& font
                   , PlayerData& playerData):
    m_window(window), m_entityManager(entityManager), m_font(font), m_playerData(playerData) {};

void PlayerGui::setBottomPanel()
{
    sf::View view = m_window.getView();
    m_panel.setSize(sf::Vector2f(game::kGridSizeX * 6, game::kGridSizeY * 1.5f));
    m_panel.setFillColor(sf::Color::Transparent);
    // m_panel.setOutlineColor(sf::Color(game::Gray));
    m_panel.setOutlineThickness(0);
    m_panel.setPosition(
        view.getCenter().x - game::kGridSizeX * 3,
        static_cast<float>(m_window.getSize().y) - game::kGridSizeY * 2.0f
        );

    auto winPos = m_panel.getPosition(); // 7.10

    setPanelIcons(winPos);
    setPanelIconTitles();
    m_window.draw(m_panel);
}

void PlayerGui::setTopPanel()
{
    sf::View view = m_window.getView();
    m_panel.setSize(sf::Vector2f(game::kGridSizeX * 6, game::kGridSizeY * 3));
    m_panel.setFillColor(sf::Color::Transparent);
    m_panel.setOutlineColor(sf::Color(game::Gray));
    m_panel.setOutlineThickness(0);
    m_panel.setPosition(view.getCenter().x - game::kGridSizeX * 8.0f,
                        static_cast<float>(m_window.getSize().y) - game::kGridSizeY * 24.0f
        );

    auto winPos = m_panel.getPosition(); // 7.10

    setPanelText(winPos);
    setPanelIcons(winPos);
    setPanelIconTitles();
    m_window.draw(m_panel);
}

void PlayerGui::setPanelText(sf::Vector2f winPos)
{
    sf::Text text;
    unsigned int charSize = 18;
    text.setFont(m_font);
    text.setCharacterSize(charSize);
    text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);

    // TIME
    text.setFillColor(sf::Color(game::Silver));
    auto time = m_playerData.time();
    int minutes = static_cast<int>(time) / 60;
    int seconds = static_cast<int>(time) % 60;
    text.setString(
        "Time: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
    text.setPosition(winPos.x - game::kGridSizeX * 1.8f, winPos.y + 10.f);
    m_window.draw(text);

    // DROPS
    text.setString("Drops: " + std::to_string(m_playerData.drops()));
    text.setPosition(text.getPosition().x, text.getPosition().y + 32.f);
    m_window.draw(text);

    // LIFE
    text.setString("Life: " + std::to_string(m_playerData.life()));
    text.setPosition(text.getPosition().x, text.getPosition().y + 32.f);
    m_window.draw(text);
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
            winPos.x + game::kGridSizeX / 2.0f + static_cast<float>(delta) * game::kGridSizeX + 2.0f,
            winPos.y + game::kGridSizeY / 2.0f
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
            animation.getSprite().getPosition().y - game::kGridSizeY / 2.0f + 4
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
    Vec2 size(game::kGridSizeX - 8.0f, 6); // bar outside size
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

void PlayerGui::gameOver(const std::function<void()>& callback) const
{
    sf::View view = m_window.getView();
    sf::RectangleShape board;
    board.setSize(sf::Vector2f(800.f, 600.f));
    board.setOrigin(
        board.getLocalBounds().width / 2.0f,
        board.getLocalBounds().height / 2.0f
        );
    board.setFillColor(game::LightGray);
    board.setOutlineThickness(0);
    board.setPosition(view.getCenter().x, static_cast<float>(m_window.getSize().y) / 2);
    m_window.draw(board);

    auto winPos = board.getPosition();

    sf::Text text;
    text.setFont(m_font);
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::Black);
    text.setOrigin(
        text.getLocalBounds().width / 2.0f,
        text.getLocalBounds().height / 2.0f
        );
    text.setString("Game Over!");
    text.setPosition(
        winPos.x - text.getLocalBounds().width / 2,
        winPos.y - text.getLocalBounds().height / 2
        );

    m_window.draw(text);

    text.setCharacterSize(24);
    text.setString("Press Esc to quit");
    text.setPosition(
        text.getPosition().x + 16.f,
        text.getPosition().y + 250.f
        );
    m_window.draw(text);
    m_window.display();

    sf::Event event{};
    while (m_window.waitEvent(event))
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            callback();
            return;
        }
    }
}
