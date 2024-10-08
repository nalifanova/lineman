#include "GameEngine.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

#include "imgui.h"
#include "imgui-SFML.h"

#include "DeltaTime.hpp"
#include "SceneMenu.hpp"

using json = nlohmann::json;

GameEngine::GameEngine(const std::string& path)
{
    init(path);
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene) { m_sceneMap.erase(m_sceneMap.find(m_currentScene)); }

    if (!scene) { return; }

    m_sceneMap[sceneName] = std::move(scene);
    m_currentScene = sceneName;
}

void GameEngine::run()
{
    while (isRunning())
    {
        sf::Time deltaTime = m_deltaClock.restart();
        DeltaTime::set(deltaTime);
        sUserInput();
        ImGui::SFML::Update(m_window, deltaTime);
        update();
        ImGui::SFML::Render(m_window);
        m_window.display(); // draw the new stuff
    }
    ImGui::SFML::Shutdown();
}

void GameEngine::quit()
{
    m_running = false;
    m_window.close();
}

void GameEngine::save(PlayerData player, std::string filename)
{
    json j;
    j["name"] = player.name();
    j["inks"] = player.inks();
    j["level"] = player.level();
    j["life"] = player.life();
    j["time"] = player.time();
    j["drops"] = player.drops();
    j["position"] = {player.pos().x, player.pos().y};
    auto path = "saves/" + filename;
    std::ofstream file(path);
    if (file.is_open())
    {
        file << j.dump(7);
        file.close();
    }
    else
    {
        std::cerr << "Unable to open file `" << path << "` for saving!";
    }
}

PlayerData GameEngine::load(std::string filename)
{
    auto path = "saves/" + filename;
    std::ifstream fin(path);
    if (!fin)
    {
        std::cerr << "Could not load " << path << " file!\n";
        return {};
    }
    while (fin.good())
    {
        json j;
        fin >> j;
        std::string name = j["name"];
        int inks = j["inks"];
        int level = j["level"];
        int life = j["life"];
        int time = j["time"];
        int drops = j["drops"];
        Vec2 position(j["position"][0], j["position"][1]);
        auto player = PlayerData(name, inks, level, life, position);
        player.addTime(time);
        player.updateDrops(drops);
        return player;
    }
    std::cout << "Check it again \n";
    return {};
}

void GameEngine::playSound(const std::string& soundName)
{
    assets().getSound(soundName).play();
}

void GameEngine::stopSound(const std::string& soundName)
{
    assets().getSound(soundName).stop();
}

void GameEngine::loopSound(const std::string& soundName)
{
    sf::Sound& sound = assets().getSound(soundName);
    if (sound.getStatus() == sf::Sound::Stopped) { sound.play(); }
}

Assets& GameEngine::assets()
{
    return m_assets;
}

sf::RenderWindow& GameEngine::window()
{
    return m_window;
}

bool GameEngine::isRunning() const
{
    return m_running && m_window.isOpen();
}

void GameEngine::registerKey(const std::string& keyName, const uint16_t keyValue)
{
    m_supportedKeys[keyName] = keyValue;
}

SupportedKeys& GameEngine::getSupportedKeys()
{
    return m_supportedKeys;
}

void GameEngine::initKeys()
{
    std::ifstream fin("config/supported_keys.ini");
    if (!fin)
    {
        std::cerr << "Could not load config/supported_keys.ini file!\n";
        exit(-1);
    }
    uint16_t keyCode = 0;
    std::string keyName = "None";
    while (fin >> keyName >> keyCode)
    {
        registerKey(keyName, keyCode);
    }
}

// protected
void GameEngine::init(const std::string& path)
{
    m_assets.loadFromFile(path);

    m_window.create(sf::VideoMode(game::kWinWidth, game::kWinHeight), "Ink man"
                  , sf::Style::Titlebar | sf::Style::Close);
    m_window.setFramerateLimit(60);
    m_window.setVerticalSyncEnabled(true);

    if (!ImGui::SFML::Init(m_window))
    {
        std::cerr << "Failed ImGui initialization\n";
    }

    initKeys();
    changeScene("MENU", std::make_shared<SceneMenu>(this));
}

void GameEngine::update()
{
    if (!isRunning() || m_sceneMap.empty()) { return; }

    currentScene()->update();
    // currentScene()->simulate(m_simulationSpeed);
}

void GameEngine::sUserInput()
{
    sf::Event event{};

    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(m_window, event);

        if (event.type == sf::Event::Closed)
        {
            quit();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::X)
            {
                std::cout << "Save screenshot to " << "test.png" << std::endl;
                sf::Texture texture;
                texture.create(m_window.getSize().x, m_window.getSize().y);
                texture.update(m_window);
                if (texture.copyToImage().saveToFile("test.png"))
                {
                    std::cout << "Screenshot saved to " << "test.png" << std::endl;
                }
            }
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
            {
                continue;
            }

            // determine start or end action by whether it was key press or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }

        // this line ignores mouse events if ImGui is the thing clicked
        if (ImGui::GetIO().WantCaptureMouse) { continue; }

        // mouse event below
        auto mousePos = sf::Mouse::getPosition(m_window);
        Vec2 pos = Vec2(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.mouseButton.button)
            {
                case sf::Mouse::Left:
                    currentScene()->doAction(Action("LEFT_CLICK", "START", pos));
                    break;
                case sf::Mouse::Right:
                    currentScene()->doAction(Action("RIGHT_CLICK", "START", pos));
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased)
        {
            switch (event.mouseButton.button)
            {
                case sf::Mouse::Left:
                    currentScene()->doAction(Action("LEFT_CLICK", "END", pos));
                    break;
                case sf::Mouse::Right:
                    currentScene()->doAction(Action("RIGHT_CLICK", "END", pos));
                    break;
                default:
                    break;
            }
        }

        if (event.type == sf::Event::MouseMoved)
        {
            currentScene()->doAction(Action("MOUSE_MOVE"
                                          , Vec2(static_cast<float>(event.mouseMove.x)
                                               , static_cast<float>(event.mouseMove.y))));
        }
    }
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}

std::shared_ptr<Scene> GameEngine::getScene(const std::string& sceneName)
{
    if (m_sceneMap.find(sceneName) == m_sceneMap.end())
    {
        std::cerr << "Warning: Scene does not exists `" << sceneName << "` \n";
        return nullptr;
    }
    return m_sceneMap[sceneName];
}
