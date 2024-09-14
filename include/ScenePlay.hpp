#ifndef SCENEPLAY_HPP
#define SCENEPLAY_HPP

#include "SFML/Graphics.hpp"

#include "Grid.hpp"
#include "GUI.hpp"
#include "Scene.hpp"

class ScenePlay final: public Scene
{
    struct PlayerConfig
    {
        float x, y, cX, cY, speed, health, gravity, jump;
        std::string weapon;
    };

public:
    ScenePlay(GameEngine* gameEngine, std::string levelPath);

    void update() override;

protected:
    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    Entity getPlayer() const;

    void onEnd() override;
    void sDoAction(const Action& action) override;
    void sRender() override;

    void sDrag();
    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCamera() const;
    void sCollision();
    void sGUI();

    void spawnPlayer(bool init = false);
    void spawnEntity(size_t tag);
    void destroyEntity(Entity entity);

    // help functions
    void drawTextures();
    void drawCollisions();

    void collisionEntities(Entity& entity, Entity& tile);
    void entityTileCollision();
    // void playerNpcCollision();
    // void entityItemCollision();
    // void teleportCollision();
    // void roomCollision(std::shared_ptr<Entity>& entity);
    void entityGroundCollision();
    //
    void stateAnimation(std::string& animName, Entity& entity);
    static void facingDirection(CTransform& transf);
    // bool isPositionOccupied(const sf::Vector2f& position);
    //

    PlayerConfig m_playerConfig{};
    std::optional<Grid> m_grid;
    std::optional<GUI> m_gui;
    Vec2 m_mousePos;

    std::string m_levelPath;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;
    bool m_zoom = false;
    bool m_zoomed = false;
    size_t m_countFrame = 0;

    bool m_isDragging = false;
};


#endif //SCENEPLAY_HPP
