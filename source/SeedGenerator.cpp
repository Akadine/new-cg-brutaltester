#include "SeedGenerator.h"

SeedGenerator::SeedGenerator() { seed = 0; used_count = 0; repeteableTests = false; }
SeedGenerator::~SeedGenerator() {}
void SeedGenerator::initialSeed(long newSeed) {
    srand(newSeed);             //< seed the rng 
    seed = newSeed;             //< save the seed
    repeteableTests = true;     //< if the seed is set then we want repetable tests
}
void SeedGenerator::noInitialSeed() {
    srand((unsigned int)time(NULL));  //< random seed
}
int SeedGenerator::nextSeed() {
    return rand();  //< get the next seed in the list
}
std::vector<int> SeedGenerator::getSeed(int playerCount) {
    used_count %= playerCount;
    if (used_count == 0) {
        seed = nextSeed();
    }
    used_count++;
    return { seed, used_count };
}
int SeedGenerator::getUsedCount() const {
    return used_count;
}