#ifndef PLAYERGUI_HPP
#define PLAYERGUI_HPP

#include "SFML/Graphics.hpp"

#include "Entity.hpp"
#include "EntityManager.hpp"

class PlayerGui
{
public:
    explicit PlayerGui(sf::RenderWindow& window, EntityManager& entityManager, const sf::Font& font);

    void setBottomPanel();
    void setPanelIcons(const sf::Vector2f& winPos);
    void setPanelIconTitles() const;
    void barsUpgrade(Entity& entity);

private:
    sf::RectangleShape m_bottomPanel;
    sf::RenderWindow& m_window;
    EntityManager& m_entityManager;
    sf::Font m_font;
};

#endif //PLAYERGUI_HPP
