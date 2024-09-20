#ifndef SCENEPLAY_HPP
#define SCENEPLAY_HPP

#include "Collision.hpp"
#include "SFML/Graphics.hpp"

#include "Grid.hpp"
#include "DebugGui.hpp"
#include "PlayerGui.hpp"
#include "Scene.hpp"

class ScenePlay final: public Scene
{
    struct PlayerConfig
    {
        float x, y, cX, cY, speed, health, gravity, jump;
        std::string weapon;
    };

    struct ConsumableConfig
    {
        float health, gravity;
    };

public:
    ScenePlay(GameEngine* gameEngine, std::string levelPath);

    void update() override;

protected:
    void initKeyBinds();
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
    void sCamera(bool reset = false);
    void sCollision();
    void sGUI();

    void doMovement();
    void doPanelAction(CInput& input, Entity& entity);
    void spawnPlayer(bool init = false);
    void spawnEntity(size_t tag, Vec2 pos);
    void spawnInk(Vec2 pos);
    void destroyEntity(Entity& entity);

    void createPanelEntities();

    // help functions
    void drawTextures();
    void drawCollisions();

    //
    void stateAnimation(std::string& animName, Entity& entity);
    static void facingDirection(CTransform& transf);
    // bool isPositionOccupied(const sf::Vector2f& position);
    //
    void setRoomBackground(sf::Texture& tex);
    void spawnSpecialWeapon(Entity& entity, const bool& hard);

    PlayerConfig m_playerConfig{};
    ConsumableConfig m_consConfig{};

    std::optional<Collision> m_collision;
    std::optional<Grid> m_grid;
    std::optional<DebugGui> m_gui;
    std::optional<PlayerGui> m_pGui;

    Vec2 m_mousePos;
    std::string m_levelPath;
    sf::ConvexShape m_background;
    std::vector<Entity> m_entityPanel;

    bool m_drawCollision = false;
    bool m_drawGrid = false;
    bool m_drawTextures = true;
    bool m_zoom = false;
    bool m_zoomed = false;

    bool m_isDragging = false;
};

#endif //SCENEPLAY_HPP
