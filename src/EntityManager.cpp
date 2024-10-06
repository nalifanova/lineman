#include "EntityManager.hpp"
#include "Tags.hpp"

EntityManager::EntityManager() = default;

Entity EntityManager::addEntity(const size_t& tag)
{
    const size_t id = EntityMemoryPool::instance().addEntity(tag);
    const Entity entity(id);
    m_entitiesToAdd.push_back(entity);

    return entity;
}

std::optional<Entity> EntityManager::getEntityById(const size_t entityId)
{
    for (auto& entity: m_entities)
    {
        if (entity.id() == entityId) { return entity; }
    }
    return std::nullopt;
}

// called at beginning of each frame by game engine
// entities added will now be available to use this frame
void EntityManager::update()
{
    for (auto& entity: m_entitiesToAdd)
    {
        m_entities.push_back(entity);
    }
    m_entitiesToAdd.clear();

    // remove dead entities from the vector
    std::erase_if(m_entities, [](Entity entity) { return !entity.isActive(); });
}

std::vector<Entity> EntityManager::getEntities()
{
    return m_entities;
}

std::vector<Entity> EntityManager::getEntities(const size_t& tag) const
{
    std::vector<Entity> entities;
    for (auto& entity: m_entities)
    {
        if (EntityMemoryPool::instance().getEntities(entity.id(), tag))
        {
            entities.push_back(entity);
        }
    }
    return entities;
}

EntityMap EntityManager::getEntityMap() const
{
    EntityMap entities;
    for (const auto& [tag, tagName]: tags)
    {
        for (auto& entity: m_entities)
        {
            if (EntityMemoryPool::instance().getEntities(entity.id(), tag))
            {
                entities[tag].push_back(entity);
            }
        }
    }
    return entities;
}
