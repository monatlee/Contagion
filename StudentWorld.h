#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld() { cleanUp(); };
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void randomXandYCoord(int& x, int& y);

private:
    Socrates* m_socrates;
    std::list<Actor*> m_actors;
    
};

#endif // STUDENTWORLD_H_
