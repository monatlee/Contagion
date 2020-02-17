#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// ******************** ACTOR BASE CLASS ************************************** //
class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world) : GraphObject(imageID, startX, startY, dir, depth), m_alive(true), m_world(world) {};
    virtual void doSomething() = 0;
    
    // getters and setters for live status
    bool isAlive() {return m_alive;};
    void setAlive(bool status) {m_alive = status;};
    
    // getter of pointer to the world
    StudentWorld* getMyWorld() {return m_world;};
    
    // check for overlap (must be overridden for bacteria)
    bool overlap(Actor& other);
    
    // check if can overlap in init (set default to false)
    bool canOverlapPlace() {return false; };

    // each actor must be able to tell if they are destructible and what they can block
    virtual bool isDestructible() = 0;
    virtual bool canBlockBacteria() = 0;
    virtual bool canBlockProjectiles() = 0;
    
private:
    bool m_alive;
    StudentWorld* m_world;
};

// ****************** SOCRATES CLASS *********************** //
class Socrates : public Actor
{
public:
    Socrates(StudentWorld* world);
    virtual void doSomething();
    void movePerimeter(double& x, double& y);
    
    // Socrates is destructible and cannot block
    bool isDestructible() {return false;};
    bool canBlockBacteria() {return false;};
    bool canBlockProjectiles() {return false;};
    
private:
    int m_hitPoints;
    int m_sprayCharges;
    int m_flameCharges;
    int m_positionalAngle;
};

// *************** PASSIVE ACTOR BASE CLASS FOR DIRT AND FOOD **************** //
class PassiveActor : public Actor
{
public:
    PassiveActor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world) : Actor (imageID, startX, startY, dir, depth, world) {};
    virtual void doSomething() {return;};
    
};

// ------- DIRT CLASS ------------ //
class Dirt : public PassiveActor
{
public:
    Dirt(double startX, double startY, StudentWorld* world) : PassiveActor(IID_DIRT, startX, startY, 90, 1, world) {};
    
    virtual bool isDestructible() {return true;};
    virtual bool canBlockBacteria() {return true;};
    virtual bool canBlockProjectiles() {return true;};
    virtual bool canOverlapPlace() {return true;};
    
};

// -------- FOOD CLASS --------- //
class Food : public PassiveActor
{
public:
    Food(double startX, double startY, StudentWorld* world) : PassiveActor(IID_FOOD, startX, startY, 90, 1, world) {};
    
    virtual bool isDestructible() {return false;};
    virtual bool canBlockBacteria() {return false;};
    virtual bool canBlockProjectiles() {return false;};
    virtual bool canOverlapPlace() {return false;};
};


// ****************** PROJECTILE ACTOR BASE CASE FOR SPRAY AND FLAME ******* //
class ProjectileActor : public Actor
{
public:
    // constructor
    virtual void doSomething();
    // will be the same for both types, only difference is m_pixels and m_damage
    
    // projectile actors are not destructible and cannot block
    virtual bool isDestructible() {return false;};
    virtual bool canBlockBacteria() {return false;};
    virtual bool canBlockProjectiles() {return false;};
    
private:
    int m_pixels; // set m_pixels to limit at beginning, decrement with every tick
    int m_damage; // damage that the projectile imparts on others
};

// -------------------- FLAME THROWER ACTOR -------------------------------- //
class Flame : public ProjectileActor
{
public:
    // constructor sets m_pixels to 32 and m_damage to 5
};

// --------------------- DISINFECTANT SPRAY ACTOR --------------------------------------- //
class DisinfectantSpray : public ProjectileActor
{
public:
    // constructor sets m_pixels to 112 and m_damage to 2
};

// ****************** EXTRA ACTOR BASE CASE FOR GOODIES AND FUNGUS ******** //
class ExtraActor : public Actor
{
public:
    // constructor
    virtual void doSomething();
    virtual void updateScore(); // each type has different amount of point gain / loss
    virtual void uniqueEffect(); // each type has a different unique effect
    
    // extra actors can be damaged and cannot block
    virtual bool isDestructible() {return true;};
    virtual bool canBlockBacteria() {return false;};
    virtual bool canBlockProjectiles() {return false;};
    
private:
    int m_lifetime; // track own lifetime
    bool m_goodie; // track if the actor is beneficial or harmful
    int m_scorePoints; // track how the object impacts the score
};

// -------------------- RESTORE HEALTH GOODIE ACTOR ---------------------- //
class RestoreHealthGoodie : public ExtraActor
{
public:
    // m_lifetime to max(rand() % (300 – 10 * L), 50)
    // m_scorePoints is +250
    // m_goodie is true
    // uniqueEffect is to restore Socrates health to full
};

// -------------------- FLAME THROWER GOODIE ACTOR ---------------------- //
class FlameThrowerGoodie : public ExtraActor
{
public:
    // m_lifetime to max(rand() % (300 – 10 * L), 50)
    // m_scorePoints is 300
    // m_goodie is true
    // uniqueEffect is to add 5 flame thrower charges to arsenal
};

// -------------------- EXTRA LIFE GOODIE ACTOR ---------------------- //
class ExtraLifeGoodie : public ExtraActor
{
public:
    // m_lifetime to max(rand() % (300 – 10 * L), 50)
    // m_scorePoints is 500
    // m_goodie is true
    // uniqueEffect is to give player an extra life
};

// -------------------- FUNGUS ACTOR ---------------------- //
class Fungus : public ExtraActor
{
public:
    // m_lifetime to max(rand() % (300 – 10 * L), 50)
    // m_scorePoints is -50
    // m_goodie is FALSE
    // uniqueEffect is to tell Socrates he has received 20 points of damage
};

#endif // ACTOR_H_
