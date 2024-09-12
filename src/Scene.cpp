#include "Scene.hpp"

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
