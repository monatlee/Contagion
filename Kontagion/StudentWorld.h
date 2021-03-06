#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <cmath>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld() { cleanUp(); };
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // methods involving socrates
    bool overlapSocrates(Actor* other);
    void healSocrates();
    void flameSocrates();
    void hurtSocrates(int damage);
    
    // add an Actor to m_actors
    void addActor(Actor* newActor) {m_actors.push_back(newActor);};
    
    // try to find the nearest food to a location
    bool findFood(int x, int y, int& dir);
    
    // try to find Socrates within a radius
    bool findSocrates(int x, int y, int r, int& dir);
    
    // damage all damageable actors
    void damageActors(int x, int y, int damage, bool& flag);
    
    // eat all eatable food
    void eatFood(int x, int y, bool& flag);
    
    // check if bacteria blocked
    bool isBacteriaBlocked(int x, int y);
    
    // decrementers and incrementrs for pits and bacteria
    void decPits() {m_pits--;};
    void decBacteria() {m_bacteria--;};
    void incBacteria() {m_bacteria++;};
    
private:
    Socrates* m_socrates;
    std::list<Actor*> m_actors;
    int m_pits;
    int m_bacteria;
    
    // helper methods for move
    void removeDeadActors();
    void addNewActors();
    void updateDisplayText();
    
    // generate a random X and Y coordinate in the circle
    void randomCenter(int& x, int& y);
    
    // generate a random X and Y coordinate along the edge of the circle
    void randomPerimeter(int& x, int& y);

    // find direction between two locations
    int findDirection(int fx, int fy, int sx, int sy);
    
};

#endif // STUDENTWORLD_H_
