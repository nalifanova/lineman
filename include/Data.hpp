#ifndef DATA_HPP
#define DATA_HPP
#include "GameEngine.hpp"

namespace game {
    inline auto w = static_cast<float>(kWinWidth);
    inline auto h = static_cast<float>(kWinHeight);
    inline std::vector<Vec2> roomsPoints = {
        {w, 0}, {2 * w, 0}, {3 * w, 0}, {4 * w, 0},
        {4 * w, h}, {3 * w, h}, {2 * w, h}, {w, h},
        {0, h}, {0, 0}
    };
    inline u_int16_t maxAmountToChange = 5;
    inline u_int16_t coolDown = 5;
}

#endif //DATA_HPP
