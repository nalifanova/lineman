#ifndef DATA_HPP
#define DATA_HPP

#include <unordered_map>

#include "GameEngine.hpp"

namespace game {
    inline auto w = static_cast<float>(kWinWidth);
    inline auto h = static_cast<float>(kWinHeight);
    inline std::vector<Vec2> roomVertices = {
        {w, 0}, {2 * w, 0}, {3 * w, 0},
        {3 * w, h}, {2 * w, h}, {w, h},
        {0, h}, {0, 0}
    };
    inline u_int16_t maxAmountToChange = 5;
    inline u_int16_t coolDown = 5;
    inline float dropSpeed = 5.5f;

    // eExit = 1, eRandomJump = 2, eExactJump = 3, eGrabAll = 4, eNoLockType = 0
    inline std::unordered_map<u_int16_t, std::string> interActions {
        {1, "Exit"}, {2, "RandomJump"}
    };
}

#endif //DATA_HPP
