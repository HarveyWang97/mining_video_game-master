#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include "GraphObject.h"    //check it later
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <queue>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
    return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
int StudentWorld::init()
{
    //Dirt initialization
    for(int i = 0; i < (VIEW_WIDTH - SPRITE_WIDTH) / 2; i++)
    {
        for(int j = 0; j < (VIEW_HEIGHT - SPRITE_HEIGHT); j++)
            m_dirt[i][j] = new Dirt(i, j, this);
    }
    
    for(int i = (VIEW_WIDTH - SPRITE_WIDTH)/2;i < (VIEW_WIDTH - SPRITE_WIDTH)/2 + SPRITE_WIDTH;i++)
    {
        for(int j = 0; j < SPRITE_HEIGHT; j++)
            m_dirt[i][j] = new Dirt(i, j, this);
    }
    
    for(int i = (VIEW_WIDTH - SPRITE_WIDTH)/2 + SPRITE_WIDTH; i < VIEW_WIDTH; i++)
    {
        for(int j = 0; j < (VIEW_HEIGHT - SPRITE_HEIGHT); j++)
            m_dirt[i][j] = new Dirt(i, j, this);
    }
    
    //player initialization
    m_player = new Frackman(this);
    
    //distributable items initialization
    int level = getLevel();
    
    //Boulder distribution
    int B = 0;
    
    while(B < min(level / 2 + 2, 6))
    {
        int b_x;
        int b_y;
        if(!validNewCoord(b_x, b_y))
            continue;
        
        m_actor.push_back(new Boulder(b_x, b_y, this));
        reMoveDirt(b_x, b_y);
        B++;
    }
    
    //Barrel distribution
    int L = 0;
    m_barrel = min(2 + level, 20);
    while(L < m_barrel)
    {
        int ba_x;
        int ba_y;
        if(!validNewCoord(ba_x, ba_y))
            continue;
        
        m_actor.push_back(new Barrel(ba_x, ba_y, this));
        L++;
    }
    
    //Gold distribution
    int G = 0;
    while(G < max(5-level / 2, 2))
    {
        int g_x;
        int g_y;
        if(!validNewCoord(g_x, g_y))
            continue;
        
        m_actor.push_back(new Gold(g_x, g_y, this));
        G++;
    }
    
    m_tick = 0;
    m_protesterNum = 0;
    lastTimeAdd = 0;
    m_level = gameLevel();
    
    
    return GWSTATUS_CONTINUE_GAME;
}

StudentWorld::~StudentWorld()
{
    delete m_player;
    
    for(int i = 0; i < VIEW_HEIGHT; i++)
        for(int j = 0; j < VIEW_WIDTH ; j++)
            delete m_dirt[i][j];
}

int StudentWorld::move()
{
    
    updateTextDisplay();
    
    //generate sonar and waterpool
    generateSonarOrWaterpool();
    
    //Protester distribution
    addNewProtester();

    //ask each character to do something
    m_player->doSomething();
    
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->isLive())
        {
            m_actor[i]->doSomething();
        
            if(!m_player->isLive())
                return GWSTATUS_PLAYER_DIED;
            
            if(barrelLeft() == 0)
                return GWSTATUS_FINISHED_LEVEL;
        }
    }
    
    
    removeDeadItems();
    
    if(!m_player->isLive())
        return GWSTATUS_PLAYER_DIED;
    
    if(barrelLeft() == 0)
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    //player cleanup
    delete m_player;
    m_player = nullptr;
    
    //dirt cleanup
    for(int i = 0; i < VIEW_HEIGHT; i++)
        for(int j = 0; j < VIEW_WIDTH ; j++)
        {
            delete m_dirt[i][j];
            m_dirt[i][j] = nullptr;
        }
    
    //vector cleanup
    vector<Base*>::iterator baseIter = m_actor.begin();
    while(baseIter != m_actor.end())
    {
        delete (*baseIter);
        baseIter = m_actor.erase(baseIter);
    }
}

void StudentWorld::updateTextDisplay()
{
    int score = getScore();
    int level = getLevel();
    int lives = getLives();
    int health = m_player->hitpoints() * 10;
    int squirts = m_player->playerWater();
    int gold = m_player->playerGold();
    int sonar = m_player->playerSonar();
    int barrel = barrelLeft();
    string s = textData(score, level, lives, health, squirts, gold, sonar, barrel);
    setGameStatText(s);
}


//helper function
GraphObject::Direction StudentWorld::leaveTheField(Protester* sp)
{
    bool path[61][61];
    for(int i = 0; i < 61; i++)
    {
        for(int j = 0; j < 61; j++)
        {
            if(!canMove(i, j))
                path[i][j] = false;
            else if(!isDirtLess(i, j))
                path[i][j] = false;
            else
                path[i][j] = true;
        }
    }
    
    
    queue<Coord> record;
    record.push(Coord(60,60));
    path[60][60] = false;
    int p_x = sp->getX();
    int p_y = sp->getY();
    
    while(!record.empty())
    {
        Coord curr = record.front();
        record.pop();
        int x = curr.xCoord();
        int y = curr.yCoord();
        
        if(x <= 59 && path[x + 1][y])
        {
            record.push(Coord(x+1, y));
            path[x+1][y] = false;
            if(p_x == x+1 && p_y == y)
                return Actor::left;
        }
        
        if(x >= 1 && path[x - 1][y])
        {
            record.push(Coord(x-1, y));
            path[x-1][y] = false;
            if(p_x == x-1 && p_y == y)
                return Actor::right;
        }
        
        if(y >= 1 && path[x][y - 1])
        {
            record.push(Coord(x,y-1));
            path[x][y-1] = false;
            if(p_x == x && p_y == y-1)
                return Actor::up;
        }
        
        if(y <= 59 && path[x][y+1])
        {
            record.push(Coord(x,y+1));
            path[x][y+1] = false;
            if(p_x == x && p_y == y+1)
                return Actor::down;
        }
    }
    
    return Actor::none;
}


bool StudentWorld::hardCoreSense(Protester *sp, int& nSteps, GraphObject::Direction& trackDir)
{
    bool path[61][61];
    int step[61][61];
    for(int i = 0; i < 61; i++)
    {
        for(int j = 0; j < 61; j++)
        {
            if(!canMove(i, j))
            {
                path[i][j] = false;
                step[i][j] = -1;
            }
            else if(!isDirtLess(i, j))
            {
                path[i][j] = false;
                step[i][j] = -1;
            }
            else
            {
                path[i][j] = true;
                step[i][j] = 0;
            }
        }
    }
    
    
    queue<Coord> record;
    int p_x = m_player->getX();
    int p_y = m_player->getY();
    int hp_x = sp->getX();
    int hp_y = sp->getY();
    record.push(Coord(p_x, p_y));
    path[p_x][p_y] = false;
    step[p_x][p_y] = 0;
    
    while(!record.empty())
    {
        Coord curr = record.front();
        record.pop();
        int x = curr.xCoord();
        int y = curr.yCoord();
        
        if(x <= 59 && path[x + 1][y])
        {
            record.push(Coord(x+1, y));
            path[x+1][y] = false;
            step[x+1][y] = step[x][y] + 1;
            if(hp_x == x+1 && hp_y == y)
            {
                nSteps = step[x+1][y];
                trackDir = Actor::left;
                return true;
            }
        }
        
        if(x >= 1 && path[x - 1][y])
        {
            record.push(Coord(x-1, y));
            path[x-1][y] = false;
            step[x-1][y] = step[x][y] + 1;
            if(hp_x == x-1 && hp_y == y)
            {
                nSteps = step[x-1][y];
                trackDir = Actor::right;
                return true;
            }
        }
        
        if(y >= 1 && path[x][y - 1])
        {
            record.push(Coord(x,y-1));
            path[x][y-1] = false;
            step[x][y-1] = step[x][y] + 1;
            if(hp_x == x && hp_y == y-1)
            {
                nSteps = step[x][y-1];
                trackDir = Actor::up;
                return true;
            }
        }
        
        if(y <= 59 && path[x][y+1])
        {
            record.push(Coord(x,y+1));
            path[x][y+1] = false;
            step[x][y+1] = step[x][y] + 1;
            if(hp_x == x && hp_y == y+1)
            {
                nSteps = step[x][y+1];
                trackDir = Actor::down;
                return true;
            }
        }
    }
    
    trackDir = Actor::none;
    return false;
}


//Return true if there is Dirt beside frackman
bool StudentWorld::overlappedDirt(int xCoord, int yCoord) const
{
    for(int i = xCoord; i < xCoord + SPRITE_WIDTH; i++)
    {
        for(int j = yCoord; j < yCoord+ SPRITE_HEIGHT; j++)
        {
            if(m_dirt[i][j] != nullptr)
                return true;
        }
    }
    
    return false;
}

//Remove the dirt that is near frackman
void StudentWorld::reMoveDirt(int xCoord, int yCoord)
{
    for(int i = xCoord; i < xCoord + SPRITE_WIDTH; i++)
    {
        for(int j = yCoord; j < yCoord+ SPRITE_HEIGHT; j++)
        {
            if(m_dirt[i][j] == nullptr)
                continue;
            
            delete m_dirt[i][j];
            m_dirt[i][j] = nullptr;
        }
    }
}

//If the actors are alive, clear them
void StudentWorld::removeDeadItems()
{
    vector<Base*>::iterator baseIter = m_actor.begin();
    while(baseIter != m_actor.end())
    {
        if(!((*baseIter)->isLive()))
        {
            delete *baseIter;
            baseIter = m_actor.erase(baseIter);
        }
        
        else
            baseIter++;
    }
}

//determine whether a boulder should move down or not
bool StudentWorld::isDirtExist(int xCoord, int yCoord) const
{
    for(int i = xCoord; i < xCoord + SPRITE_WIDTH; i++)
    {
        if(m_dirt[i][yCoord - 1] != nullptr)
            return true;
    }
    
    return false;
}

//determine whether player can move because of a boudler
bool StudentWorld::canMove(int xCoord, int yCoord)
{
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_BOULDER)
        {
            int b_x = m_actor[i]->getX();
            int b_y = m_actor[i]->getY();
            if(isRadiusLessThanThree(xCoord, yCoord, b_x, b_y))
                return false;
        }
    }
    return true;
}

//determine whether two objects are overlapped
bool StudentWorld::isRadiusLessThanThree(int xCoord1, int yCoord1, int xCoord2, int yCoord2)
{
    return (sqrt((xCoord1-xCoord2)*(xCoord1-xCoord2)+(yCoord1-yCoord2)*(yCoord1-yCoord2)) <= 3);
}


void StudentWorld::hgNearBribe(Bribe* bp)
{
    int b_x = bp->getX();
    int b_y = bp->getY();
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_HARD_CORE_PROTESTER)
        {
            if(isRadiusLessThanThree(b_x, b_y, m_actor[i]->getX(), m_actor[i]->getY()))
            {
                m_actor[i]->setStunned(max(50, 100-(gameLevel()) * 10));
            }
        }
    }
}


//determine is bouldering falling on another boulder or not
bool StudentWorld::isFallingOnBoulder(int xCoord, int yCoord, Base* sp)
{
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_BOULDER && m_actor[i] != sp)
            if(isRadiusLessThanThree(xCoord, yCoord, m_actor[i]->getX(), m_actor[i]->getY()))
                return true;
    }
    
    return false;
}

//determine whether a player will be hit by a boulder or not
//If so, decrease the hitpoints
void StudentWorld::playerNearFallingBoulder(Boulder* bp)
{
    int b_x = bp->getX();
    int b_y = bp->getY();
    int p_x = m_player->getX();
    int p_y = m_player->getY();
    if(isRadiusLessThanThree(b_x, b_y, p_x, p_y))
    {
        m_player->decreaseHitpoints(100);
    }
}

void StudentWorld::protesterNearFallingBoudler(Boulder* bp)
{
    int b_x = bp->getX();
    int b_y = bp->getY();
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_PROTESTER || m_actor[i]->myIID() == IID_HARD_CORE_PROTESTER)
        {
            if(isRadiusLessThanThree(b_x, b_y, m_actor[i]->getX(), m_actor[i]->getY()))
                m_actor[i]->getAnnoyed(100);
        }
    }
}

void StudentWorld::protesterNearSquirt(Squirt* sp)
{
    int s_x = sp->getX();
    int s_y = sp->getY();
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_PROTESTER || m_actor[i]->myIID() == IID_HARD_CORE_PROTESTER)
        {
            if(isRadiusLessThanThree(s_x, s_y, m_actor[i]->getX(), m_actor[i]->getY()))
                m_actor[i]->getAnnoyed(2);
        }
    }
}

bool StudentWorld::isPlayerLessThan(int distance, int x, int y)
{
    int p_x = m_player->getX();
    int p_y = m_player->getY();
    return (sqrt((p_x - x)*(p_x - x)+(p_y - y)*(p_y - y)) <= distance);
}

bool StudentWorld::isProtesterLessThan(int distance, int x, int y, char& type)//use string to mark rg hg
{
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_PROTESTER)
        {
            int pro_x = m_actor[i]->getX();
            int pro_y = m_actor[i]->getY();
            if(sqrt((pro_x - x)*(pro_x - x) + (pro_y - y)*(pro_y - y)) <= distance)
            {
                type = 'R';
                return true;
            }
        }
        
        if(m_actor[i]->myIID() == IID_HARD_CORE_PROTESTER)
        {
            int pro_x = m_actor[i]->getX();
            int pro_y = m_actor[i]->getY();
            if(sqrt((pro_x - x)*(pro_x - x) + (pro_y - y)*(pro_y - y)) <= distance)
            {
                type = 'H';
                return true;
            }
        }
    }
    
    return false;
}

void StudentWorld::increaseSonar()
{
    m_player->increaseSonar();
}

void StudentWorld::increaseWater()
{
    m_player->increaseWater();
}
void StudentWorld::increaseGold()
{
    m_player->increaseGold();
}

void StudentWorld::newBribe(int x, int y)
{
    m_actor.push_back(new Bribe(x, y, this));
}
void StudentWorld::newSquirt(int x, int y, GraphObject::Direction startD)
{
    m_actor.push_back(new Squirt(x, y, startD,this));
}

void StudentWorld::showUp()
{
    int p_x = m_player->getX();
    int p_y = m_player->getY();
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(m_actor[i]->myIID() == IID_BARREL || m_actor[i]->myIID() == IID_GOLD)
        {
            int x = m_actor[i]->getX();
            int y = m_actor[i]->getY();
            if(sqrt((p_x - x)*(p_x - x)+(p_y - y)*(p_y - y)) <= 12)
                m_actor[i]->setVisible(true);
        }
    }
    
}

bool StudentWorld::canSquirtMove(int x, int y)
{
    if(!isDirtLess(x, y))
        return false;
    
   for(int i = 0; i < m_actor.size(); i++)
    {
    if(m_actor[i]->myIID() == IID_BOULDER)
        {
            
            int b_x = m_actor[i]->getX();
            int b_y = m_actor[i]->getY();
            if(isRadiusLessThanThree(x, y, b_x, b_y))
                return false;
        }

    }
    
    return true;
}

int StudentWorld::playerX(){return m_player->getX();}
int StudentWorld::playerY(){return m_player->getY();}

void StudentWorld::decreasePlayerHitpoints(int hitpoints)
{
    m_player->decreaseHitpoints(hitpoints);
}

void StudentWorld::protesterPickedBribe(Bribe* bp)
{
    int b_x = bp->getX();
    int b_y = bp->getY();
    for(int i = 0; i < m_actor.size(); i++)
    {
        if(isRadiusLessThanThree(b_x, b_y, m_actor[i]->getX(), m_actor[i]->getY()) && m_actor[i]->myIID() == IID_PROTESTER)
        {
            m_actor[i]->changeState('L');
            return;
        }
    }
}

//private member function

//Give valid new coordinates to all distributable items
bool StudentWorld::validNewCoord(int& xCoord, int& yCoord)
{
    int x = rand()% 61;
    int y = rand()% 37 + 20;
    
    if(x > 26 && x < 34)
        return false;
    
    if(y < 20 || y >56)
        return false;
    
    for(int i = 0; i < m_actor.size(); i++)
    {
        int p_x = m_actor[i]->getX();
        int p_y = m_actor[i]->getY();
        if(m_actor[i]->isDistributedItem())
        {
            if(sqrt((x - p_x)*(x - p_x) + (y - p_y)*(y - p_y)) <= 6)
                return false;
        }
    }
    
    xCoord = x;
    yCoord = y;
    return true;
}

string StudentWorld::textData(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrelLeft)
{
    stringstream ss;
    ss<< score;
    string scoreS = ss.str();
    while(scoreS.size() != 6)
    {
        scoreS.insert(0,1,'0');
    }
    
    ss.str("");
    ss << level;
    string levelS = ss.str();
    while(levelS.size() != 2)
    {
        levelS.insert(0,1,' ');
    }
    
    ss.str("");
    ss << lives;
    string livesS = ss.str();
    
    
    ss.str("");
    ss << health;
    string healthS = ss.str();
    if(health >= 0)
    {
    while(healthS.size() != 3)
    {
        healthS.insert(0,1,' ');
    }
    healthS += "%";
    }
    
    ss.str("");
    ss << squirts;
    string squirtS = ss.str();
    while(squirtS.size() != 2)
    {
        squirtS.insert(0,1,' ');
    }
    
    ss.str("");
    ss << gold;
    string goldS = ss.str();
    while(goldS.size() != 2)
    {
        goldS.insert(0,1,' ');
    }
    
    ss.str("");
    ss << sonar;
    string sonarS = ss.str();
    while(sonarS.size() != 2)
    {
        sonarS.insert(0,1,' ');
    }
    
    ss.str("");
    ss << barrelLeft;
    string barrelS = ss.str();
    while(barrelS.size() != 2)
    {
        barrelS.insert(0,1,' ');
    }
    
    string s = "Scr: " + scoreS + "  " + "Lvl: " + levelS + "  "+ "Lives: " + livesS + "  "+ "Hlth: " + healthS + "  "+ "Wtr: " + squirtS + "  "+ "Gld: " + goldS+ "  "+ "Sonar: " + sonarS + "  "+ "Oil Left: " + barrelS;
    return s;
}

void StudentWorld::generateSonarOrWaterpool()
{
    int w_x;
    int w_y;
    
    while(true)
    {
        w_x = rand() % 61;
        w_y = rand() % 61;
        if(!isDirtLess(w_x, w_y) || !canMove(w_x, w_y))
            continue;
        
        break;
    }
    
    int i = rand() % (getLevel()*25 + 300);
    int j = rand() % 5;
    if(i == 50)
    {
        if(j == 0)
            m_actor.push_back(new Sonar(this));
        else
            m_actor.push_back(new Waterpool(w_x, w_y, this));
    }
    return;
}

bool StudentWorld::isDirtLess(int xCoord, int yCoord)
{
    for(int i = xCoord; i < xCoord + SPRITE_WIDTH; i++)
        for(int j = yCoord; j < yCoord + SPRITE_HEIGHT; j++)
        {
            if(m_dirt[i][j] != nullptr)
                return false;
        }
    
    return true;
}

void StudentWorld::addNewProtester()
{
    int level = getLevel();
    
    int i = rand() % 100 + 1;
    int Pro = min(90, level* 10 + 30);
    
    if(m_tick == 0)
    {
        if(i > Pro)
          m_actor.push_back(new regularProtester(this));
        else
            m_actor.push_back(new HardCoreProtester(this));
        
        lastTimeAdd = m_tick;
        m_protesterNum++;
        
    }
    
    
    int T = max(25, 200 - level);
    int num = 2 + m_level * 1.5;
    int P = min(15, num);

    
    if((m_tick - lastTimeAdd) >= T && m_protesterNum < P)
    {
        if(i > Pro)
            m_actor.push_back(new regularProtester(this));
        else
            m_actor.push_back(new HardCoreProtester(this));
        
        lastTimeAdd = m_tick;
        m_protesterNum++;
    }
    
    m_tick++;
}















