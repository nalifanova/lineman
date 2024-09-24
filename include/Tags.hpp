#ifndef TAGS_HPP
#define TAGS_HPP

#include <map>

enum TagName { ePlayer = 10, eNpc, eTile, eDecoration, eConsumable, ePanel, eWeapon, eInteractable };

inline std::vector vectorTags = {eTile, eDecoration, eConsumable, eInteractable, eNpc, ePlayer, eWeapon};

// string names are used in config settings
inline std::map<size_t, std::string> tags = {
    {TagName::ePlayer, "Player"},
    {TagName::eNpc, "Npc"},
    {TagName::eTile, "Tile"},
    {TagName::eDecoration, "Dec"},
    {TagName::eConsumable, "Cons"},
    {TagName::ePanel, "Panel"},
    {TagName::eWeapon, "Weapon"},
    {TagName::eInteractable, "Interactable"},
};

#endif //TAGS_HPP
