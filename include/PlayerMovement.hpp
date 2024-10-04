#ifndef PLAYERMOVEMENT_HPP
#define PLAYERMOVEMENT_HPP

#include "EffectManager.hpp"
#include "Entity.hpp"

class PlayerMovement
{
public:
    explicit PlayerMovement(Entity& player, const float& speed, const float& jump);

    bool correctInputs();
    Vec2 getVelocityMove(float& dt);
    void runInteract();
    void changeState(EffectManager& effectManager, size_t& currentFrame);

private:
    Entity& m_player;
    CInput& m_input;
    CState& m_state;
    CTransform& m_transf;

    float m_maxTime = 0.65f;
    bool m_isMoving = true;
    float m_speed = 0.0f;
    float m_jump = 0.0f;
    Vec2 m_facing = Vec2(0.0, 0.0f);
    float m_coyoteTimer = 0.0f;
    // for dust effect
    float m_minFallDistance = 192.f;
};
#endif //PLAYERMOVEMENT_HPP
