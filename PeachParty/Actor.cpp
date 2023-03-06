#include "Actor.h"
#include "StudentWorld.h"
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

template<typename T>
void swapStats(T& stats1, T& stats2);

    // Check if the direction that a player is attempting to walk in is a valid direction
bool Player::isValidDirection(int dir) const {
    switch (dir) {
        case right:
            return getWorld()->isValidPosition(getX()+16, getY());
            break;
        case left:
            return getWorld()->isValidPosition(getX()-16, getY());
            break;
        case up:
            return getWorld()->isValidPosition(getX(), getY()+16);
            break;
        case down:
            return getWorld()->isValidPosition(getX(), getY()-16);
            break;
        default:
            return false;
            break;
    }
}

void Player::doSomething() {
        // Store all valid directions that a player can walk in
    vector<int> validDirections;
    if (isValidDirection(right))
        validDirections.push_back(0);
    if (isValidDirection(left))
        validDirections.push_back(180);
    if (isValidDirection(up))
        validDirections.push_back(90);
    if (isValidDirection(down))
        validDirections.push_back(270);
    
        // If the player is in the waiting to roll state
    if (!m_state) {
            // If the player was justed teleported
        if (m_get_teleported == true) {
                // If the player has an invalid direction, pick a random valid direction
            if(!isValidDirection(m_walk_direction)) {
                cerr << "Picked a random direction" << endl;
                m_walk_direction = validDirections[randInt(0, static_cast<int>(validDirections.size())-1)];
                updateSpriteDirection();
            }
            m_get_teleported = false;
        }
        
        switch (getWorld()->getAction(m_playerNumber)) {
            case ACTION_NONE:
                return;
                break;
                // Roll the dice
            case ACTION_ROLL: {
                int die_roll = randInt(1, 10);
                m_ticks_to_move = die_roll * 8;
                m_state = true;
                break;
            }
                // Fire the vortex
            case ACTION_FIRE: {
                    // Check if player has vortex
                if (m_has_vortex) {
                    int vortexX;
                    int vortexY;
                        // Create vortex in player's current walk direction
                    getPositionInThisDirection(m_walk_direction, 16, vortexX, vortexY);
                    getWorld()->insertVortex(new Vortex(getWorld(), vortexX, vortexY, m_walk_direction));
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    setHasVortex(false);
                }
                return;
                break;
            }
        }
    }
        // If the player is in the walking state
    else {
        cerr << "walking" << endl;
            // If the player is directly on top of a directional square
        if (m_on_directional_square == true) {
            cerr << "On directional square" << endl;
            m_walk_direction = m_square_direction;
            updateSpriteDirection();
            m_on_directional_square = false;
        }
            // If the player is directly on top of a square at a fork
        else if (validDirections.size() >= 3) {
            cerr << "At a fork" << endl;
            
                // When landing on a fork, a player can't choose the direction they came from
            int positionComeFrom;
            if (m_walk_direction >= 180)
                positionComeFrom = m_walk_direction - 180;
            else
                positionComeFrom = m_walk_direction + 180;
            cerr << "Position come from is " << positionComeFrom << endl;

            switch (getWorld()->getAction(m_playerNumber)) {
                case ACTION_NONE:
                    return;
                    break;
                case ACTION_LEFT:
                    if (isValidDirection(left) && left != positionComeFrom)
                        m_walk_direction = left;
                    else
                        return;
                    break;
                case ACTION_RIGHT:
                    if (isValidDirection(right) && right != positionComeFrom)
                        m_walk_direction = right;
                    else
                        return;
                    break;
                case ACTION_UP:
                    if (isValidDirection(up) && up != positionComeFrom)
                        m_walk_direction = up;
                    else
                        return;
                    break;
                case ACTION_DOWN:
                    if (isValidDirection(down) && down != positionComeFrom)
                        m_walk_direction = down;
                    else
                        return;
                    break;
                default:
                    return;
            }
            updateSpriteDirection();
        }
            // If the Avatar can't continue moving forward in its current direction
        else if (!isValidDirection(m_walk_direction)) {
            cerr << m_walk_direction << " is not valid direction" << endl;
            switch (m_walk_direction) {
                case right:
                case left:
                        // Always prefer up over down if both options are available
                    if (isValidDirection(up))
                        m_walk_direction = up;
                    else if (isValidDirection(down))
                        m_walk_direction = down;
                    break;
                    
                case up:
                case down:
                        // Always prefer right over left if both options are available
                    if (isValidDirection(right))
                        m_walk_direction = right;
                    else if (isValidDirection(left))
                        m_walk_direction = left;
                    break;
            }
            cerr << "Direction changed to " << m_walk_direction << endl;
            updateSpriteDirection();
        }
        
        moveAtAngle(m_walk_direction, 2);
        cerr << "Walk direction is " << m_walk_direction << endl;
        cerr << "The current location is " << getX() << " " << getY() << endl;
        
        m_ticks_to_move--;
        
        if (m_ticks_to_move == 0) {
            m_state = false;
        }
    }
}

    // Update the direction the player's sprite faces based on the walk direction
void Player::updateSpriteDirection() {
        // If the walk direction is left, the sprite direction must be 180 degrees
    if (m_walk_direction == left)
        setDirection(left);
        // For all other walk directions, the sprite direction must be 0 degrees
    else
        setDirection(right);
}

    // Randomly teleport the player to another square on the board
void Player::teleportPlayer() {
    Actor* pos = getWorld()->generateRandomPos();
    moveTo(pos->getX(), pos->getY());
    m_get_teleported = true;
}

void Player::swapPos(Player& other) {
        // the player's roll/walk state
    swapStats(m_state, other.m_state);
        // the player's walk direction
    swapStats(m_walk_direction, other.m_walk_direction);
        // the number of ticks left that the player has to move before completing their roll
    swapStats(m_ticks_to_move, other.m_ticks_to_move);
        // the player's roll/walk state
    swapStats(m_state, other.m_state);
        // the player's sprite direction
    int tempDirection = this->getDirection();
    this->setDirection(other.getDirection());
    other.setDirection(tempDirection);
        // x, y coordinates
    int tempX = this->getX();
    int tempY = this->getY();
    this->moveTo(other.getX(), other.getY());
    other.moveTo(tempX, tempY);
}

void Player::swapCoins(Player& other) {
    swapStats(m_coins, other.m_coins);
}

void Player::swapStars(Player& other) {
    swapStats(m_stars, other.m_stars);
}

bool ActivateOnPlayer::isActivated(int playerNum) const {
    if (playerNum == 1)
        return activatedByPlayer1;
    else
        return activatedByPlayer2;
}

void ActivateOnPlayer::setActivationStatus(int playerNum) {
    if (playerNum == 1)
        activatedByPlayer1 = !activatedByPlayer1;
    else
        activatedByPlayer2 = !activatedByPlayer2;
}

void ActivateOnPlayer::doSomething() {
    getWorld()->isPlayerPresent(getX(), getY(), this);
}

void CoinSquare::doSomething() {
    if (!isAlive())
        return;
    ActivateOnPlayer::doSomething();
}

void BlueCoinSquare::performAction(Player* player) {
        // Blue coin squares are only activated if a new player has landed upon the square
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
        getWorld()->playSound(SOUND_GIVE_COIN);
        player->setCoins(player->getCoins()+3);
        setActivationStatus(player->getPlayerNumber());
    }
}

void RedCoinSquare::performAction(Player* player) {
        // Red coin squares are only activated if a new player has landed upon the square
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
        getWorld()->playSound(SOUND_TAKE_COIN);
        
            // Deduct 3 coins from the player
        if (player->getCoins() >= 3)
            player->setCoins(player->getCoins()-3);
            // If the player has fewer than 3 coins, deduct as many as the player has
        else
            player->setCoins(0);
        
        setActivationStatus(player->getPlayerNumber());
    }
}

void StarSquare::performAction(Player* player) {
    if (!isActivated(player->getPlayerNumber())) {
            // Star squares won't activate if the player has less than 20 coins
        if (player->getCoins() < 20)
            return;
        else {
            getWorld()->playSound(SOUND_GIVE_STAR);
                // Deduct 20 coins from the player and then give them one star
            player->setCoins(player->getCoins()-20);
            player->setStars(player->getStars()+1);
        }
        
        setActivationStatus(player->getPlayerNumber());
    }
}

void DirectionalSquare::performAction(Player* player) {
    if (!isActivated(player->getPlayerNumber())) {
        player->activateDirectionalSquare(m_forcing_direction);
        setActivationStatus(player->getPlayerNumber());
    }
}

void BankSquare::performAction(Player* player) {
    if (!isActivated(player->getPlayerNumber())) {
            // If the player lands on (but does not pass over) the Bank square
        if (!player->getState()){
                // Gives the player ALL coins in the bank account
            player->setCoins(getWorld()->getBank()+player->getCoins());
            getWorld()->setBank(0);
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
        }
            // If the player passes over (but does not land on) the Bank square
        else {
                // Deduct up to 5 coins from the player and deposits it in the bank account
            if (player->getCoins() >= 5) {
                player->setCoins(player->getCoins()-5);
                getWorld()->setBank(getWorld()->getBank()+5);
            }
            else {
                getWorld()->setBank(getWorld()->getBank()+player->getCoins());
                player->setCoins(0);
            }
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
        }
        
        setActivationStatus(player->getPlayerNumber());
    }
}

void EventSquare::performAction(Player* player) {
        // Event squares are only activated if a new player has landed upon the square
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
            // Choose randomly amongst the following actions
        int choice = randInt(1, 3);
        switch (choice) {
                // Teleport the player to another random square on the board
            case 1:
                cerr << "Player is teleported" << endl;
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                player->teleportPlayer();
                setActivationStatus(player->getPlayerNumber());
                break;
                
                // Swap the positions and movement states of two players
            case 2:
                cerr << "Players swap positions" << endl;
                getWorld()->swapPlayersPos();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                setActivationStatus(1);
                setActivationStatus(2);
                break;
                
                // Give the player a Vortex projectile with which they can later shoot a baddie
            case 3:
                cerr << "Player recieved vortex" << endl;
                if (!player->hasVortex()) {
                    player->setHasVortex(true);
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                }
                setActivationStatus(player->getPlayerNumber());
                break;
        }
    }
}

void DroppingSquare::performAction(Player* player) {
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
        int choice = randInt(1, 2);
        switch (choice) {
                // Deduct 10 coins from the player (or as many as the player has)
            case 1:
                cerr << "Dropping square deducts coins" << endl;
                if (player->getCoins() >= 10)
                    player->setCoins(player->getCoins()-10);
                else
                    player->setCoins(0);
                break;
                
                // Deduct one star from the player if the player has at least one star
            case 2:
                cerr << "Dropping square deducts stars" << endl;
                if (player->getStars() >= 1)
                    player->setStars(player->getStars()-1);
                break;
        }
        
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        setActivationStatus(player->getPlayerNumber());
    }
}

void Vortex::doSomething() {
    if (!isAlive())
        return;
    
    moveAtAngle(m_fire_direction, 2);
    
        // If the Vortex leaves the boundaries of the screen
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT) {
        setDead();
        cerr << "Vortex is out of bound" << endl;
    }
    
        // If the Vortex currently overlaps with an object that can be impacted by a Vortex object
    if (getWorld()->vortexHitsVillain(getX(), getY())) {
        setDead();
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
        cerr << "Vortex hits a villain" << endl;
    }
    
}

void Villain::doSomething() {
        // Store all valid directions that a villain can walk in
    vector<int> validDirections;
    if (isValidDirection(right))
        validDirections.push_back(0);
    if (isValidDirection(left))
        validDirections.push_back(180);
    if (isValidDirection(up))
        validDirections.push_back(90);
    if (isValidDirection(down))
        validDirections.push_back(270);
    
        // If the villain is in the Paused state
    if (m_state == false) {
        ActivateOnPlayer::doSomething();
        m_pause_counter--;
        if (m_pause_counter == 0) {
            int squares_to_move = randSquares();
            m_ticks_to_move = squares_to_move * 8;
                // Pick a new random direction for the villain to walk that is legal
            m_walk_direction = validDirections[randInt(0, static_cast<int>(validDirections.size())-1)];
            updateSpriteDirection();
            m_state = true;
        }
    }
        // If the villain is in the Walking state
    else {
            // If the villain is at a fork where there are multiple directions he could choose to move
        if (validDirections.size() >= 3) {
            m_walk_direction = validDirections[randInt(0, static_cast<int>(validDirections.size())-1)];
            updateSpriteDirection();
        }
            // If the villain is at a turning point
        else if (!isValidDirection(m_walk_direction)) {
            switch (m_walk_direction) {
                case right:
                case left:
                    if (isValidDirection(up))
                        m_walk_direction = up;
                    else if (isValidDirection(down))
                        m_walk_direction = down;
                    break;
                    
                case up:
                case down:
                    if (isValidDirection(right))
                        m_walk_direction = right;
                    else if (isValidDirection(left))
                        m_walk_direction = left;
                    break;
            }
            updateSpriteDirection();
        }
        
        moveAtAngle(m_walk_direction, 2);
        m_ticks_to_move--;
        
        if (m_ticks_to_move == 0) {
            m_state = false;
            m_pause_counter = 180;
            ticksToZero();
        }
    }
}

    // If a villain is hit by a vortex
bool Villain::impactedByVortex() {
    Actor* pos = getWorld()->generateRandomPos();
    moveTo(pos->getX(), pos->getY());
    m_walk_direction = right;
    setDirection(0);
    m_state = false;
    m_pause_counter = 180;
    return true;
}

bool Villain::isValidDirection(int dir) const {
    switch (dir) {
        case right:
            return getWorld()->isValidPosition(getX()+16, getY());
            break;
        case left:
            return getWorld()->isValidPosition(getX()-16, getY());
            break;
        case up:
            return getWorld()->isValidPosition(getX(), getY()+16);
            break;
        case down:
            return getWorld()->isValidPosition(getX(), getY()-16);
            break;
        default:
            return false;
            break;
    }
}

void Villain::updateSpriteDirection() {
    if (m_walk_direction == left)
        setDirection(left);
    else
        setDirection(right);
}

void Bowser::performAction(Player* player) {
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
            // There is a 50% chance that Bowser will cause the player to lose all their coins and stars
        bool choice = randInt(0, 1);
        if (choice) {
            cerr << "Bowser clears " << player->getPlayerNumber() << "s coins and stars" << endl;
            player->setCoins(0);
            player->setStars(0);
            getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
        }
        setActivationStatus(player->getPlayerNumber());
    }
}

void Bowser::ticksToZero() {
        // There is a 25% chance that Bowser will deposit a dropping square
    int k = randInt(1, 4);
    if (k == 1) {
        cerr << "Bowser deposits a dropping square" << endl;
        getWorld()->depositDroppingSquare(getX(), getY());
        getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
    }
}

void Boo::performAction(Player* player) {
    if (!player->getState() && !isActivated(player->getPlayerNumber())) {
            // Boo will randomly execute one of the following two options
        bool choice = randInt(0, 1);
        if (choice) {
                // Swap that player's coins with the other player
            cerr << "Boo swaps players' coins" << endl;
            getWorld()->swapPlayersCoins();
        }
        else {
                // Swap that player's stars with the other player
            cerr << "Boo swaps players' stars" << endl;
            getWorld()->swapPlayersStars();
        }
        getWorld()->playSound(SOUND_BOO_ACTIVATE);
        setActivationStatus(player->getPlayerNumber());
    }
}

template<typename T>
void swapStats(T& stats1, T& stats2) {
    T temp = stats1;
    stats1 = stats2;
    stats2 = temp;
}
