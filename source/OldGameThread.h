#ifndef OLDGAMETHREAD_H
#define OLDGAMETHREAD_H

#include "ThreadedGame.h"

/*
 * @brief Class describing an OldGameThread object. This is the old way Referees communicate with Players.
 */
class OldGameThread : public ThreadedGame {

public:
    /*
     * @brief Constructs an OldGameThread object.
     *
     * @param id The id.
     * @param refereeCmd The referee command line.
     * @param playersCmd The vector of player command lines.
     * @param count The shared count of games.
     * @param playerStats The playerStats object.
     * @param seeder the shared rng seeder object.
     * @param swap Are we swapping player positions?
     * @param verbose The verbosity to use for the logs.
     */
    OldGameThread(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file);

    /*
     * @brief Destructs the OldGameThread object.
     */
    ~OldGameThread();   

    /*
     * @brief sets up the processes and calls start in the base class ThreadedGame 
     */
    bool start();

    /*
     * @brief The run method from the Threadable base class we must overide.
     */
    void run() override;    
};
#endif