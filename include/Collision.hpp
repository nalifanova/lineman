#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Entity.hpp"
#include "EntityManager.hpp"

class Collision
{
public:
    explicit Collision(EntityManager& entityManager, size_t& currentFrame);

    void resolveCollision(Entity& entity, Entity& tile);
    void handleEffectsInCollision(Entity& entity, Entity& tile);
    void entityTileCollision(Entity& player);
    void weaponTileCollision();
    void playerNpcCollision();
    void entityItemCollision(Entity& player);
    // void teleportCollision();
    // void roomCollision(std::shared_ptr<Entity>& entity);
    void entityGroundCollision();
    void moveEntity(Entity& entity);
    void destroyEntity(Entity& entity);

private:
    EntityManager& m_entityManager;
    size_t m_currentFrame;
};

#endif //COLLISION_HPP
