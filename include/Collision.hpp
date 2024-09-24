#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Entity.hpp"
#include "EntityManager.hpp"

class Collision
{
public:
    explicit Collision(EntityManager& entityManager, size_t& currentFrame);

    void resolveCollision(Entity& entity, Entity& another);
    void handleEffectsInCollision(Entity& entity, Entity& another);
    void entityTileCollision(Entity& player);
    void entityInteractableCollision(Entity& player);
    void checkInteraction(Entity& player);
    void weaponEntityCollision();
    void playerNpcCollision();
    void entityItemCollision(Entity& player);
    bool isClimbing(Entity& entity) const;
    // void teleportCollision();
    void entityRoomCollision(float width, float height);
    void moveEntity(Entity& entity);
    void destroyEntity(Entity& entity);

private:
    EntityManager& m_entityManager;
    size_t m_currentFrame;
};

#endif //COLLISION_HPP
