#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// --------- ACTOR BASE CLASS -------------//
bool Actor::overlapActor(Actor& other)
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

void Socrates::takeDamage(int damage)
{
    int afterDamage=getHitPoints() - damage;
    setHitPoints(afterDamage);
    
    if(afterDamage>0) // damaged but still aliave
    {
        getMyWorld()->playSound(SOUND_PLAYER_HURT);
    }
    
    else // he dead
    {
        setAlive(false);
        getMyWorld()->playSound(SOUND_PLAYER_DIE);
    }
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

// ********** DIRT ACTOR CLASS ************* //
void Dirt::takeDamage(int damage)
{
    int afterDamage=getHitPoints() - damage;
    setHitPoints(afterDamage);
    if(afterDamage<=0) setAlive(false);
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
    if( this->overlapActor(*getMyWorld()->getSocrates()))
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
    getMyWorld()->getSocrates()->takeDamage(5);
}

// ************* PIT ACTOR ***************** //
void Pit::doSomething()
{
    if(m_rSalmonella==0 && m_aSalmonella==0 && m_eColi==0 )
    {
        // update StudentWorld
        getMyWorld()->decPits();
        setAlive(false); // set to dead
    }
    
    // 1 in 50 chance of bacteria generated
    int chance = randInt(1, 50);
    if(chance==1)
    {
        // choose a bacteria to generate
        int bact = randInt(1,3);

        switch(bact)
        {
            case(1): // regular salmonella
            {
                if(m_rSalmonella > 0)
                {
                    // add a RegularSalmonella to student world
                    getMyWorld()->addActor(new RegularSalmonella(getX(), getY(), getMyWorld()));
                    
                    m_rSalmonella--; // decrement count
                }
                break;
            }
            
            case(2): // aggressive salmonella
            {
                if(m_aSalmonella > 0)
                {
                    // add an AggressiveSalmonella to student world
                    getMyWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getMyWorld()));
                    
                    m_aSalmonella--; // decrement count
                }
                break;
            }

            case(3): // eColi
            {
                if(m_eColi > 0)
                {
                    // add an eColi to student world
                    getMyWorld()->addActor(new Ecoli(getX(), getY(), getMyWorld()));
                    
                    m_eColi--; // decrement count
                }
                break;
            }
        }
        
        // play sound
        getMyWorld()->playSound(SOUND_BACTERIUM_BORN);
    }
}

// *************** BACTERIA ACTOR ******************* //
void Bacteria::doSomething()
{
    // if dead, immediately return
    if(!isAlive()) return;
    
    bool flag = true;
    // aggressive salmonella will look for socrates
    firstLookForSocrates(flag);
    
    // check for overlap with socrates
    if(overlapActor(*getMyWorld()->getSocrates()))
    {
        getMyWorld()->getSocrates()->takeDamage(m_damage);
    }
    
    // eaten 3 pieces of food since last divided or was born
    else if(m_food == 3)
    {
        // find new x
        int new_x = getX();
        if(new_x < VIEW_WIDTH/2) new_x+=SPRITE_RADIUS;
        if(new_x > VIEW_WIDTH/2) new_x-=SPRITE_RADIUS;
        
        // find new y
        int new_y = getY();
        if(new_y < VIEW_WIDTH/2) new_y+=SPRITE_RADIUS;
        if(new_y > VIEW_WIDTH/2) new_y-=SPRITE_RADIUS;
        
        // create self
        addSelf(new_x, new_y);
        getMyWorld()->incBacteria();
        
        // reset food count
        m_food=0;
    }
    
    else
    {
        // eat eatable food
        bool eaten = false;
        getMyWorld()->eatFood(getX(), getY(), eaten);
    
        // if ate, increase food
        if(eaten) m_food++;
    }
    
    // salmonella and ecoli have different movement patterns
    if(flag) bacteriaMove();
    else return;
}

void Bacteria::takeDamage(int damage)
{
    int afterDamage=getHitPoints() - damage;
    setHitPoints(afterDamage);
    
    if(afterDamage>0) // damaged but still aliave
    {
        getMyWorld()->playSound(m_damageSound);
    }
    
    else if (isAlive())// he dead
    {
        setAlive(false);
        getMyWorld()->playSound(m_deadSound);
        getMyWorld()->increaseScore(100);
        getMyWorld()->decBacteria();
        
        // 50% chance bacteria becomes food
        int food = randInt(0,1);
        if(food == 0)
        {
            // create a new food
            getMyWorld()->addActor(new Food(getX(), getY(), getMyWorld()));
        }
    }
}

void Salmonella::bacteriaMove()
{
    double dx, dy;
    
    if(getMovement()>0) // keep moving in same direction
    {
        setMovement(getMovement()-1);
        
        // find position 3 units forward
        getPositionInThisDirection(getDirection(), 3, dx, dy);
        
        // check blockage
        bool blocked = getMyWorld()->isBacteriaBlocked(dx,dy );
            
        // check if outside perimeter
        double x = dx - VIEW_WIDTH/2;
        double y = dy - VIEW_WIDTH/2;
        double dist = pow(x, 2) + pow(y, 2);
        dist = sqrt(dist);
        bool outside = dist >= VIEW_RADIUS;
            
        // move to new valid location
        if(!blocked && !outside) moveTo(dx, dy);
            
        else // regenerate direction
        {
            int dir = randInt(0, 359);
            setDirection(dir);
            setMovement(10); // resets movement plan distance
        }
        
        return;
    }
    
    else // try to find food
    {
        Direction dir;
        // check if food is accessible
        if(getMyWorld()->findFood(getX(), getY(), dir))
        {
            setDirection(dir);
            
            // check position 3 units forward
            getPositionInThisDirection(dir, 3, dx, dy);

            // check blockage
            bool blocked = getMyWorld()->isBacteriaBlocked(dx,dy);

            if(!blocked)
            {
                moveTo(dx, dy);
            }

            else // regenerate direction
            {
                dir = randInt(0, 359);
                setDirection(dir);
                setMovement(10); // resets movement plan distance
            }

            return;
        }

        // if no food within
        else
        {
            int dir = randInt(0, 359);
            setDirection(dir);
            setMovement(10); // resets movement plan distance
            return;
        }
    }
}

// --------- REGULAR SALMONELLA ------------------ //
void RegularSalmonella::addSelf(int x, int y)
{
    getMyWorld()->addActor(new RegularSalmonella(getX(), getY(), getMyWorld()));
}

// --------- AGGRESSIVE SALMONELLA --------------- //
void AggressiveSalmonella::addSelf(int x, int y)
{
    getMyWorld()->addActor(new AggressiveSalmonella(getX(), getY(), getMyWorld()));
}
void AggressiveSalmonella::firstLookForSocrates(bool& flag)
{
    Direction dir;
    // if socrates within 72
    if(getMyWorld()->findSocrates(getX(), getY(), 72, dir))
    {
        // try to move towards
        setDirection(dir);
        
        double dx, dy;
        
        // find position 3 units forward
        getPositionInThisDirection(getDirection(), 3, dx, dy);
        
        // check blockage
        bool blocked = getMyWorld()->isBacteriaBlocked(dx,dy );
        
        if(!blocked) moveTo(dx, dy);
        
        flag = false;
    }
}

// --------- E COLI --------------- //
void Ecoli::addSelf(int x, int y)
{
    getMyWorld()->addActor(new Ecoli(getX(), getY(), getMyWorld()));
}

void Ecoli::bacteriaMove()
{
    Direction dir;
    // if socrates within 72
    if(getMyWorld()->findSocrates(getX(), getY(), 256, dir))
    {
        // try to move towards
        setDirection(dir);
        
        double dx, dy;
        
        for(int i = 0; i < 10; i++)
        {
            // find position 3 units forward
            getPositionInThisDirection(getDirection(), 3, dx, dy);
            
            // check blockage
            bool blocked = getMyWorld()->isBacteriaBlocked(dx,dy );
            
            if(!blocked)
            {
                moveTo(dx, dy);
                return;
            }
            
            setDirection(dir+=10);
        }
    }
}
