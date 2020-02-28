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
    
    // check for overlap both with another actor and a given location 
    bool overlapActor(Actor& other);
    bool overlapLocation(int x, int y);
    
    // check if can overlap in init (set default to false)
    virtual bool canOverlapPlace() {return false; };

    // each actor must be able to tell if they are destructible by projectile / can destruct and what they can block
    virtual bool isDamageable() = 0;
    virtual bool canBlockBacteria() = 0;
    
    // check if actor can be eaten (default is false)
    virtual bool canEat() {return false;};
    
    // take damage
    // default is to do nothing => only changes for passive, bacteria, Socrates
    virtual void takeDamage(int damage) {return;};
    
    
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
    
    // Socrates is not damageable by projectile and cannot block bacteria
    virtual bool isDamageable() {return false;};
    virtual bool canBlockBacteria() {return false;};
    
    // getters and setters for flame charges
    int getFlameCharges() {return m_flameCharges;};
    void setFlameCharges(int newCharges) {m_flameCharges = newCharges;};
    
    // getter for spray chargers
    int getSprayCharges() {return m_sprayCharges;};
    
    // unique takeDamage function
    virtual void takeDamage(int damage);
    
private:
    int m_sprayCharges;
    int m_flameCharges;
    int m_positionalAngle;
    bool m_justSprayed;
    
    void movePerimeter(double& x, double& y);
};

// *************** PASSIVE ACTOR BASE CLASS FOR DIRT AND FOOD **************** //
class PassiveActor : public Actor
{
public:
    PassiveActor(int imageID, double startX, double startY, int dir, int depth, StudentWorld* world, int hitPoints) : Actor (imageID, startX, startY, dir, depth, world, hitPoints) {};
    virtual void doSomething() {return;};
};

// ------- DIRT CLASS ------------ //
class Dirt : public PassiveActor
{
public:
    Dirt(double startX, double startY, StudentWorld* world) : PassiveActor(IID_DIRT, startX, startY, 0, 1, world, 1) {};
    
    virtual bool isDamageable() {return true;};
    virtual bool canBlockBacteria() {return true;};
    virtual bool canOverlapPlace() {return true;};
    
    //redefine takeDamage
    virtual void takeDamage(int damage);
};

// -------- FOOD CLASS --------- //
class Food : public PassiveActor
{
public:
    Food(double startX, double startY, StudentWorld* world) : PassiveActor(IID_FOOD, startX, startY, 90, 1, world, 0) {};
    
    virtual bool isDamageable() {return false;};
    virtual bool canBlockBacteria() {return false;};
    
    virtual bool canEat() {return true;};
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
    virtual bool canBlockBacteria() {return false;};
    
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
    
    // extra actors can be damaged and cannot block or damage
    virtual bool isDamageable() {return true;};
    virtual bool canBlockBacteria() {return false;};
    
private:
    int m_lifetime; // track own lifetime
    bool m_goodie; // track if the actor is beneficial or harmful
    int m_scorePoints; // track how the object impacts the score
    
    virtual void uniqueEffect()=0; // each type has a different unique effect

};

// -------------------- RESTORE HEALTH GOODIE ACTOR ---------------------- //
class RestoreHealthGoodie : public ExtraActor
{
public:
    // constructor
    // m_scorePoints is +250
    // m_goodie is true
    RestoreHealthGoodie(double startX, double startY, StudentWorld* world) : ExtraActor(IID_RESTORE_HEALTH_GOODIE, startX, startY, world, true, 250) {};
    
private:
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
    
private:
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
    
private:
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
    
private:
    // uniqueEffect is to tell Socrates he has received 20 points of damage
    virtual void uniqueEffect();
};


// ******************** PIT ACTOR *********************** //
class Pit : public Actor
{
public:
    // constructor
    Pit(double startX, double startY, StudentWorld* world) : Actor(IID_PIT, startX, startY, 0, 1, world, 1), m_rSalmonella(5), m_aSalmonella(3), m_eColi(2) {};
    
    virtual void doSomething();
    
    virtual bool isDamageable() {return false;};
    virtual bool canBlockBacteria() {return false;};
    
private:
    int m_rSalmonella;
    int m_aSalmonella;
    int m_eColi;
};

// ***************** BACTERIA ACTOR ********************** //
class Bacteria : public Actor
{
public:
    Bacteria(int imageID, double startX, double startY, StudentWorld* world, int hitPoints, int damage, int damageSound, int deadSound) : Actor(imageID, startX, startY, 90, 0, world, hitPoints), m_movementPlanDistance(0), m_damage(damage), m_food(0), m_damageSound(damageSound), m_deadSound(deadSound) {};
    
    virtual void doSomething();
    
    virtual bool isDamageable() {return true;};
    virtual bool canBlockBacteria() {return false;};\
    
    
    // take damage
    virtual void takeDamage(int damage);
    
    // getters and setters for m_movementPlanDistance
    int getMovement() {return m_movementPlanDistance;};
    void setMovement(int newDistance) {m_movementPlanDistance=newDistance;};
    
private:
    int m_movementPlanDistance;
    int m_damage;
    int m_food;
    int m_damageSound;
    int m_deadSound;
    
    virtual void bacteriaMove() = 0;    // unique movement plan

    // add copy of self to world
    virtual void addSelf(int x, int y)=0;
    
    // only aggressive salmonella will look for socrates
    virtual void firstLookForSocrates(bool& flag) { }; // default do nothing
};

// ------------------ SALMONELLA ACTOR -------------------- //
class Salmonella : public Bacteria
{
public:
    Salmonella(int imageID, double startX, double startY, StudentWorld* world, int hitPoints, int damage, int damageSound, int deadSound) : Bacteria (imageID, startX, startY, world, hitPoints, damage, damageSound, deadSound) {};
    
private:
    virtual void bacteriaMove();
    // salmonella will check movement plan and determine potential movement
    
    // Bacteria(int imageID, double startX, double startY, StudentWorld* world, int hitPoints, int damage, int damageSound, int deadSound)
};

// ------------------ REGULAR SALMONELLA ACTOR ---------- //
class RegularSalmonella : public Salmonella
{
public:
    // constructor
    RegularSalmonella(double startX, double startY, StudentWorld* world) : Salmonella(IID_SALMONELLA, startX, startY, world, 4, 1, SOUND_SALMONELLA_HURT, SOUND_SALMONELLA_DIE) {};
    
private:
    virtual void addSelf(int x, int y);
    
};

// ------------------- AGGRESSIVE SALMONELLA ACTOR -------- //
class AggressiveSalmonella : public Salmonella
{
public:
    AggressiveSalmonella(double startX, double startY, StudentWorld* world) : Salmonella(IID_SALMONELLA, startX, startY, world, 10, 2, SOUND_SALMONELLA_HURT, SOUND_SALMONELLA_DIE) {};
    
private:
    virtual void firstLookForSocrates(bool& flag); // need to redefine
    
    virtual void addSelf(int x, int y);
    
};

// -------------------- ECOLI ACTOR ---------------------- //
class Ecoli : public Bacteria
{
public:
    Ecoli(double startX, double startY, StudentWorld* world) : Bacteria (IID_ECOLI, startX, startY, world, 5, 4, SOUND_ECOLI_HURT, SOUND_ECOLI_DIE) {};

private:
    virtual void bacteriaMove(); // ecoli will only look for socrates
    
    virtual void addSelf(int x, int y);
};

#endif // ACTOR_H_
