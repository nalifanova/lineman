#ifndef SINVENTORY_HPP
#define SINVENTORY_HPP

#include "Entity.hpp"

class SInventory
{
public:
    static void pickUpItem(Entity& entity, Entity& item, int quantity = 1);
    static void useItem(Entity& entity, std::string itemName);
    static bool hasItem(Entity& entity, std::string itemName);
};

#endif //SINVENTORY_HPP
