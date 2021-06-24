//
//  zombies.hpp
//  p2-zombies
//
//  Created by John Corio on 10/2/19.
//  Copyright © 2019 John Corio. All rights reserved.
//
// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#ifndef zombies_hpp
#define zombies_hpp

#include <stdio.h>
#include <string>
#include <algorithm>
#include "P2random.h"

class Zombie {
public:
    
    Zombie () // INITIALIZER LIST
    {
        name = P2random::getNextZombieName();
        distance = P2random::getNextZombieDistance();
        speed = P2random::getNextZombieSpeed();
        health = P2random::getNextZombieHealth();
        rounds_active = 1; 
    }
    
    Zombie(std::string &nameGiven, unsigned int distGiven,
           unsigned int spdGiven, unsigned int healGiven) // INITIALIZER LIST?
    {
        name = nameGiven;
        distance = distGiven;
        speed = spdGiven;
        health = healGiven;
        rounds_active = 1;
    }
    
    unsigned int getSpeed()
    {
        return speed;
    }
    
    // MAYBE A BOOL FOR MAKING 0 DISTANCE EASY??
    void newDistance()
    {
        if (distance - speed > distance) // wrap around
        {
            distance = 0;
        }
        else
        {
            distance = distance - speed;
        }
    }
    
    unsigned int getDistance()
    {
        return distance;
    }
    
    // return bool to signal it's dead?
    bool gotShot()
    {
        health--;
        
        if (health == 0)
        {
            return true;
        }
        return false;
    }
    
    std::string getName() const
    {
        return name;
    }
        
    unsigned int getHealth() const
    {
        return health;
    }
    
    void increaseRoundsActive()
    {
        rounds_active++;
    }
    
    int getRoundsActive() const
    {
        return rounds_active;
    }
    
    unsigned int getETA() const
    {
        return distance/speed;
    }
    
private:
    std::string name;
    unsigned int distance;
    unsigned int speed;
    unsigned int health;
    int rounds_active;
}; // should i default initialized round active to 0 or 1

#endif /* zombies_hpp */
