#include "ScenePlay.hpp"

#include <fstream>

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui-SFML.h"

#include "Components.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "SceneMenu.hpp"
#include "Tags.hpp"

ScenePlay::ScenePlay(GameEngine* game, std::string levelPath) :
    Scene(game), m_levelPath(std::move(levelPath))
{
    init(m_levelPath);
}

void ScenePlay::update()
{
    m_entityManager.update();

    sDrag();
    if (!m_paused)
    {
        sAI();
        sMovement();
        sStatus();
        sCollision();
        sAnimation();
        sCamera();
        sGUI();
        m_currentFrame++;
    }
    sRender();
}

// protected
void ScenePlay::init(const std::string& levelPath)
{
    m_grid.emplace(m_game->window()); // initialize m_grid
    loadLevel(levelPath);

    m_grid->text().setCharacterSize(12);
    m_grid->text().setFont(m_game->assets().getFont("Tech"));

    registerAction(sf::Keyboard::Escape, "QUIT");
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE"); // toggle drawing (T)extures
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION"); // toggle drawing (C)ollision Box
    registerAction(sf::Keyboard::G, "TOGGLE_GRID"); // toggle drawing (G)rid
    registerAction(sf::Keyboard::Z, "TOGGLE_ZOOM"); // toggle drawing (Z)oom

    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::E, "INTERACT");
}

void ScenePlay::loadLevel(const std::string& fileName)
{
    m_entityManager = EntityManager();

    auto& path = fileName; // should think of a path

    std::ifstream fin(path);
    if (!fin)
    {
        std::cerr << "Could not load " << path << " file!\n";
        exit(-1);
    }

    std::string token; // player, tile, npc
    std::string name;
    while (fin.good())
    {
        fin >> token;
        if (token == "Tile")
        {
            int rx, ry, tx, ty, bm, bv;
            fin >> name >> rx >> ry >> tx >> ty >> bm >> bv;
            auto tile = m_entityManager.addEntity(TagName::eTile);
            tile.add<CAnimation>(m_game->assets().getAnimation(name), true);
            tile.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            tile.add<CBoundingBox>(tile.get<CAnimation>().animation.getSize(), bm, bv);
            tile.add<CDraggable>();
            if (name.find("Saw") != std::string::npos)
            {
                tile.add<CDamage>(4); // saw takes 4 HP
            }
            std::cout << "DEBUG: Tile is onboarded: " << name << "\n";
        }
        else if (token == "NPC")
        {
            std::string aiType;
            int rx, ry, tx, ty, bm, bv, health, damage;
            float speed;
            fin >> name >> rx >> ry >> tx >> ty >> bm >> bv >> health >> damage >> aiType >> speed;

            auto npc = m_entityManager.addEntity(TagName::eNpc);
            npc.add<CAnimation>(m_game->assets().getAnimation(name), true);
            npc.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            npc.add<CBoundingBox>(npc.get<CAnimation>().animation.getSize(), bm, bv);
            npc.add<CDraggable>();
            npc.add<CHealth>(health, health);
            npc.add<CDamage>(damage);

            if (aiType == "Follow")
            {
                npc.add<CFollowPlayer>(npc.get<CTransform>().pos, speed);
            }
            else if (aiType == "Patrol")
            {
                int n, posX, posY;
                std::vector<Vec2> patrolNodes{};
                fin >> n;
                // Could be N positions with coords posX, posY
                for (size_t i = 0; i < n; i++)
                {
                    fin >> posX >> posY;
                    patrolNodes.emplace_back(posX, posY);
                }
                npc.add<CPatrol>(patrolNodes, speed);
            }
        }
        else if (token == "Player")
        {
            fin >> m_playerConfig.x >> m_playerConfig.y >> m_playerConfig.cX >> m_playerConfig.cY
                >> m_playerConfig.speed >> m_playerConfig.health;
        }
    }
    spawnPlayer();
}

Entity ScenePlay::player() const
{
    for (auto entity: m_entityManager.getEntities(TagName::ePlayer))
    {
        return entity;
    }
    return Entity(kMaxEntities);
}

void ScenePlay::onEnd()
{
    m_zoom = false;
    sCamera();
    m_game->changeScene("Menu", std::make_shared<SceneMenu>(m_game));
}

void ScenePlay::sDoAction(const Action& action)
{
    auto& input = player().get<CInput>();

    // Only the setting of the player's input component variables should be set here
    if (action.name() == "MOUSE_MOVE") { m_mousePos = action.pos(); }

    if (action.type() == "START")
    {
        if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { onEnd(); }
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_ZOOM") { m_zoom = !m_zoom; }

        else if (action.name() == "UP") { input.up = true; }
        else if (action.name() == "DOWN") { input.down = true; }
        else if (action.name() == "LEFT") { input.left = true; }
        else if (action.name() == "RIGHT") { input.right = true; }
        else
            if (action.name() == "INTERACT") { input.interact = true; }

    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { input.up = false; }
        else if (action.name() == "DOWN") { input.down = false; }
        else if (action.name() == "LEFT") { input.left = false; }
        else if (action.name() == "RIGHT") { input.right = false; }
        else
            if (action.name() == "INTERACT") { input.interact = false; }
    }
}

void ScenePlay::sRender()
{
    m_game->window().clear(sf::Color(sf::Color::Black));

    // draw all textures
    drawTextures();
    // draw all Entity collision bounding boxes with a rectangle shape
    drawCollisions();
    // draw the grid so that can easily debug
    Entity mPlayer = player();
    m_grid->drawGrid(m_drawGrid, mPlayer);
}

void ScenePlay::sDrag()
{
    //
}

void ScenePlay::sMovement()
{
    auto mPlayer = player();
    if (!mPlayer.isActive()) return;

    const auto& input = mPlayer.get<CInput>();
    auto& state = mPlayer.get<CState>();
    auto& transf = mPlayer.get<CTransform>();

    Vec2 playerVelocity(0, 0);
    Vec2 facing = transf.facing;
    bool isMoving = true;

    if ((input.up && input.down) || (input.left && input.right) || (input.up && input.attack) ||
        (input.down && input.attack) || (input.left && input.attack) || (input.right && input.attack)
    )
    {
        mPlayer.get<CAnimation>().paused = true;
        return; // can't hold keys in opposite directions
    }

    if (input.up)
    {
        playerVelocity.y = -m_playerConfig.speed;
        facing = Vec2(0.0, 1.0f);
    }
    else if (input.down)
    {
        playerVelocity.y = m_playerConfig.speed;
        facing = Vec2(0.0, -1.0f);
    }
    else if (input.right)
    {
        playerVelocity.x = m_playerConfig.speed;
        facing = Vec2(1.0, 0.0f);
    }
    else if (input.left)
    {
        playerVelocity.x = -m_playerConfig.speed;
        facing = Vec2(-1.0, 0.0f);
    }
    else
    {
        playerVelocity = Vec2(0.0, 0.0f);;
        isMoving = false;
    }

    if (transf.facing != facing)
    {
        transf.facing = facing; // update
        state.changed = true;
    }

    // attack / move / stand
    if (input.attack)
    {
        if (state.state != "attak") // prefix is set here, suffix in animation
        {
            state.state = "attak";
            state.changed = true;
            // might spawn smth
        }
    }
    else if (input.interact)
    {
        if (state.state != "interact") // prefix is set here, suffix in animation
        {
            state.state = "interact";
            state.changed = true;
            // might consume something
        }
    }
    else if (isMoving)
    {
        if (state.state != "move")
        {
            state.state = "move";
            state.changed = true;
        }
    }
    else
    {
        if (state.state != "stand")
        {
            state.state = "stand";
            state.changed = true;
        }
    }

    mPlayer.get<CTransform>().velocity = playerVelocity;

    for (auto& el: m_entityManager.getEntities())
    {
        auto& t = el.get<CTransform>();
        t.prevPos = t.pos;
        if (t.velocity.x > m_playerConfig.speed) { t.velocity.x = m_playerConfig.speed; }
        if (t.velocity.y > m_playerConfig.speed) { t.velocity.y = m_playerConfig.speed; }
        t.pos += t.velocity;
    }
}

void ScenePlay::sAI() {}

void ScenePlay::sStatus()
{
    for (auto& entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile) { continue; }

        if (entity.has<CInvincibility>())
        {
            auto& invincibility = entity.get<CInvincibility>();
            invincibility.iframes -= 1;
            if (invincibility.iframes < 0) { entity.remove<CInvincibility>(); }
        }
    }
}

void ScenePlay::sAnimation()
{
    // Implement destruction of entities with non-repeating finished animations ?
    auto mPlayer = player();
    auto& state = mPlayer.get<CState>();

    if (state.changed)
    {
        std::string animName = state.state; // prefix like LinkStand
        // stateAnimation(animName, mPlayer);
        state.changed = false;
    }

    // Animate entities
    for (auto& entity: m_entityManager.getEntities())
    {
        if (!entity.has<CAnimation>()) { continue; }

        auto& anim = entity.get<CAnimation>();
        if (anim.animation.hasEnded() && !anim.repeat) { entity.destroy(); }
        if (!anim.paused) { anim.animation.update(); }
    }
}

void ScenePlay::sCamera()
{
    sf::View view = m_game->window().getView();
    m_game->window().setView(view);
}

void ScenePlay::sCollision()
{
    entityTileCollision();
    // playerNpcCollision();
}

void ScenePlay::sGUI() {}

void ScenePlay::spawnPlayer()
{
    m_playerConfig.x = 200.0f;
    m_playerConfig.y = 100.0f;
    m_playerConfig.cX = 64.0f;
    m_playerConfig.cY = 64.0f;
    m_playerConfig.health = 10;
    m_playerConfig.speed = 5;

    auto player = m_entityManager.addEntity(TagName::ePlayer);
    player.add<CTransform>(Vec2(m_playerConfig.x, m_playerConfig.y));
    player.get<CTransform>().facing = Vec2(0.0, -1.0f); // down

    player.add<CAnimation>(m_game->assets().getAnimation("standIdle"), true);
    player.add<CBoundingBox>(Vec2(m_playerConfig.cX, m_playerConfig.cY), true, false);
    player.add<CHealth>(m_playerConfig.health, m_playerConfig.health);
    player.add<CState>("standIdle");
}

void ScenePlay::spawnEntity(const size_t tag)
{
    if (tag == ePlayer) { spawnPlayer(); }
}

// helpers
void ScenePlay::drawTextures()
{
    sf::RectangleShape tick({1.0f, 6.0f});
    tick.setFillColor(sf::Color::Black);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        // Note: last rendered is on top of previous rendered
        std::vector<TagName> tags = {TagName::eTile, TagName::eNpc, TagName::ePlayer};
        for (const auto& tag: tags)
        {
            for (auto& entity: m_entityManager.getEntities(tag))
            {
                auto& transform = entity.get<CTransform>();
                sf::Color c = sf::Color::White;
                if (entity.has<CInvincibility>()) { c = sf::Color(255, 255, 255, 128); }
                if (entity.has<CAnimation>())
                {
                    auto& animation = entity.get<CAnimation>().animation;
                    animation.getSprite().setRotation(transform.angle);
                    animation.getSprite().setPosition(
                        transform.pos.x, transform.pos.y
                        );
                    animation.getSprite().setScale(
                        transform.scale.x, transform.scale.y
                        );
                    animation.getSprite().setColor(c);
                    m_game->window().draw(animation.getSprite());
                }

                if (entity.has<CHealth>())
                {
                    auto& h = entity.get<CHealth>();
                    Vec2 size(64, 6);
                    sf::RectangleShape rect({size.x, size.y});
                    rect.setPosition(
                        transform.pos.x - 32,
                        transform.pos.y - 48
                        );
                    rect.setFillColor(sf::Color::Black);
                    rect.setOutlineColor(sf::Color(game::DarkGray));
                    rect.setOutlineThickness(1);
                    m_game->window().draw(rect);

                    float ratio = (float)(h.current) / h.max;
                    size.x *= ratio;
                    rect.setSize({size.x, size.y});
                    rect.setFillColor(sf::Color(game::LightGray));
                    rect.setOutlineThickness(0);
                    m_game->window().draw(rect);

                    for (int i = 0; i < h.max; i++)
                    {
                        tick.setPosition(
                            rect.getPosition() + sf::Vector2f(i * 64 * 1.0 / h.max, 0)
                            );
                        m_game->window().draw(tick);
                    }
                }
            }
        }
    }
}

void ScenePlay::drawCollisions()
{
    if (m_drawCollision)
    {
        // draw bounding box
        sf::CircleShape dot(4);
        for (auto entity: m_entityManager.getEntities())
        {
            if (entity.has<CBoundingBox>())
            {
                auto& box = entity.get<CBoundingBox>();
                auto& transform = entity.get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                // rect.setPosition(box.center.x, box.center.y);
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));

                if (box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color(game::Gray));
                }
                if (box.blockMove && !box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Blue);
                }
                if (!box.blockMove && box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::Red);
                }
                if (!box.blockMove && !box.blockVision)
                {
                    rect.setOutlineColor(sf::Color::White);
                }
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);

                // draw line between player and npc
                if (entity.tag() == "NPC")
                {
                    auto& ePos = entity.get<CTransform>().pos;
                    auto view = m_game->window().getView().getCenter();
                    if (ePos.x >= view.x - (float)width() / 2.0 &&
                        ePos.x <= view.x + (float)width() / 2.0 &&
                        ePos.y >= view.y - (float)height() / 2.0 &&
                        ePos.y <= view.y + (float)height() / 2.0
                    )
                    {
                        m_grid->drawLine(
                            player().get<CTransform>().pos,
                            entity.get<CTransform>().pos
                            );
                    }
                }
            }

            // draw patrol points
            if (entity.has<CFollowPlayer>())
            {
                auto& h = entity.get<CFollowPlayer>().home;
                dot.setPosition(h.x, h.y);
                m_game->window().draw(dot);
            }
            if (entity.has<CPatrol>())
            {
                for (auto p: entity.get<CPatrol>().positions)
                {
                    Vec2 r = m_grid->getRoomXY(entity.get<CTransform>().pos);
                    Vec2 pos = m_grid->getPosition(r.x, r.y, p.x, p.y);
                    dot.setPosition(pos.x, pos.y);
                    m_game->window().draw(dot);
                }
            }
        }
    }
}

void ScenePlay::collisionEntities(Entity& entity, Entity& tile)
{
    const auto overlap = Physics::getOverlap(entity, tile);
    if (overlap.x > 0.0f && overlap.y > 0.0f)
    {
        if (tile.get<CBoundingBox>().blockMove)
        {
            // Overlap: defining a direction
            const auto prevOverlap = Physics::getPreviousOverlap(entity, tile);
            auto& entityPos = entity.get<CTransform>().pos;
            auto& tilePos = tile.get<CTransform>().pos;

            // top/bottom collision
            if (prevOverlap.x > 0.0f) { entityPos.y += entityPos.y < tilePos.y ? -overlap.y : overlap.y; }

            // side collision
            if (prevOverlap.y > 0.0f) { entityPos.x += entityPos.x < tilePos.x ? -overlap.x : overlap.x; }
        }

        if (entity.has<CInvincibility>()) { return; }
        if (tile.has<CDamage>())
        {
            entity.get<CHealth>().current -= tile.get<CDamage>().damage;
            // entity.get<CTransform>().velocity.x -= 1;
            entity.add<CInvincibility>(kInvincibility);
            // Debug msg
            std::cout << "Damage from " << tile.get<CAnimation>().animation.getName() << " is "
                << tile.get<CDamage>().damage << "\n";

            if (entity.get<CHealth>().current <= 0)
            {
                entity.destroy();
                // m_game->playSound("died");
                spawnEntity(entity.tagId());
            }
            else
            {
                // m_game->playSound("hit");
            }
        }
    }
}

void ScenePlay::entityTileCollision()
{
    auto mPlayer = player();
    for (auto& tile: m_entityManager.getEntities(TagName::eTile))
    {
        collisionEntities(mPlayer, tile);

        for (auto& npc: m_entityManager.getEntities(TagName::eNpc))
        {
            collisionEntities(npc, tile);
        }
    }
}
