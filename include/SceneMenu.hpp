#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include <deque>

#include "SFML/Graphics/Text.hpp"
#include "SFML/Audio/Sound.hpp"

#include "Scene.hpp"

class SceneMenu: public Scene
{
public:
    explicit SceneMenu(GameEngine* gameEngine = nullptr);

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
    std::vector<std::string> m_levelPaths;

    sf::Sound m_titleMusic{};
    sf::Text m_menuText;

    std::string m_title;
    size_t m_selectedMenuIndex = 0;
    std::string m_selectedMenuItem;

    bool m_active = false;
};

#endif //SCENE_MENU_H
