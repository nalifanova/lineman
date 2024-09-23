#include "EntityMemoryPool.hpp"

[[nodiscard]] const size_t& EntityMemoryPool::getTag(const size_t entityId) const
{
    return m_tags[entityId];
}

size_t EntityMemoryPool::addEntity(const size_t& tag)
{
    const size_t index = getNextEntityIndex();
    m_tags[index] = tag;
    m_actives[index] = true;

    resetEntityComponents(index);
    deactivateEntityComponents(index);

    m_numEntities++;
    return index;
}

bool EntityMemoryPool::getEntities(const size_t entityId, const size_t& tag)
{
    return m_actives[entityId] && (m_tags[entityId] == tag);
}

bool EntityMemoryPool::isActive(const size_t entityId)
{
    if (entityId >= kMaxEntities) { return false; }
    return m_actives[entityId];
}

void EntityMemoryPool::destroy(const size_t entityId)
{
    if (entityId < m_actives.size())
    {
        m_actives[entityId] = false;
        // m_tags[entityId] = std::numeric_limits<size_t>::max(); // humm, why?
        m_numEntities--;
    }
}

// private
EntityMemoryPool::EntityMemoryPool(size_t maxEntities):
    m_numEntities(0)
{
    for (size_t i = 0; i < maxEntities; i++)
    {
        m_tags.push_back(std::numeric_limits<size_t>::max());
        m_actives.push_back(false);
    }
    m_pool = createComponentPool<
        CAnimation,
        CBoundingBox,
        CBuff,
        CClimbable,
        CConsumable,
        CDamage,
        CDraggable,
        CFollowPlayer,
        CGravity,
        CHealth,
        CInput,
        CInvincibility,
        CLifespan,
        CPatrol,
        CState,
        CSurprise,
        CTransform
    >(maxEntities);
}

size_t EntityMemoryPool::getNextEntityIndex()
{
    for (size_t i = 0; i < m_actives.size(); i++)
    {
        if (!m_actives[i] && m_tags[i] == std::numeric_limits<size_t>::max())
        {
            return i;
        }
    }
    return kMaxEntities;
}

void EntityMemoryPool::resetEntityComponents(const size_t index)
{
    std::get<std::vector<CAnimation>>(m_pool)[index] = CAnimation();
    std::get<std::vector<CBoundingBox>>(m_pool)[index] = CBoundingBox();
    std::get<std::vector<CBuff>>(m_pool)[index] = CBuff();
    std::get<std::vector<CClimbable>>(m_pool)[index] = CClimbable();
    std::get<std::vector<CConsumable>>(m_pool)[index] = CConsumable();
    std::get<std::vector<CDamage>>(m_pool)[index] = CDamage();
    std::get<std::vector<CDraggable>>(m_pool)[index] = CDraggable();
    std::get<std::vector<CFollowPlayer>>(m_pool)[index] = CFollowPlayer();
    std::get<std::vector<CGravity>>(m_pool)[index] = CGravity();
    std::get<std::vector<CHealth>>(m_pool)[index] = CHealth();
    std::get<std::vector<CInput>>(m_pool)[index] = CInput();
    std::get<std::vector<CInvincibility>>(m_pool)[index] = CInvincibility();
    std::get<std::vector<CLifespan>>(m_pool)[index] = CLifespan();
    std::get<std::vector<CPatrol>>(m_pool)[index] = CPatrol();
    std::get<std::vector<CState>>(m_pool)[index] = CState();
    std::get<std::vector<CSurprise>>(m_pool)[index] = CSurprise();
    std::get<std::vector<CTransform>>(m_pool)[index] = CTransform();
}

void EntityMemoryPool::deactivateEntityComponents(const size_t index)
{
    std::get<std::vector<CAnimation>>(m_pool)[index].active = false;
    std::get<std::vector<CBoundingBox>>(m_pool)[index].active = false;
    std::get<std::vector<CBuff>>(m_pool)[index].active = false;
    std::get<std::vector<CClimbable>>(m_pool)[index].active = false;
    std::get<std::vector<CConsumable>>(m_pool)[index].active = false;
    std::get<std::vector<CDamage>>(m_pool)[index].active = false;
    std::get<std::vector<CDraggable>>(m_pool)[index].active = false;
    std::get<std::vector<CFollowPlayer>>(m_pool)[index].active = false;
    std::get<std::vector<CGravity>>(m_pool)[index].active = false;
    std::get<std::vector<CHealth>>(m_pool)[index].active = false;
    std::get<std::vector<CInput>>(m_pool)[index].active = false;
    std::get<std::vector<CInvincibility>>(m_pool)[index].active = false;
    std::get<std::vector<CLifespan>>(m_pool)[index].active = false;
    std::get<std::vector<CPatrol>>(m_pool)[index].active = false;
    std::get<std::vector<CState>>(m_pool)[index].active = false;
    std::get<std::vector<CSurprise>>(m_pool)[index].active = false;
    std::get<std::vector<CTransform>>(m_pool)[index].active = false;
}
