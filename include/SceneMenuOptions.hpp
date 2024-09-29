#ifndef SCENEMENUOPTIONS_HPP
#define SCENEMENUOPTIONS_HPP

#include "SFML/Graphics.hpp"

#include "Scene.hpp"

class SceneMenuOptions final: public Scene
{
public:
    explicit SceneMenuOptions(GameEngine* gameEngine = nullptr);

    void sRender() override;

protected:
    void init();
    void update() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;
    void createMenu();
    void runMenu();

    std::vector<std::string> m_menuStrings;
    std::vector<sf::Text> m_menuItems;

    std::string m_title;
    sf::Text m_menuText;
    sf::RectangleShape m_buttons;
    size_t m_selectedMenuIndex = 0;
    std::string m_selectedMenuItem;
};

#endif //SCENEMENUOPTIONS_HPP
