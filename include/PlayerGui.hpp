#ifndef PLAYERGUI_HPP
#define PLAYERGUI_HPP

#include <map>

#include "SFML/Graphics.hpp"

#include "Entity.hpp"
#include "EntityManager.hpp"

class PlayerGui
{
public:
    explicit PlayerGui(sf::RenderWindow& window, EntityManager& entityManager, const sf::Font& font
                     , std::map<std::string, int>& scoreData);

    void setBottomPanel();
    void setTopPanel();
    void setPanelText(sf::Vector2f winPos);
    void setPanelIcons(const sf::Vector2f& winPos);
    void setPanelIconTitles() const;
    void barsUpgrade(Entity& entity);
    void showCoolDownProgress(Entity& entity);
    void gameOver(const std::function<void()>& callback) const;

private:
    sf::RectangleShape m_panel;
    sf::RenderWindow& m_window;
    EntityManager& m_entityManager;
    sf::Font m_font;
    sf::Clock m_clock;
    std::map<std::string, int>& m_scoreData;
};

#endif //PLAYERGUI_HPP
