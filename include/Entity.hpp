#ifndef ENTITY_H
#define ENTITY_H

#include <string>

#include "EntityMemoryPool.hpp"

class Entity
{
public:
    explicit Entity(size_t id);

    [[nodiscard]] size_t id() const;
    [[nodiscard]] bool isActive() const;
    [[nodiscard]] size_t tagId() const;
    [[nodiscard]] const std::string& tag() const;

    void destroy() const;

    template <class T>
    [[nodiscard]] bool has() const
    {
        return EntityMemoryPool::instance().has<T>(m_id);
    }

    template <class T, typename... TArgs>
    T& add(TArgs&&... mArgs)
    {
        auto& component = EntityMemoryPool::instance().get<T>(m_id);
        component = T(std::forward<TArgs>(mArgs)...);
        component.active = true;
        return component;
    }

    template <class T>
    T& get()
    {
        return EntityMemoryPool::instance().get<T>(m_id);
    }

    template <class T>
    void remove()
    {
        EntityMemoryPool::instance().get<T>(m_id).active = false;
    }

private:
    size_t m_id;
};

#endif //ENTITY_H
