#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// --------- ACTOR BASE CLASS -------------//
bool Actor::overlap(Actor& other)
{
    // store locations of current and other actors
    // my location
    double m_x = this->getX();
    double m_y = this->getY();
    // other location
    double o_x = other.getX();
    double o_y = other.getY();
    
    // find distance
    double x = m_x - o_x;
    double y = m_y - o_y;
    double dist = pow(x,2) + pow(y, 2);
    dist = sqrt(dist);
    
    // if less than 2*sprite distance
    if(dist < 2*SPRITE_RADIUS) return true;
    return false;
}


// --------- SOCRATES BASE CLASS --------- //

Socrates::Socrates(StudentWorld* world): Actor(IID_PLAYER, 0, 128, 0, 0, world, 100)
{
    m_sprayCharges = 20;
    m_flameCharges = 5;
    m_positionalAngle = 180;
}

void Socrates::movePerimeter(double& x, double& y)
{
    double pi = 3.14159267;
    
    x = 128*cos(m_positionalAngle*pi/180) + VIEW_WIDTH/2;
    y = 128*sin(m_positionalAngle*pi/180) + VIEW_WIDTH/2;
}

void Socrates::doSomething()
{
    // if dead, immediately done
    if(!isAlive()) return;
    
    // try to get user input
    int ch;
    if (getMyWorld()->getKey(ch))
    {
        // user hit a key during this tick!
        switch (ch)
        {
            case KEY_PRESS_LEFT: //move Socrates counterclockwise
            {
                // update positional angle
                m_positionalAngle +=5;
                
                // move according to positional angle
                double dx, dy;
                movePerimeter(dx, dy);
                moveTo(dx, dy);
                
                // change direction
                setDirection(m_positionalAngle + 180);
                break;
            }
            case KEY_PRESS_RIGHT: // move Socrates clockwise...;
            {
                // update positional angle
                m_positionalAngle -=5;
                
                // move according to positional angle
                double dx, dy;
                movePerimeter(dx, dy);
                moveTo(dx, dy);
                
                // change direction
                setDirection(m_positionalAngle + 180);
                break;
            }
        }
    }
    
    // if no user input, regenerate spray charges
    if(m_sprayCharges < 20) m_sprayCharges ++;
}


// ********** PROJECTILE ACTOR BASE CLASS *********** //
void ProjectileActor::doSomething()
{
    // check if alive, if not => return
    if(!isAlive()) return;
    
    // create iterator over this world's other actos
    list<Actor*>::iterator it;
    it = getMyWorld()->getActors().begin();
    
    // check for overlap with a damageable object
    while(it != getMyWorld()->getActors().end())
    {
        // if overlap, damage by m_damage
        if( this->overlap(**it) && (*it)->isDamageable())
        {
            int afterDamage = (*it)->getHitPoints() - m_damage;
            (*it)->setHitPoints(afterDamage);
            setAlive(false);
            return; // only check for overlap with one object
        }
        else
        {
            // move forward in current direction by SPRITE_RADIUS*2 pixels
            moveForward(SPRITE_RADIUS*2);
            
            // decrement pixels
            m_pixels--;
            
            // check if moved maximum distance and set to dead if so
            if(m_pixels <= 0) { setAlive(false); }
        }
        
        it++;
    }

}

// ********** EXTRA ACTOR BASE CLASS *********** //
ExtraActor::ExtraActor(int imageID, double startX, double startY, StudentWorld* world, bool goodie, int scorePoints) : Actor(imageID, startX, startY, 0, 1, world, 1)
{
    int level = getMyWorld()->getLevel();
    m_lifetime = max(rand()%(300-10*level), 50);
    m_goodie = goodie;
    m_scorePoints = scorePoints;
};

void ExtraActor::doSomething()
{
    // if not alive, immediately return
    if(!isAlive()) return;
    
    Actor* temp = getMyWorld()->getSocrates();
    // check for overlap with socrates
    if( this->overlap(*temp))
    {
        // update points
        getMyWorld()->increaseScore(m_scorePoints);
        
        // set state to dead
        setAlive(false);
        
        // if goodie, play sound
        if(m_goodie) getMyWorld()->playSound(SOUND_GOT_GOODIE);
        
        // every goodie / fungus has a unique effect
        uniqueEffect();
    }
    
    // if lifetime has expired, remove self from game
    if(m_lifetime <= 0) setAlive(false);
    
    // decrement lifetime every time
    m_lifetime--;
}

// ------- RESTORE HEALTH GOODIE ACTOR --------- //
void RestoreHealthGoodie::uniqueEffect()
{
    // restore Socrates health to full
    getMyWorld()->getSocrates()->setHitPoints(100);
}

// ------- FLAME THROWER GOODIE ACTOR --------- //
void FlameThrowerGoodie::uniqueEffect()
{
    // add 5 flame thrower charges to arsenal
    int newFlames = getMyWorld()->getSocrates()->getFlameCharges() + 5;
    getMyWorld()->getSocrates()->setFlameCharges(newFlames);
}

// ------- EXTRA LIFE GOODIE ACTOR --------- //
void ExtraLifeGoodie::uniqueEffect()
{
    // give player an extra life
    getMyWorld()->incLives();
}

// ------- FUNGUS ACTOR --------- //
void Fungus::uniqueEffect()
{
    // give Socrates 5 points of damage
    int newPoints = getMyWorld()->getSocrates()->getHitPoints() - 5;
    getMyWorld()->getSocrates()->setHitPoints(newPoints);
}
