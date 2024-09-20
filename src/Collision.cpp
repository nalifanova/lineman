#include "Collision.hpp"

#include "GameEngine.hpp"
#include "Physics.hpp"
#include "Tags.hpp"

Collision::Collision(EntityManager& entityManager, size_t& currentFrame):
    m_entityManager(entityManager), m_currentFrame(currentFrame) {}

void Collision::resolveCollision(Entity& entity, Entity& tile)
{
    const auto overlap = Physics::getOverlap(entity, tile);
    if (overlap.x > 0.0f && overlap.y > 0.0f)
    {
        if (tile.get<CBoundingBox>().blockMove)
        {
            if (entity.tagId() == eWeapon)
            {
                tile.destroy();
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
                entity.get<CState>().canJump = true;
            }

            // side collision
            if (prevOverlap.y > 0.0f)
            {
                entityPos.x += entityPos.x < tilePos.x ? -overlap.x : overlap.x;
                entity.get<CTransform>().velocity.x = 0.0f;
            }
        }
        handleEffectsInCollision(entity, tile);
    }
}

void Collision::handleEffectsInCollision(Entity& entity, Entity& tile)
{
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

void Collision::entityTileCollision(Entity& player)
{
    for (auto& tile: m_entityManager.getEntities(eTile))
    {
        resolveCollision(player, tile);

        for (auto& npc: m_entityManager.getEntities(eNpc))
        {
            resolveCollision(npc, tile);
        }
    }
}

void Collision::weaponTileCollision()
{
    for (auto& weapon: m_entityManager.getEntities(eWeapon))
    {
        for (auto& tile: m_entityManager.getEntities(eTile))
        {
            resolveCollision(weapon, tile);
        }
    }
}

void Collision::entityGroundCollision()
{
    for (auto entity: m_entityManager.getEntities())
    {
        if (entity.tagId() == eTile) { continue; }

        if (entity.get<CTransform>().pos.y > game::kWinHeight - entity.get<CBoundingBox>().halfSize.y)
        {
            destroyEntity(entity);
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

void Collision::destroyEntity(Entity& entity)
{
    if (!entity.has<CLifespan>())
    {
        entity.add<CLifespan>(2, m_currentFrame);
    } else
    {
        entity.get<CLifespan>().lifespan = 2;
        entity.get<CLifespan>().frameCreated = static_cast<int>(m_currentFrame);
    }
}
