#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// ---- ACTOR BASE CASE ------- //
class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world) : GraphObject(imageID, startX, startY, dir, depth), m_alive(true), m_world(world) {};
    bool isAlive() {return m_alive;};
    StudentWorld* getMyWorld() {return m_world;};
    virtual void doSomething() = 0;
    
private:
    bool m_alive;
    StudentWorld* m_world;
};

// ------- DIRT CLASS ------------ //
class Dirt : public Actor
{
public:
    Dirt(double startX, double startY, StudentWorld* world) : Actor(IID_DIRT, startX, startY, 90, 1, world) {};
    virtual void doSomething() {return;};
    
private:
};

// ---------- SOCRATES BASE CLASS ----------- //
class Socrates : public Actor
{
public:
    Socrates(StudentWorld* world);
    virtual void doSomething();
    void movePerimeter(double& x, double& y);
    
private:
    int m_hitPoints;
    int m_sprayCharges;
    int m_flameCharges;
    int m_positionalAngle; 
};


#endif // ACTOR_H_
