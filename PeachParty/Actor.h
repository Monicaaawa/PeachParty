#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <string>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject
{
public:
        // Constructor
    Actor(StudentWorld* world, int imageID, int startX, int startY, int dir = right, int depth = 0, double size = 1.0)
    : GraphObject(imageID, startX, startY, dir, depth, size), m_world(world), m_alive(true) {}
        // Destructor
    virtual ~Actor() {}
    
        // Getters
    StudentWorld* getWorld() const { return m_world; }
    bool isAlive() const { return m_alive; }
    
    virtual void setDead() { m_alive = false; }
    virtual void doSomething() = 0;
    virtual bool impactedByVortex() { return false; }
    
private:
    StudentWorld* m_world;
    bool m_alive;
};

class Player : public Actor
{
public:
        // Constructor
        // A Player object has a sprite direction of 0 degrees, a starting walk direction of right, a graphical depth of 0, and a size of 1
    Player(StudentWorld* world, int imageID, int startX, int startY, int playerNumber)
    : Actor(world, imageID, startX, startY, right, 0, 1.0), m_playerNumber(playerNumber), m_state(false), m_walk_direction(right), m_ticks_to_move(0), m_coins(0), m_stars(0),m_has_vortex(false), m_on_directional_square(false), m_get_teleported(false) {}
    
        // Getters
    int getPlayerNumber() const { return m_playerNumber; }
    int getState() const { return m_state; }
    int getCoins() const { return m_coins; }
    int getStars() const { return m_stars; }
    int getRolls() const { return (m_ticks_to_move + 7) / 8; }
    bool hasVortex() const { return m_has_vortex; }
    
        // Setters
    void setCoins(int coinNumber) { m_coins = coinNumber; }
    void setStars(int starNumber) { m_stars = starNumber; }
    void setHasVortex(bool hasVortex) { m_has_vortex = hasVortex; }
    void activateDirectionalSquare(int dir) { m_on_directional_square = true; m_square_direction = dir; }
    
    virtual void doSomething();
    virtual void setDead() {}
    void teleportPlayer();
    void swapPos(Player& other);
    void swapCoins(Player& other);
    void swapStars(Player& other);
    
private:
        // 1 means player 1 (left side of keyboard), 2 means player 2 (right side of keyboard)
    int m_playerNumber;
        // false represents waiting to roll and true represents walking
    bool m_state;
    int m_walk_direction;
    int m_ticks_to_move;
    int m_coins;
    int m_stars;
    bool m_has_vortex;
    bool m_on_directional_square;
    int m_square_direction;
    bool m_get_teleported;
    
        // Private helper functions
    bool isValidDirection(int dir) const;
    void updateSpriteDirection();
};

class ActivateOnPlayer : public Actor
{
public:
        // Constructor
    ActivateOnPlayer(StudentWorld* world, int imageID, int startX, int startY, int dir = right)
    : Actor(world, imageID, startX, startY, dir, 1, 1.0), activatedByPlayer1(false), activatedByPlayer2(false) {}
    
        // Destructor
    virtual ~ActivateOnPlayer() {}
    
    virtual void doSomething();
    bool isActivated(int playerNum) const;
    void setActivationStatus(int playerNum);
    virtual void performAction(Player* player) = 0;
    
private:
    bool activatedByPlayer1;
    bool activatedByPlayer2;
};

class CoinSquare : public ActivateOnPlayer
{
public:
    CoinSquare(StudentWorld* world, int imageID, int startX, int startY)
    : ActivateOnPlayer(world, imageID, startX, startY) {}
    virtual ~CoinSquare() {}
    
    virtual void doSomething();
    virtual void performAction(Player* player) = 0;
};

class BlueCoinSquare : public CoinSquare {
public:
    BlueCoinSquare(StudentWorld* world, int startX, int startY)
    : CoinSquare(world, IID_BLUE_COIN_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class RedCoinSquare : public CoinSquare {
public:
    RedCoinSquare(StudentWorld* world, int startX, int startY)
    : CoinSquare(world, IID_RED_COIN_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class StarSquare : public ActivateOnPlayer
{
public:
    StarSquare(StudentWorld* world, int startX, int startY)
    : ActivateOnPlayer(world, IID_STAR_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class DirectionalSquare : public ActivateOnPlayer
{
public:
    DirectionalSquare(StudentWorld* world, int startX, int startY, int dir)
    : ActivateOnPlayer(world, IID_DIR_SQUARE, startX, startY, dir), m_forcing_direction(dir) {}
    virtual void performAction(Player* player);
    
private:
    int m_forcing_direction;
};

class BankSquare : public ActivateOnPlayer {
public:
    BankSquare(StudentWorld* world, int startX, int startY)
    : ActivateOnPlayer(world, IID_BANK_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class EventSquare : public ActivateOnPlayer {
public:
    EventSquare(StudentWorld* world, int startX, int startY)
    : ActivateOnPlayer(world, IID_EVENT_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class DroppingSquare : public ActivateOnPlayer {
public:
    DroppingSquare(StudentWorld* world, int startX, int startY)
    : ActivateOnPlayer(world, IID_DROPPING_SQUARE, startX, startY) {}
    virtual void performAction(Player* player);
};

class Villain : public ActivateOnPlayer {
public:
    Villain(StudentWorld* world, int imageID, int startX, int startY)
    :  ActivateOnPlayer(world, imageID, startX, startY), m_pause_counter(180), m_state(false), m_walk_direction(right), m_ticks_to_move(0), m_get_teleported(false) {}
    virtual ~Villain() {}
    virtual void doSomething();
    virtual void setDead() {}
    virtual void performAction(Player* player) = 0;
    virtual int randSquares() = 0;
    virtual void ticksToZero() {}
    virtual bool impactedByVortex();
    
private:
    int m_pause_counter;
    bool m_state;
    int m_walk_direction;
    int m_ticks_to_move;
    bool m_get_teleported;
    bool isValidDirection(int dir) const;
    void updateSpriteDirection();
};

class Bowser : public Villain {
public:
    Bowser(StudentWorld* world, int startX, int startY): Villain(world, IID_BOWSER, startX, startY) {}
    virtual int randSquares() { return randInt(1, 10); }
    virtual void performAction(Player* player);
    virtual void ticksToZero();
};

class Boo : public Villain {
public:
    Boo(StudentWorld* world, int startX, int startY): Villain(world, IID_BOO, startX, startY) {}
    virtual int randSquares() { return randInt(1, 3); }
    virtual void performAction(Player* player);
};

class Vortex : public Actor {
public:
    Vortex(StudentWorld* world, int startX, int startY, int fireDirection)
    : Actor(world, IID_VORTEX, startX, startY, right, 0, 1.0), m_fire_direction(fireDirection) {}
    virtual void doSomething();
    
private:
    int m_fire_direction;
};

template<typename T>
void swapStats(T& stats1, T& stats2);

#endif // ACTOR_H_
