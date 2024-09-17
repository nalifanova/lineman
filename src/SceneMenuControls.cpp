#include "SceneMenuControls.hpp"

#include "SFML/Graphics.hpp"

#include "Action.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "SceneMenu.hpp"

SceneMenuControls::SceneMenuControls(GameEngine* gameEngine) :
    Scene(gameEngine)
{
    init();
}

void SceneMenuControls::sRender()
{
    // set menu background
    m_game->window().clear(sf::Color(0, 0, 0, 100));

    // draw title
    m_game->window().draw(m_menuText);
    for (int i = 0; i < m_menuStrings.size(); i++)
    {
        if (i == m_menuStrings.size() - 1) { m_menuItems[i].setFillColor(sf::Color(game::LightGray)); }
        else { m_menuItems[i].setFillColor(sf::Color(game::Silver)); }

        m_game->window().draw(m_menuItems[i]);
    }
}

// protected
void SceneMenuControls::init()
{
    sf::View view = m_game->window().getView();
    view.setCenter(
        static_cast<float>(m_game->window().getSize().x) / 2.0f,
        static_cast<float>(m_game->window().getSize().y) / 2.0f
        );
    m_game->window().setView(view);

    registerAction(sf::Keyboard::Escape, "QUIT");

    controls();
}

void SceneMenuControls::update()
{
    sRender();
}

void SceneMenuControls::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<SceneMenu>(m_game));
}

void SceneMenuControls::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void SceneMenuControls::controls()
{
    m_title = "Controls";
    int titleSize = 60;

    m_menuText.setString(m_title);
    m_menuText.setFont(m_game->assets().getFont("Tech"));
    m_menuText.setCharacterSize(titleSize);
    m_menuText.setFillColor(sf::Color(game::Gray));
    m_menuText.setOrigin(
        m_menuText.getLocalBounds().width / 2.0f,
        m_menuText.getLocalBounds().height / 2.0f
        );
    m_menuText.setPosition(width() / 2.0f, height() / 4.0f);

    m_menuStrings.emplace_back("[W] - Movement Up");
    m_menuStrings.emplace_back("[A] - Movement Left");
    m_menuStrings.emplace_back("[S] - Movement Down");
    m_menuStrings.emplace_back("[D] - Movement Right");
    m_menuStrings.emplace_back("---------------------");
    m_menuStrings.emplace_back("[1] - Restore Health");
    m_menuStrings.emplace_back("[2] - Activate Shield");
    m_menuStrings.emplace_back("[3] - Make Boom");
    m_menuStrings.emplace_back("---------------------");
    m_menuStrings.emplace_back("[E] - Interact");
    m_menuStrings.emplace_back("[E + 2] - Transform Ink to Shield");
    m_menuStrings.emplace_back("[E + 3] - Transform Ink to Boom");

    m_menuStrings.emplace_back("[P] - Pause");
    m_menuStrings.emplace_back("---------------------");
    m_menuStrings.emplace_back("ESC back to Menu");

    int menuSize = 20;
    for (int i = 0; i < m_menuStrings.size(); i++)
    {
        sf::Text text(m_menuStrings[i], m_game->assets().getFont("Tech"), menuSize);
        text.setPosition(
            width() / 2.0f - 150.0f,
            m_menuText.getGlobalBounds().top + static_cast<float>(menuSize * i) * 1.2f + 80.0F
            );
        m_menuItems.push_back(text);
    }
}
