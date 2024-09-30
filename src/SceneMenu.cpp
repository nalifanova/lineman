#include "SceneMenu.hpp"

#include "SFML/Graphics.hpp"

#include "Action.hpp"
#include "Assets.hpp"
#include "GameEngine.hpp"
#include "SceneMenuControls.hpp"
#include "SceneMenuOptions.hpp"
#include "ScenePlay.hpp"

SceneMenu::SceneMenu(GameEngine* gameEngine) :
    Scene(gameEngine)
{
    init();
}

void SceneMenu::sRender()
{
    // set menu background
    m_game->window().clear(sf::Color::Black);

    // draw title
    m_game->window().draw(m_menuText);

    // draw menu items
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

// protected
void SceneMenu::init()
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
    registerAction(sf::Keyboard::Enter, "RUN");
    registerAction(sf::Keyboard::M, "MUTE");
    registerAction(sf::Keyboard::Q, "QUIT");

    // m_titleMusic = m_game->assets().getSound("TitleTheme");
    // m_titleMusic.play();

    createMenu();
    m_levelPaths.emplace_back("config/level1.txt");
    m_levelPaths.emplace_back("config/level2.txt");
}

void SceneMenu::update()
{
    sRender();
}

void SceneMenu::onEnd()
{
    m_game->changeScene("QUIT", nullptr, true);
    m_game->quit();
}

void SceneMenu::sDoAction(const Action& action)
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
        else if (action.name() == "RUN")
        {
            runMenu();
        }
        else if (action.name() == "MUTE")
        {
            if (m_titleMusic.getStatus() == sf::SoundSource::Playing) { m_titleMusic.stop(); }
            else { m_titleMusic.play(); }
        }
        else if (action.name() == "QUIT")
        {
            onEnd();
        }
    }
}

void SceneMenu::createMenu()
{
    m_title = "Ink Man";
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

    m_menuStrings.emplace_back("New");
    m_menuStrings.emplace_back("Load");
    m_menuStrings.emplace_back("Controls");
    m_menuStrings.emplace_back("Options");
    m_menuStrings.emplace_back("Quit");

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

void SceneMenu::runMenu()
{
    auto playerData = m_game->load(game::fileName);
    if (playerData.level() > 1) { m_active = true; }
    else { m_active = false; }

    if (m_selectedMenuItem == "New")
    {
        m_titleMusic.stop();
        std::cout << "New game is chosen\n";
        m_playerData = PlayerData();
        m_game->save(m_playerData, game::fileName); // TODO: extend and change the name
        m_game->changeScene("LEVEL1", std::make_shared<ScenePlay>(m_game, m_levelPaths[0]));
    }
    else if (m_selectedMenuItem == "Load" && m_active)
    {
        m_titleMusic.stop();
        std::cout << "Load game is chosen\n";
        m_game->changeScene("LEVEL2", std::make_shared<ScenePlay>(m_game, m_levelPaths[1]));
    }
    else if (m_selectedMenuItem == "Controls")
    {
        std::cout << "Controls is chosen\n";
        m_game->changeScene("MENU_CONTROLS", std::make_shared<SceneMenuControls>(m_game));
    }
    else if (m_selectedMenuItem == "Options")
    {
        std::cout << "Options is chosen\n";
        m_game->changeScene("MENU_OPTIONS", std::make_shared<SceneMenuOptions>(m_game));
    }
    else if (m_selectedMenuItem == "Quit")
    {
        std::cout << "Quit is chosen\n";
        onEnd();
    }
}
