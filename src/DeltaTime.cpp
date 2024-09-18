#include "DeltaTime.hpp"

void DeltaTime::setTime(sf::Time time)
{
    m_time = time;
}

sf::Time DeltaTime::getTime()
{
    return m_time;
}

sf::Time DeltaTime::get()
{
    return instance().getTime();
}

void DeltaTime::set(const sf::Time time)
{
    instance().setTime(time);
}
