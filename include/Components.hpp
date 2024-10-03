#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <vector>

#include "Animation.hpp"
#include "Tags.hpp"
#include "Vec2.hpp"

typedef unsigned short u_int16_t;

constexpr int kInvincibility = 60;

class Component
{
public:
    bool active = false;
};

class CConsumable: public Component
{
public:
    CConsumable() = default;

    explicit CConsumable(const u_int16_t cooldown):
        cooldown(cooldown) {}

    u_int16_t amount = 0;
    size_t frameCreated = 0;
    u_int16_t cooldown = 0;
};

class CDraggable: public Component
{
public:
    CDraggable() = default;

    bool dragging = false;
};

class CClimbable: public Component
{
public:
    CClimbable() = default;

    bool climbing = true;
};

class CTransform: public Component
{
public:
    CTransform() = default;

    explicit CTransform(const Vec2& p):
        pos(p) {}

    CTransform(const Vec2& p, const Vec2& speed, const Vec2& s, float a):
        pos(p), prevPos(p), velocity(speed), scale(s), angle(a) {}

    Vec2 pos = {0.0f, 0.0f};
    Vec2 prevPos = {0.0f, 0.0f};
    Vec2 scale = {1.0f, 1.0f};
    Vec2 velocity = {0.0f, 0.0f};
    Vec2 facing = {0.0f, 1.0f};
    float angle = 0;
};

class CLifespan: public Component
{
public:
    CLifespan() = default;

    explicit CLifespan(int duration, int frame):
        lifespan(duration), frameCreated(frame) {}

    int lifespan = 0;
    int frameCreated = 0;
};

class CDamage: public Component
{
public:
    CDamage() = default;

    explicit CDamage(int d):
        damage(d) {}

    int damage = 1;
};

class CInvincibility: public Component
{
public:
    CInvincibility() = default;

    explicit CInvincibility(int f):
        iframes(f) {}

    int iframes = 1;
};

class CHealth: public Component
{
public:
    CHealth() = default;

    CHealth(int m, int c):
        max(m), current(c) {}

    int max = 1;
    int current = 1;
};

class CBuff: public Component
{
public:
    CBuff() = default;

    CBuff(bool shield, bool speed):
        shield(shield), speed(speed) {}

    CBuff(bool shield, bool speed, size_t frameCreated, u_int16_t cooldown):
        shield(shield), speed(speed), frameCreated(frameCreated), cooldown(cooldown) {}

    bool shield = false;
    bool speed = false;
    size_t frameCreated = 0;
    u_int16_t cooldown = 0;
};

class CInput: public Component
{
public:
    CInput() = default;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool interact = false;
    bool attack = false;
    bool key1 = false;
    bool key2 = false;
    bool key3 = false;
};

class CBoundingBox: public Component
{
public:
    CBoundingBox() = default;

    explicit CBoundingBox(const Vec2& s):
        size(s), halfSize(s.x / 2, s.y / 2) {}

    CBoundingBox(const Vec2& s, bool m, bool v):
        size(s), halfSize(s.x / 2.0f, s.y / 2.0f), blockMove(m), blockVision(v) {}

    Vec2 size;
    Vec2 halfSize;
    bool blockMove = false;
    bool blockVision = false;
};

class CInteractableBox: public Component
{
public:
    CInteractableBox() = default;

    explicit CInteractableBox(const Vec2& s):
        size(s), sizeOneHalf(s.x * 1.5f, s.y * 1.5f) {}

    Vec2 size;
    Vec2 sizeOneHalf;
};

class CAnimation: public Component
{
public:
    CAnimation() = default;

    CAnimation(Animation a, bool r):
        animation(std::move(a)), repeat(r) {}

    Animation animation;
    bool repeat = false;
    bool paused = false;
};

class CState: public Component
{
public:
    CState() = default;

    explicit CState(std::string s):
        state(std::move(s)) {}

    std::string state = "Stand";
    bool inAir = true;
    bool changed = false;
    bool canJump = false;
    bool canAttack = false;
    bool climbing = false;
    std::string interAction;
};

class CGravity: public Component
{
public:
    CGravity() = default;

    explicit CGravity(const float g):
        gravity(g) {};

    float gravity = 0;
};

class CFollowPlayer: public Component
{
public:
    CFollowPlayer() = default;

    CFollowPlayer(Vec2 p, float s):
        home(p), speed(s) {}

    Vec2 home = {0.0f, 0.0f};
    float speed = 0.0f;
};

class CSurprise: public Component
{
public:
    CSurprise() = default;

    CSurprise(int rx, int ry, int tx, int ty, int t):
        roomX(rx), roomY(ry), tileX(tx), tileY(ty), tagId(static_cast<TagName>(t))
    {
        if (tagId != eConsumable && tagId != eNpc && tagId != eInteractable)
        {
            tagId = eInteractable;
        }
    }

    int roomX = 0;
    int roomY = 0;
    int tileX = 0;
    int tileY = 0;
    TagName tagId = eInteractable;
    bool isActivated = false;
};

enum KeyType { eDoorBig = 99, eDoorSmall = 97, eChestBig = 95, eChestSmall = 93, eNoKey = 90 };

enum LockTypeAction { eExit = 1, eRandomJump = 2, eExactJump = 3, eGrabAll = 4, eNoLockType = 0 };

class CLockable: public Component
{
public:
    CLockable() = default;

    explicit CLockable(bool c):
        isOpen(c) {}

    CLockable(bool c, bool l, int t, int a):
        isOpen(c), isLocked(l), keyType(static_cast<KeyType>(t)), action(static_cast<LockTypeAction>(a)) {}

    bool isOpen = false;
    bool isLocked = false;
    bool isActivated = false;
    KeyType keyType = eNoKey;
    LockTypeAction action = eNoLockType;
};

class Triggerable: public Component
{
public:
    Triggerable() = default;

    explicit Triggerable(bool a):
        isActivated(a) {}

    bool isActivated = false;
};

class CMovable: public Component
{
public:
    CMovable() = default;

    CMovable(std::vector<Vec2>& pos, float s, bool isMoving = false):
        positions(pos), speed(s), isMoving(isMoving) {}

    std::vector<Vec2> positions;
    size_t currentPosition = 0;
    float speed = 0;
    float isMoving = false;
};

#endif //COMPONENTS_H
