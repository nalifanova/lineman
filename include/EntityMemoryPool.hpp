#ifndef ENTITYMEMORYPOOL_HPP
#define ENTITYMEMORYPOOL_HPP

#include <vector>

#include "Components.hpp"

constexpr size_t kMaxEntities = 10'000;

typedef std::tuple<
    std::vector<CAnimation>,
    std::vector<CBoundingBox>,
    std::vector<CBuff>,
    std::vector<CClimbable>,
    std::vector<CConsumable>,
    std::vector<CDamage>,
    std::vector<CDraggable>,
    std::vector<CFollowPlayer>,
    std::vector<CGravity>,
    std::vector<CHealth>,
    std::vector<CInput>,
    std::vector<CInteractableBox>,
    std::vector<CInventory>,
    std::vector<CInvincibility>,
    std::vector<CKey>,
    std::vector<CLifespan>,
    std::vector<CLock>,
    std::vector<CMovable>,
    std::vector<CState>,
    std::vector<CSurprise>,
    std::vector<CTransform>,
    std::vector<Trigger>
> EntityComponentVectorTuple;

class EntityMemoryPool
{
public:
    static EntityMemoryPool& instance()
    {
        static EntityMemoryPool pool(kMaxEntities);
        return pool;
    }

    [[nodiscard]] const size_t& getTag(size_t entityId) const;
    size_t addEntity(const size_t& tag);
    bool getEntities(size_t entityId, const size_t& tag);

    bool isActive(size_t entityId);
    void destroy(size_t entityId);

    template <typename T>
    bool has(size_t entityId)
    {
        return std::get<std::vector<T>>(m_pool)[entityId].active;
    }

    template <typename T>
    T& get(size_t entityId)
    {
        return std::get<std::vector<T>>(m_pool)[entityId];
    }

    template <typename... Components>
    auto createComponentPool(std::size_t maxEntities)
    {
        return std::make_tuple(std::vector<Components>(maxEntities)...);
    }

    void resetEntityComponents(size_t index);
    void deactivateEntityComponents(size_t index);

private:
    explicit EntityMemoryPool(size_t maxEntities);
    size_t getNextEntityIndex();

    size_t m_numEntities = 0;
    EntityComponentVectorTuple m_pool;
    std::vector<size_t> m_tags;
    std::vector<bool> m_actives;
};

#endif //ENTITYMEMORYPOOL_HPP
