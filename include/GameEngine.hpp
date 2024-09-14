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
};

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
public:
    explicit GameEngine(const std::string& path);
    void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);
    std::shared_ptr<Scene> getScene(const std::string& sceneName);

    void run();
    void quit();

    void playSound(const std::string& soundName);
    void stopSound(const std::string& soundName);

    Assets& assets();
    sf::RenderWindow& window();
    bool isRunning() const;

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
    size_t m_simulationSpeed = 1;
    bool m_running = true;
};

#endif //GAME_ENGINE_H
