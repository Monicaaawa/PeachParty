#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
        // Central bank account starts off with a balance of 0
    m_bank = 0;
}

int StudentWorld::init()
{
    string board = to_string(getBoardNumber());
        // Load the specified board data file into a Board object
    Board bd;
    string board_file = assetPath() + "board0" + board + ".txt";
    Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                Board::GridEntry ge = bd.getContentsOf(i, j);
                switch (ge) {
                    case Board::empty:
                        cerr << "Location " << i << ", " << j << " is empty\n";
                        break;
                    case Board::boo:
                        cerr << "Location " << i << ", " << j << " has a Boo and a blue coin square\n";
                        actors.push_back(new Boo(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        actors.push_back(new BlueCoinSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::bowser:
                        cerr << "Location " << i << ", " << j << " has a Bowser and a blue coin square\n";
                        actors.push_back(new Bowser(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        actors.push_back(new BlueCoinSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::player: {
                        cerr << "Location " << i << ", " << j << " has Peach & Yoshi and a blue coin square\n";
                        players.push_back(new Player(this, IID_PEACH, SPRITE_WIDTH * i, SPRITE_WIDTH * j, 1));
                        players.push_back(new Player(this, IID_YOSHI, SPRITE_WIDTH * i, SPRITE_WIDTH * j, 2));
                        actors.push_back(new BlueCoinSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    }
                    case Board::red_coin_square:
                        cerr << "Location " << i << ", " << j << " has a red coin square\n";
                        actors.push_back(new RedCoinSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::blue_coin_square:
                        cerr << "Location " << i << ", " << j << " has a blue coin square\n";
                        actors.push_back(new BlueCoinSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::up_dir_square:
                        cerr << "Location " << i << ", " << j << " has a up direction square\n";
                        actors.push_back(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j, 90));
                        break;
                    case Board::down_dir_square:
                        cerr << "Location " << i << ", " << j << " has a down direction square\n";
                        actors.push_back(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j,270));
                        break;
                    case Board::left_dir_square:
                        cerr << "Location " << i << ", " << j << " has a left direction square\n";
                        actors.push_back(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j, 180));
                        break;
                    case Board::right_dir_square:
                        cerr << "Location " << i << ", " << j << " has a right direction square\n";
                        actors.push_back(new DirectionalSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j, 0));
                        break;
                    case Board::event_square:
                        cerr << "Location " << i << ", " << j << " has a event square\n";
                        actors.push_back(new EventSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::bank_square:
                        cerr << "Location " << i << ", " << j << " has a bank square\n";
                        actors.push_back(new BankSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                    case Board::star_square:
                        cerr << "Location " << i << ", " << j << " has a star square\n";
                        actors.push_back(new StarSquare(this, SPRITE_WIDTH * i, SPRITE_WIDTH * j));
                        break;
                }
            }
        }
            // Start the countdown timer for the 99-second game
        startCountdownTimer(99);
        return GWSTATUS_CONTINUE_GAME;
    }
    return GWSTATUS_BOARD_ERROR;
}

int StudentWorld::move()
{
        // Ask all of the actors that are currently active in the game world to do something
    for (int k = 0; k < actors.size(); k++) {
        if (actors[k]->isAlive())
            actors[k]->doSomething();
    }
        // Ask all players in the game world to do something
    for (int k = 0; k < players.size(); k++) {
        if (players[k]->isAlive())
            players[k]->doSomething();
    }
    
        // Delete any actors that have become inactive/dead during this tick
    vector<Actor*>::iterator p = actors.begin();
    while (p != actors.end()) {
        if (!(*p)->isAlive()) {
            cerr << "Delete dead actor" << endl;
            delete *p;
            p = actors.erase(p);
        }
        else
            p++;
    }
    
        // Update the status text
    ostringstream oss;
    oss << "P1 Roll: " << players[0]->getRolls()
    << " Stars: " << players[0]->getStars()
    << " $$: " << players[0]->getCoins();
    if (players[0]->hasVortex())
        oss << " VOR";
    oss << " | Time: " << timeRemaining()
    << " | Bank: " << getBank()
    << " | P2 Roll: " << players[1]->getRolls()
    << " Stars: " << players[1]->getStars()
    << " $$: " << players[1]->getCoins();
    if (players[1]->hasVortex())
        oss << " VOR";
    string s = oss.str();
    setGameStatText(s);
    
        // If time has expired for the game
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        int winner_num_stars;
        int winner_num_coins;
        int winner = decideWinnerStats(winner_num_stars, winner_num_coins);
        setFinalScore(winner_num_stars, winner_num_coins);
        if (winner == 0)
            return GWSTATUS_PEACH_WON;
        else
            return GWSTATUS_YOSHI_WON;
    }

	return GWSTATUS_CONTINUE_GAME;
}

    // Delete every actor and player in the game
void StudentWorld::cleanUp()
{
    for (int k = 0; k < actors.size(); k++)
        delete actors[k];
    actors.clear();
    for (int k = 0; k < players.size(); k++)
        delete players[k];
    players.clear();
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::isValidPosition(int x, int y) const {
    for (int k = 0; k < actors.size(); k++) {
        if (actors[k]->getX() == x && actors[k]->getY() == y) {
            return true;
        }
    }
    return false;
}

void StudentWorld::isPlayerPresent(int x, int y, ActivateOnPlayer* object) {
    for (int k = 0; k < players.size(); k++) {
        if (players[k]->getX() == x && players[k]->getY() == y)
            object->performAction(players[k]);
        else {
            if (object->isActivated(k+1))
                object->setActivationStatus(k+1);
        }
    }
}

Actor* StudentWorld::generateRandomPos() {
    return actors[randInt(0, static_cast<int>(actors.size())-1)];
}

void StudentWorld::swapPlayersPos() {
    players[0]->swapPos(*players[1]);
}

void StudentWorld::swapPlayersCoins() {
    players[0]->swapCoins(*players[1]);
}

void StudentWorld::swapPlayersStars() {
    players[0]->swapStars(*players[1]);
}

void StudentWorld::insertVortex(Vortex* vortex) {
    actors.push_back(vortex);
}

void StudentWorld::depositDroppingSquare(int x, int y) {
        // Remove the original square in the given position
    for (int k = 0; k < actors.size(); k++) {
        if (actors[k]->getX() == x && actors[k]->getY() == y) {
            actors[k]->setDead();
        }
    }
        // Insert a new Dropping Square in its place
    actors.push_back(new DroppingSquare(this, x, y));
}

bool StudentWorld::vortexHitsVillain(int x, int y) {
        // Check to see if the vortex overlaps with a villain
    for (int k = 0; k < actors.size(); k++) {
        if (actors[k]->getX() >= x - 15 && actors[k]->getX() <= x + 15 && actors[k]->getY() >= y - 15 && actors[k]->getY() <= y + 15) {
            if (actors[k]->impactedByVortex())
                return true;
        }
    }
    return false;
}

int StudentWorld::decideWinnerStats(int& winnerStars, int& winnerCoins) {
    if (players[0]->getStars() > players[1]->getStars()) {
        winnerStars = players[0]->getStars();
        winnerCoins = players[0]->getCoins();
        return 0;
    }
    else if (players[0]->getStars() < players[1]->getStars()) {
        winnerStars = players[1]->getStars();
        winnerCoins = players[1]->getCoins();
        return 1;
    }
    else {
        if (players[0]->getCoins() > players[1]->getCoins()) {
            winnerStars = players[0]->getStars();
            winnerCoins = players[0]->getCoins();
            return 0;
        }
        else if (players[0]->getCoins() < players[1]->getCoins()) {
            winnerStars = players[1]->getStars();
            winnerCoins = players[1]->getCoins();
            return 1;
        }
        else {
            int k = randInt(0, 1);
            winnerStars = players[k]->getStars();
            winnerCoins = players[k]->getCoins();
            return k;
        }
    }
}
