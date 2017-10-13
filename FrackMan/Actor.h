#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
#include "GameConstants.h"
using namespace std;

const double DIRT_SIZE = 0.25;
const int DIRT_DEPTH = 3;
const int PLAYRER_DEPTH = 0;
const int PROTESTER_DEPTH = 0;
const int BARREL_DEPTH = 2;
const int SONAR_DEPTH = 2;
const int WATERPOOL_DEPTH = 2;
const int GOLD_DEPTH = 2;
const int WATERSQUIRT_DEPTH = 1;
const int BOULDER_DEPTH = 1;

const double IMAGE_SIZE = 1.0;

const int PLAYER_X = 30;
const int PLAYER_Y = 60;
const int PROTESTER_X = 60;
const int PROTESTER_Y = 60;
const int SONAR_X = 0;
const int SONAR_Y = 60;

const int PLAYER_HITPOINTS = 10;
const int WATER = 5;
const int SONAR = 1;
const int GOLD = 0;


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

//Base classes
class Base: public GraphObject
{
public:
    Base(int IID, int startX, int startY, Direction startD,double imageSize, unsigned int imageDep, StudentWorld* sp): GraphObject(IID, startX, startY, startD, imageSize, imageDep), m_isLive(true), m_world(sp), m_IID(IID),m_leaving(false)
    {}
    
    virtual ~Base(){}
    virtual void doSomething() = 0;
    bool isLive() const {return m_isLive;}
    void setDeath(){m_isLive = false;}
    StudentWorld* getWorld() const {return m_world;}
    virtual bool isDistributedItem() {return false;}
    int myIID(){return m_IID;}
    virtual void getAnnoyed(int nPoints){}
    virtual void changeState(char state){}
    bool isLeaving(){return m_leaving;}
    virtual void changeToLeave(){m_leaving = true;}
    virtual void setStunned(int n){}
private:
    bool m_isLive;
    StudentWorld* m_world;
    int m_IID;
    char m_state;
    bool m_leaving;
};


class Actor: public Base
{
public:
    Actor(int IID, int startX, int startY, Direction startD,double imageSize, unsigned int imageDep, StudentWorld* sp, unsigned int intialHit): Base(IID, startX, startY, startD, imageSize, imageDep, sp), m_hitpoints(intialHit)
    {
        setVisible(true);
    }
    virtual void doSomething(){}
    virtual ~Actor(){}
    int hitpoints(){return m_hitpoints;}
    void decreaseHitpoints(int points){m_hitpoints-=points;}
    void moveActor(Direction dir);
private:
    unsigned int m_hitpoints;
};


class Goodies: public Base
{
public:
    Goodies(int IID, int startX, int startY, Direction startD,double imageSize, unsigned int imageDep, StudentWorld* sp): Base(IID, startX, startY, startD, imageSize, imageDep, sp){}
    virtual ~Goodies(){}
    virtual bool isVisible(){return false;}
    virtual bool isDistributedItem(){return true;}
    virtual void doSomething(){}
};

class Protester: public Actor
{
public:
    Protester(int IID, int startX, int startY, Direction startD,double imageSize, unsigned int imageDep, StudentWorld* sp, unsigned int intialHit);
    virtual ~Protester(){}
    void doSomething();
    virtual bool isLeaving(){return (m_protesterState == 'L');}
    virtual char protesterState(){return m_protesterState;}
    virtual void changeState(char state){m_protesterState = state;}
    void setTicksToWait(int n){m_ticksToWait = n;}
    int getTicksToWait(){return m_ticksToWait;}
    void setNumSquareToMoveInCurrDir(int n){m_numSquareToMoveInCurrDir = n;}
    int getNumSquareToMoveInCurrDir(){return m_numSquareToMoveInCurrDir;}
    int generateRand();
    virtual void getAnnoyed(int nPoints);
    int getStunnedTime() const{return m_stunnedTime;}
    void decreaseStunned(){m_stunnedTime--;}
    void setStunned(int n){m_stunnedTime = n;}
    
    //helper function
    void attackPlayer(int rp_x, int rp_y, int p_x, int p_y);
    void inTheSameLineAndMove(int rp_x, int rp_y, int p_x, int p_y);
    bool canMoveInTheSameLine(int rp_x, int rp_y, int p_x, int p_y);
    bool isChangePossible(int direction);
    void setNewDirAndChange(int direction);
    int canProtesterMovePerpen(Direction currDirection, Direction& perpendDirec);
    void changeDirToPerpen(Direction perpenDirection);
    bool canProtesterMove(Direction d);
    
    int protesterTick() const{return m_tick;}
    void increaseTick(){m_tick++;}
    int protesterShoutedTick() const {return m_shoutedTick;}
    void setProtesterShoutedTick(int shoutedTick){m_shoutedTick = shoutedTick;}
    int protesterPerpenTurnTick() const{return m_perpenTurnTick;}
    void setProtesterPerpenTurnTick(int perpenTurnTick){m_perpenTurnTick = perpenTurnTick;}
private:
    char m_protesterState;  // 'N' notes for not leaving; 'L' notes for leaving
    int m_ticksToWait;
    int m_numSquareToMoveInCurrDir;
    int m_stunnedTime;
    
    unsigned int m_tick;
    unsigned int m_shoutedTick;
    unsigned int m_perpenTurnTick;
    int m_IID;
    
    //private member function
    virtual bool somethingDifferent(int& nSteps, Direction& direct, int protester_x, int protester_y) = 0;
};


//Trivial classes
class Dirt: public Base
{
public:
    Dirt(double x, double y, StudentWorld* sp): Base(IID_DIRT, x, y, right, DIRT_SIZE, DIRT_DEPTH, sp)
    {
        setVisible(true);
    }
    virtual ~Dirt(){}
    virtual void doSomething(){}
    virtual bool isDistributedItem(){return false;}
};

class Boulder: public Base
{
public:
    Boulder(double x, double y, StudentWorld* sp): Base(IID_BOULDER, x, y, down, IMAGE_SIZE, BOULDER_DEPTH, sp), m_tick(0), m_state("stable")
    {
        setVisible(true);
    }
    virtual void doSomething();
    virtual bool isDistributedItem(){return true;}
    string boulderState(){return m_state;}
private:
    unsigned int m_tick;
    string m_state;
};


//Frackman and Protester classes
class Frackman: public Actor
{
public:
    Frackman(StudentWorld* sp): Actor(IID_PLAYER, PLAYER_X, PLAYER_Y, right, IMAGE_SIZE, PLAYRER_DEPTH, sp, PLAYER_HITPOINTS), m_water(WATER), m_sonar(SONAR), m_gold(GOLD){}
    virtual ~Frackman(){}
    virtual void doSomething();
    int playerWater(){return m_water;}
    int playerGold(){return m_gold;}
    int playerSonar(){return m_sonar;}
    void increaseSonar(){m_sonar += 1;}
    void increaseWater(){m_water+=5;}
    void increaseGold(){m_gold += 1;}
private:
    unsigned int m_water;
    unsigned int m_sonar;
    unsigned int m_gold;
};

// Goodies classes
class Barrel: public Goodies
{
public:
    Barrel(double x, double y, StudentWorld* sp): Goodies(IID_BARREL, x, y, right, IMAGE_SIZE, BARREL_DEPTH, sp), m_visibility(false){}
    virtual ~Barrel(){}
    bool isVisible(){return m_visibility;}
    void setBarrelVisible()
    {
        m_visibility = true;
        setVisible(true);
    }
    virtual void doSomething();
private:
    bool m_visibility;
};


class Sonar: public Goodies
{
public:
    Sonar(StudentWorld* sp);
    virtual ~Sonar(){}
    void doSomething();
private:
    unsigned int m_tick;
};

class Waterpool: public Goodies
{
public:
    Waterpool(double x, double y, StudentWorld* sp);
    virtual ~Waterpool(){}
    void doSomething();
private:
    unsigned int m_tick;
};

class Gold: public Goodies
{
public:
    Gold(double x, double y, StudentWorld* sp): Goodies(IID_GOLD, x, y, right, IMAGE_SIZE,GOLD_DEPTH, sp), m_visibility(false){}
    virtual ~Gold(){}
    virtual bool isVisible(){return m_visibility;}
    virtual void setGoldVisible()
    {
        m_visibility = true;
        setVisible(true);
    }
    virtual void doSomething();
private:
    bool m_visibility;
    unsigned int m_tick;
};

class Bribe: public Goodies
{
public:
    Bribe(double x, double y, StudentWorld* sp);
    virtual ~Bribe(){}
    virtual void doSomething();
private:
    unsigned int m_tick;
};

class Squirt: public Goodies
{
public:
    Squirt(double x, double y, Direction startD,StudentWorld* sp): Goodies(IID_WATER_SPURT, x, y, startD, IMAGE_SIZE, WATERSQUIRT_DEPTH, sp), m_tick(4), m_direction(startD){setVisible(true);}
    virtual ~Squirt(){}
    virtual void doSomething();
private:
    unsigned int m_tick;
    Direction m_direction;
};

class regularProtester: public Protester
{
public:
    regularProtester(StudentWorld* sp): Protester(IID_PROTESTER, PROTESTER_X, PROTESTER_Y, left, IMAGE_SIZE,PROTESTER_DEPTH, sp, 5){}
    virtual ~regularProtester(){}
private:
    bool somethingDifferent(int& nSteps, Direction& direct, int protester_x, int protester_y){return false;}
};

class HardCoreProtester: public Protester
{
public:
    HardCoreProtester(StudentWorld* sp): Protester(IID_HARD_CORE_PROTESTER, PROTESTER_X, PROTESTER_Y, left, IMAGE_SIZE, PROTESTER_DEPTH, sp, 20){}
    virtual ~HardCoreProtester(){}
private:
    bool somethingDifferent(int& nSteps, Direction& direct, int protester_x, int protester_y);
};



#endif // ACTOR_H_
