#ifndef THREADEDGAME_H
#define THREADEDGAME_H

#include <vector>
#include <string_view>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <thread>
#include "Threadable.h"
#include "PlayerStats.h"
#include "Process.h"
#include "Logger.h"
#include "Mutable.h"
#include "SeedGenerator.h"

/*
 * @brief Class describing a base ThreadedGame object. This combines the reused code from OldGameThread and GameThread reducing them to run functions.
 */
class ThreadedGame : public Threadable {
    friend class GameThread;
    friend class OldGameThread;
private:
    Mutable<PlayerStats>& playerStats;      //< Shared Player Stats.
    Mutable<int>& count;                    //< Shared count of games played.
    Mutable<SeedGenerator>& seeder;         //< Shared rng seeder.
    int playersCount;                       //< the number of players.
    int n;                                  //< Number of games to play.
    int rotate;                             //< Rotating the seeds.
    int game;                               //< Game number we are on.
    bool swap;                              //< Are we swapping player positions?
    std::string path;                       //< logfile path 
    std::string file;                       //< logfile name 
    std::vector<int> seedRotate;            //< The seeds to rotate.

    std::string logString;                  //< Temporary logString, string to add to the log.
    Level verbose;                          //< The lowest level to log.
    Logger logger;                          //< The log object.

    std::string refereeCmd;                 //< The Referee command line.
    std::vector<std::string> playersCmd;    //< The Players command lines.

    Process referee;                        //< The Referee Process.
    std::vector<Process> players;           //< The Players Processes.
        
protected:
    /*
     * @brief Allows child classes access to setFinished in Threadable.
     */
    friend void Threadable::setFinished();

    /*
     * @brief Clears the error stream.
     *
     * @param errStream The stream.
     * @param prefix The header to print.
     */
    void clearErrorStream(HANDLE& handle, std::string prefix);

    /*
     * @brief Checks the stream for an int. Just like javas Scanner.hasNextInt;
     *
     * @param stream The stream
     * @return true if next value is an integer, false if not
     */
    bool hasNextInt(HANDLE& handle);

    /*
     * @brief Checks the stream for any value. Just like javas Scanner.hasNext;
     *
     * @param stream The stream
     * @return true if more exists in stram, false if stream is empty
     */
    bool hasNext(HANDLE& handle);

    /*
     * @brief Checks the stream for a line. Just like javas Scanner.hasNextLine;
     *
     * @param stream The stream
     * @return true another line exists in stream, false if stream is empty
     */
    bool hasNextLine(HANDLE& handle);

    /*
     * @brief Converts a string to an integer.
     *
     * @param s The string.
     * @return an integer
     */
    static int toInteger(std::string_view s);

    /*
     * @brief Converts a char to an integer.
     *
     * @param c The char.
     * @return an integer
     */
    static int toInteger(char c);

    /*
     * @brief Splits a string at delimeter.
     *
     * @param str The string.
     * @param delimeter The delimeter.
     * @return a std::vector of the strings
     */
    static std::vector<std::string_view> splitString(std::string_view str, char delimiter);

    template <typename T>
    /*
     * @brief Joins a std::vector<string> or std::vector<string_view> with a std::string or std::string_view delimeter (" " or ", " for example) 
     *
     * @param strings The string or string_view vector.
     * @param begin std::vector<string or string_view>::iterator where to begin
     * @param end std::vector<string or string_view>::iterator where to end
     * @param delimeter The delimeter.
     * @return a std::string or string_view
     */
     static T joinString(const std::vector<T>& strings, typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, T delimiter) {
        T str{};

        for (typename std::vector<T>::iterator& it = begin; it != end; ++it) {
            str += *it;
            if (it + 1 != end) str += delimiter;
        }

        return str;
    }

    /*
     * @brief Adds a log to the logger.
     *
     * @param v The verbosity level of the log
     * @param message the message to log
     */
    void log(Level v, std::string message);

    /*
     * @brief Calls start in Threadable base class to start the thread.
     */
    void start();
    
    /*
     * @brief The run method from the Threadable base class we must overide.
     */
    virtual void run() = 0;

public:
    /*
     * @brief Constructs a ThreadedGame object.
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
    ThreadedGame(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file);

    /*
     * @brief Denstructs a ThreadedGame object.
     */
    ~ThreadedGame();

    /*
     * @brief Gets the log.
     * 
     * @return the log.
     */
    Logger& getLog();

};

#endif