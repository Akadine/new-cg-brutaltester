

#include "OldGameThread.h"

OldGameThread::OldGameThread(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file)
	:ThreadedGame{ id, refereeCmd, playersCmd, count, playerStats, seeder, n, swap, verbose, path, file } {
}

OldGameThread::~OldGameThread() {
	// Delete vector objects
	seedRotate.clear();
	players.clear();
}

bool OldGameThread::start() {
	int pid = game * 10;
	referee = Process(pid, refereeCmd);
	for (size_t i = 0; i < playersCount; i++) {
		pid++;
		players.push_back(Process(pid, playersCmd[i]));
	}

	// Spawn referee process
	logger.addLog(Level::VERBOSE, "Attempting to start Referee.");
	if (!this->referee.start()) {
		logger.addLog(Level::FATAL, "Cannot start Referee.");
		return false;
	}
	logger.addLog(Level::VERBOSE, "Referee Started.");

	// Spawn players process
	for (size_t i = 0; i < playersCount; i++) {
		logString = "Attempting to start player " + std::to_string(i);
		logger.addLog(Level::VERBOSE, logString);
		if (!players[i].start()) {
			logger.addLog(Level::FATAL, "Cannot start player file.");
			return false;
		}
		logString = "Player " + std::to_string(i);
		logString += " started.";
		logger.addLog(Level::VERBOSE, logString);
	}
	

	seedRotate = seeder.get().getSeed(playersCount);
	rotate = swap ? seedRotate[1] : 0;

	logString = "Swap flag set to ";
	logString += (swap ? "true." : "false.");
	logger.addLog(Level::VERBOSE, logString);

	// Call the start function in ThreadedGame
	logger.addLog(Level::VERBOSE, "OldGameThread, calling start in ThreadedGame.");
	ThreadedGame::start();
	return true;
}

void OldGameThread::run() {

	while (!shouldStop() || !isFinished()) {
		
		game = 0;
		int c = count.get();
		if (c < n) {
			game = c + 1;
			count.set(game);
		}
		if (game == 0) {
			// End of this thread
			setFinished();
			break;
		}
		try {
			//start a log block
			logString = "Game " + std::to_string(game);
			logger.addLog(Level::VERBOSE, logString);
			
			//send the seed to the referee
			if (swap) {
				logString = "###Seed " + std::to_string(seedRotate[0]);
				if (!referee.writePipe(logString)) { 
					throw std::exception("Could not write to the referee.");
				}
			}
			else if (seeder.get().repeteableTests) {
				logString = "###Seed " + std::to_string(seeder.get().getSeed(playersCount)[0]);
				if (!referee.writePipe(logString)) {
					throw std::exception("Could not write to the referee.");
				}
			}

			//send number of players to referee
			logString = "###Start " + std::to_string(players.size());
			if (!referee.writePipe(logString)) {
				throw std::exception("Could not write to the referee.");
			}

			//get a line from the referee
			std::string line = referee.readPipe(Process::OUTPUT);
			if (line != "") {
				//log the referee line
				this->logger.addLog(Level::VERBOSE, "Referee " + line);

				//run the game
				while (!line.starts_with("###End")) {

					//clear the error stream
					clearErrorStream(referee.getHandle(Process::ERR), "Referee error: ");

					if (line.starts_with("###Input")) {
						// Read all lines from the referee until next command and give it to the targeted process

						// player target
						int target = toInteger(line[9]);

						//get next line from referee
						line = referee.readPipe(Process::OUTPUT);

						//send these lines to the targeted player
						while (!line.starts_with("###")) {

							//log the line
							this->logger.addLog(Level::VERBOSE, "Referee: " + line);

							//send the line to the targeted player 
							if (!players[target].writePipe(line)) {
								throw std::exception("Could not write to the Player " + target);
							}

							//get next line from referee
							line = referee.readPipe(Process::OUTPUT);
						}
					}
					else if (line.starts_with("###Output")) {
						// Read x lines from the targeted process and give to the referee
						std::vector<std::string_view> parts = splitString(line, ' ');

						//target to give lines to
						int target = toInteger(parts[1]);

						//number of lines
						int x = toInteger(parts[2]);
												
						//clear error stream
						std::string prefix = "Player " + target;
						prefix += " error: ";
						clearErrorStream(players[target].getHandle(Process::ERR), prefix);
						
						//iterate through the lines
						for (int i = 0; i < x; ++i) {
							//get line from player
							std::string playerLine;
							playerLine = players[target].readPipe(Process::OUTPUT);

							//log the line
							prefix = "Player " + target;
							prefix += ": ";
							this->logger.addLog(Level::VERBOSE, prefix + playerLine);

							//send line to referee
							if (!referee.writePipe(playerLine)) {
								throw std::exception("Could not write to the referee.");
							}
						}
						
						//get next line from referee
						line = referee.readPipe(Process::OUTPUT);
					}
					else if (line.starts_with("###Error")) {
						//player process made an warning level error, lets log it.
						int target = toInteger(line[9]);
						std::string logLine = "Error for player " + target;
						logLine += " in game " + game;
						logLine += ": " + line.substr(9);
						this->logger.addLog(Level::WARN, logLine);
					}
				}


				// End of the game
				// unswap the positions to declare the correct winner
				std::string unrotated = "";
				for (int i = 0; i < line.length(); i++) {
					char c = line[i];
					if (c >= '0' && c <= '9') {
						c -= '0';
						c += rotate;
						c %= players.size();
						c += '0';
					}
					unrotated += c;
				}
				line = unrotated;

				//log it
				this->logger.addLog(Level::VERBOSE, "Referee: " + line);

				//add it to stats object
				playerStats.get().add(line);

				//log end of game
				std::string logLine = "End of game " + game;
				logLine += ": " + line.substr(7);
				logLine += "\t" + playerStats.get().toString();
				this->logger.addLog(Level::INFO, "Referee: " + logLine);
			}
			else {
				throw std::exception("Unexpected output from referee");

			}
		}
		catch (std::exception& e) {
			//something went really wrong, lets log it. 
			logString = "Exception in game " + std::to_string(game) + ": " + e.what();
			logger.addLog(Level::FATAL, logString);
		}

		setFinished();
	}
}