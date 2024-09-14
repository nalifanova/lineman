#ifndef GUI_HPP
#define GUI_HPP

#include <functional>

#include "SFML/Graphics.hpp"

#include "Assets.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class GUI
{
public:
    explicit GUI(Assets& assets, EntityManager& entityManager);
    ~GUI() = default;

    void guiShowTable(const std::vector<Entity>& entities, bool showHeader = true);
    void showDebugWindow(bool& m_drawGrid, bool& m_drawTextures, bool& m_drawCollision, bool& m_zoom);
    void addCallBack(std::function<void()> callback);

private:
    Assets& m_assets;
    EntityManager m_entityManager;
    std::function<void()> m_callback;
};


#endif //GUI_HPP
