#include "ScenePlay.hpp"

#include <fstream>

#include "imgui.h"
#include <imgui_internal.h>
#include "imgui-SFML.h"

#include "Components.hpp"
#include "Data.hpp"
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
    auto& font = m_game->assets().getFont("Tech");

    m_grid->text().setCharacterSize(12);
    m_grid->text().setFont(font);

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

    registerAction(sf::Keyboard::Num1, "KEY1");
    registerAction(sf::Keyboard::Num2, "KEY2");
    registerAction(sf::Keyboard::Num3, "KEY3");

    createPanelEntities();
    m_pGui.emplace(m_game->window(), m_entityManager, font);
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
            // std::cout << "DEBUG: Tile is onboarded: " << name << "\n";
            token = "None";
        }
        else if (token == "Dec")
        {
            int rx, ry, tx, ty;
            fin >> name >> rx >> ry >> tx >> ty;
            auto dec = m_entityManager.addEntity(TagName::eDecoration);
            dec.add<CAnimation>(m_game->assets().getAnimation(name), true);
            dec.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            dec.add<CBoundingBox>(dec.get<CAnimation>().animation.getSize());
            dec.add<CDraggable>();
            token = "None";
        }
        else if (token == "Cons")
        {
            int rx, ry, tx, ty;
            fin >> name >> rx >> ry >> tx >> ty >> m_consConfig.health >> m_consConfig.gravity;
            auto cons = m_entityManager.addEntity(TagName::eConsumable);
            cons.add<CAnimation>(m_game->assets().getAnimation(name), true);
            cons.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            cons.get<CTransform>().pos.y += 16;
            cons.add<CBoundingBox>(cons.get<CAnimation>().animation.getSize());
            cons.add<CDraggable>();
            cons.add<CGravity>(m_consConfig.gravity);
            cons.add<CHealth>(m_consConfig.health, m_consConfig.health);
            token = "None";
        }
        else if (token == "Panel")
        {
            int rx, ry, tx, ty;
            fin >> name >> rx >> ry >> tx >> ty;
            auto cons = m_entityManager.addEntity(TagName::ePanel);
            cons.add<CAnimation>(m_game->assets().getAnimation(name), true);
            cons.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            cons.add<CBoundingBox>(cons.get<CAnimation>().animation.getSize());
            token = "None";
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
            token = "None";
        }
        else if (token == "Player")
        {
            // e.g. Player 200 100 60 64 4 10 5 20
            fin >> m_playerConfig.x >> m_playerConfig.y >> m_playerConfig.cX >> m_playerConfig.cY
                >> m_playerConfig.speed >> m_playerConfig.health >> m_playerConfig.gravity
                >> m_playerConfig.jump;
            spawnPlayer(true);
            token = "None";
        }
    }
}

Entity ScenePlay::getPlayer() const
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
    sCamera(true);
    for (auto entity: m_entityManager.getEntities())
    {
        entity.destroy();
    }
    m_game->changeScene("MENU", m_game->getScene("MENU"), true);
}

void ScenePlay::sDoAction(const Action& action)
{
    auto& input = getPlayer().get<CInput>();

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
        else if (action.name() == "INTERACT") { input.interact = true; }
        else if (action.name() == "KEY1") { input.key1 = true; }
        else if (action.name() == "KEY2") { input.key2 = true; }
        else
            if (action.name() == "KEY3") { input.key3 = true; }
    }
    else if (action.type() == "END")
    {
        if (action.name() == "UP") { input.up = false; }
        else if (action.name() == "DOWN") { input.down = false; }
        else if (action.name() == "LEFT") { input.left = false; }
        else if (action.name() == "RIGHT") { input.right = false; }
        else if (action.name() == "INTERACT") { input.interact = false; }
        else if (action.name() == "KEY1") { input.key1 = false; }
        else if (action.name() == "KEY2") { input.key2 = false; }
        else
            if (action.name() == "KEY3") { input.key3 = false; }
    }
}

void ScenePlay::sRender()
{
    m_game->window().clear(sf::Color(sf::Color::Black));
    // background
    auto tex = m_game->assets().getTexture("TexDecBgrnd");
    setRoomBackground(tex);
    m_pGui->setBottomPanel();

    // draw all textures
    drawTextures();
    // draw all Entity collision bounding boxes with a rectangle shape
    drawCollisions();
    // draw the grid so that can easily debug
    Entity player = getPlayer();
    m_grid->drawGrid(m_drawGrid, player);
}

void ScenePlay::sDrag()
{
    //
}

void ScenePlay::doPanelAction(CInput& input, Entity& entity)
{
    auto& ink = m_entityPanel[0];
    auto& shield = m_entityPanel[1];
    auto& boom = m_entityPanel[2];

    if (input.interact)
    {
        if (auto& c = shield.get<CConsumable>();
            (input.key2 && ink.get<CConsumable>().amount >= game::maxAmountToChange)
            && (m_currentFrame - c.frameCreated > c.cooldown))
        {
            ink.get<CConsumable>().amount -= game::maxAmountToChange;
            c.amount += 1;
            c.frameCreated = m_currentFrame;
        }
        else if (auto& c = boom.get<CConsumable>();
            (input.key3 && ink.get<CConsumable>().amount >= game::maxAmountToChange)
            && (m_currentFrame - c.frameCreated > c.cooldown))
        {
            ink.get<CConsumable>().amount -= game::maxAmountToChange;
            c.amount += 1;
            c.frameCreated = m_currentFrame;
        }
    }
    else if (input.key1)
    {
        auto& c = ink.get<CConsumable>();
        auto& h = entity.get<CHealth>();
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            h.current = (h.current + h.max / 2 < h.max) ? h.max / 2 : h.max;
        }
    }
    else if (input.key2)
    {
        auto& c = shield.get<CConsumable>();
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            entity.add<CBuff>(true, false, c.frameCreated, c.cooldown); // shield
        }
    }
    else if (input.key3)
    {
        auto& c = boom.get<CConsumable>();
        const bool hard = c.amount >= game::maxAmountToChange;
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            spawnSpecialWeapon(entity, hard);
        }
    }
}

void ScenePlay::sMovement()
{
    auto player = getPlayer();
    if (!player.isActive()) { return; }
    auto& input = player.get<CInput>();
    auto& state = player.get<CState>();
    auto& transf = player.get<CTransform>();

    doPanelAction(input, player);

    Vec2 playerVelocity(transf.velocity.x, 0);
    Vec2 facing = transf.facing;
    bool isMoving = true;

    if ((input.up && input.down) || (input.left && input.right) || (input.up && input.attack) ||
        (input.down && input.attack) || (input.left && input.attack) || (input.right && input.attack)
    )
    {
        player.get<CAnimation>().paused = true;
        return; // can't hold keys in opposite directions
    }

    if (input.up && !state.inAir)
    {
        playerVelocity.y = -m_playerConfig.jump * 10;
        state.inAir = true;
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
    if (isMoving)
    {
        if (state.inAir)
        {
            state.state = "Air";
            state.changed = true;
        }
        else if (state.state != "Walk")
        {
            state.state = "Walk";
            state.changed = true;
        }
    }
    else
    {
        if (!state.inAir && state.state != "Stand")
        {
            state.state = "Stand";
            state.changed = true;
        }
    }

    transf.velocity = playerVelocity;

    // move entities
    for (auto entity: m_entityManager.getEntities())
    {
        auto& t = entity.get<CTransform>();
        if (entity.has<CGravity>())
        {
            t.velocity.y += entity.get<CGravity>().gravity;
            if (t.velocity.y > m_playerConfig.gravity) { t.velocity.y = m_playerConfig.gravity; }
        }
        t.prevPos = t.pos;
        t.pos += t.velocity * 1.3f; // delta time
    }
}

void ScenePlay::sAI() {}

void ScenePlay::sStatus()
{
    for (auto& entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile) { continue; }

        if (entity.has<CLifespan>())
        {
            const auto& life = entity.get<CLifespan>();
            const auto remainingTime = m_currentFrame - life.frameCreated;
            const auto color = entity.get<CAnimation>().animation.getSprite().getColor();
            const auto alpha = 255 - 128 * remainingTime / life.lifespan;
            entity.get<CAnimation>().animation.getSprite().setColor(
                sf::Color(color.r, color.g, color.b, alpha)
                );
            if (remainingTime > life.lifespan) { destroyEntity(entity); }
        }

        if (entity.has<CInvincibility>())
        {
            auto& invincibility = entity.get<CInvincibility>();
            invincibility.iframes -= 1;
            if (invincibility.iframes < 0) { entity.remove<CInvincibility>(); }
        }

        if (entity.has<CBuff>())
        {
            auto& buff = entity.get<CBuff>();
            if (m_currentFrame - buff.frameCreated > buff.cooldown) { entity.remove<CBuff>(); }
        }
    }
}

void ScenePlay::sAnimation()
{
    auto player = getPlayer();
    if (!player.isActive()) { return; }

    auto& state = player.get<CState>();

    if (state.changed)
    {
        std::string animName = state.state;
        stateAnimation(animName, player);
        state.changed = false;
    }

    // Animate entities
    for (auto& entity: m_entityManager.getEntities())
    {
        if (!entity.has<CAnimation>()) { continue; }

        auto& anim = entity.get<CAnimation>();
        if (anim.animation.hasEnded() && !anim.repeat) { destroyEntity(entity); }
        if (!anim.paused) { anim.animation.update(); }
    }
}

void ScenePlay::sCamera(bool reset)
{
    // set the viewport of the window to be centered on the player if it's far enough right
    auto& pPos = getPlayer().get<CTransform>().pos;
    float windowCenterX = std::max(
        static_cast<float>(m_game->window().getSize().x) / 2.0f,
        pPos.x
        );
    sf::View view = m_game->window().getView();
    view.setCenter(
        windowCenterX,
        static_cast<float>(m_game->window().getSize().y) - view.getCenter().y
        );

    if (m_zoom)
    {
        if (!m_zoomed)
        {
            view.zoom(2.0f);
            m_zoomed = true;
        }
    }
    else
    {
        if (m_zoomed)
        {
            view.setSize(game::kWinWidth, game::kWinHeight);
            m_zoomed = false;
        }
    }

    if (reset)
    {
        view = m_game->window().getDefaultView();
    }
    m_game->window().setView(view);
}

void ScenePlay::sCollision()
{
    entityTileCollision();
    weaponTileCollision();
    // playerNpcCollision();
    entityItemCollision();
    entityGroundCollision();
}

void ScenePlay::sGUI()
{
    m_gui.emplace(m_game->assets(), m_entityManager);
    m_gui->addCallBack([this] { spawnPlayer(); });
    m_gui->showDebugWindow(m_drawGrid, m_drawTextures, m_drawCollision, m_zoom);
}

void ScenePlay::spawnPlayer(bool init)
{
    auto player = m_entityManager.addEntity(TagName::ePlayer);
    player.add<CTransform>(Vec2(m_playerConfig.x, m_playerConfig.y));
    player.add<CAnimation>(m_game->assets().getAnimation("Air"), true);
    player.add<CBoundingBox>(Vec2(m_playerConfig.cX, m_playerConfig.cY), true, false);
    player.add<CHealth>(m_playerConfig.health, m_playerConfig.health);
    player.add<CGravity>(m_playerConfig.gravity);
    player.add<CInput>();
    player.add<CState>("Air");
    if (init) { std::cout << "A player is born with id[" << player.id() << "]\n"; }
    else { std::cout << "A player is respawned, id[" << player.id() << "]\n"; }
}

void ScenePlay::spawnEntity(const size_t tag, Vec2 pos)
{
    if (tag == ePlayer) { spawnPlayer(); }
    if (tag == eTile) { spawnInk(pos); }
}

void ScenePlay::spawnInk(Vec2 pos)
{
    auto cons = m_entityManager.addEntity(eConsumable);
    cons.add<CAnimation>(m_game->assets().getAnimation("ConsInk"), true);
    cons.add<CTransform>(pos);
    cons.add<CBoundingBox>(cons.get<CAnimation>().animation.getSize());
    cons.add<CDraggable>();
    cons.add<CGravity>(m_consConfig.gravity);
    cons.add<CHealth>(m_consConfig.health, m_consConfig.health);
}

void ScenePlay::destroyEntity(Entity& entity)
{
    // std::cout << "DEBUG: Remove " << entity.tag() << " with id[" << entity.id() << "]\n";
    const auto pos = entity.get<CTransform>().pos;

    if (entity.tagId() == ePlayer)
    {
        auto dead = m_entityManager.addEntity(eDecoration);
        dead.add<CAnimation>(m_game->assets().getAnimation("Dead"), true);
        dead.get<CAnimation>().animation.getSprite().setColor(game::LightGray);
        dead.add<CTransform>(pos);
        dead.add<CLifespan>(entity.get<CHealth>().max * 60, m_currentFrame);
    }
    entity.destroy();
    spawnEntity(entity.tagId(), pos);
}

void ScenePlay::moveEntity(Entity& entity)
{
    if (entity.tagId() != eConsumable) { return; }

    // get consumable name and convert to panel name
    auto name = entity.get<CAnimation>().animation.getName();
    size_t pos = name.find(entity.tag());
    if (pos != std::string::npos)
    {
        name.replace(pos, entity.tag().length(), tags[ePanel]);
    }
    entity.destroy();

    for (auto item: m_entityManager.getEntities(ePanel))
    {
        if (item.get<CAnimation>().animation.getName() == name)
        {
            item.get<CConsumable>().amount += 1;
            break;
        }
    }
}

void ScenePlay::createPanelEntities()
{
    std::vector<std::string> panelEntitiesNames{"PanelInk", "PanelShield", "PanelBoom"};
    std::map<std::string, u_int16_t> cooldown{
        {"PanelInk", 0.3 * 60}, {"PanelShield", 5 * 60}, {"PanelBoom", 2 * 60}
    };
    m_entityPanel.reserve(panelEntitiesNames.size());

    for (const auto& name: panelEntitiesNames)
    {
        auto panelEntity = m_entityManager.addEntity(ePanel);
        panelEntity.add<CTransform>();
        auto& anim = m_game->assets().getAnimation(name);
        panelEntity.add<CAnimation>(anim, true);
        panelEntity.add<CBoundingBox>(Vec2(anim.getSize().x, anim.getSize().y), true, false);
        panelEntity.add<CConsumable>(cooldown[name]);
        m_entityPanel.push_back(panelEntity);
    }
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
        std::vector tags = {eTile, eDecoration, eConsumable, eNpc, ePlayer, eWeapon};
        for (const auto& tag: tags)
        {
            for (auto& entity: m_entityManager.getEntities(tag))
            {
                auto& transform = entity.get<CTransform>();
                sf::Color c = sf::Color::White;
                if (entity.has<CInvincibility>()) { c = sf::Color(255, 255, 255, 128); }

                if (entity.has<CBuff>())
                {
                    sf::CircleShape circle(70);
                    circle.setOutlineColor(sf::Color(game::Gray));
                    circle.setOutlineThickness(1);
                    circle.setFillColor(sf::Color::Transparent);
                    circle.setPosition(transform.pos.x - 70, transform.pos.y - 70);
                    m_game->window().draw(circle);
                }

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
                    m_game->window().draw(animation.getSprite());
                }

                if (entity.tagId() != eConsumable && entity.has<CHealth>())
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
                            getPlayer().get<CTransform>().pos,
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
            if (entity.tagId() == eWeapon)
            {
                destroyEntity(tile);
                return;
            }
            // Overlap: defining a direction
            const auto prevOverlap = Physics::getPreviousOverlap(entity, tile);
            auto& entityPos = entity.get<CTransform>().pos;
            auto& tilePos = tile.get<CTransform>().pos;

            // top/bottom collision
            if (prevOverlap.x > 0.0f)
            {
                if (entityPos.y < tilePos.y) // down
                {
                    entityPos.y -= overlap.y;
                    if (entity.has<CState>()) { entity.get<CState>().inAir = false; }
                }
                else // up
                {
                    entityPos.y += overlap.y;
                    // TODO: logic of interaction with somwthing
                }

                // stop moving
                entity.get<CTransform>().velocity.y = 0.0f;
            }

            // side collision
            if (prevOverlap.y > 0.0f) { entityPos.x += entityPos.x < tilePos.x ? -overlap.x : overlap.x; }
        }

        if (entity.has<CInvincibility>()) { return; }
        if (entity.has<CBuff>() && entity.get<CBuff>().shield) { return; }
        if (tile.has<CDamage>())
        {
            entity.get<CHealth>().current -= tile.get<CDamage>().damage;
            entity.add<CInvincibility>(kInvincibility);

            if (entity.get<CHealth>().current <= 0)
            {
                destroyEntity(entity);
                // m_game->playSound("died");
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
    auto player = getPlayer();
    for (auto& tile: m_entityManager.getEntities(eTile))
    {
        collisionEntities(player, tile);

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            collisionEntities(npc, tile);
        }
    }
}

void ScenePlay::weaponTileCollision()
{
    for (auto& weapon: m_entityManager.getEntities(eWeapon))
    {
        for (auto& tile: m_entityManager.getEntities(eTile))
        {
            collisionEntities(weapon, tile);
        }
    }
}

void ScenePlay::entityGroundCollision()
{
    for (auto entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile) { continue; }

        if (entity.get<CTransform>().pos.y > height() - entity.get<CBoundingBox>().halfSize.y)
        {
            destroyEntity(entity);
        }
    }
}

void ScenePlay::playerNpcCollision() {}

void ScenePlay::entityItemCollision()
{
    auto player = getPlayer();
    for (auto& consumable: m_entityManager.getEntities(TagName::eConsumable))
    {
        const auto overlap = Physics::getOverlap(player, consumable);
        if (overlap.x > 0.0f && overlap.y > 0.0f)
        {
            moveEntity(consumable);
            //m_game->playSound("pickupInk");
        }

        for (auto tile: m_entityManager.getEntities(eTile))
        {
            collisionEntities(consumable, tile);
        }
    }
}

void ScenePlay::stateAnimation(std::string& animName, Entity& entity)
{
    facingDirection(entity.get<CTransform>());
    entity.add<CAnimation>(m_game->assets().getAnimation(animName), true);
}

void ScenePlay::facingDirection(CTransform& transf)
{
    if (transf.facing.y == 1.0f) { transf.scale.x = 1.0f; }
    else if (transf.facing.y == -1.0f) { transf.scale.x = 1.0f; }
    else // flipping to left if needed
        if (transf.facing.x != 0.0f) { transf.scale.x = transf.facing.x; }
}

void ScenePlay::setRoomBackground(sf::Texture& tex)
{
    auto points = game::roomsPoints;
    m_background.setPointCount(points.size());
    m_background.setOutlineColor(sf::Color(40, 40, 40));
    m_background.setOutlineThickness(1.0f);
    for (size_t i = 0; i < points.size(); i++)
    {
        m_background.setPoint(i, sf::Vector2f(points[i].x, points[i].y));
    }
    tex.setRepeated(true);
    m_background.setTexture(&tex);
    m_background.setOrigin(static_cast<float>(tex.getSize().x) / 2.0f, static_cast<float>(tex.getSize().y) / 2.0f);
    m_background.setTextureRect(sf::IntRect(
        0, 0, static_cast<int>(width() * 4.0f), static_cast<int>(height() * 1.0f)
        ));
    m_game->window().draw(m_background);
}

void ScenePlay::spawnSpecialWeapon(Entity& entity, const bool& hard)
{
    const size_t vertices = hard ? 12 : 6;
    for (size_t i = 0; i < vertices; i++)
    {
        auto boom = m_entityManager.addEntity(eWeapon);
        const float angle = 360.0f / vertices * i;
        const float radian = angle * 3.1415f / 180;

        Vec2& pos = entity.get<CTransform>().pos;
        Vec2 velocity(cos(radian), sin(radian));
        Vec2 scale = {0.8f, 0.8f};
        velocity *= 5; // speed

        boom.add<CAnimation>(m_game->assets().getAnimation("WeaponInk"), true);
        boom.add<CTransform>(pos, velocity, scale, angle - 90);
        boom.add<CLifespan>(45, m_currentFrame);
    }
}
