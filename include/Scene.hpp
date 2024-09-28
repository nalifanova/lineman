#ifndef SCENE_H
#define SCENE_H

#include <map>

#include "Action.hpp"
#include "EntityManager.hpp"
#include "PlayerData.hpp"

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene
{
public:
    Scene();
    explicit Scene(GameEngine* gameEngine);
    virtual ~Scene();

    virtual void update() = 0;
    virtual void sDoAction(const Action& action) = 0;
    virtual void sRender() = 0;
    virtual void doAction(const Action& action);

    void simulate(size_t frames);
    void registerAction(int inputKey, const std::string& actionName);

    [[nodiscard]] float width() const;
    [[nodiscard]] float height() const;
    [[nodiscard]] size_t currentFrame() const;
    [[nodiscard]] bool hasEnded() const;
    [[nodiscard]] const ActionMap& getActionMap() const;

protected:
    virtual void onEnd() = 0;
    void setPaused(bool paused);

    GameEngine* m_game = nullptr;
    EntityManager m_entityManager;
    ActionMap m_actionMap;

    bool m_paused = false;
    bool m_hasEnded = false;
    size_t m_currentFrame = 0;
    float m_accel = 0.0f;
    float m_timeChecker = 0.f;
    PlayerData m_playerData;
};

#endif //SCENE_H
