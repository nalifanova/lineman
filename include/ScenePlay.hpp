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
        int roomX = 0;
        int roomY = 0;
        int tileX = 1;
        int tileY = 1;
    };

    struct ConsumableConfig
    {
        float health = 1.0f;
        float gravity = game::gravity * 1.5f;
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
    void sCamera(bool reset = false);
    void sCollision();
    void sGUI();
    void backToMenu();

    void doMovement();
    void doPanelAction(Entity& entity);
    void spawnPlayer(bool init = false);
    void spawnEntity(size_t tag, size_t spawnTag, Vec2 pos);
    void spawnEntity(size_t tag, size_t spawnTag, int rx, int ry, int tx, int ty);
    void spawnInk(Vec2 pos);
    void spawnInteractable(Vec2 pos);
    void spawnSpecialWeapon(Entity& entity, const bool& hard);
    void spawnWeaponDrop(Entity& entity);
    void destroyEntity(Entity& entity);

    void createPanelEntities();

    // help functions
    void drawTextures();
    void drawCollisions();

    //
    void stateAnimation(std::string& animName, Entity& entity);
    static void facingDirection(CTransform& transf, std::string& animName);
    // bool isPositionOccupied(const sf::Vector2f& position);
    //
    void setRoomBackground(sf::Texture& tex);
    void updateScoreData();

    Entity& ink();
    Entity& shield();
    Entity& boom();

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
