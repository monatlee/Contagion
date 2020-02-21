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
    
    // set the number of pits and bacteria
    m_pits = level;
    m_bacteria = 10*level;
    
    // place PITS while checking for overlap
    list<Actor*>::iterator it;
    it = m_actors.begin();
    for(int i = 0; i < level; i++)
    {
        // generate random location
        int dx, dy;
        randomCenter(dx, dy);
        
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if( (*it)->overlapLocation(dx, dy) )
            {
                randomCenter(dx, dy);
                it = m_actors.begin();
            }
            else it++;
        }
        
        // add food and reset iterator
        m_actors.push_back(new Pit(dx, dy, this));
        it = m_actors.begin();
    }
    
    // place FOOD while checking for overlap
    int numFood = min(5*level, 25);
    it = m_actors.begin();
    for(int i = 0; i < numFood; i++)
    {
        // generate random location
        int dx, dy;
        randomCenter(dx, dy);
        
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if( (*it)->overlapLocation(dx, dy) )
            {
                randomCenter(dx, dy);
                it = m_actors.begin();
            }
            else it++;
        }
        
        // add food and reset iterator
        m_actors.push_back(new Food(dx, dy, this));
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
        
        // check for overlap
        while(it != m_actors.end())
        {
            // if overlap, regenerate coordinates and restart iterator
            if(!(*it)->canOverlapPlace() && (*it)->overlapLocation(dx, dy) )
            {
                //cerr << "overlap" << endl;
                randomCenter(dx, dy);
                it = m_actors.begin();
            }
            else it++;
        }

        // add dirt & reset iterator
        m_actors.push_back(new Dirt(dx, dy, this));
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
            if(m_bacteria==0 & m_pits==0) return GWSTATUS_FINISHED_LEVEL;
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
    
    oss << "health: " << m_socrates->getHitPoints();
    oss << setw(10);
    
    oss << "Sprays: " << m_socrates->getSprayCharges();
    oss << setw(10);
    
    oss << "Flames: " << m_socrates->getFlameCharges() << endl;
    
    oss << "Bacteria: " << m_bacteria << endl;
    
    setGameStatText(oss.str());
}

 // --------- DAMAGE ALL DAMAGEABLE ACTORS -------- //
void StudentWorld::damageActors(int x, int y, int damage, bool& flag)
{
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        // check for overlap with damageable object
        if((*it)->overlapLocation(x, y) && (*it)->isDamageable())
        {
            // tell actor to take damage
            (*it)->takeDamage(damage);
            
            // change flag to true if used
            flag=true;
            
            return; // only kill one thing 
        }
        it++;
    }
}


// ------------ CHECK FOR OVERLAP WITH FOOD -------- //
void StudentWorld::eatFood(int x, int y, bool& flag)
{
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        // check for overlap with food
        if((*it)->overlapLocation(x, y) && (*it)->canEat())
        {
            (*it)->setAlive(false);
            flag=true;
            return; // only eat one thing
        }
        it++;
    }
}

// ---------- CHECK FOR BLOCKED BACTERIA --------- //
bool StudentWorld::isBacteriaBlocked(int x, int y)
{
    // definition of overlap means that the Euclidean distance from the regular salmonella’s proposed new (x, y) location to the dirt pile is <= SPRITE_RADIUS pixels.
    
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        // find distance
        double x_dis = (*it)->getX() - x;
        double y_dis = (*it)->getY() - y;
        double dist = pow(x_dis, 2) + pow(y_dis, 2);
        dist = sqrt(dist);
        
        // check if less than SPRITE_RADIUS and if actor can block bacteria
        if(dist <= SPRITE_RADIUS && (*it)->canBlockBacteria()) return true;
        
        it++;
    }
    
    return false;
}

// ------------ LOOK FOR FOOD ----------------- //
bool StudentWorld::findFood(int x, int y, int& dir)
{
    int minDistance=VIEW_RADIUS;
    
    list<Actor*>::iterator it;
    it = m_actors.begin();
    while(it != m_actors.end())
    {
        // look for food
        if((*it)->canEat())
        {
            // find distance
            int x_dist = x-(*it)->getX();
            int y_dist = y-(*it)->getY();
            double dist = pow(x_dist, 2) + pow(y_dist, 2);
            dist = sqrt(dist);
            
            if(dist < minDistance)
            {
                minDistance = dist;
                dir = findDirection(x, y, (*it)->getX(), (*it)->getY());
            }
        }
        it++;
    }
    
    return minDistance < 128;
}

// ----------- LOOK FOR SOCRATES -------------------------- //
bool StudentWorld::findSocrates(int x, int y, int r, int& dir)
{
    int x_dist = x-m_socrates->getX();
    int y_dist = y-m_socrates->getY();
    
    double dist = pow(x_dist, 2) + pow(y_dist, 2);
    dist = sqrt(dist);
    
    // if within range
    if(dist < r)
    {
        dir = findDirection(x, y, m_socrates->getX(), m_socrates->getY());
        return true;
    }
    
    return false;
}

// ----------- FIND DIRECTION BETWEEN TWO LOCATIONS ------- //
int StudentWorld::findDirection(int fx, int fy, int sx, int sy)
{
    return 180+atan2(fy-sy, fx - sx)*180/3.14159265;
}
