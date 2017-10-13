#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include "GraphObject.h"// check later
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Base;
class Frackman;
class Dirt;
class Boulder;
class Barrel;
class Squirt;
class Bribe;
class Protester;

class Coord
{
public:
    Coord(int x, int y):m_x(x), m_y(y){}
    int xCoord(){return m_x;}
    int yCoord(){return m_y;}
private:
    int m_x;
    int m_y;

};


class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir)
    : GameWorld(assetDir)
    {
        for(int i = 0; i < VIEW_WIDTH; i++)
        {
            for(int j = 0; j < VIEW_HEIGHT; j++)
                m_dirt[i][j] = nullptr;
        }
        
         srand( static_cast<unsigned int>(time(NULL)));
    }
    
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void updateTextDisplay();
    
    //helper functions
    GraphObject::Direction leaveTheField(Protester* sp);
    bool hardCoreSense(Protester* sp, int& nSteps, GraphObject::Direction& trackDir);
    bool overlappedDirt(int xCoord, int yCoord) const;
    void reMoveDirt(int xCoord, int yCoord);
    void removeDeadItems();
    bool isDirtExist(int xCoord, int yCoord) const;
    bool canMove(int xCoord, int yCoord);
    bool isRadiusLessThanThree(int xCoord1, int yCoord1, int xCoord2, int yCoord2);
    void hgNearBribe(Bribe* bp);
    bool isFallingOnBoulder(int xCoord, int yCoord, Base* sp);
    void playerNearFallingBoulder(Boulder* bp);
    void protesterNearFallingBoudler(Boulder* bp);
    void protesterNearSquirt(Squirt* sp);
    bool isPlayerLessThan(int distance, int x, int y);// x and y coord of other obejcts
    bool isProtesterLessThan(int distance, int x, int y, char& type);// x and y: coord of other obejcts
    void decreaseBarrel(){m_barrel -= 1;}
    void increaseSonar();
    void increaseWater();
    void increaseGold();
    void newBribe(int x, int y);
    void newSquirt(int x, int y, GraphObject::Direction startD);
    void showUp();
    bool canSquirtMove(int x, int y);// x, y for squirt
    int playerX();
    int playerY();
    void decreasePlayerHitpoints(int hitpoints);
    void protesterPickedBribe(Bribe* bp);
    void decreaseProtester(){m_protesterNum--;}
    int gameLevel(){return getLevel();}
    
private:
    //member variable
    vector<Base*> m_actor;
    Frackman* m_player;
    Dirt* m_dirt[VIEW_WIDTH][VIEW_HEIGHT];
    int m_barrel;
    int m_protesterNum;
    unsigned int m_tick;
    int m_level;
    unsigned int lastTimeAdd;
    
    //private function
    bool validNewCoord(int& xCoord, int& yCoord);
    int barrelLeft(){return m_barrel;}
    string textData(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrelLeft);
    void generateSonarOrWaterpool();
    bool isDirtLess(int xCoord, int yCoord);
    void addNewProtester();
};

#endif // STUDENTWORLD_H_
