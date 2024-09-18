#ifndef DELTATIME_HPP
#define DELTATIME_HPP

#include "SFML/Graphics.hpp"

class DeltaTime
{
public:
    static DeltaTime& instance()
    {
        static DeltaTime instance;
        return instance;
    }
    sf::Time getTime();
    static sf::Time get();
    void setTime(sf::Time time);
    static void set(sf::Time time);
private:
    DeltaTime() = default;
    sf::Time m_time{};
};

#endif //DELTATIME_HPP
