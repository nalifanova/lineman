#include "Collision.hpp"

#include "GameEngine.hpp"
#include "Physics.hpp"
#include "Tags.hpp"

Collision::Collision(EntityManager& entityManager, size_t& currentFrame):
    m_entityManager(entityManager), m_currentFrame(currentFrame) {}

void Collision::resolveCollision(Entity& entity, Entity& another)
{
    const auto overlap = Physics::getOverlap(entity, another);
    if (overlap.x > 0.0f && overlap.y > 0.0f)
    {
        if (another.get<CBoundingBox>().blockMove)
        {
            if (entity.tagId() == eWeapon)
            {
                destroyEntity(another);
                return;
            }

            // ladders
            if (another.has<CClimbable>())
            {
                if (entity.has<CState>())
                {
                    entity.get<CState>().inAir = false;
                    entity.get<CState>().climbing = true;
                }
                return;
            }

            // Overlap: defining a direction
            const auto prevOverlap = Physics::getPreviousOverlap(entity, another);
            auto& entityPos = entity.get<CTransform>().pos;
            auto& tilePos = another.get<CTransform>().pos;

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
                entity.get<CState>().canJump = true;
            }

            // side collision
            if (prevOverlap.y > 0.0f)
            {
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
            // m_game->playSound("hit");
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

        for (auto& inter: m_entityManager.getEntities(eInteractable))
        {
            resolveCollision(inter, tile);
        }
    }
}

void Collision::entityInteractableCollision(Entity& player)
{
    for (auto& inter: m_entityManager.getEntities(eInteractable))
    {
        resolveCollision(player, inter);

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            resolveCollision(npc, inter);
        }

        for (auto& cons: m_entityManager.getEntities(eConsumable))
        {
            resolveCollision(cons, inter);
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

        for (auto& inter: m_entityManager.getEntities(eInteractable))
        {
            resolveCollision(weapon, inter);
        }
    }
}

void Collision::entityGroundCollision()
{
    for (auto entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile && !entity.has<CGravity>()) { continue; }

        if (entity.get<CTransform>().pos.y > game::kWinHeight - entity.get<CBoundingBox>().halfSize.y)
        {
            destroyEntity(entity);
        }

        if (entity.get<CTransform>().pos.x < entity.get<CBoundingBox>().halfSize.x)
        {
            entity.get<CTransform>().pos.x = entity.get<CBoundingBox>().halfSize.x;
        }
    }
}

void Collision::playerNpcCollision() {}

void Collision::entityItemCollision(Entity& player)
{
    for (auto& consumable: m_entityManager.getEntities(TagName::eConsumable))
    {
        if (Physics::isColliding(player, consumable))
        {
            moveEntity(consumable);
            //m_game->playSound("pickupInk");
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
    size_t pos = name.find(entity.tag());
    if (pos != std::string::npos)
    {
        name.replace(pos, entity.tag().length(), tags[ePanel]);
    }
    if (entity.has<CSurprise>())
    {
        entity.get<CSurprise>().isActivated = true;
        destroyEntity(entity);
    }
    else
    {
        entity.destroy();
    }

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
