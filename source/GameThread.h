#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <stdexcept>
#include "ThreadedGame.h"

/*
 *
 * @brief Class describing a GameThread object. This is the current way Referees communicate with Players.
 */
class GameThread : public ThreadedGame {
private:
    size_t commandSize;                 //< size of command to send to the ref
    size_t refereeInputIdx;             //< referee input index
    std::vector<size_t> pArgIdx;        //< player argument index
    std::vector<std::string> command;   //< command to send to the ref

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
    GameThread(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file);

    /*
     * @brief Destructs the OldGameThread object.
     */
    ~GameThread();

    /*
     * @brief sets up the processes and calls start in the base class ThreadedGame
     */
    void start();

    /*
     * @brief The run method from the Threadable base class we must overide.
     */
    void run() override;
};
#endif
