#include "ScenePlay.hpp"

#include <fstream>

#include <imgui_internal.h>
#include "imgui-SFML.h"

#include "Components.hpp"
#include "Data.hpp"
#include "DeltaTime.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "PlayerMovement.hpp"
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
    float deltaTime = DeltaTime::get().asSeconds();
    m_accel += deltaTime;

    sDrag();
    if (!m_paused)
    {
        m_effectManager.update(deltaTime);
        sAI();
        sMovement();
        sStatus();
        sCollision();
        sAnimation();
        sCamera();
        m_currentFrame++;
        if (m_timeChecker += deltaTime; m_timeChecker >= 1.f)
        {
            m_playerData.addTime();
            m_timeChecker -= 1.f;
        }
    }
    sGUI();
    sRender();
}

// protected
void ScenePlay::init(const std::string& levelPath)
{
    m_grid.emplace(m_game->window()); // initialize m_grid
    loadLevel(levelPath);
    m_playerData = m_game->load(game::fileName);
    if (m_playerData.life() <= 0) { m_playerData = PlayerData(); }
    auto& font = m_game->assets().getFont("Tech");

    m_grid->text().setCharacterSize(12);
    m_grid->text().setFont(font);

    initKeyBinds();
    createPanelEntities();

    m_pGui.emplace(m_game->window(), m_entityManager, font, m_playerData);
    m_collision.emplace(m_entityManager, m_currentFrame, m_game->assets());

    m_game->playSound("MusicLoop");
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
            tile.add<CBoundingBox>(tile.get<CAnimation>().animation.getSize() - 4.0f, bm, bv);
            tile.add<CDraggable>();
            if (name.find("Saw") != std::string::npos)
            {
                tile.add<CDamage>(4); // saw takes 4 HP
            }
            else if (name.find("Ladder") != std::string::npos)
            {
                tile.add<CClimbable>(); // can be climbed
            }
            else if (name.find("Platform") != std::string::npos)
            {
                // isM speed N X1 Y1 X2 Y2 ... XN YN
                int n, posX, posY;
                float speed;
                bool isMoving;
                std::vector<Vec2> platformNodes{};
                fin >> isMoving >> speed >> n;
                // Could be N positions with coords posX, posY
                for (size_t i = 0; i < n; i++)
                {
                    fin >> posX >> posY;
                    // std::cout << "Platform node: (" << posX << ", " << posY << ")\n";
                    platformNodes.emplace_back(posX, posY);
                }
                tile.add<CMovable>(platformNodes, speed, isMoving);
            }
            token = "None";
        }
        else if (token == "Intr")
        {
            int rx, ry, tx, ty, bm, bv;
            fin >> name >> rx >> ry >> tx >> ty >> bm >> bv;
            auto intr = m_entityManager.addEntity(TagName::eInteractable);
            intr.add<CAnimation>(m_game->assets().getAnimation(name), true);
            intr.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            intr.add<CBoundingBox>(intr.get<CAnimation>().animation.getSize() - 4.0f, bm, bv);
            intr.add<CInteractableBox>(intr.get<CAnimation>().animation.getSize());
            intr.add<CDraggable>();
            if (name.find("Door") != std::string::npos)
            {
                int keyType, actionType;
                bool open, locked;
                fin >> keyType >> open >> locked >> actionType;
                intr.add<CLockable>(open, locked, keyType, actionType);
            }
            else if (name.find("Lever") != std::string::npos)
            {
                intr.add<Triggerable>();
            }

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
            int rx, ry, tx, ty, surprise;
            fin >> name >> rx >> ry >> tx >> ty >> m_consConfig.gravity >> surprise;
            auto cons = m_entityManager.addEntity(TagName::eConsumable);
            cons.add<CAnimation>(m_game->assets().getAnimation(name), true);
            cons.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            cons.get<CTransform>().pos.y += 16;
            cons.add<CBoundingBox>(cons.get<CAnimation>().animation.getSize());
            cons.add<CDraggable>();
            if (!name.ends_with("Air"))
            {
                cons.add<CGravity>(m_consConfig.gravity);
            }
            cons.add<CHealth>(m_consConfig.health, m_consConfig.health);
            if (surprise)
            {
                int sRx, sRy, sTx, sTy, tagId;
                fin >> sRx >> sRy >> sTx >> sTy >> tagId;
                cons.add<CSurprise>(sRx, sRy, sTx, sTy, tagId);
            }
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
        else if (token == "Npc")
        {
            std::string aiType;
            int rx, ry, tx, ty, bm, bv, health, damage;
            float speed = 0.0f;
            fin >> name >> rx >> ry >> tx >> ty >> bm >> bv >> health >> damage >> aiType;

            auto npc = m_entityManager.addEntity(TagName::eNpc);
            npc.add<CAnimation>(m_game->assets().getAnimation(name), true);
            npc.add<CTransform>(m_grid->getPosition(rx, ry, tx, ty));
            Vec2 animSize = npc.get<CAnimation>().animation.getSize();
            if (npc.get<CAnimation>().animation.getName().ends_with("Eraser"))
            {
                animSize = Vec2(animSize.x - 20, animSize.y);
            }
            npc.add<CBoundingBox>(animSize, bm, bv);
            npc.add<CDraggable>();
            npc.add<CHealth>(health, health);
            npc.add<CDamage>(damage);
            npc.add<CGravity>(200); // By default

            // Npc stays still
            if (aiType != "Guard") { fin >> speed; }

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
                npc.add<CMovable>(patrolNodes, speed);
            }
            token = "None";
        }
        else if (token == "Player")
        {
            fin >> m_playerConfig.x >> m_playerConfig.y >> m_playerConfig.cX >> m_playerConfig.cY
                >> m_playerConfig.health >> m_playerConfig.speed >> m_playerConfig.gravity
                >> m_playerConfig.jump;
            spawnPlayer(true);
            token = "None";
        }
    }
}

Entity ScenePlay::getPlayer() const
{
    for (auto& entity: m_entityManager.getEntities(TagName::ePlayer))
    {
        return entity;
    }
    return Entity(kMaxEntities);
}

void ScenePlay::onEnd()
{
    // Save data of a player
    m_playerData.setInks(ink().get<CConsumable>().amount);
    m_game->save(m_playerData, game::fileName); // think how to make filename
    m_zoom = false;
    sCamera(true);
    m_game->stopSound("MusicLoop");
    for (auto& entity: m_entityManager.getEntities())
    {
        entity.destroy();
    }
}

void ScenePlay::backToMenu()
{
    onEnd();
    m_game->changeScene("MENU", m_game->getScene("MENU"), true);
}

void ScenePlay::sDoAction(const Action& action)
{
    auto player = getPlayer();
    auto& input = player.get<CInput>();

    // Only the setting of the player's input component variables should be set here
    if (action.name() == "MOUSE_MOVE") { m_mousePos = action.pos(); }

    if (action.type() == "START")
    {
        if (action.name() == "PAUSE") { setPaused(!m_paused); }
        else if (action.name() == "QUIT") { backToMenu(); }
        else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "TOGGLE_ZOOM") { m_zoom = !m_zoom; }

        else if (action.name() == "UP") { input.up = true; }
        else if (action.name() == "DOWN") { input.down = true; }
        else if (action.name() == "LEFT") { input.left = true; }
        else if (action.name() == "RIGHT") { input.right = true; }
        else if (action.name() == "INTERACT") { input.interact = true; }
        else if (action.name() == "ATTACK") { input.attack = true; }
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
        else if (action.name() == "ATTACK")
        {
            input.attack = false;
            player.get<CState>().canAttack = true;
        }
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
    sf::Texture tex = m_game->assets().getTexture("TexDecBgrnd");
    setRoomBackground(tex);
    m_pGui->setTopPanel();
    Entity player = getPlayer();
    doPanelAction(player);

    m_effectManager.render(m_game->window());

    // draw all textures
    drawTextures();
    // draw all Entity collision bounding boxes with a rectangle shape
    drawCollisions();

    // draw the grid so that can easily debug
    m_grid->drawGrid(m_drawGrid, player);
}

void ScenePlay::sDrag()
{
    //
}

void ScenePlay::doPanelAction(Entity& entity)
{
    auto& input = entity.get<CInput>();
    size_t timeToInteract = 0.3 * 60;

    if (input.interact)
    {
        if (auto& c = shield().get<CConsumable>();
            (input.key2 && ink().get<CConsumable>().amount >= game::maxAmountToChange)
            && (m_currentFrame - c.frameCreated > timeToInteract))
        {
            ink().get<CConsumable>().amount -= game::maxAmountToChange;
            c.amount += 1;
            c.frameCreated = m_currentFrame;
        }
        else if (auto& c = boom().get<CConsumable>();
            (input.key3 && ink().get<CConsumable>().amount >= game::maxAmountToChange)
            && (m_currentFrame - c.frameCreated > timeToInteract))
        {
            ink().get<CConsumable>().amount -= game::maxAmountToChange;
            c.amount += 1;
            c.frameCreated = m_currentFrame;
        }
        updateScoreData();
    }
    else if (input.key1)
    {
        auto& c = ink().get<CConsumable>();
        auto& h = entity.get<CHealth>();
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            h.current = (h.current + h.max / 2 < h.max) ? h.max / 2 : h.max;
            updateScoreData();
        }
    }
    else if (input.key2)
    {
        auto& c = shield().get<CConsumable>();
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            entity.add<CBuff>(true, false, c.frameCreated, c.cooldown); // shield
        }
    }
    else if (input.key3)
    {
        auto& c = boom().get<CConsumable>();
        const bool hard = c.amount >= game::maxAmountToChange;
        if (c.amount > 0 && m_currentFrame - c.frameCreated >= c.cooldown)
        {
            c.frameCreated = m_currentFrame;
            c.amount--;
            spawnSpecialWeapon(entity, hard);
        }
    }
}

void ScenePlay::doMovement()
{
    auto dt = DeltaTime::get().asSeconds();
    if (dt > 0.0167) { dt = 0.0167f; }
    dt *= 10;

    // move entities
    for (auto& entity: m_entityManager.getEntities())
    {
        auto& t = entity.get<CTransform>();

        if (!(entity.has<CState>() && entity.get<CState>().climbing))
        {
            // No gravity when character is climing
            if (entity.has<CGravity>())
            {
                t.velocity.y += entity.get<CGravity>().gravity * dt;
                // if (t.velocity.y > m_playerConfig.gravity) { t.velocity.y = m_playerConfig.gravity; }
            }
        }
        t.prevPos = t.pos;
        t.pos += t.velocity * dt; // delta time
    }
}

void ScenePlay::sMovement()
{
    auto player = getPlayer();
    if (!player.isActive()) { return; }

    auto pm = PlayerMovement(player, m_playerConfig.speed, m_playerConfig.jump);

    player.get<CTransform>().velocity = pm.getVelocityMove(m_accel);
    pm.changeState(m_effectManager, m_currentFrame);

    if (!m_collision->isClimbing(player))
    {
        player.get<CState>().climbing = false;
    }

    pm.runInteract();
    if (player.get<CState>().canAttack && m_playerData.drops() > 0)
    {
        spawnWeaponDrop(player);
        player.get<CState>().canAttack = false;
        m_playerData.updateDrops(-1);
        int ratio = 2;
        if (m_playerData.drops() % ratio > 0) { ink().get<CConsumable>().amount -= 1; }
    }

    doMovement();
}

void ScenePlay::sAI()
{
    for (auto& npc: m_entityManager.getEntities(eNpc))
    {
        if (npc.has<CMovable>()) // Patrol behavior
        {
            aiMoveByNodes(npc, true);
        }
    }

    for (auto& platform: m_entityManager.getEntities(eTile))
    {
        if (platform.has<CMovable>() && platform.get<CMovable>().isMoving)
        {
            aiMoveByNodes(platform);
        }
    }
}

void ScenePlay::sStatus()
{
    for (auto& entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == ePlayer && entity.get<CState>().interAction == game::interActions[eExit])
        {
            // TODO: Hardcoded!
            m_playerData.setLevel(2); // TODO: hardcoded;
            onEnd();
            m_game->changeScene("LEVEL2",
                                std::make_shared<ScenePlay>(m_game, "config/level2.txt"));
        }

        if (entity.has<CSurprise>() && entity.get<CSurprise>().isActivated)
        {
            auto& s = entity.get<CSurprise>();
            spawnEntity(entity.tagId(), s.tagId, s.roomX, s.roomY, s.tileX, s.tileY);
        }

        if (entity.has<CLockable>())
        {
            auto& l = entity.get<CLockable>();
            auto& anim = entity.get<CAnimation>().animation;
            if (l.isOpen && !anim.getName().ends_with("Open"))
            {
                anim = m_game->assets().getAnimation(anim.getName() + "Open");
            }
        }

        if (entity.has<CLifespan>())
        {
            const auto& life = entity.get<CLifespan>();
            const auto remainingTime = m_currentFrame - life.frameCreated;
            const auto& color = entity.get<CAnimation>().animation.getSprite().getColor();
            const auto alpha = 255 - 128 * remainingTime / life.lifespan;
            entity.get<CAnimation>().animation.getSprite().setColor(
                sf::Color(color.r, color.g, color.b, alpha)
                );
            if (remainingTime > life.lifespan) { destroyEntity(entity); }
        }

        if (entity.has<CInvincibility>())
        {
            auto& inv = entity.get<CInvincibility>();
            inv.iframes -= 1;
            if (inv.iframes <= 0)
            {
                entity.remove<CInvincibility>();
                entity.get<CState>().changed = true;
            }
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
    auto& pPos = getPlayer().get<CTransform>().pos;
    float levelWidth = 3 * width(); // width of all rooms
    float windowCenterX = std::max(width() / 2.0f, pPos.x);
    float maxCameraX = levelWidth - width() / 2.0f;
    windowCenterX = std::min(windowCenterX, maxCameraX);

    sf::View view = m_game->window().getView();

    view.setCenter(windowCenterX, height() - view.getCenter().y);

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
    auto player = getPlayer();
    m_collision->entityTileCollision(player);
    m_collision->entityInteractableCollision(player);
    m_collision->checkInteraction(player);
    m_collision->weaponEntityCollision();
    m_collision->playerNpcCollision(player);
    m_collision->entityItemCollision(player);
    m_collision->entityRoomCollision(3 * width(), height());
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
    if (init)
    {
        player.add<CTransform>(Vec2(m_playerConfig.x, m_playerConfig.y));
    }
    else
    {
        player.add<CTransform>(m_grid->getPosition(
            m_playerConfig.roomX, m_playerConfig.roomY, m_playerConfig.tileX, m_playerConfig.tileY
            ));
    }

    player.add<CAnimation>(m_game->assets().getAnimation("Air"), true);
    player.add<CBoundingBox>(Vec2(m_playerConfig.cX, m_playerConfig.cY), true, false);
    player.add<CHealth>(m_playerConfig.health, m_playerConfig.health);
    player.add<CGravity>(m_playerConfig.gravity);
    player.add<CInput>();
    player.add<CState>("Air");
    if (init) { std::cout << "A player is born with id[" << player.id() << "]\n"; }
    else { std::cout << "A player is respawned, id[" << player.id() << "]\n"; }
}

void ScenePlay::spawnEntity(const size_t tag, size_t spawnTag, Vec2 pos)
{
    if (tag == ePlayer)
    {
        if (m_playerData.life() > 0) { spawnPlayer(); }
        else
        {
            m_pGui->gameOver([this] { backToMenu(); });
        }
    }
    else if (tag == eTile || tag == eNpc)
    {
        if (spawnTag == eConsumable) { spawnInk(pos); } // Note: condition when tiles are ruined - ink appears
    }
    else if (tag == eConsumable)
    {
        if (spawnTag == eInteractable) { spawnInteractable(pos); }
        // if (spawnTag == eConsumable) { spawnInk(pos); } // TODO: dangerous! Check it out
    }
}

void ScenePlay::spawnEntity(const size_t tag, const size_t spawnTag, int rx, int ry, int tx, int ty)
{
    auto pos = m_grid->getPosition(rx, ry, tx, ty);
    std::cout << "Room (" << rx << ", " << ry << "), tile (" << tx << ", " << ty << ")\n";
    std::cout << "Position for " << tags.at(spawnTag) << " is " << pos << "\n";
    spawnEntity(tag, spawnTag, m_grid->getPosition(rx, ry, tx, ty));
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

void ScenePlay::spawnInteractable(Vec2 pos)
{
    // TODO: make random tile!!! :D TileBrick vs TileDoor
    for (size_t i = 0; i < 4; i++)
    {
        pos.y -= 32.0f * static_cast<float>(i);
        auto intr = m_entityManager.addEntity(eInteractable);
        intr.add<CAnimation>(m_game->assets().getAnimation("TileBrick"), true);
        intr.add<CTransform>(pos);
        intr.add<CBoundingBox>(intr.get<CAnimation>().animation.getSize() - 4, true, true);
        intr.add<CDraggable>();
        intr.add<CGravity>(game::gravity * 0.1);
    }
}

void ScenePlay::spawnSpecialWeapon(Entity& entity, const bool& hard)
{
    const size_t vertices = hard ? 12 : 6;
    for (size_t i = 0; i < vertices; i++)
    {
        auto boom = m_entityManager.addEntity(eWeapon);
        const float angle = 360.0f / static_cast<float>(vertices) * static_cast<float>(i);
        const float radian = angle * 3.1415f / 180;

        Vec2& pos = entity.get<CTransform>().pos;
        Vec2 velocity(cos(radian), sin(radian));
        Vec2 scale = {0.8f, 0.8f};
        velocity *= game::weaponSpeed; // speed

        boom.add<CAnimation>(m_game->assets().getAnimation("BoomInk"), true);
        boom.add<CTransform>(pos, velocity, scale, angle - 90);
        boom.add<CLifespan>(45, m_currentFrame);
    }
}

void ScenePlay::spawnWeaponDrop(Entity& entity)
{
    auto& transf = entity.get<CTransform>();

    auto drop = m_entityManager.addEntity(eWeapon);
    drop.add<CAnimation>(m_game->assets().getAnimation("DropInk"), true);
    float sign = transf.scale.x > 0 ? 1.0f : -1.0f; // attack to left or right only
    drop.add<CTransform>(transf.pos + Vec2(32.0f, 0.0f) * sign);
    drop.get<CTransform>().velocity.x = game::weaponSpeed * sign;
    drop.add<CBoundingBox>(drop.get<CAnimation>().animation.getSize() / 2.0f);
    drop.add<CDamage>(1);
    drop.add<CLifespan>(2 * 60, m_currentFrame);
    m_game->playSound("ShootWeapon");
}

void ScenePlay::destroyEntity(Entity& entity)
{
    // std::cout << "DEBUG: Remove " << entity.tag() << " with id[" << entity.id() << "]\n";
    const auto pos = entity.get<CTransform>().pos;
    auto tagId = entity.tagId();
    auto spawnableId = entity.tagId();

    if (tagId == ePlayer)
    {
        auto dead = m_entityManager.addEntity(eDecoration);
        dead.add<CAnimation>(m_game->assets().getAnimation("Dead"), true);
        dead.get<CAnimation>().animation.getSprite().setColor(game::LightGray);
        dead.add<CTransform>(pos);
        m_playerData.updateLife(-1);
        m_game->playSound("PlayerDies");
    }
    else if (tagId == eConsumable)
    {
        updateScoreData();
    }
    else if (tagId == eNpc)
    {
        spawnableId = eConsumable;
    }
    else if (tagId == eTile)
    {
        spawnableId = eConsumable;
    }
    entity.destroy();
    m_entityManager.update();
    spawnEntity(tagId, spawnableId, pos);
}

void ScenePlay::aiMoveByNodes(Entity& entity, const bool addShift)
{
    auto& movable = entity.get<CMovable>();
    auto& transf = entity.get<CTransform>();

    const auto roomPos = m_grid->getRoomXY(transf.pos);
    const Vec2 entityTilePos = movable.positions[movable.currentPosition];
    auto entityPos = m_grid->getPosition(
        static_cast<int>(roomPos.x), static_cast<int>(roomPos.y),
        static_cast<int>(entityTilePos.x), static_cast<int>(entityTilePos.y)
        );
    if (addShift) { entityPos.y -= 16.f; }
    if (entity.tagId() == eTile && (
            entity.get<CBoundingBox>().size.x > 64.f && entity.get<CBoundingBox>().size.x < 128)
    ) // two-gridsize platform
    {
        entityPos.x += 32.f;
    }

    if (entityPos.dist(transf.pos) < 5.0f) // how smoothly npc changes angle
    {
        movable.currentPosition = (1 + movable.currentPosition) % movable.positions.size();
    }

    transf.velocity = (entityPos - transf.pos).magnitude(movable.speed);
}


void ScenePlay::createPanelEntities()
{
    std::vector<std::string> panelEntitiesNames{"PanelInk", "PanelShield", "PanelBoom"};
    std::map<std::string, float> cooldown{
        {"PanelInk", 0.3f * 60}, {"PanelShield", 5.f * 60}, {"PanelBoom", 2.f * 60}
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
    // Update Inks if there are any in PlayerData
    if (m_playerData.inks() > 0)
    {
        std::cout << "Set inks from playerData\n";
        m_entityPanel[0].get<CConsumable>().amount = m_playerData.inks();
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
        for (const auto& tag: vectorTags)
        {
            for (auto& entity: m_entityManager.getEntities(tag))
            {
                auto& transform = entity.get<CTransform>();
                sf::Color c = entity.get<CAnimation>().animation.getSprite().getColor();
                if (entity.has<CInvincibility>()) { c = game::LightGray; }

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
                    animation.getSprite().setColor(c);
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

                    float ratio = static_cast<float>(h.current) / static_cast<float>(h.max);
                    size.x *= ratio;
                    rect.setSize({size.x, size.y});
                    rect.setFillColor(sf::Color(game::LightGray));
                    rect.setOutlineThickness(0);
                    m_game->window().draw(rect);

                    for (int i = 0; i < h.max; i++)
                    {
                        tick.setPosition(
                            rect.getPosition() + sf::Vector2f(
                                static_cast<float>(i) * 64.f * 1.0f / static_cast<float>(h.max), 0)
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
        for (auto& entity: m_entityManager.getEntities())
        {
            if (entity.has<CBoundingBox>())
            {
                auto& box = entity.get<CBoundingBox>();
                auto& transf = entity.get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                // rect.setPosition(box.center.x, box.center.y);
                rect.setPosition(transf.pos.x, transf.pos.y);
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
                if (entity.tagId() == eNpc)
                {
                    auto& ePos = entity.get<CTransform>().pos;
                    auto& view = m_game->window().getView().getCenter();
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

            if (entity.has<CInteractableBox>())
            {
                auto& box = entity.get<CInteractableBox>();
                auto& transf = entity.get<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.sizeOneHalf.x, box.sizeOneHalf.y));
                rect.setOrigin(sf::Vector2f(box.sizeOneHalf.x, box.sizeOneHalf.y));

                rect.setPosition(transf.pos.x + box.sizeOneHalf.x / 2.0f, transf.pos.y + box.sizeOneHalf.y / 2.0f);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color::Green);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }

            // draw patrol points
            if (entity.has<CFollowPlayer>())
            {
                auto& h = entity.get<CFollowPlayer>().home;
                dot.setPosition(h.x, h.y);
                m_game->window().draw(dot);
            }
            if (entity.has<CMovable>() && entity.tagId() == eNpc)
            {
                for (auto& p: entity.get<CMovable>().positions)
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

void ScenePlay::stateAnimation(std::string& animName, Entity& entity)
{
    facingDirection(entity.get<CTransform>(), animName);
    entity.add<CAnimation>(m_game->assets().getAnimation(animName), true);
}

void ScenePlay::facingDirection(CTransform& transf, std::string& animName)
{
    if (transf.facing.y == 1.0f)
    {
        transf.scale.x = 1.0f;
        if (animName == "Climb") { animName.append("Up"); }
    }
    else if (transf.facing.y == -1.0f)
    {
        transf.scale.x = 1.0f;
        if (animName.find("Climb") != std::string::npos) { animName.append("Down"); }
    }
    else // flipping to left if needed
        if (transf.facing.x != 0.0f) { transf.scale.x = transf.facing.x; }
}

void ScenePlay::setRoomBackground(sf::Texture& tex)
{
    auto& vertices = game::roomVertices;
    m_background.setPointCount(vertices.size());
    m_background.setOutlineColor(sf::Color(40, 40, 40));
    m_background.setOutlineThickness(1.0f);
    for (size_t i = 0; i < vertices.size(); i++)
    {
        m_background.setPoint(i, sf::Vector2f(vertices[i].x, vertices[i].y));
    }
    tex.setRepeated(true);
    m_background.setTexture(&tex);
    m_background.setTextureRect(sf::IntRect(
        0, 0, static_cast<int>(width() * 3.0f),
        static_cast<int>(height() * 1.0f)
        ));
    m_game->window().draw(m_background);
}

void ScenePlay::updateScoreData()
{
    auto delta = m_playerData.drops() % 2;
    m_playerData.setDrops(ink().get<CConsumable>().amount * 2 + delta);
}

Entity& ScenePlay::ink()
{
    return m_entityPanel[0];
}

Entity& ScenePlay::shield()
{
    return m_entityPanel[1];
};

Entity& ScenePlay::boom()
{
    return m_entityPanel[2];
}
