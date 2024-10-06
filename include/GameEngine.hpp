#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <map>
#include <SFML/Graphics.hpp>

#include "Assets.hpp"
#include "Scene.hpp"

namespace game
{
    const sf::Color DarkGray(50, 50, 50);
    const sf::Color Gray(100, 100, 100);
    const sf::Color LightGray(150, 150, 150);
    const sf::Color Silver(200, 200, 200);

    constexpr unsigned int kWinWidth = 1280;
    constexpr unsigned int kWinHeight = 768;
    constexpr int kGridSizeX = 64;
    constexpr int kGridSizeY = 32;
};

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;
typedef std::map<std::string, uint16_t> SupportedKeys;

class GameEngine
{
public:
    explicit GameEngine(const std::string& path);
    void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
    std::shared_ptr<Scene> getScene(const std::string& sceneName);

    void run();
    void quit();
    void save(PlayerData player, std::string filename);
    PlayerData load(std::string filename);

    void playSound(const std::string& soundName);
    void stopSound(const std::string& soundName);
    void loopSound(const std::string& soundName);

    Assets& assets();
    sf::RenderWindow& window();
    bool isRunning() const;

    void registerKey(const std::string& keyName, uint16_t keyValue);
    SupportedKeys& getSupportedKeys();
    void initKeys();

protected:
    void init(const std::string& path);
    void update();

    void sUserInput();
    std::shared_ptr<Scene> currentScene();

    Assets m_assets;
    SceneMap m_sceneMap;
    sf::Clock m_deltaClock;
    sf::RenderWindow m_window;

    std::string m_currentScene;
    SupportedKeys m_supportedKeys;
    size_t m_simulationSpeed = 1;
    bool m_running = true;
};

#endif //GAME_ENGINE_H
