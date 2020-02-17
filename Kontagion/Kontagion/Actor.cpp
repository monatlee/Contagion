#include "Actor.h"
#include "StudentWorld.h"

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

Socrates::Socrates(StudentWorld* world): Actor(IID_PLAYER, 0, 128, 0, 0, world)
{
    m_sprayCharges = 20;
    m_flameCharges = 5;
    m_hitPoints = 100;
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

