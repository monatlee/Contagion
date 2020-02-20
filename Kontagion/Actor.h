#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cmath>
#include <list>
#include <cstdlib>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// ******************** ACTOR BASE CLASS ************************************** //
class Actor : public GraphObject
{
public:
    Actor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world, int hitPoints) : GraphObject(imageID, startX, startY, dir, depth), m_alive(true), m_hitPoints(hitPoints), m_world(world) {};
    virtual void doSomething() = 0;
    
    // getters and setters for live status
    bool isAlive() {return m_alive;};
    void setAlive(bool status) {m_alive = status;};
    
    // getter of pointer to the world
    StudentWorld* getMyWorld() {return m_world;};
    
    // getters and setters for hit points
    int getHitPoints() {return m_hitPoints;};
    void setHitPoints(int newPoints) {m_hitPoints = newPoints;};
    
    // check for overlap (must be overridden for bacteria)
    bool overlap(Actor& other);
    bool overlapLocation(int x, int y);
    
    // check if can overlap in init (set default to false)
    virtual bool canOverlapPlace() {return false; };

    // each actor must be able to tell if they are destructible / can destruct and what they can block
    virtual bool isDamageable() = 0;
    virtual bool canDamage() = 0;
    virtual bool canBlockBacteria() = 0;
    virtual bool canBlockProjectiles() = 0;
    
    
private:
    bool m_alive;
    int m_hitPoints;
    StudentWorld* m_world;
};

// ****************** SOCRATES CLASS *********************** //
class Socrates : public Actor
{
public:
    Socrates(StudentWorld* world);
    virtual void doSomething();
    void movePerimeter(double& x, double& y);
    
    // Socrates is destructible and cannot block or damage
    virtual bool isDamageable() {return false;};
    virtual bool canBlockBacteria() {return false;};
    virtual bool canBlockProjectiles() {return false;};
    virtual bool canDamage() {return false;};
    
    // getters and setters for flame charges
    int getFlameCharges() {return m_flameCharges;};
    void setFlameCharges(int newCharges) {m_flameCharges = newCharges;};
    
    // getter for spray chargers
    int getSprayCharges() {return m_sprayCharges;};
    
private:
    int m_sprayCharges;
    int m_flameCharges;
    int m_positionalAngle;
    bool m_justSprayed;
};

// *************** PASSIVE ACTOR BASE CLASS FOR DIRT AND FOOD **************** //
class PassiveActor : public Actor
{
public:
    PassiveActor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world, int hitPoints) : Actor (imageID, startX, startY, dir, depth, world, hitPoints) {};
    virtual void doSomething() {return;};
    
    // dirt cannot do damage
    virtual bool canDamage() {return false;};
};

// ------- DIRT CLASS ------------ //
class Dirt : public PassiveActor
{
public:
    Dirt(double startX, double startY, StudentWorld* world) : PassiveActor(IID_DIRT, startX, startY, 0, 1, world, 1) {};
    
    virtual bool isDamageable() {return true;};
    virtual bool canBlockBacteria() {return true;};
    virtual bool canBlockProjectiles() {return true;};
    virtual bool canOverlapPlace() {return true;};
};

// -------- FOOD CLASS --------- //
class Food : public PassiveActor
{
public:
    Food(double startX, double startY, StudentWorld* world) : PassiveActor(IID_FOOD, startX, startY, 90, 1, world, 0) {};
    
    virtual bool isDamageable() {return false;};
    virtual bool canBlockBacteria() {return false;};
    virtual bool canBlockProjectiles() {return false;};
    virtual bool canOverlapPlace() {return false;};
};


// ****************** PROJECTILE ACTOR BASE CASE FOR SPRAY AND FLAME ******* //
class ProjectileActor : public Actor
{
public:
    // constructor
    ProjectileActor(int imageID, double startX, double startY, int dir, StudentWorld* world, int hitPoints, int pixels, int damage) : Actor (imageID, startX, startY, dir, 1, world, hitPoints), m_pixels(pixels), m_damage(damage) {};
    
    virtual void doSomething();
    // will be the same for both types, only difference is m_pixels and m_damage
    
    // projectile actors are not destructible and cannot block but can damage
    virtual bool isDamageable() {return false;};
    virtual bool canDamage() {return true;};
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
    Flame(double startX, double startY, int dir, StudentWorld* world) : ProjectileActor(IID_FLAME, startX, startY, dir,  world, 1, 32, 5) {};

};

// --------------------- DISINFECTANT SPRAY ACTOR --------------------------------------- //
class DisinfectantSpray : public ProjectileActor
{
public:
    // constructor sets m_pixels to 112 and m_damage to 2
    DisinfectantSpray(double startX, double startY, int dir, StudentWorld* world) : ProjectileActor(IID_SPRAY, startX, startY, dir, world, 1, 112, 2) {};
};

// ****************** EXTRA ACTOR BASE CASE FOR GOODIES AND FUNGUS ******** //
class ExtraActor : public Actor
{
public:
    // constructor
    ExtraActor(int imageID, double startX, double startY, StudentWorld* world, bool goodie, int scorePoints);
    
    virtual void doSomething();
    virtual void uniqueEffect()=0; // each type has a different unique effect
    
    // extra actors can be damaged and cannot block or damage
    virtual bool isDamageable() {return true;};
    virtual bool canDamage() {return false;};
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
    // constructor
    // m_scorePoints is +250
    // m_goodie is true
    RestoreHealthGoodie(double startX, double startY, StudentWorld* world) : ExtraActor(IID_RESTORE_HEALTH_GOODIE, startX, startY, world, true, 250) {};
    
    // uniqueEffect is to restore Socrates health to full
    virtual void uniqueEffect();
};

// -------------------- FLAME THROWER GOODIE ACTOR ---------------------- //
class FlameThrowerGoodie : public ExtraActor
{
public:
    // constructor
    // m_scorePoints is 300
    // m_goodie is true
    FlameThrowerGoodie(double startX, double startY, StudentWorld* world) : ExtraActor(IID_FLAME_THROWER_GOODIE, startX, startY, world, true, 300) {};
    
    // uniqueEffect is to add 5 flame thrower charges to arsenal
    virtual void uniqueEffect();
};

// -------------------- EXTRA LIFE GOODIE ACTOR ---------------------- //
class ExtraLifeGoodie : public ExtraActor
{
public:
    // constructor
    // m_scorePoints is 500
    // m_goodie is true
    ExtraLifeGoodie(double startX, double startY, StudentWorld* world) : ExtraActor(IID_EXTRA_LIFE_GOODIE, startX, startY, world, true, 500) {};
    
    // uniqueEffect is to give player an extra life
    virtual void uniqueEffect();
};

// -------------------- FUNGUS ACTOR ---------------------- //
class Fungus : public ExtraActor
{
public:
    // constructor
    // m_scorePoints is -50
    // m_goodie is FALSE
    Fungus(double startX, double startY, StudentWorld* world) : ExtraActor(IID_FUNGUS, startX, startY, world, false, -50) {};
    
    // uniqueEffect is to tell Socrates he has received 20 points of damage
    virtual void uniqueEffect();
};

#endif // ACTOR_H_
