#ifndef GRID_HPP
#define GRID_HPP

#include "SFML/Graphics.hpp"

#include "Entity.hpp"
#include "Vec2.hpp"

class Grid
{
public:
    explicit Grid(sf::RenderWindow& window);

    [[nodiscard]] Vec2 windowToWorld(const Vec2& mousePosition) const;
    [[nodiscard]] Vec2 getRoomXY(const Vec2& pos) const;

    [[nodiscard]] Vec2 getPosition(int rx, int ry, int tx, int ty) const;
    [[nodiscard]] Vec2 setPosition(const Vec2& vec) const;

    sf::Text& text();

    void drawLine(const Vec2& p1, const Vec2& p2);
    void drawGrid(bool& drawGrid, Entity& player);

    // helpers
    int widthInt() const;
    int heightInt() const;
    float width() const;
    float height() const;

private:
    sf::RenderWindow& m_window;
    const Vec2 m_gridSize = {64, 64};
    sf::Text m_gridText;
};

#endif //GRID_HPP
