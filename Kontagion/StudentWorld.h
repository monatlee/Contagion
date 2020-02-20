#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>
#include <cstdlib>
#include <sstream>
#include <iomanip>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld() { cleanUp(); };
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // helper methods for move
    void removeDeadActors();
    void addNewActors();
    void updateDisplayText();
    
    // generate a random X and Y coordinate in the circle
    void randomCenter(int& x, int& y);
    
    // generate a random X and Y coordinate along the edge of the circle
    void randomPerimeter(int& x, int& y);
    
    // damage all damageable actors
    void damageActors(int x, int y, int damage, bool& flag);
    
    // add an Actor to m_actors
    void addActor(Actor* newActor) {m_actors.push_back(newActor);};
    
    // getter for Socrates
    Socrates* getSocrates() {return m_socrates;};

private:
    Socrates* m_socrates;
    std::list<Actor*> m_actors;
    
};

#endif // STUDENTWORLD_H_
