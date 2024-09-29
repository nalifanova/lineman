#include "Scene.hpp"

#include "fstream"

#include "GameEngine.hpp"

Scene::Scene() = default;

Scene::Scene(GameEngine* gameEngine) :
    m_game(gameEngine) {}

Scene::~Scene() = default;

// abstract update, sDoAction, sRender
void Scene::doAction(const Action& action)
{
    sDoAction(action);
}

void Scene::simulate(const size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        update();
    }
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
    m_actionMap[inputKey] = actionName;
}

void Scene::initKeyBinds()
{
    auto& keys = m_game->getSupportedKeys();
    std::ifstream fin("config/gamescene_keybinds.ini");
    if (!fin)
    {
        std::cerr << "Could not load gamescene_keybinds.ini file!\n";
        exit(-1);
    }
    std::string actionName = " ";
    std::string keyName = " ";
    while (fin >> actionName >> keyName)
    {
        registerAction(keys.at(keyName), actionName);
    }
}

KeyMap Scene::getKeyMap()
{
    initKeyBinds();
    auto& keys = m_game->getSupportedKeys(); // string int
    std::map<std::string, std::string> keyMap;
    for (auto& [name, keyNumber]: keys)
    {
        keyMap[m_actionMap[keyNumber]] = name;
    }

    return keyMap;
}

float Scene::width() const
{
    return static_cast<float>(m_game->window().getSize().x);
}

float Scene::height() const
{
    return static_cast<float>(m_game->window().getSize().y);
}

size_t Scene::currentFrame() const
{
    return m_currentFrame;
}

bool Scene::hasEnded() const
{
    return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
    return m_actionMap;
}

// protected
// onEnd is abstract

void Scene::setPaused(bool paused)
{
    m_paused = paused;
}
