#pragma once

#include "PlayerStats.h"

PlayerStats::PlayerStats() : number{ 0 }, total{ 0 }, empty{ true } {}
PlayerStats::PlayerStats(int num) : number{ num }, total{ 0 }, empty{ true } {}
PlayerStats::~PlayerStats() { stats.clear(); global.clear(); }
void PlayerStats::add(std::vector<int> scores) {
	empty = false;
	for (int i = 0; i < number; ++i) {
		for (int j = i + 1; j < number; ++j) {
			if (scores[i] > scores[j]) {
				stats[i][j][VICTORY] += 1;
				stats[j][i][DEFEAT] += 1;
				global[i][VICTORY] += 1;
				global[j][DEFEAT] += 1;
			}
			else if (scores[i] < scores[j]) {
				stats[j][i][VICTORY] += 1;
				stats[i][j][DEFEAT] += 1;
				global[j][VICTORY] += 1;
				global[i][DEFEAT] += 1;
			}
			else {
				stats[i][j][DRAW] += 1;
				stats[j][i][DRAW] += 1;
				global[i][DRAW] += 1;
				global[j][DRAW] += 1;
			}
		}
	}

	total += 1;
}
void PlayerStats::add(std::string line) {
	empty = false;
	std::vector<std::string> params;
	std::vector<int> positions;

	std::string delim = " ";

	auto start = 0U;
	auto end = line.find(delim);
	while (end != std::string::npos)
	{
		params.push_back(line.substr(start, end - start));
		start = (unsigned int)end + (unsigned int)delim.length();
		end = line.find(delim, start);
	}
	params.push_back(line.substr(start, end));
	for (int i = 1; i < params.size(); ++i) {
		for (char c : params[i]) {
			int pos = c - '0';
			if (pos > 9) {
				pos = c - 'A';
			}
			if (pos > 26) {
				pos = c - 'a';
			}
			positions[pos] = i - 1;
		}
		for (int i = 0; i < number; ++i) {
			for (int j = i + 1; j < number; ++j) {
				if (positions[i] < positions[j]) {
					stats[i][j][VICTORY] += 1;
					stats[j][i][DEFEAT] += 1;
					global[i][VICTORY] += 1;
					global[j][DEFEAT] += 1;
				}
				else if (positions[i] > positions[j]) {
					stats[j][i][VICTORY] += 1;
					stats[i][j][DEFEAT] += 1;
					global[j][VICTORY] += 1;
					global[i][DEFEAT] += 1;
				}
				else {
					stats[i][j][DRAW] += 1;
					stats[j][i][DRAW] += 1;
					global[i][DRAW] += 1;
					global[j][DRAW] += 1;
				}
			}
		}

		total += 1;
	}	
}

std::string PlayerStats::percent(float amount) {
	return std::format("{:.2f}", amount * 100.0 / total) + "%";
}
std::string PlayerStats::toString() {
	std::string result = "";
	if (!empty) {
		for (int i = 0; i < number; ++i) {
			result += " ";
			result += percent(global[i][VICTORY] / ((float)number - 1));
		}
	}
	return result;
}
void PlayerStats::print() {
	/*
	+----------+----------+----------+----------+----------+
	| Results  | Player 1 | Player 2 | Player 3 | Player 4 |
	+----------+----------+----------+----------+----------+
	| Player 1 |          |  42%     | 100%     |  32.46%  |
	+----------+----------+----------+----------+----------+
	| Player 2 | 42.36%   |          | 100%     |  100%    |
	+----------+----------+----------+----------+----------+
	| Player 3 | 42.36%   |  99.99%  |          |  100%    |
	+----------+----------+----------+----------+----------+
	| Player 4 | 42.36%   |  99.99%  | 17.8%    |          |
	+----------+----------+----------+----------+----------+
	*/
	if (!empty) {
		std::string separator = "";

		if (number == 2) { separator = "+----------+----------+----------+"; }
		else if (number == 3) { separator = "+----------+----------+----------+----------+"; }
		else if (number == 4) { separator = "+----------+----------+----------+----------+----------+"; }

		std::cout << separator << std::endl;
		std::cout << "| Results  |" << std::endl;
		for (int i = 0; i < number; ++i) {
			std::cout << " Player " << (i + 1) << " |";
		}
		std::cout << std::endl;
		std::cout << separator << std::endl;
		std::string space = "         ";
		std::string result = "";
		for (int i = 0; i < number; ++i) {
			std::cout << "| Player " << (i + 1) << " |";

			for (int j = 0; j < number; ++j) {
				result = "";

				if (i != j) {
					result = percent((float)stats[i][j][VICTORY]);
				}

				std::cout << " " << result << space.substr(result.length()) + "|";
			}

			std::cout << std::endl;
			std::cout << separator << std::endl;

		}
	}
}