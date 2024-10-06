#include "Sinventory.hpp"
#include "EntityManager.hpp"

void SInventory::pickUpItem(Entity& entity, Entity& item, int quantity)
{
    if (entity.has<CInventory>())
    {
        std::string itemName = item.get<CAnimation>().animation.getName();
        entity.get<CInventory>().addItem(itemName, item.id(), quantity);
    }
}

void SInventory::useItem(Entity& entity, std::string itemName)
{
    if (entity.has<CInventory>())
    {
        auto& inventory = entity.get<CInventory>();
        if (inventory.hasItem(itemName))
        {
            const size_t entityId = inventory.getEntityId(itemName);
            inventory.removeItem(itemName, 1);
            auto em = EntityManager();
            auto e = em.getEntityById(entityId);
            if (e.has_value()) { e->destroy(); }
        }
    }
}

bool SInventory::hasItem(Entity& entity, std::string itemName)
{
    if (entity.has<CInventory>()) { return entity.get<CInventory>().hasItem(itemName); }
    return false;
}
