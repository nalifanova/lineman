#ifndef TAGS_HPP
#define TAGS_HPP

#include <map>

enum TagName { ePlayer = 10, eNpc, eTile, eDecoration, eConsumable, ePanel };

// string names are used in config settings
inline std::map<size_t, std::string> tags = {
    {TagName::ePlayer, "Player"},
    {TagName::eNpc, "Npc"},
    {TagName::eTile, "Tile"},
    {TagName::eDecoration, "Dec"},
    {TagName::eConsumable, "Cons"},
    {TagName::ePanel, "Panel"},
};

#endif //TAGS_HPP
