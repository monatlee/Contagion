#include "StudentWorld.h"
#include "GameConstants.h"

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

void StudentWorld::randomCenter(int& x, int& y)
{
    double pi = 3.14159267;
    int r = randInt(0,120);
    int theta = randInt(0,360);
    
    x = r*cos(theta*pi/180) + VIEW_WIDTH/2;
    y = r*sin(theta*pi/180) + VIEW_HEIGHT/2;
}

void StudentWorld::randomPerimeter(int& x, int& y)
{
    double pi = 3.14159267;
    int r = VIEW_RADIUS; // fixed view_radius
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
        randomCenter(dx, dy);
        Actor* temp = new Food(dx, dy, this);
        
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if( (*it)->overlap(*temp) )
            {
                delete temp;
                randomCenter(dx, dy);
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
        randomCenter(dx, dy);
        Actor* temp = new Dirt(dx, dy, this);
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if(!(*it)->canOverlapPlace() && (*it)->overlap(*temp) )
            {
                //cerr << "overlap" << endl;
                delete temp;
                randomCenter(dx, dy);
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
        if((*it)->isAlive())
        {
            // tell each actor to do something
            (*it)->doSomething();
            
            // if socrates is dead, end level
            if(!m_socrates->isAlive()) return GWSTATUS_PLAYER_DIED;
            
            // if killed all bacteria and pits are done, move on to next level
        }
        it++;
    }
    
    // delete actors that have died
    removeDeadActors();
    
    // add actors
    addNewActors();
    
    // update status text
    updateDisplayText();

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


// -------- HELPER METHODS FOR MOVE ------------ //
void StudentWorld::removeDeadActors()
{
    // check if Socrates is dead
    if(!m_socrates->isAlive()) delete m_socrates;
    
    // remove dead elements of m_actors
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        if(!(*it)->isAlive())
        {
            delete *it;
            it=m_actors.erase(it);
        }
        else it++;
    }
}

void StudentWorld::addNewActors()
{
    int level = getLevel();
    
    // check for fungus
    int chanceFungus = min(510-level*10, 200);
    int randFungus = randInt(0, chanceFungus);
    //int randFungus=0;
    if(randFungus==0)
    {
        // generate new fungus
        int dx, dy;
        randomPerimeter(dx, dy);
        m_actors.push_back(new Fungus(dx, dy, this));
        
        // m_actors.push_back(temp);
    }
    
    // check for goodies
    int chanceGoodie = min(510-level*10, 250);
    int randGoodie = randInt(0, chanceGoodie);
    if(randGoodie==0)
    {
        // determine which goodie
        int whichGoodie = randInt(0, 100);
        
        // 10% chance of extra-life
        if(whichGoodie<10)
        {
            int dx, dy;
            randomPerimeter(dx, dy);
            m_actors.push_back(new ExtraLifeGoodie(dx, dy, this));
        }
        
        // 30% chance of flame thrower
        else if(whichGoodie<40)
        {
            int dx, dy;
            randomPerimeter(dx, dy);
            m_actors.push_back(new FlameThrowerGoodie(dx, dy, this));
        }
        
        // 60% chance of restore health
        else
        {
            int dx, dy;
            randomPerimeter(dx, dy);
            m_actors.push_back(new RestoreHealthGoodie(dx, dy, this));
        }
        
    }
}

void StudentWorld::updateDisplayText()
{
    ostringstream oss;
    oss << "Score: " << getScore();
    oss << setw(10);
    
    oss << "Level: " << getLevel();
    oss << setw(10);
    
    oss << "health: " << getSocrates()->getHitPoints();
    oss << setw(10);
    
    oss << "Sprays: " << getSocrates()->getSprayCharges();
    oss << setw(10);
    
    oss << "Flames: " << getSocrates()->getFlameCharges() << endl;
    
    setGameStatText(oss.str());
}
