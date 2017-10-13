#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

void Actor::moveActor(Direction dir)
{
    int x = getX();
    int y = getY();
    if(dir == left)
        moveTo( x- 1, y);
    
    if(dir == right)
        moveTo(x + 1, y);
    
    if(dir == up)
        moveTo(x, y + 1);
    
    if(dir == down)
        moveTo(x, y - 1);
}

Protester::Protester(int IID, int startX, int startY, Direction startD,double imageSize, unsigned int imageDep, StudentWorld* sp, unsigned int intialHit): Actor(IID, startX, startY, startD,imageSize, imageDep, sp, intialHit), m_protesterState('N'), m_ticksToWait(0),m_stunnedTime(0), m_tick(0), m_shoutedTick(0), m_perpenTurnTick(0), m_IID(IID)
{
    setVisible(true);
    m_numSquareToMoveInCurrDir = rand()% 53 + 8;
    int n = max(0, 3 - (getWorld()->gameLevel())/4);
    setTicksToWait(n);
}

void Protester::getAnnoyed(int nPoints)
{
    if(m_protesterState == 'L')
        return;
    
    decreaseHitpoints(nPoints);
    if(hitpoints()<=0)
    {
        changeState('L');
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        if(nPoints == 100)
            getWorld()->increaseScore(500);
        
        if(nPoints == 2)
        {
            if(myIID() == IID_PROTESTER)
                getWorld()->increaseScore(100);
            
            if(myIID() == IID_HARD_CORE_PROTESTER)
                getWorld()->increaseScore(250);
        }
    }
    
    else
    {
        getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        m_stunnedTime = max(50, 100 - (getWorld()->gameLevel())* 10);
    }
}


void Frackman::doSomething()
{
    // return if Frackman dies
    if(!isLive())
        return;
    
    
    int x = getX();
    int y = getY();
    int direction = getDirection();
    
    // Remove Dirt and play digging sound
    if(getWorld()->overlappedDirt(x, y))
    {
        getWorld()->reMoveDirt(x, y);
        getWorld()->playSound(SOUND_DIG);
    }
    
    //get input from users
    //move the frackman
    else
    {
        int key;
        if(getWorld()->getKey(key) == true)
        {
            switch (key)
            {
                case KEY_PRESS_LEFT:
                    if(direction == left && getX() >= 1)
                    {
                        if(getWorld()->canMove(x - 1, y))
                            moveTo(x - 1, y);
                        break;
                    }
                    
                    setDirection(left);
                    moveTo(x, y);
                    
                    break;
                    
                case KEY_PRESS_RIGHT:
                    if(direction == right && getX() < VIEW_WIDTH - SPRITE_WIDTH)
                    {
                        if(getWorld()->canMove(x + 1, y))
                            moveTo(x + 1, y);
                        break;
                    }
                    
                    setDirection(right);
                    moveTo(x, y);
                    
                    break;
                    
                case KEY_PRESS_UP:
                    if(direction == up && getY() < VIEW_HEIGHT - SPRITE_HEIGHT)
                    {
                        if(getWorld()->canMove(x, y + 1))
                            moveTo(x, y + 1);
                        break;
                    }
                    
                    setDirection(up);
                    moveTo(x, y);
                    
                    break;
                    
                case KEY_PRESS_DOWN:
                    if(direction == down && getY() >= 1)
                    {
                        if(getWorld()->canMove(x, y - 1))
                            moveTo(x, y - 1);
                        break;
                    }
                    
                    
                    setDirection(down);
                    moveTo(x, y);
                    
                    break;
                    
                
                    
                case KEY_PRESS_ESCAPE:
                    setDeath();
                    getWorld()->decLives();
                    return;
                
                    
                case 'Z':
                case 'z':
                    if(m_sonar > 0)
                    {
                        m_sonar--;
                        getWorld()->showUp();
                        getWorld()->playSound(SOUND_SONAR);
                    }
                    
                    break;
                    
                case KEY_PRESS_TAB:
                    if(m_gold > 0)
                    {
                        m_gold--;
                        getWorld()->newBribe(x, y);
                        
                    }
                    break;
                    
                case KEY_PRESS_SPACE:
                    if(m_water > 0)
                    {
                        if(direction == left)
                            getWorld()->newSquirt(x - 4, y, getDirection());
                        if(direction == right)
                            getWorld()->newSquirt(x + 4, y, getDirection());
                        if(direction == up)
                            getWorld()->newSquirt(x, y + 4, getDirection());
                        if(direction == down)
                            getWorld()->newSquirt(x, y - 4, getDirection());
                        m_water--;
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    if(hitpoints() <= 0)
    {
        setDeath();
        getWorld()->decLives();
        getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
    }
}


void Boulder::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        if(m_state == "stable" && !(getWorld()->isDirtExist(getX(), getY())))
            m_state = "waiting";
    
        else if(m_state == "waiting")
        {
            m_tick++;
    
            if(m_tick == 30)
            {
                m_state = "falling";
                getWorld()->playSound(SOUND_FALLING_ROCK);
            }
        }
    
        else if(m_state == "falling")
        {
            int x = getX();
            int y = getY();
        
            if(y >= 1 && !(getWorld()->isDirtExist(x, y)) &&
                !getWorld()->isFallingOnBoulder(x, y, this))
            {
                moveTo(x, y - 1);
                getWorld()->playerNearFallingBoulder(this);
                getWorld()->protesterNearFallingBoudler(this);
            }
                
            else
                setDeath();
        }
    }
}

void Barrel::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
      if(!isVisible() && getWorld()->isPlayerLessThan(4, getX(), getY()))
      {
          setBarrelVisible();
          return;
      }
        
      else if(getWorld()->isPlayerLessThan(3, getX(), getY()))
      {
          setDeath();
          getWorld()->decreaseBarrel();
          getWorld()->increaseScore(1000);
          getWorld()->playSound(SOUND_FOUND_OIL);
          
      }
          
    }
}

Sonar::Sonar(StudentWorld* sp): Goodies(IID_SONAR, SONAR_X, SONAR_Y, right, IMAGE_SIZE, SONAR_DEPTH, sp)
{
    setVisible(true);
    m_tick = max(100, 300-10*(getWorld()->gameLevel()));
}

void Sonar::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        if(m_tick != 0)
        {
            if(getWorld()->isPlayerLessThan(3, getX(), getY()))
            {
                setDeath();
                getWorld()->playSound(SOUND_GOT_GOODIE);
                getWorld()->increaseSonar();
                getWorld()->increaseScore(75);
            }
            m_tick--;
        }
        
        else
            setDeath();
    }
}


Waterpool::Waterpool(double x, double y, StudentWorld* sp):Goodies(IID_WATER_POOL, x, y, right,IMAGE_SIZE, WATERPOOL_DEPTH, sp)
{
    setVisible(true);
    m_tick = max(100, 300-10*(getWorld()->gameLevel()));
}

void Waterpool::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        if(m_tick != 0)
        {
            if(getWorld()->isPlayerLessThan(3, getX(), getY()))
            {
                setDeath();
                getWorld()->playSound(SOUND_GOT_GOODIE);
                getWorld()->increaseWater();
                getWorld()->increaseScore(100);
            }
            m_tick--;
        }
        
        else
            setDeath();
    }
}


void Gold::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        if(!isVisible() && getWorld()->isPlayerLessThan(4, getX(), getY()))
        {
            setGoldVisible();
            return;
        }
        
        else if(getWorld()->isPlayerLessThan(3, getX(), getY()))
        {
            setDeath();
            getWorld()->increaseGold();
            getWorld()->increaseScore(10);
            getWorld()->playSound(SOUND_GOT_GOODIE);
            
        }
    }
}

Bribe::Bribe(double x, double y, StudentWorld*sp):Goodies(IID_GOLD, x, y, right, IMAGE_SIZE, GOLD_DEPTH, sp)
{
    setVisible(true);
     m_tick = max(100, 300-10*(getWorld()->gameLevel()));
}

void Bribe::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        if(m_tick != 0)
        {
            char type = ' ';
            if(getWorld()->isProtesterLessThan(3, getX(), getY(), type))
            {
                if(type == 'R')
                {
                    setDeath();
                    getWorld()->increaseScore(25);
                }
                
                if(type == 'H')
                {
                    setDeath();
                    getWorld()->increaseScore(50);
                    getWorld()->hgNearBribe(this);

                    
                }
                getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
                getWorld()->protesterPickedBribe(this);
            }
            m_tick--;
        }
        
        else
            setDeath();
    }
}

void Squirt::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        int s_x = getX();
        int s_y = getY();
        char type = ' ';
    
        if(m_tick == 0)
            setDeath();
    
        else if(getWorld()->isProtesterLessThan(3, s_x, s_y, type))
        {
            getWorld()->protesterNearSquirt(this);
            setDeath();
        }
    
        else
        {
            if(getWorld()->canSquirtMove(getX(), getY()))
            {
                if(m_direction == left)
                {
                    if(s_x <= 0)
                        setDeath();
                
                    else if(getWorld()->canSquirtMove(s_x - 1, s_y))
                    {
                        moveTo(s_x - 1, s_y);
                    }
                    else
                        setDeath();
                }
            
                if(m_direction == right)
                {
                    if(s_x >= 60)
                        setDeath();
                
                    else if(getWorld()->canSquirtMove(s_x + 4, s_y))
                    {
                        moveTo(s_x + 1, s_y);
                    }
                    else
                        setDeath();
                }
            
                if(m_direction == up)
                {
                    if(s_y >= 60)
                        setDeath();
                   
                    else if(getWorld()->canSquirtMove(s_x, s_y + 4))
                    {
                        moveTo(s_x, s_y + 1);
                    }
                    else
                        setDeath();
                }
            
                if(m_direction == down)
                {
                    if(s_y <= 0)
                        setDeath();
                
                    else if(getWorld()->canSquirtMove(s_x, s_y - 1))
                    {
                        moveTo(s_x, s_y - 1);
                    }
                    else
                        setDeath();
                }
            }
        
            else
                setDeath();
        }
        m_tick--;
    }
}


int Protester::generateRand()
{
    int n = rand() % 4 + 1;
    return n;
}


// regular protester member function

void Protester::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        int rp_x = getX();
        int rp_y = getY();
        int p_x = getWorld()->playerX();
        int p_y = getWorld()->playerY();
        int nSteps = -1;
        Direction direct = none;
        
        if(getStunnedTime() > 0 && protesterState() != 'L')
        {
            decreaseStunned();
            increaseTick();
            return;
        }
        
        
        if(getTicksToWait() != 0 && (protesterTick() % (1+getTicksToWait()) != 0))
        {
            increaseTick();
            return;
        }
        
        else    // not in a rest state
        {
            if(protesterState() == 'L')
            {
                if(getX()  == 60 && getY() == 60)
                {
                    setDeath();
                    getWorld()->decreaseProtester();
                }
                
                else
                {
                    Direction d = getWorld()->leaveTheField(this);
                    setDirection(d);
                    if(d == up)
                        moveTo(rp_x, rp_y + 1);
                    if(d == down)
                        moveTo(rp_x, rp_y - 1);
                    if(d == left)
                        moveTo(rp_x - 1, rp_y);
                    if(d == right)
                        moveTo(rp_x + 1, rp_y);
                    increaseTick();
                    return;
                }
            }
            
            else    // not in a rest state, not in a leaving state
            {
                if(getWorld()->isPlayerLessThan(4, rp_x, rp_y)) //if less than 4 and face
                {
                    if(protesterShoutedTick() == 0)
                    {
                        attackPlayer(rp_x, rp_y, p_x, p_y);
                        increaseTick();
                        return;
                    }
                    
                    else if(((protesterTick() - protesterShoutedTick()) >= (15 * (1+getTicksToWait()))))
                    {
                        attackPlayer(rp_x, rp_y, p_x, p_y);
                        increaseTick();
                        return;
                    }
                }
                
                else if(somethingDifferent(nSteps, direct, rp_x, rp_y))
                {
                    Direction d = direct;
                    setDirection(d);
                    if(d == up)
                        moveTo(rp_x, rp_y + 1);
                    if(d == down)
                        moveTo(rp_x, rp_y - 1);
                    if(d == left)
                        moveTo(rp_x - 1, rp_y);
                    if(d == right)
                        moveTo(rp_x + 1, rp_y);
                    increaseTick();
                    return;
                }
                
                else if((rp_x == p_x || rp_y == p_y) && canMoveInTheSameLine(rp_x, rp_y, p_x, p_y))   // not in a rest state, not within 4, in the same line, can move
                {
                    
                    inTheSameLineAndMove(rp_x, rp_y, p_x, p_y);
                    increaseTick();
                    return;
                }
                
                else    // not in a rest state, not within 4, not in the same lines
                {
                    setNumSquareToMoveInCurrDir(getNumSquareToMoveInCurrDir() - 1);
                    if(getNumSquareToMoveInCurrDir() <= 0)
                    {
                        int directionNum = generateRand();
                        while(!isChangePossible(directionNum))
                            directionNum = generateRand();
                        
                        //already generate changeable direction
                        setNewDirAndChange(directionNum);
                    }
                    
                    else
                    {
                        Direction perpenDirction = none;
                        int numOfPer = canProtesterMovePerpen(getDirection(), perpenDirction);
                        if(numOfPer!= 0)
                        {
                            // if there are two directions, choose one randomly
                            if(numOfPer == 2)
                            {
                                int n = rand() % 2 + 1;
                                if(n == 1)
                                {
                                    if(getDirection() == up)
                                        perpenDirction = left;
                                    
                                    if(getDirection() == left)
                                        perpenDirction = up;
                                    
                                    if(getDirection() == down)
                                        perpenDirction = right;
                                    
                                    if(getDirection() == right)
                                        perpenDirction = down;
                                }
                                
                                if(n == 2)
                                {
                                    if(getDirection() == up)
                                        perpenDirction = right;
                                    
                                    if(getDirection() == left)
                                        perpenDirction = down;
                                    
                                    if(getDirection() == down)
                                        perpenDirction = left;
                                    
                                    if(getDirection() == right)
                                        perpenDirction = up;
                                }
                            }
                            
                            if((protesterTick()- protesterPerpenTurnTick()) >= (200 * (1+getTicksToWait())))
                            {
                                changeDirToPerpen(perpenDirction);
                            }
                        }
                    }
                    
                if(canProtesterMove(getDirection()))
                    moveActor(getDirection());
                    
                else
                {
                    setNumSquareToMoveInCurrDir(0);
                }
            }
                
                
            }
            
            
        }
        increaseTick();
    }
    
    if(hitpoints() <= 0)
    {
        changeState('L');

        changeToLeave();
    }

}


/*void HardCoreProtester::doSomething()
{
    if(!isLive())
        return;
    
    else
    {
        int rp_x = getX();
        int rp_y = getY();
        int p_x = getWorld()->playerX();
        int p_y = getWorld()->playerY();
        int nSteps = -1;
        Direction direct = none;
        
        if(getStunnedTime() > 0 && protesterState() != 'L')
        {
            decreaseStunned();
            increaseTick();
            return;
        }
        
        
        if(getTicksToWait() != 0 && (protesterTick() % (1+getTicksToWait()) != 0))
        {
            increaseTick();
            return;
        }
        
        else    // not in a rest state
        {
            if(protesterState() == 'L')
            {
                if(getX()  == 60 && getY() == 60)
                {
                    setDeath();
                    getWorld()->decreaseProtester();
                }
                
                else
                {
                    Direction d = getWorld()->leaveTheField(this);
                    setDirection(d);
                    if(d == up)
                        moveTo(rp_x, rp_y + 1);
                    if(d == down)
                        moveTo(rp_x, rp_y - 1);
                    if(d == left)
                        moveTo(rp_x - 1, rp_y);
                    if(d == right)
                        moveTo(rp_x + 1, rp_y);
                    increaseTick();
                    return;
                }
            }
            
            else    // not in a rest state, not in a leaving state
            {
                
                if(getWorld()->isPlayerLessThan(4, rp_x, rp_y)) //if less than 4 and face
                {
                    if(protesterShoutedTick() == 0)
                    {
                        attackPlayer(rp_x, rp_y, p_x, p_y);
                        increaseTick();
                        return;
                    }
                    
                    else if(((protesterTick() - protesterShoutedTick()) >= (15 * (1+getTicksToWait()))))
                    {
                        attackPlayer(rp_x, rp_y, p_x, p_y);
                        increaseTick();
                        return;
                    }
                }

                
                else if(getWorld()->hardCoreSense(this, nSteps, direct) && nSteps <= (16 + 2 * getWorld()->gameLevel()))
                {
                    Direction d = direct;
                    setDirection(d);
                    if(d == up)
                        moveTo(rp_x, rp_y + 1);
                    if(d == down)
                        moveTo(rp_x, rp_y - 1);
                    if(d == left)
                        moveTo(rp_x - 1, rp_y);
                    if(d == right)
                        moveTo(rp_x + 1, rp_y);
                    increaseTick();
                    return;
                }
                
                else if((rp_x == p_x || rp_y == p_y) && canMoveInTheSameLine(rp_x, rp_y, p_x, p_y))   // not in a rest state, not within 4, in the same line, can move
                {
                    
                    inTheSameLineAndMove(rp_x, rp_y, p_x, p_y);
                    increaseTick();
                    return;
                }
                
                else    // not in a rest state, not within 4, not in the same lines
                {
                    setNumSquareToMoveInCurrDir(getNumSquareToMoveInCurrDir() - 1);
                    if(getNumSquareToMoveInCurrDir() <= 0)
                    {
                        int directionNum = generateRand();
                        while(!isChangePossible(directionNum))
                            directionNum = generateRand();
                        
                        //already generate changeable direction
                        setNewDirAndChange(directionNum);
                    }
                    
                    else
                    {
                        Direction perpenDirction = none;
                        int numOfPer = canProtesterMovePerpen(getDirection(), perpenDirction);
                        if(numOfPer!= 0)
                        {
                            // if there are two directions, choose one randomly
                            if(numOfPer == 2)
                            {
                                int n = rand() % 2 + 1;
                                if(n == 1)
                                {
                                    if(getDirection() == up)
                                        perpenDirction = left;
                                    
                                    if(getDirection() == left)
                                        perpenDirction = up;
                                    
                                    if(getDirection() == down)
                                        perpenDirction = right;
                                    
                                    if(getDirection() == right)
                                        perpenDirction = down;
                                }
                                
                                if(n == 2)
                                {
                                    if(getDirection() == up)
                                        perpenDirction = right;
                                    
                                    if(getDirection() == left)
                                        perpenDirction = down;
                                    
                                    if(getDirection() == down)
                                        perpenDirction = left;
                                    
                                    if(getDirection() == right)
                                        perpenDirction = up;
                                }
                            }
                            
                            if((protesterTick()- protesterPerpenTurnTick()) >= (200 * (1+getTicksToWait())))
                            {
                                changeDirToPerpen(perpenDirction);
                            }
                        }
                    }
                    
                    if(canProtesterMove(getDirection()))
                        moveActor(getDirection());
                    
                    else
                    {
                        setNumSquareToMoveInCurrDir(0);
                    }
                }
                
                
            }
            
            
        }
        increaseTick();
    }
    
    if(hitpoints() <= 0)
    {
        changeState('L');
        
        changeToLeave();
    }
    

}*/

/*bool Protester::somethingDifferent(int nSteps, Direction direct, int protester_x, int protester_y)
{
    if(m_IID == IID_PROTESTER)
        return false;
    
    if(m_IID == IID_HARD_CORE_PROTESTER)
    {
        if(getWorld()->hardCoreSense(this, nSteps, direct) && nSteps <= (16 + 2 * getWorld()->gameLevel()))
        {
            Direction d = direct;
            setDirection(d);
            if(d == up)
                moveTo(protester_x, protester_y + 1);
            if(d == down)
                moveTo(protester_x, protester_y - 1);
            if(d == left)
                moveTo(protester_x - 1, protester_y);
            if(d == right)
                moveTo(protester_x + 1, protester_y);
            return true;
        }
    }
    
    return false;
}*/

bool HardCoreProtester::somethingDifferent(int& nSteps, Direction& direct, int protester_x, int protester_y)
{
    if(getWorld()->hardCoreSense(this, nSteps, direct) && nSteps <= (16 + 2 * getWorld()->gameLevel()))
        return true;
    
    return false;
}



//Protester helper function
void Protester::attackPlayer(int rp_x, int rp_y, int p_x, int p_y)
{
    int direction = getDirection();
    if(direction == left && p_x < rp_x)
    {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->decreasePlayerHitpoints(2);
        setProtesterShoutedTick(m_tick);
        setStunned(15 * (1+getTicksToWait()));
        return;
    }
    
    if(direction == right && p_x > rp_x)
    {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->decreasePlayerHitpoints(2);
        setProtesterShoutedTick(m_tick);
        setStunned(15 * (1+getTicksToWait()));
        return;
    }
    
    if(direction == up && p_y > rp_y)
    {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->decreasePlayerHitpoints(2);
        setProtesterShoutedTick(m_tick);
        setStunned(15 * (1+getTicksToWait()));
        return;
    }
    
    if(direction == down && p_y < rp_y)
    {
        getWorld()->playSound(SOUND_PROTESTER_YELL);
        getWorld()->decreasePlayerHitpoints(2);
        setProtesterShoutedTick(m_tick);
        setStunned(15 * (1+getTicksToWait()));
        return;
    }
    
}

bool Protester::canMoveInTheSameLine(int rp_x, int rp_y, int p_x, int p_y)
{
    if(rp_x == p_x)
    {
        if(rp_y < p_y)
        {
            int i = 0;
            for(i = rp_y; i < p_y; i++)
            {
                if(!getWorld()->overlappedDirt(rp_x, i) && getWorld()->canMove(rp_x, i))
                    continue;
                
                else
                    break;
            }
            
            if(i == p_y)
                return true;
        }
        
        else
        {
            int i = 0;
            for(i = rp_y; i> p_y; i--)
            {
                if(!getWorld()->overlappedDirt(rp_x, i) && getWorld()->canMove(rp_x, i))
                    continue;
                
                else
                    break;
            }
            
            if(i == p_y)
                return true;
        }
    }
    
    if(rp_y == p_y)
    {
        if(rp_x < p_x)
        {
            int i = 0;
            for(i = rp_x; i < p_x; i++)
            {
                if(!getWorld()->overlappedDirt(i, rp_y) && getWorld()->canMove(i, rp_y))
                    continue;
                
                else
                    break;
            }
            
            if(i == p_x)
                return true;
        }
        
        else
        {
            int i = 0;
            for(i = rp_x; i > p_x; i--)
            {
                if(!getWorld()->overlappedDirt(i, rp_y) && getWorld()->canMove(i, rp_y))
                    continue;
                
                else
                    break;
            }
            
            if(i == p_x)
                return true;
        }
    }
    return false;
}


void Protester::inTheSameLineAndMove(int rp_x, int rp_y, int p_x, int p_y)
{
    if(rp_x == p_x)
    {
        if(rp_y < p_y)
        {
            setDirection(up);
            moveTo(rp_x, rp_y + 1);
            setNumSquareToMoveInCurrDir(0);
        }
        
        else
        {
            setDirection(down);
            moveTo(rp_x, rp_y - 1);
            setNumSquareToMoveInCurrDir(0);
        }
    }
    
    else if(rp_y == p_y)
    {
        if(rp_x < p_x)
        {
        
            setDirection(right);
            moveTo(rp_x + 1, rp_y);
            setNumSquareToMoveInCurrDir(0);
        }
        
        else
        {
            setDirection(left);
            moveTo(rp_x - 1, rp_y);
            setNumSquareToMoveInCurrDir(0);
        }
    }
}

bool Protester::isChangePossible(int direction)
{
    int rp_x = getX();
    int rp_y = getY();
    if(direction == 1)
    {
        setDirection(up);
        if(rp_y >= VIEW_HEIGHT - SPRITE_HEIGHT || getWorld()->overlappedDirt(rp_x, rp_y + 1) || !getWorld()->canMove(rp_x, rp_y + 1))
            return false;
    }
    
    if(direction == 2)
    {
        setDirection(down);
        if(rp_y < 1 || getWorld()->overlappedDirt(rp_x, rp_y - 1) || !getWorld()->canMove(rp_x, rp_y - 1) )
            return false;
    }
    
    if(direction == 3)
    {
        setDirection(left);
        if(rp_x < 1 ||  getWorld()->overlappedDirt(rp_x - 1, rp_y) || !getWorld()->canMove(rp_x - 1, rp_y))
            return false;
    }
    
    if(direction == 4)
    {
        setDirection(right);
        if(rp_x >= VIEW_WIDTH - SPRITE_WIDTH || getWorld()->overlappedDirt(rp_x + 1, rp_y) || !getWorld()->canMove(rp_x + 1, rp_y))
            return false;
    }
    
    return true;

}

void Protester::setNewDirAndChange(int direction)
{
    int random = rand() % 53 + 8;
    setNumSquareToMoveInCurrDir(random);
    
    if(direction == 1)
    {
        setDirection(up);
    }
    
    if(direction == 2)
    {
        setDirection(down);
    }
    
    if(direction == 3)
    {
        setDirection(left);
    }
    
    if(direction == 4)
    {
        setDirection(right);
    }
    
}

int Protester::canProtesterMovePerpen(Direction currDirection, Direction& perpenDirec)
{
    int numDirections = 0;
    int rp_x = getX();
    int rp_y = getY();
    if(currDirection == up || currDirection == down)
    {
        // can move left
        if(rp_x >= 1 && !getWorld()->overlappedDirt(rp_x - 1, rp_y) && getWorld()->canMove(rp_x - 1, rp_y))
        {
            numDirections++;
            perpenDirec = left;
        }
        
        // can move right
        if(rp_x < VIEW_WIDTH - SPRITE_WIDTH && !getWorld()->overlappedDirt(rp_x + 1, rp_y) && getWorld()->canMove(rp_x + 1, rp_y))
        {
            numDirections++;
            perpenDirec = right;
        }
    }
    
    
    if(currDirection == left || currDirection == right)
    {
        //can move down
        if(rp_y >= 1 && !getWorld()->overlappedDirt(rp_x, rp_y - 1) && getWorld()->canMove(rp_x, rp_y - 1))
        {
            numDirections++;
            perpenDirec = down;
        }
        
        // can move up
        if(rp_y < VIEW_HEIGHT - SPRITE_HEIGHT && !getWorld()->overlappedDirt(rp_x, rp_y + 1) && getWorld()->canMove(rp_x, rp_y + 1))
        {
            numDirections++;
            perpenDirec = up;
        }
    }
    
    return numDirections;
    
}

void Protester::changeDirToPerpen(Direction perpenDirection)
{
    setDirection(perpenDirection);
    int num = rand() % 53 + 8;
    setNumSquareToMoveInCurrDir(num);
    setProtesterPerpenTurnTick(m_tick);
    
    if(perpenDirection == up)
    {
        setDirection(up);
    }
    
    if(perpenDirection == down)
    {
        setDirection(down);
    }
    
    if(perpenDirection == left)
    {
        setDirection(left);
    }
    
    if(perpenDirection == right)
    {
        setDirection(right);
    }

}

bool Protester::canProtesterMove(Direction d)
{
    int rp_x = getX();
    int rp_y = getY();
    if(d == up)
    {
        if(rp_y >= VIEW_HEIGHT - SPRITE_HEIGHT || getWorld()->overlappedDirt(rp_x, rp_y + 1) || !getWorld()->canMove(rp_x, rp_y + 1))
            return false;
    }
    
    if(d == down)
    {
        if(rp_y < 1 || getWorld()->overlappedDirt(rp_x, rp_y - 1) || !getWorld()->canMove(rp_x, rp_y - 1) )
            return false;
    }
    
    if(d == left)
    {
        if(rp_x < 1 ||  getWorld()->overlappedDirt(rp_x - 1, rp_y) || !getWorld()->canMove(rp_x - 1, rp_y))
            return false;
    }
    
    
    if(d == right)
    {
        if(rp_x >= VIEW_WIDTH - SPRITE_WIDTH || getWorld()->overlappedDirt(rp_x + 1, rp_y) || !getWorld()->canMove(rp_x + 1, rp_y))
            return false;
    }
    
    return true;
    
}











