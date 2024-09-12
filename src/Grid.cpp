#include "Grid.hpp"

#include "GameEngine.hpp"

Grid::Grid(sf::RenderWindow& window): m_window(window){};

Vec2 Grid::windowToWorld(const Vec2& mousePosition) const
{
    auto view = m_window.getView();
    float wx = view.getCenter().x - width() / 2.0f;
    float wy = view.getCenter().y - height() / 2.0f;
    return {mousePosition.x + wx, mousePosition.y + wy};
}

Vec2 Grid::getRoomXY(const Vec2& pos) const
{
    auto winSize = m_window.getSize();
    int roomX = static_cast<int>(pos.x) / static_cast<int>(winSize.x);
    int roomY = static_cast<int>(pos.y) / static_cast<int>(winSize.y);
    if (pos.x < 0) { roomX--; }
    if (pos.y < 0) { roomY--; }
    return {static_cast<float>(roomX), static_cast<float>(roomY)};
}

Vec2 Grid::getPosition(const int rx, const int ry, const int tx, const int ty) const
{
    const auto roomX = static_cast<float>(rx);
    const auto roomY = static_cast<float>(ry);
    const auto tileX = static_cast<float>(tx);
    const auto tileY = static_cast<float>(ty);

    return {
        roomX * width() + tileX * m_gridSize.x + m_gridSize.x / 2.0f
      , roomY * height() + tileY * m_gridSize.y + m_gridSize.y / 2.0f,
    };
}

Vec2 Grid::setPosition(const Vec2& vec) const
{
    const float tileX = std::round((vec.x - m_gridSize.x / 2.0f) / m_gridSize.x);
    const float tileY = std::round((vec.y - m_gridSize.y / 2.0f) / m_gridSize.y);

    return {
        tileX * m_gridSize.x + m_gridSize.x / 2.0f, tileY * m_gridSize.y + m_gridSize.y / 2.0f,
    };
}

sf::Text& Grid::text()
{
    return m_gridText;
}

void Grid::drawLine(const Vec2& p1, const Vec2& p2)
{
    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(p1.x, p1.y), sf::Color(game::DarkGray)),
        sf::Vertex(sf::Vector2f(p2.x, p2.y), sf::Color(game::DarkGray))
    };

    m_window.draw(line, 2, sf::Lines);
}

void Grid::drawGrid(bool& drawGrid, Entity& player)
{
    if (drawGrid)
    {
        float leftX = m_window.getView().getCenter().x - width() / 2.0f;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - static_cast<float>(static_cast<int>(leftX) % static_cast<int>(m_gridSize.x));
        float topY = m_window.getView().getCenter().y - height() / 2.0f;
        float bottomY = topY + height() + m_gridSize.y;
        float nextGridY = topY - static_cast<float>((static_cast<int>(topY) % static_cast<int>(m_gridSize.x)));

        // draw room coordinate
        if (player.isActive())
        {
            Vec2 r = getRoomXY(player.get<CTransform>().pos);

            m_gridText.setString(
                "room \n" + std::to_string(static_cast<int>(r.x)) + " " +
                std::to_string(static_cast<int>(r.y))
                );
            m_gridText.setPosition(
                leftX + m_gridSize.x + 3,
                topY + m_gridSize.y / 2
                );
            m_gridText.setFillColor(sf::Color::Red);
            m_window.draw(m_gridText);
        }

        m_gridText.setFillColor(sf::Color(game::Gray));

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, topY), Vec2(x, bottomY));
        }

        for (float y = nextGridY; y < bottomY; y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, y), Vec2(rightX, y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                const auto w = widthInt();
                const auto h = heightInt();
                std::string xCell = std::to_string(
                    ((((int)x % w) + w) % w) / (int)m_gridSize.x
                    );
                std::string yCell = std::to_string(
                    ((((int)y % h) + h) % h) / (int)m_gridSize.y
                    );
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, y + 2);
                m_window.draw(m_gridText);
            }
        }
    }
}

int Grid::widthInt() const
{
    return static_cast<int>(m_window.getSize().x);
}

int Grid::heightInt() const
{
    return static_cast<int>(m_window.getSize().y);
}

float Grid::width() const
{
    return static_cast<float>(m_window.getSize().x);
}

float Grid::height() const
{
    return static_cast<float>(m_window.getSize().y);
}
