#ifndef SCENEPLAY_HPP
#define SCENEPLAY_HPP

#include "SFML/Graphics.hpp"

#include "EntityManager.hpp"
#include "Grid.hpp"
#include "Scene.hpp"
#include "Tags.hpp"

class ScenePlay final: public Scene
{
    struct PlayerConfig
    {
        float x, y, cX, cY, speed, health;
        std::string weapon;
    };

public:
    ScenePlay(GameEngine* gameEngine, std::string levelPath);

    void update() override;

protected:
    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    Entity player() const;

    void onEnd() override;
    void sDoAction(const Action& action) override;
    void sRender() override;

    void sDrag();
    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCamera();
    void sCollision();
    void sGUI();

    void spawnPlayer();
    void spawnEntity(size_t tag);

    // help functions
    void drawTextures();
    void drawCollisions();

    void collisionEntities(Entity& entity, Entity& tile);
    void entityTileCollision();
    // void playerNpcCollision();
    // void entityItemCollision();
    // void teleportCollision();
    // void roomCollision(std::shared_ptr<Entity>& entity);
    //
    // void stateAnimation(std::string& animName, const Entity& entity) const;
    // bool isPositionOccupied(const sf::Vector2f& position);
    //
    // void guiShowTable(const std::vector<std::shared_ptr<Entity>>& entities, bool showHeader = true);

    PlayerConfig m_playerConfig{};
    std::optional<Grid> m_grid;
    Vec2 m_mousePos;

    std::string m_levelPath;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;
    bool m_zoom = false;
    bool m_zoomed = false;

    bool m_isDragging = false;
};


#endif //SCENEPLAY_HPP
