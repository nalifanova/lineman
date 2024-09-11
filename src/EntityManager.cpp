#include "EntityManager.hpp"

EntityManager::EntityManager() = default;

Entity EntityManager::addEntity(const size_t& tag)
{
    const size_t id = EntityMemoryPool::instance().addEntity(tag);
    const Entity entity(id);
    m_entitiesToAdd.push_back(entity);

    return entity;
}

// called at beginning of each frame by game engine
// entities added will now be available to use this frame
void EntityManager::update()
{
    for (const auto& entity: m_entitiesToAdd)
    {
        m_entities.push_back(entity);
    }
    m_entitiesToAdd.clear();

    // remove dead entities from the vector
    std::erase_if(m_entities, [](const Entity entity) { return !entity.isActive(); });
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
