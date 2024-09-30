#ifndef PLAYERDATA_HPP
#define PLAYERDATA_HPP

#include <iostream>

#include "Vec2.hpp"

class PlayerData
{
public:
    PlayerData();
    explicit PlayerData(std::string name, int inks, int level, int life, Vec2 pos);

    int time();
    int life();
    int drops();
    int inks();
    int level();
    Vec2 pos();
    std::string name();

    void addTime(int value = 1);
    void updateDrops(int value);
    void updateLife(int value);
    void setInks(int value);
    void setDrops(int value);
    void setLevel(int value);

private:
    std::string m_name = "Player";
    int m_inks = 0;
    int m_level = 1;
    int m_life = 3;
    int m_time = 0;
    int m_drops = 0;
    Vec2 m_pos = {92.f, 100.f}; // taken as default from assets
};

#endif //PLAYERDATA_HPP
