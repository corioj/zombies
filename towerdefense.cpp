//
//  towerdefense.cpp
//  p2-zombies
//
// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
//
//  Created by John Corio on 10/2/19.
//  Copyright © 2019 John Corio. All rights reserved.
//

#include <stdio.h>
#include "P2random.h"
#include "xcode_redirect.hpp"
#include <getopt.h>
#include <iostream>
#include "zombies.hpp"
#include <vector>
#include <deque>
#include <queue>

class LowActivityLevel {
public:
    bool operator() (const Zombie* z1, const Zombie *z2)
    {
        if (z1->getRoundsActive() == z2->getRoundsActive())
        {
            return z1->getName() > z2->getName();
        }
        // returns true if left is lower priorit, low activity preferred, left must be more active to be lower priority, return >
        return z1->getRoundsActive() > z2->getRoundsActive();
    }
};

class HighActivityLevel {
public:
    bool operator() (const Zombie* z1, const Zombie *z2)
    {
        if (z1->getRoundsActive() == z2->getRoundsActive())
        {
            return z1->getName() > z2->getName();
        }
        // return true if left is lower priority, high activity preferred, left must be less active to be lower priority, return <
        return z1->getRoundsActive() < z2->getRoundsActive();
    }
};

// stats, use a range constructor and comparator, MAKE A VECTOR OF ZOMBIE POINTERS the pq of zombie pointers

class Compare {

public:
    bool operator() (const Zombie* z1, const Zombie* z2) const
    {
        uint32_t etaZ1 = static_cast<uint32_t>(z1->getETA());
        uint32_t etaZ2 = static_cast<uint32_t>(z2->getETA());
        // was originally just z1->geteta() in there
        if (etaZ1 == etaZ2)
        {
            if (z1->getHealth() == z2->getHealth())
            {
                return z1->getName() > z2->getName();
            }
            return z1->getHealth() > z2->getHealth();
        }
        return etaZ1 > etaZ2;
         /*
        if (z1->getETA() != z2->getETA())
        {
            return (z1->getETA() > z2->getETA());
        }
        if (z1->getHealth() != z2->getHealth())
        {
            return (z1->getHealth() > z2->getHealth());
        }
        else
        {
            return (z1->getName() > z2->getName());
        }
        */
    }
};


class TowerDefense {
public:
    TowerDefense()
    {
        killer = nullptr;
        lastzombie = nullptr;
        verbose = false;
        statistics = false;
        median = false;
        statsNUM = 0;
        rounds = 0;
    }
    
    unsigned int getMedVal()
    {
        if (lowActPQ.size() == highActPQ.size())
        {
            return ((lowActPQ.top()->getRoundsActive() + highActPQ.top()->getRoundsActive()) / 2);
        }
        else if (lowActPQ.size() > highActPQ.size())
        {
            return lowActPQ.top()->getRoundsActive();
        }
        else
        {
            return highActPQ.top()->getRoundsActive();
        }
        
    }
    
    // median
    void getMedian()
    {
        // make sure median is actually executable
        if (median && (!(lowActPQ.empty()) || !(highActPQ.empty())))
        {
            std::cout << "At the end of round " << rounds << ", the median zombie lifetime is " << getMedVal() << "\n";
        }
    }
    
    void medianinsertion(Zombie* zomb)
    {
        // arbitrary push
        if (lowActPQ.size() == 0)
        {
            lowActPQ.push(zomb);
        }
        else if (zomb->getRoundsActive() >= lowActPQ.top()->getRoundsActive())
        {
            lowActPQ.push(zomb);
        }
        else
        {
            highActPQ.push(zomb);
        }
        
        // rebalance
        if (lowActPQ.size() - highActPQ.size() == 2)
        {
            highActPQ.push(lowActPQ.top());
            lowActPQ.pop();
        }
        else if (highActPQ.size() - lowActPQ.size() == 2)
        {
            lowActPQ.push(highActPQ.top());
            highActPQ.pop();
        }
    }
    
    // shoot function?
    void shootZombies()
    {
        if (zombiepq.empty())
        {
            return;
        }
        for (int i = 0; i < quivercapacity; i++)
        {
            // maybe be an issue on this loop, i != qpac - 1 gives a worrying condition if i == qpac - 1
            if (zombiepq.empty())
            {
                break;
            }
            /*
            if (zombiepq.empty() && i != (quivercapacity - 1))
            {
                lastzombie = zombiepq.top();
                break;
            }
             */
            bool deadzombie = zombiepq.top()->gotShot();
            if (deadzombie)
            {
                // push to separate queue for median
                if (median)
                {
                    medianinsertion(zombiepq.top());
                }
                if (statistics)
                {
                    Zombie* ptr = zombiepq.top();
                    deadzombies.push_back(ptr);
                }
                // verbose tag
                if (verbose)
                {
                    std::cout << "Destroyed: " << zombiepq.top()->getName() << " (distance: " << zombiepq.top()->getDistance() << ", speed: " << zombiepq.top()->getSpeed() << ", health: " << zombiepq.top()->getHealth() << ")\n";
                }
                // get rid of dumb dead zombie, DOESNT REORDER IN A REASONABLE MANNER HERE
                lastzombie = zombiepq.top();
                zombiepq.pop();
            }
        }
    }
    // optimize later
    void roundGreeting()
    {
        if (verbose)
        {
            std::cout << "Round: " << (rounds + 1) << "\n";
        }
    }
    
    // move zombies function?
    void moveZombies()
    {
        if (!zombiepq.empty())
        {
            for (size_t i = 0; i < zombiedeque.size(); i++)
            {
                if (zombiedeque[i].getHealth() != 0)
                {
                    zombiedeque[i].newDistance();
                    zombiedeque[i].increaseRoundsActive();
                    if (verbose)
                    {
                        std::cout << "Moved: " << zombiedeque[i].getName() << " (distance: " << zombiedeque[i].getDistance() << ", speed: " << zombiedeque[i].getSpeed() << ", health: " << zombiedeque[i].getHealth() << ")\n";
                    }
                    if (zombiedeque[i].getDistance() == 0 && killer == nullptr)
                    {
                        killer = &zombiedeque[i];
                    }
                }
            }
        }
        
    }
    
    // check death
    bool playerDead()
    {
        bool death = false;
        if (zombiepq.empty())
        {
            return false;
        }
        else if (zombiepq.top()->getDistance() == 0)
        {
            death = true;
            // could do verbose mode here
        }
        return death;
    }
    
    // generate named
    void generateNamedZombies (std::string &name, unsigned int distGiv, unsigned int spdGiv, unsigned int hGiv)
    {
        zombiedeque.push_back(Zombie(name, distGiv, spdGiv, hGiv));
        // not pushing correctly!!!
        zombiepq.push(&zombiedeque.back());
        // verbose
        if (verbose)
        {
            std::cout << "Created: " << name << " (distance: " << distGiv << ", speed: " << spdGiv << ", health: " << hGiv << ")\n";
        }
    }
    
    // generate random
    void generateRandomZombies(int x)
    {
        for (int i = 0; i < x; i++)
        {
            zombiedeque.push_back(Zombie());
            zombiepq.push(&zombiedeque.back());
            // verbose
            if (verbose)
            {
                std::cout << "Created: " << zombiedeque.back().getName() << " (distance: " << zombiedeque.back().getDistance() << ", speed: " << zombiedeque.back().getSpeed() << ", health: " << zombiedeque.back().getHealth() << ")\n";
            }
        }
    }
    
    unsigned int getRoundNum()
    {
        return rounds;
    }
    
    void set_stats (bool statsmode, int num)
    {
        if (statsmode)
        {
            statistics = true;
            statsNUM = num;
            return;
        }
        statistics = false;
    }
    
    void set_verbose (bool verbosemode)
    {
        if (verbosemode)
        {
            verbose = true;
            return;
        }
        verbose = false;
    }
    
    void set_qpac (int qc)
    {
        quivercapacity = qc;
    }
    // median may not be bool, hold off on creating anything for it yet
    
    
    void initializeP2 (unsigned int seed, unsigned int mDist, unsigned int mSpeed,
                       unsigned int mHealth)
    {
        P2random::initialize(seed, mDist, mSpeed, mHealth);
    }
    
    void increaseRoundNum()
    {
        // always happens at end of round, so median is safe to do here
        rounds++;
    }
    
    Zombie* get_killer()
    {
        return killer;
    }
    
    Zombie* get_lastzombie()
    {
        return lastzombie;
    }
    
    bool empty()
    {
        return zombiepq.empty();
    }
    
    void set_Median(bool med)
    {
        median = med;
    }
    
    void runstatistics()
    {
        if (!statistics)
        {
            return;
        }
        else
        {
            // make vector of zombie*
            std::vector<Zombie*> zvector;
            zvector.reserve(zombiedeque.size());
            for (size_t i = 0; i < zombiedeque.size(); i++)
            {
                zvector.push_back(&zombiedeque[i]);
            }
            // put into a pq
            std::priority_queue<Zombie*, std::vector<Zombie*>, LowActivityLevel> statisticsPQLOW(zvector.begin(), zvector.end());
            std::priority_queue<Zombie*, std::vector<Zombie*>, HighActivityLevel> statisticsPQHIGH(zvector.begin(), zvector.end());
            
            // zombies still active
            std::cout << "Zombies still active: " << zombiepq.size() << "\n";
            // first N killed
            std::cout << "First zombies killed:\n";
            size_t z = 0;
            while (static_cast<int>(z) < statsNUM && z < deadzombies.size())
            {
                std::cout << deadzombies[z]->getName() << " " << (z+1) << "\n";
                z++;
            }
            // last N killed
            std::cout << "Last zombies killed:\n";
            size_t i = (deadzombies.size() - 1);
            // comparison of unsigned expression >= 0 is always true
            if (i <= static_cast<size_t>(statsNUM))
            {
                // i will wraparound when decremented past 0
                while (i < deadzombies.size())
                {
                    std::cout << deadzombies[i]->getName() << " " << z << "\n";
                    i--;
                    z--;
                }
            }
            // deadzombies is size 0, prevent bad access just in case after it wraps around
            /*
            else if (i > deadzombies.size()) // removing this check entirely might save some tiem
            {
                // CHECK THIS AGAIN JUST TO BE SURE
            }
             */
            else if (i <= deadzombies.size())
            {
                while (i >= (deadzombies.size() - statsNUM))
                {
                    std::cout << deadzombies[i]->getName() << " " << z << "\n";
                    i--;
                    z--;
                }
            }
            // N most active zombies
            std::cout << "Most active zombies:\n";
            int x = 0;
            while (x < statsNUM && !statisticsPQHIGH.empty())
            {
                std::cout << statisticsPQHIGH.top()->getName() << " " << statisticsPQHIGH.top()->getRoundsActive() << "\n";
                statisticsPQHIGH.pop();
                x++;
            }
            
            // N least active zombies
            std::cout << "Least active zombies:\n";
            int y = 0;
            while (y < statsNUM && !statisticsPQLOW.empty())
            {
                std::cout << statisticsPQLOW.top()->getName() << " " << statisticsPQLOW.top()->getRoundsActive() << "\n";
                statisticsPQLOW.pop();
                y++;
            }
        }
    }
    
private:
    // zombie deque
    std::deque<Zombie> zombiedeque;
    // various pqs
    std::priority_queue<Zombie*, std::vector<Zombie*>, Compare> zombiepq;
    // this pq should give the lowest at the top, thus its a min PQ of the most active half of zombies
    std::priority_queue<Zombie*, std::vector<Zombie*>, LowActivityLevel> lowActPQ;
    // this pq should give highest at top, this its a max PQ of the least active half of zombies
    std::priority_queue<Zombie*, std::vector<Zombie*>, HighActivityLevel> highActPQ;
    
    // std::priority_queue<Zombie*, std::vector<Zombie*>, ???> low, high rounds PQ, iterate based on desired functionality, pass in zombievector and construct on iterator range, then go by N
    
    //
    std::vector<Zombie*> deadzombies;
  
    Zombie* killer;
    Zombie* lastzombie;
    
    
    // round # tracker
    unsigned int rounds;
    int quivercapacity;
    int statsNUM;
    
    // mode variables
    bool verbose;
    bool statistics;
    bool median;
};

int make_int(char* chars)
{
    std::string str(chars);
    return stoi(str);
}

// MOVE ALL THESE TESTING AND RANDOM FILES DOWN INTO XCODE PROJECT FOLDER FOR PQS AS WELL
int main (int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    int gotopt;
    int opt_index = 0;
    option long_opts[] = {
        {"help", no_argument, nullptr, 'h'},
        {"verbose", no_argument, nullptr, 'v'},
        {"statistics", required_argument, nullptr, 's'},
        {"median", no_argument, nullptr, 'm'},
        { nullptr, 0, nullptr, '\0'}
    }; // option, REVISE LATER

    TowerDefense letskillzombies;
    
    while((gotopt = getopt_long(argc, argv, "hvs:m", long_opts, &opt_index)) != -1) {
    switch (gotopt) {
        // HELP OPTION
        case 'h':
            std::cout << "Consider doing something other " <<
                        "than using this option to kill zombies :)\n";
            exit(0);
    
        case 'v':
            letskillzombies.set_verbose(true);
            break;
            
        case 'm':
            letskillzombies.set_Median(true);
            break;
            
        case 's':
            letskillzombies.set_stats(true, make_int(optarg));
            break;
            
        default:
            std::cerr << "UHHHHH NICE ERROR LOSER LOL\n";
            exit(1);
            break;
        }
    }
    
    std::string junk;
    getline(std::cin, junk); // read in # line off top file
    
    // read in 5 lines of WORD: NUM, in a very specific order
    // q capac, random seed, max dist, max speed, max health
    int qcap;
    unsigned int seed, mdist, mspd, mhealth;
    std::cin >> junk >> qcap;
    std::cin >> junk >> seed;
    std::cin >> junk >> mdist;
    std::cin >> junk >> mspd;
    std::cin >> junk >> mhealth;
    
    letskillzombies.initializeP2(seed, mdist, mspd, mhealth);
    letskillzombies.set_qpac(qcap);
    // read in ---
    bool dead = false;
    while (std::cin >> junk)
    {
        unsigned int roundNum = 0;
        
        // read in round # after junk
        std::cin >> junk >> roundNum;
        letskillzombies.roundGreeting();
        bool nonZround = false;
        // execute rounds if not == letskillzombies.getRounds() + 1, ie next round
        if (roundNum != letskillzombies.getRoundNum() + 1)
        {
            // execute rounds without zombie generation until round finishing is round before our read-in value
            int count = 0;
            while (letskillzombies.getRoundNum() != roundNum - 1)
            {
                nonZround = true;
                // do a round w/o generating zombies, update and shoot, check for death
                // move zombies
                if (count != 0)
                {
                    letskillzombies.roundGreeting();
                }
                letskillzombies.moveZombies();
                if (letskillzombies.playerDead())
                {
                    // we've died, break from reading in
                    dead = true;
                    break;
                }
                // verbose
                
                // shoot
                letskillzombies.shootZombies();
                    // push into dead PQ
                    // verbose
                letskillzombies.increaseRoundNum();
                // get median here
                letskillzombies.getMedian();
                count++;
            }
            if (dead)
            {
                // we've died, break from reading in
                break;
            }
        }
        // then execute rounds, with reading in and creation of zombies
        int numRzombies = 0, numNzombies = 0;
        if (nonZround)
        {
            letskillzombies.roundGreeting();
        }
        letskillzombies.moveZombies();
        // death check
        if (letskillzombies.playerDead())
        {
            // we've died, break from reading in
            dead = true;
            break;
        }
        // read in # of named and random zombies
        std::cin >> junk >> numRzombies;
        std::cin >> junk >> numNzombies;
        
        // from this point on maybe just do all individual functions within this, writing things as I need them
        letskillzombies.generateRandomZombies(numRzombies);
        // say round
        // generate random zombies
        
        // loop over numNzombies named zombie data, create zombie
        for (int i = 0; i < numNzombies; i++)
        {
            unsigned int givenD = 0, givenS = 0, givenH = 0;
            std::string name;
            std::cin >> name;
            std::cin >> junk >> givenD >> junk >> givenS >> junk >> givenH;
            // generate named zombies
            letskillzombies.generateNamedZombies(name, givenD, givenS, givenH);
        }
        
    // shoot zombies
        letskillzombies.shootZombies();
    // median call
    // round ending stuff
        letskillzombies.increaseRoundNum();
        // get median here?
        letskillzombies.getMedian();
    // do rest of round
        
    }
    // something goes wrong when we exit loop with no zombies left on pq, special condition here
    if (letskillzombies.empty())
    {
        //letskillzombies.getMedian();
        // we won stuff
        std::cout << "VICTORY IN ROUND " << (letskillzombies.getRoundNum()) << "! " << letskillzombies.get_lastzombie()->getName() << " was the last zombie.\n";
        
        // NEED MORE INPUT
        // run statistics
        letskillzombies.runstatistics();
        return 0;
    }
    // continue execution after rounds all read in, whether player wins or zombies kill you
    while (!dead)
    {
        letskillzombies.roundGreeting();
        letskillzombies.moveZombies();
        if (letskillzombies.playerDead())
        {
            dead = true;
            break;
        }
        letskillzombies.shootZombies();
        if (letskillzombies.empty())
        {
            break;
        }
        letskillzombies.increaseRoundNum();
        // get median here?
        letskillzombies.getMedian();
    }
    
    if (dead)
    {
        // defeat stuff
        letskillzombies.increaseRoundNum();
        // letskillzombies.getMedian();
        // have to MAKE highest priority zombie left
        std::cout << "DEFEAT IN ROUND " << (letskillzombies.getRoundNum()) << "! " << letskillzombies.get_killer()->getName() << " ate your brains!\n";
        // run statistics
        letskillzombies.runstatistics();
        
    }
    else
    {
        letskillzombies.increaseRoundNum();
        letskillzombies.getMedian();
        // we won stuff
        std::cout << "VICTORY IN ROUND " << (letskillzombies.getRoundNum()) << "! " << letskillzombies.get_lastzombie()->getName() << " was the last zombie.\n";
        
        // NEED MORE INPUT
        // run statistics
        letskillzombies.runstatistics();
    }
    
    // letskillzombies.displaystatistics();
    
    return 0;
}

