#ifndef TAGS_HPP
#define TAGS_HPP

#include <map>

enum TagName { ePlayer = 10, eNpc, eTile };

inline std::map<size_t, std::string> tags = {
    {TagName::ePlayer, "Player"},
    {TagName::eNpc, "Npc"},
    {TagName::eTile, "Tile"},
};

#endif //TAGS_HPP
