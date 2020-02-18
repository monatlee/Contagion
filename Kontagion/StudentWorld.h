#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>
#include <cstdlib>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld() { cleanUp(); };
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // generate a random X and Y coordinate in the circle
    void randomXandYCoord(int& x, int& y);
    
    // return the list of actors
    std::list<Actor*> getActors() {return m_actors;};
    
    // getters for begin and end
    std::list<Actor*>::iterator getActorsBegin() {return m_actors.begin();};
    std::list<Actor*>::iterator getActorsEnd() {return m_actors.end();};
    
    // add an Actor to m_actors
    void addActor(Actor* newActor) {m_actors.push_back(newActor);};
    
    // getter for Socrates
    Socrates* getSocrates() {return m_socrates;};

private:
    Socrates* m_socrates;
    std::list<Actor*> m_actors;
    
};

#endif // STUDENTWORLD_H_
