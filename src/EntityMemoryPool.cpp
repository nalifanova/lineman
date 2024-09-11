#include "EntityMemoryPool.hpp"

[[nodiscard]] const size_t& EntityMemoryPool::getTag(const size_t entityId) const
{
    return m_tags[entityId];
}

size_t EntityMemoryPool::addEntity(const size_t& tag)
{
    const size_t index = getNextEntityIndex();
    m_tags[index] = tag;
    m_actives[tag] = true;

    std::get<std::vector<CAnimation>>(m_pool)[index].active = false;
    std::get<std::vector<CBoundingBox>>(m_pool)[index].active = false;
    std::get<std::vector<CDamage>>(m_pool)[index].active = false;
    std::get<std::vector<CDraggable>>(m_pool)[index].active = false;
    std::get<std::vector<CFollowPlayer>>(m_pool)[index].active = false;
    std::get<std::vector<CHealth>>(m_pool)[index].active = false;
    std::get<std::vector<CInput>>(m_pool)[index].active = false;
    std::get<std::vector<CInvincibility>>(m_pool)[index].active = false;
    std::get<std::vector<CLifespan>>(m_pool)[index].active = false;
    std::get<std::vector<CPatrol>>(m_pool)[index].active = false;
    std::get<std::vector<CState>>(m_pool)[index].active = false;
    std::get<std::vector<CTransform>>(m_pool)[index].active = false;

    m_numEntities++;
    return index;
}

bool EntityMemoryPool::getEntities(const size_t entityId, const size_t& tag)
{
    return m_actives[entityId] && (m_tags[entityId]) == tag;
}

bool EntityMemoryPool::isActive(const size_t entityId)
{
    if (entityId >= kMaxEntities) { return false; }
    return m_actives[entityId];
}

void EntityMemoryPool::destroy(const size_t entityId)
{
    m_actives[entityId] = false;
    m_numEntities--;
}

// private
EntityMemoryPool::EntityMemoryPool(size_t maxEntities):
    m_numEntities(0)
{
    for (size_t i = 0; i < maxEntities; i++)
    {
        m_tags.push_back(0);
        m_actives.push_back(false);
    }
    auto m_pool = createComponentPool<
        CAnimation,
        CBoundingBox,
        CDamage,
        CDraggable,
        CFollowPlayer,
        CHealth,
        CInput,
        CInvincibility,
        CLifespan,
        CPatrol,
        CState,
        CTransform
    >(maxEntities);
}

size_t EntityMemoryPool::getNextEntityIndex()
{
    for (size_t i = 0; i < m_actives.size(); i++)
    {
        if (!m_actives[i]) { return i; }
    }
    return kMaxEntities;
}
