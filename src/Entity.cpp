#include "Entity.hpp"
#include "Tags.hpp"

Entity::Entity(const size_t id):
    m_id(id) {}

size_t Entity::id() const
{
    return m_id;
}

bool Entity::isActive() const
{
    return EntityMemoryPool::instance().isActive(m_id);
}

const std::string& Entity::tag() const
{
    const auto tagId = EntityMemoryPool::instance().getTag(m_id);
    return tags[tagId];
}

void Entity::destroy()
{
    EntityMemoryPool::instance().destroy(m_id);
}
