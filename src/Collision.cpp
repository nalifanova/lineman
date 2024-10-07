#include "Collision.hpp"

#include "Data.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
#include "Sinventory.hpp"
#include "Tags.hpp"

Collision::Collision(EntityManager& entityManager, size_t& currentFrame, Assets& assets):
    m_entityManager(entityManager), m_currentFrame(currentFrame), m_assets(assets) {}

void Collision::resolveCollision(Entity& entity, Entity& another)
{
    const auto overlap = Physics::getOverlap(entity, another);
    if (overlap.x > 0.0f && overlap.y > 0.0f)
    {
        if (another.get<CBoundingBox>().blockMove)
        {
            if (entity.tagId() == eWeapon)
            {
                resolveWeaponCollision(entity, another);
                return;
            }
            if (another.has<CClimbable>()) // ladders, etc.
            {
                resolveLadderCollision(entity);
                return;
            }

            if (entity.tagId() == ePlayer && another.tagId() == eItem) { resolveItemCollision(entity, another); }

            if (another.has<CInteractableBox>())
            {
                if (entity.tagId() == ePlayer && another.has<CLock>() && another.get<CLock>().isActivated)
                {
                    entity.get<CState>().interAction = game::interActions.at(another.get<CLock>().action);
                }
            }

            // Overlap: defining a direction
            const auto prevOverlap = Physics::getPreviousOverlap(entity, another);
            auto& entityPos = entity.get<CTransform>().pos;
            auto& tilePos = another.get<CTransform>().pos;

            // top/bottom collision
            if (prevOverlap.x > 0.0f)
            {
                if (entity.has<CInput>()) { pushFromSides(entity, another); }

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
                entity.get<CState>().canJump = true;
            }

            // side collision
            if (prevOverlap.y > 0.0f)
            {
                if (entity.has<CInput>()) { pushFromSides(entity, another); }

                entityPos.x += entityPos.x < tilePos.x ? -overlap.x : overlap.x;
                entity.get<CTransform>().velocity.x = 0.0f;
            }
        }
        handleEffectsInCollision(entity, another);
    }
}

void Collision::handleEffectsInCollision(Entity& entity, Entity& another)
{
    if (entity.has<CInvincibility>()) { return; }

    if (entity.has<CBuff>() && entity.get<CBuff>().shield) { return; }

    if (another.has<CDamage>())
    {
        entity.get<CHealth>().current -= another.get<CDamage>().damage;
        entity.add<CInvincibility>(kInvincibility);

        if (entity.get<CHealth>().current <= 0)
        {
            destroyEntity(entity);
            // m_game->playSound("died");
        }
        else
        {
            auto& damage = m_assets.getSound("Damage");
            damage.play();
        }
    }
}

void Collision::entityTileCollision(Entity& player)
{
    for (auto& tile: m_entityManager.getEntities(eTile))
    {
        resolveCollision(player, tile);

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            resolveCollision(npc, tile);
        }

        for (auto& cons: m_entityManager.getEntities(eConsumable))
        {
            resolveCollision(cons, tile);
        }

        for (auto& inter: m_entityManager.getEntities(eItem))
        {
            resolveCollision(inter, tile);
        }
    }
}

void Collision::entityInteractableCollision(Entity& player)
{
    for (auto& intr: m_entityManager.getEntities(eItem))
    {
        resolveCollision(player, intr);

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            resolveCollision(npc, intr);
        }

        for (auto& another: m_entityManager.getEntities(eItem))
        {
            if (another.id() == intr.id()) { continue; }
            resolveCollision(another, intr);
        }
    }
}

void Collision::checkInteraction(Entity& player)
{
    for (auto& intr: m_entityManager.getEntities(eItem))
    {
        if (Physics::isInteractableColliding(player, intr))
        {
            if (intr.has<CLock>() && player.get<CState>().state == "Interact")
            {
                auto& l = intr.get<CLock>();
                if (l.isLocked && SInventory::hasItem(player, "ItemKey"))
                {
                    SInventory::useItem(player, "ItemKey");
                    l.isLocked = false;
                }

                if (!l.isLocked && !l.isOpen)
                {
                    l.isOpen = true;
                    // Let character overlap the eInteractable
                    intr.get<CBoundingBox>().size = Vec2(2.0f, 2.0f);
                    intr.get<CBoundingBox>().halfSize = intr.get<CBoundingBox>().size / 2.f;
                }
                else if (l.isOpen && !l.isActivated)
                {
                    l.isActivated = true;
                    std::cout << "We enter the door!\n";
                }
            }
        }
    }
}

void Collision::weaponEntityCollision()
{
    for (auto& weapon: m_entityManager.getEntities(eWeapon))
    {
        for (auto& tile: m_entityManager.getEntities(eTile))
        {
            resolveCollision(weapon, tile);
        }

        for (auto& inter: m_entityManager.getEntities(eItem))
        {
            resolveCollision(weapon, inter);
        }

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            resolveCollision(weapon, npc);
        }
    }
}

void Collision::entityRoomCollision(float width, float height)
{
    for (auto entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile && !entity.has<CGravity>()) { continue; }

        if (entity.get<CTransform>().pos.y < entity.get<CBoundingBox>().halfSize.y + 20.f)
        {
            entity.get<CTransform>().pos.y = entity.get<CBoundingBox>().halfSize.y + 20.f;
        }
        if (entity.get<CTransform>().pos.y > height - entity.get<CBoundingBox>().halfSize.y)
        {
            destroyEntity(entity);
        }

        // room
        if (entity.get<CTransform>().pos.x < entity.get<CBoundingBox>().halfSize.x)
        {
            entity.get<CTransform>().pos.x = entity.get<CBoundingBox>().halfSize.x;
        }

        if (entity.get<CTransform>().pos.x > width - entity.get<CBoundingBox>().halfSize.x)
        {
            entity.get<CTransform>().pos.x = width - entity.get<CBoundingBox>().halfSize.x;
        }
    }
}

void Collision::playerNpcCollision(Entity& player)
{
    for (auto& npc: m_entityManager.getEntities(eNpc))
    {
        resolveCollision(player, npc);
    }
}

void Collision::entityItemCollision(Entity& player)
{
    for (auto& item: m_entityManager.getEntities(eItem))
    {
        resolveCollision(player, item);

        for (auto tile: m_entityManager.getEntities(eTile))
        {
            resolveCollision(item, tile);
        }
    }

    for (auto& consumable: m_entityManager.getEntities(eConsumable))
    {
        if (Physics::isColliding(player, consumable))
        {
            moveEntity(consumable);
            auto& pickupInk = m_assets.getSound("PickupInk");
            pickupInk.play();
        }

        for (auto tile: m_entityManager.getEntities(eTile))
        {
            resolveCollision(consumable, tile);
        }
    }
}

bool Collision::isClimbing(Entity& entity) const
{
    for (auto& tile: m_entityManager.getEntities(eTile))
    {
        if (!tile.has<CClimbable>()) { continue; }

        if (Physics::isColliding(entity, tile))
        {
            return true;
        }
    }
    return false;
}

void Collision::moveEntity(Entity& entity)
{
    if (entity.tagId() != eConsumable) { return; }

    // get consumable name and convert to panel name
    auto name = entity.get<CAnimation>().animation.getName();
    std::string suffix = "Air"; // Note: maybe later we'll have a vector of suffixes
    if (name.ends_with(suffix)) { name.erase(name.size() - suffix.length()); }

    size_t pos = name.find(entity.tag());
    if (pos != std::string::npos)
    {
        name.replace(pos, entity.tag().length(), tags[ePanel]);
    }
    if (entity.has<CSurprise>()) { entity.get<CSurprise>().isActivated = true; }
    destroyEntity(entity);

    for (auto item: m_entityManager.getEntities(ePanel))
    {
        if (item.get<CAnimation>().animation.getName() == name)
        {
            item.get<CConsumable>().amount += 1;
            break;
        }
    }
}

void Collision::destroyEntity(Entity& entity)
{
    if (!entity.has<CLifespan>())
    {
        entity.add<CLifespan>(1, m_currentFrame);
    }
    else
    {
        entity.get<CLifespan>().lifespan = 1;
        entity.get<CLifespan>().frameCreated = static_cast<int>(m_currentFrame);
    }
}

// helper functions
void Collision::resolveWeaponCollision(Entity& weapon, Entity& another)
{
    auto& anim = weapon.get<CAnimation>().animation;

    if (anim.getName().starts_with("Boom")) // only special weapon destroys everything
    {
        if (another.has<CHealth>())
        {
            destroyEntity(another);
            destroyEntity(weapon);
        }
    }
    else if (another.tagId() == eNpc)
    {
        auto& h = another.get<CHealth>();
        h.current -= weapon.get<CDamage>().damage;

        if (h.current <= 0)
        {
            destroyEntity(another);
        }
        destroyEntity(weapon);
    }
}

void Collision::resolveLadderCollision(Entity& entity)
{
    if (entity.has<CState>())
    {
        entity.get<CState>().inAir = false;
        entity.get<CState>().climbing = true;
    }
}

void Collision::resolveItemCollision(Entity& player, Entity& another)
{
    if (another.has<CKey>())
    {
        SInventory::pickUpItem(player, another);
        another.remove<CGravity>();
        another.get<CTransform>().pos = Vec2(0.f, 800.0f); // TODO: creepy workaround
        std::cout << player.get<CInventory>().totalItems() << "\n";
    }
}

void Collision::pushFromSides(Entity& entity, Entity& another)
{
    auto& entityPos = entity.get<CTransform>().pos;
    auto& tilePos = another.get<CTransform>().pos;

    if (entity.get<CInput>().up || entity.get<CInput>().down)
    {
        const float entityRight = entityPos.x + entity.get<CBoundingBox>().halfSize.x;
        const float tileRight = tilePos.x + another.get<CBoundingBox>().halfSize.x;
        const float entityLeft = entityPos.x - entity.get<CBoundingBox>().halfSize.x;
        const float tileLeft = tilePos.x - another.get<CBoundingBox>().halfSize.x;

        if (entityRight > tileRight - 5) { entityPos.x += 1.0f; }
        else
            if (entityLeft < tileLeft + 5) { entityPos.x -= 1.0f; }
    }

    if (entity.get<CInput>().left || entity.get<CInput>().right)
    {
        const float entityTop = entityPos.y + entity.get<CBoundingBox>().halfSize.y;
        const float tileTop = tilePos.y + another.get<CBoundingBox>().halfSize.y;
        const float entityBottom = entityPos.y - entity.get<CBoundingBox>().halfSize.y;
        const float tileBottom = tilePos.y - another.get<CBoundingBox>().halfSize.y;

        if (entityTop > tileTop - 5) { entityPos.y += 1.0f; }
        else
            if (entityBottom < tileBottom + 5) { entityPos.y -= 1.0f; }
    }
}
