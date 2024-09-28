#include "SceneMenuControls.hpp"

#include <format>

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
    m_game->changeScene("MENU", m_game->getScene("MENU"), true);
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
    m_menuText.setPosition(width() / 2.0f, height() / 6.0f);

    auto keyMap = getKeyMap();

    // std::string result = std::format("some text {} text goes on", "smth");
    m_menuStrings.emplace_back(std::format("[{}] - Movement Up", keyMap["UP"]));
    m_menuStrings.emplace_back(std::format("[{}] - Movement Left", keyMap["LEFT"]));
    m_menuStrings.emplace_back(std::format("[{}] - Movement Down", keyMap["DOWN"]));
    m_menuStrings.emplace_back(std::format("[{}] - Movement Right", keyMap["RIGHT"]));
    m_menuStrings.emplace_back("---------------------");
    m_menuStrings.emplace_back(std::format("[{}] - Restore Health", keyMap["KEY1"]));
    m_menuStrings.emplace_back(std::format("[{}] - Activate Shield", keyMap["KEY2"]));
    m_menuStrings.emplace_back(std::format("[{}] - Make Boom", keyMap["KEY3"]));
    m_menuStrings.emplace_back("---------------------");
    m_menuStrings.emplace_back(std::format("[{}] - Attack", keyMap["ATTACK"]));
    m_menuStrings.emplace_back(std::format("[{}] - Interact", keyMap["INTERACT"]));
    m_menuStrings.emplace_back(std::format("[{} + {}] - Transform Ink to Shield",
        keyMap["INTERACT"], keyMap["KEY2"]));
    m_menuStrings.emplace_back(std::format("[{} + {}] - Transform Ink to Boom",
        keyMap["INTERACT"], keyMap["KEY3"]));

    m_menuStrings.emplace_back(std::format("[{}] - Pause", keyMap["PAUSE"]));
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

std::map<std::string, std::string> SceneMenuControls::getKeyMap()
{
    initKeyBinds();
    auto& keys = m_game->getSupportedKeys(); // string int
    std::map<std::string, std::string> keyMap;
    for (auto& [name, keyNumber]: keys)
    {
        keyMap[m_actionMap[keyNumber]] = name;
        // std::cout << "key name: " << name << " action: " << m_actionMap[keyNumber] << "\n";
    }

    return keyMap;
}
