#ifndef SCENE_MENU_CONTROLS_H
#define SCENE_MENU_CONTROLS_H

#include <deque>

#include "SFML/Graphics.hpp"

#include "Scene.hpp"

class SceneMenuControls final: public Scene
{
public:
    explicit SceneMenuControls(GameEngine* gameEngine = nullptr);

    void sRender() override;

protected:
    void init();
    void update() override;
    void onEnd() override;
    void sDoAction(const Action& action) override;
    void controls();

    std::vector<std::string> m_menuStrings;
    std::vector<sf::Text> m_menuItems;

    std::string m_title;
    sf::Text m_menuText;
    sf::RectangleShape m_buttons;
};

#endif //SCENE_MENU_CONTROLS_H
