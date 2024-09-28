#include "PlayerData.hpp"

PlayerData::PlayerData() = default;

PlayerData::PlayerData(std::string name, int inks, int level, int life, Vec2 pos) :
    m_name(std::move(name)), m_inks(inks), m_level(level), m_life(life), m_pos(pos) {}

int PlayerData::time()
{
    return m_time;
}

int PlayerData::life()
{
    return m_life;
}

int PlayerData::drops()
{
    return m_drops;
}

int PlayerData::inks()
{
    return m_inks;
}

int PlayerData::level()
{
    return m_level;
}

Vec2 PlayerData::pos()
{
    return m_pos;
}

std::string PlayerData::name()
{
    return m_name;
}

void PlayerData::addTime(int value)
{
    m_time += value;
}

void PlayerData::updateDrops(int value)
{
    m_drops += value;
    // if (m_inks != m_drops / 2)
    // {
    //     m_drops = m_inks * 2;
    // }
}

void PlayerData::updateLife(int value)
{
    m_life += value;
}

void PlayerData::setInks(int value)
{
    m_inks = value;
}

void PlayerData::setDrops(int value)
{
    m_drops = value;
}
