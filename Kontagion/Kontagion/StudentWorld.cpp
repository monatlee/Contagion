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
    
    // place FOOD while checking for overlap
    int numFood = min(5*level, 25);
    list<Actor*>::iterator it;
    it = m_actors.begin();
    for(int i = 0; i < numFood; i++)
    {
        // generate random location
        int dx, dy;
        randomXandYCoord(dx, dy);
        Actor* temp = new Food(dx, dy, this);
        
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if( (*it)->overlap(*temp) )
            {
                randomXandYCoord(dx, dy);
                temp = new Food(dx, dy, this);
                it = m_actors.begin();
            }
            else it++;
        }
        
        // add food and reset iterator
        m_actors.push_back(temp);
        it = m_actors.begin();
    }
    
    
    // place DIRT while checking for overlap
    int numDirt = max(180-20*level, 20);
    it = m_actors.begin();
    for(int i = 0; i < numDirt; i++)
    {
        // generate a random location
        int dx, dy;
        randomXandYCoord(dx, dy);
        Actor* temp = new Dirt(dx, dy, this);
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if(!(*it)->canOverlapPlace() && (*it)->overlap(*temp) )
            {
                //cerr << "overlap" << endl;
                randomXandYCoord(dx, dy);
                temp = new Dirt(dx, dy, this);
                it = m_actors.begin();
            }
            else it++;
        }

        // add dirt & reset iterator
        m_actors.push_back(temp);
        it = m_actors.begin();
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
