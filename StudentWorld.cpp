#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

void StudentWorld::randomXandYCoord(int& x, int& y)
{
    double pi = 3.14159267;
    int r = randInt(0,120);
    int theta = randInt(0,360);
    
    x = r*cos(theta*pi/180) + VIEW_WIDTH/2;
    y = r*sin(theta*pi/180) + VIEW_HEIGHT/2;
}

int StudentWorld::init()
{
    // save the level of the game
    int level = getLevel();
    
    // allocate a new socrates
    m_socrates = new Socrates(this);
    
    // create the right amount of dirt piles
    int numDirt = max(180-20*level, 20);
    
    for(int i = 0; i < numDirt; i++)
    {
        int dx, dy;
        randomXandYCoord(dx, dy);
        m_actors.push_back(new Dirt(dx, dy, this));
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // make socrates do something
    m_socrates->doSomething();
    
    // iterate over the remaining actors
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        (*it)->doSomething();
        it++;
    }

      return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    // delete Socrates
    delete m_socrates;
    
    // remove all elements of m_actors
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        delete *it;
        it=m_actors.erase(it);
    }
}
