#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <map>
#include <optional>
#include <vector>

#include "Entity.hpp"

typedef std::map<size_t, std::vector<Entity>> EntityMap;

class EntityManager
{
public:
    EntityManager();

    Entity addEntity(const size_t& tag);
    std::optional<Entity> getEntityById(size_t entityId);
    void update();

    std::vector<Entity> getEntities();
    [[nodiscard]] std::vector<Entity> getEntities(const size_t& tag) const;
    [[nodiscard]] EntityMap getEntityMap() const;

private:
    std::vector<Entity> m_entities; // all entities
    std::vector<Entity> m_entitiesToAdd; // entities to add next update
};

#endif //ENTITY_MANAGER_H
