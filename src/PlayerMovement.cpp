#include "PlayerMovement.hpp"

#include "DeltaTime.hpp"
#include "GameEngine.hpp"

PlayerMovement::PlayerMovement(Entity& player, const float& speed, const float& jump):
    m_player(player),
    m_speed(speed),
    m_jump(jump),
    m_input(player.get<CInput>()),
    m_state(player.get<CState>()),
    m_transf(player.get<CTransform>()) {}

bool PlayerMovement::correctInputs()
{
    if ((m_input.up && m_input.down) || (m_input.left && m_input.right) || (m_input.up && m_input.attack) ||
        (m_input.down && m_input.attack) || (m_input.left && m_input.attack) || (m_input.right && m_input.attack)
    )
    {
        m_player.get<CAnimation>().paused = true;
        return false;
    }
    return true;
}

Vec2 PlayerMovement::getVelocityMove(float& dt)
{
    Vec2 playerVelocity(m_transf.velocity.x, 0);

    if (!m_state.inAir) { m_coyoteTimer = 0.1f; }
    else { m_coyoteTimer -= dt; }

    if (!m_input.up)
    {
        dt = 0.0f;
        if (m_coyoteTimer <= 0.0f) { m_state.canJump = false; }
    }
    if (m_input.up)
    {
        if (m_state.climbing)
        {
            m_state.inAir = false;
            m_state.canJump = true;
            playerVelocity.y = -m_speed;
        }
        else
        {
            if ((m_state.canJump && m_maxTime >= dt) || m_coyoteTimer > 0.0f)
            {
                playerVelocity.y = -m_jump;
                m_state.inAir = true;
                m_coyoteTimer = 0.0f;
            }
            else
            {
                dt = 0.0f;
                m_state.canJump = false;
            }
        }
        m_facing = Vec2(0.0, 1.0f);
    }
    else if (m_input.down)
    {
        if (m_state.climbing)
        {
            m_state.inAir = false;
            m_state.canJump = true;
        }
        m_facing = Vec2(0.0, -1.0f);
        playerVelocity.y += m_speed;
    }

    if (m_input.right)
    {
        playerVelocity.x = m_speed;
        m_facing = Vec2(1.0, 0.0f);
    }
    else if (m_input.left)
    {
        playerVelocity.x = -m_speed;
        m_facing = Vec2(-1.0, 0.0f);
    }
    else
    {
        playerVelocity.x = 0;
        m_isMoving = false;
    }

    if (m_transf.facing != m_facing)
    {
        m_transf.facing = m_facing; // update
        m_state.changed = true;
    }

    return playerVelocity;
}

void PlayerMovement::runInteract()
{
    // attack / move
    if (m_input.attack)
    {
        if (m_state.state != "Attack")
        {
            m_state.state = "Attack";
            m_state.changed = true;
            // might spawn smth
        }
    }
    else if (m_input.interact)
    {
        if (m_state.state != "Interact")
        {
            m_state.state = "Interact";
            m_state.changed = true;
            // might consume something
        }
    }
}

void PlayerMovement::changeState(EffectManager& effectManager, size_t& currentFrame)
{
    if (m_state.climbing)
    {
        if (m_state.state != "Climb")
        {
            m_state.state = "Climb";
            m_state.changed = true;
        }
    }
    else if (m_isMoving)
    {
        if (m_state.inAir)
        {
            m_state.state = "Air";
            m_state.changed = true;
        }
        else if (m_state.state != "Walk")
        {
            m_state.state = "Walk";
            m_state.changed = true;
        }
    }
    else
    {
        if (m_state.inAir)
        {
            m_state.state = "Air";
            m_state.changed = true;
        }
        else if (m_state.state != "Stand")
        {
            m_state.state = "Stand";
            m_state.changed = true;
            if (m_transf.prevPos.y + 3.f < m_transf.pos.y)
            {
                // delta 24.f is based on halfSize of bbox of entity
                effectManager.createDust(Vec2(m_transf.pos.x, m_transf.pos.y + 24.f), 100, currentFrame);
            }
        }
    }
}
