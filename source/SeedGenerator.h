#ifndef SEEDGENERATOR_H
#define SEEDGENERATOR_H

#include <time.h>
#include <vector>
#include <stdlib.h>

/**
 * 
 * @brief Class representing shared seeds
 */
class SeedGenerator {
private:
    int seed;               //< the seed
    int used_count;         //< how many used
        
public:
    bool repeteableTests;   //< do we want the same seed?

    /**
     *
     * @brief Gets the next seed.
     *
     * @return the next seed
     */
    int nextSeed();

    /**
     *
     * @brief Constructs a new Seed Generator object.
     */
    SeedGenerator();

    /**
     *
     * @brief Destructs the Seed Generator object.
     */
    ~SeedGenerator();

    /**
     *
     * @brief Sets the initial seed.
     *
     * @param newSeed the new seed to set.
     */
    void initialSeed(long newSeed);

    /**
     *
     * @brief Sets a random initial seed.
     */
    void noInitialSeed();

    /**
     *
     * @brief Gets the next set of seeds.
     *
     * @param playerCount the number of seeds to return.
     * 
     * @return a vector containing the seed and the used count 
     */
    std::vector<int> getSeed(int playerCount);

    /**
     *
     * @brief Gets the count of seeds used.
     *
     * @return the count of seeds used
     * 
     */
    int getUsedCount() const;
};

#endif