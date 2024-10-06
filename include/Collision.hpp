#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Assets.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class Collision
{
public:
    explicit Collision(EntityManager& entityManager, size_t& currentFrame, Assets& assets);

    void resolveCollision(Entity& entity, Entity& another);
    void handleEffectsInCollision(Entity& entity, Entity& another);
    void entityTileCollision(Entity& player);
    void entityInteractableCollision(Entity& player);
    void checkInteraction(Entity& player);
    void weaponEntityCollision();
    void playerNpcCollision(Entity& player);
    void entityItemCollision(Entity& player);
    bool isClimbing(Entity& entity) const;
    // void teleportCollision();
    void entityRoomCollision(float width, float height);
    void moveEntity(Entity& entity);
    void destroyEntity(Entity& entity);

private:
    // helper functions
    void resolveWeaponCollision(Entity& weapon, Entity& another);
    static void resolveLadderCollision(Entity& entity);
    static void resolveItemCollision(Entity& player, Entity& another);
    static void pushFromSides(Entity& entity, Entity& another);

    EntityManager& m_entityManager;
    Assets& m_assets;
    size_t m_currentFrame;
};

#endif //COLLISION_HPP
