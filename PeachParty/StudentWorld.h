#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Player;
class ActivateOnPlayer;
class Vortex;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isValidPosition(int x, int y) const;
    void isPlayerPresent(int x, int y, ActivateOnPlayer* object);
    int getBank() const { return m_bank; }
    void setBank(int value) { m_bank = value; }
    Actor* generateRandomPos();
    void swapPlayersPos();
    void swapPlayersCoins();
    void swapPlayersStars();
    void insertVortex(Vortex* vortex);
    void depositDroppingSquare(int x, int y);
    bool vortexHitsVillain(int x, int y);
    
private:
    std::vector<Actor*> actors;
    std::vector<Player*> players;
    int m_bank;
    int decideWinnerStats(int& winnerStars, int& winnerCoins);
};

#endif // STUDENTWORLD_H_
