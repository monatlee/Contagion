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
    if(dist <= 2*SPRITE_RADIUS) return true;
    return false;
}

bool Actor::overlapLocation(int x, int y)
{
    // find distance
    int x_dis = getX() - x;
    int y_dis = getY() - y;
    double dist = pow(x_dis,2) + pow(y_dis, 2);
    dist = sqrt(dist);
    
    // if less than 2*sprite distance
    if(dist <= 2*SPRITE_RADIUS) return true;
    
    return false;
}


// --------- SOCRATES BASE CLASS --------- //

Socrates::Socrates(StudentWorld* world): Actor(IID_PLAYER, 0, 128, 0, 0, world, 100)
{
    m_sprayCharges = 20;
    m_flameCharges = 5;
    m_positionalAngle = 180;
    m_justSprayed = false;
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
            case KEY_PRESS_SPACE: // spray charge
            {
                if(m_sprayCharges >= 1) // if he still has spray charges
                {
                    // add a spray 2*SPRITE_RADIUS pixels in front in same direction
                    
                    // find new location
                    double dx, dy;
                    getPositionInThisDirection(getDirection(), 2*SPRITE_RADIUS, dx, dy);

                    // allocate a new spray
                    Actor* newSpray = new DisinfectantSpray(dx, dy, getDirection(), getMyWorld());

                    // add spray to m_actors list
                    getMyWorld()->addActor(newSpray);
                    
                    // decrement spray count
                    m_sprayCharges--;
                    
                    // play sound
                    getMyWorld()->playSound(SOUND_PLAYER_SPRAY);
                    
                    // set just sprayed to true
                    m_justSprayed = true;
                }
                break;
            }
            case KEY_PRESS_ENTER: // flame thrower
            {
                if(m_flameCharges >= 1) // if still has flame thrower charges
                {
                    // add 16 flame objects 2*SPRITE_RADIUS pixels in front
                    // 22 degree increments around Socrates
                    // face the same direction/angle created around socrates
                    int dir = getDirection();
                    for(int i = 0; i < 16; i++)
                    {
                        // find new locatoin
                        double dx, dy;
                        getPositionInThisDirection(dir, 2*SPRITE_RADIUS, dx, dy);
                        
                        // allocate a new flame
                        Actor* newFlame = new Flame(dx, dy, dir, getMyWorld());
                        
                        // add flame to m_actors list
                        getMyWorld()->addActor(newFlame);
                        
                        dir+=22;
                    }
                    
                    // decrement flame count
                    m_flameCharges--;
                    
                    // play sound
                    getMyWorld()->playSound(SOUND_PLAYER_FIRE);
                    
                }
                
                // set just sprayed to false
                m_justSprayed = false;
                break;
            }
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
                
                // set just sprayed to false
                m_justSprayed = false;

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
                
                // set just sprayed to false
                m_justSprayed = false;

                break;

            }
        }
    }
    
    // if no user input, regenerate spray charges
    if(m_sprayCharges < 20 && !m_justSprayed) m_sprayCharges++;
    
    // set just sprayed to false
    m_justSprayed = false;
}


// ********** PROJECTILE ACTOR BASE CLASS *********** //
void ProjectileActor::doSomething()
{
    // check if alive, if not => return
    if(!isAlive()) return;
    
    // damage all damageable actors
    bool hit = false;
    getMyWorld()->damageActors(getX(), getY(), m_damage, hit);
    
    if(hit) // if hit something
    {
        //cerr << "here" << endl;
        setAlive(false); // set self to not alive
        return; // only check for overlap with one object
    }
    
    // move forward in current direction by SPRITE_RADIUS*2 pixels
    moveForward(SPRITE_RADIUS*2);

    // decrement pixels
    m_pixels--;

    // check if moved maximum distance and set to dead if so
    if(m_pixels <= 0) { setAlive(false); }

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
    
    // check for overlap with socrates
    if( this->overlap(*getMyWorld()->getSocrates()))
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
    if(newPoints<=0) getMyWorld()->getSocrates()->setAlive(false); // check if socrates is now dead
}
