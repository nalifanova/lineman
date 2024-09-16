#ifndef DEBUG_GUI_HPP
#define DEBUG_GUI_HPP

#include <functional>

#include "Assets.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

class DebugGui
{
public:
    explicit DebugGui(Assets& assets, EntityManager& entityManager);
    ~DebugGui() = default;

    void guiShowTable(const std::vector<Entity>& entities, bool showHeader = true);
    void showDebugWindow(bool& m_drawGrid, bool& m_drawTextures, bool& m_drawCollision, bool& m_zoom);
    void addCallBack(std::function<void()> callback);

private:
    Assets& m_assets;
    EntityManager m_entityManager;
    std::function<void()> m_callback;
};


#endif //DEBUG_GUI_HPP
