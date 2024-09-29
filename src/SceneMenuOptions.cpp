#include "SceneMenuOptions.hpp"

#include "SFML/Graphics.hpp"

#include "Action.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "SceneMenu.hpp"

SceneMenuOptions::SceneMenuOptions(GameEngine* gameEngine):
    Scene(gameEngine)
{
    init();
}

void SceneMenuOptions::sRender()
{
    // set menu background
    m_game->window().clear(sf::Color::Black);

    // draw title
    m_game->window().draw(m_menuText);

    for (int i = 0; i < m_menuStrings.size(); i++)
    {
        if (i != m_selectedMenuIndex)
        {
            m_menuItems[i].setFillColor(sf::Color(game::Gray));
        }
        else
        {
            m_menuItems[i].setFillColor(sf::Color::White);
            m_selectedMenuItem = m_menuItems[i].getString();
        }
        m_game->window().draw(m_menuItems[i]);
    }
}

//protected
void SceneMenuOptions::init()
{
    sf::View view = m_game->window().getView();
    view.setCenter(
        static_cast<float>(m_game->window().getSize().x) / 2.0f,
        static_cast<float>(m_game->window().getSize().y) / 2.0f
        );
    m_game->window().setView(view);

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Enter, "ENTER");
    registerAction(sf::Keyboard::Escape, "QUIT");

    createMenu();
}

void SceneMenuOptions::update()
{
    sRender();
}

void SceneMenuOptions::onEnd()
{
    m_game->changeScene("MENU", m_game->getScene("MENU"), true);
}

void SceneMenuOptions::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
        }
        else if (action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        if (action.name() == "ENTER")
        {
            std::cout << "Change controls\n";
        }

        if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void SceneMenuOptions::createMenu()
{
    m_title = "Options";
    int titleSize = 60;

    m_menuText.setString(m_title);
    m_menuText.setFont(m_game->assets().getFont("Tech"));
    m_menuText.setCharacterSize(titleSize);
    m_menuText.setFillColor(sf::Color(game::Gray));
    m_menuText.setOrigin(
        m_menuText.getLocalBounds().width / 2.0f,
        m_menuText.getLocalBounds().height / 2.0f
        );
    m_menuText.setPosition(width() / 2.0f, height() / 6.0f);

    m_menuStrings.emplace_back("Change key bindings");
    m_menuStrings.emplace_back("Reset key bindings");

    int menuSize = 30;
    for (int i = 0; i < m_menuStrings.size(); i++)
    {
        sf::Text text(m_menuStrings[i], m_game->assets().getFont("Tech"), menuSize);
        auto textRect = text.getLocalBounds();
        // Set the reference point to the center of the text
        text.setOrigin(textRect.width / 2.0f, 0);
        // Set the position to the center of the window
        text.setPosition(
            width() / 2.0f,
            m_menuText.getGlobalBounds().top + static_cast<float>(menuSize * i * 2 + 100)
            );
        m_menuItems.push_back(text);
    }
}
