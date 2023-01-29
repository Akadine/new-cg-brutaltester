#ifndef PLAYERSTATS_H
#define PLAYERSTATS_H

#include <string>
#include <vector>
#include <format>
#include <iostream>

enum Result { VICTORY, DEFEAT, DRAW };
/**
 * @brief Class representing Player Statistics
 */
class PlayerStats {
private:
	std::vector<std::vector<std::vector<int> > > stats;		//< the individual statistics
	std::vector<std::vector<int> > global;					//< the global statistics
	int number;												//< the number of players
	int total;												//< the total
	bool empty;												//< has anything been added?

public:
	/**
	 * @brief Constructs a default Player Statistics object with size of 0.
	 */
	PlayerStats();
	
	/**
	 * @brief Constructs a Player Statistics object with size num.
	 * 
	 * @param num the number of players
	 */
	PlayerStats(int num);

	/**
	 * @brief Destructs the Player Statistics object.
	 */
	~PlayerStats();

	/**
	 * @brief append the stat list.
	 *
	 * @param scores array the scores to append
	 */
	void add(std::vector<int> scores);

	/**
	 * @brief append the stat list.
	 *
	 * @param line csv list the scores to append
	 */
	void add(std::string line);
	std::string percent(float amount);
	std::string toString();
	void print();
};
#endif