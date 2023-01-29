#include "GameThread.h"

GameThread::GameThread(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file)
	:ThreadedGame{ id, refereeCmd, playersCmd, count, playerStats, seeder, n, swap, verbose, path, file }, commandSize{ 0 }, refereeInputIdx{ 0 } {
	pArgIdx.reserve(playersCount);
}

GameThread::~GameThread() {
	// Delete vector objects
	seedRotate.clear();
	players.clear();
}

void GameThread::start() {
	bool haveSeedArgs = swap || seeder.get().repeteableTests;
		
	std::vector<std::string_view> splitted = splitString(refereeCmd, ' ');
	commandSize = (size_t)(splitted.size() + playersCount * 2 + (logger.getFile() != "" ? 2 : 0) + (haveSeedArgs ? 2 : 0));
	command.reserve(commandSize);

	//for (size_t i = 0; i < splitted.size(); ++i) command.push_back((std::string)splitted[i]);

	for (size_t i = 0; i < playersCount; ++i) {
		pArgIdx.push_back(splitted.size() + i * 2 + 1);
		std::string test = "-p" + std::to_string(i + 1);
		command.push_back(test);
		command.push_back(playersCmd[i]);
	}

	if (haveSeedArgs) {
		this->n *= playersCount;
		refereeInputIdx = splitted.size() + playersCount * 2 + 1;
		command[refereeInputIdx - 1] = "-d";
		command[refereeInputIdx] = "";
	}

	if (logger.getFile() != "") {
		command[commandSize - 2] = "-l";
	}
	ThreadedGame::start();
}

void GameThread::run() {
	bool keepRunning = true;
	while (keepRunning) {

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
			logString = "Starting game " + std::to_string(game);
			logString += ".";
			logger.addLog(Level::VERBOSE, logString);

			if (logger.getFile() != "") {
				logString = logger.toString() + "/";
				logString += logger.getFile();
				logString += game + ".json";
				command[commandSize - 1] = logString;
			}

			std::vector<int> seedRotate = seeder.get().getSeed(playersCount);
			if (swap) {
				command[refereeInputIdx] = "seed=" + seedRotate[0];
				for (int i = 0; i < playersCount; i++) {
					command[pArgIdx[i]] = playersCmd[(i + seedRotate[1]) % playersCount];
				}
			}
			else if (seeder.get().repeteableTests) {
				command[refereeInputIdx] = "seed=" + seeder.get().nextSeed();
			}

			// Spawn referee process
			std::string args(joinString<std::string>(command, command.begin(), command.end(), " "));
			logString = "Atempting to start Referee " + refereeCmd;
			logString += " with " + (args == "" ? "no args" : args);
			logString += ".";
			logger.addLog(Level::VERBOSE, logString);

			this->referee = Process(game, refereeCmd, args);
			if (!this->referee.start()) {
				throw std::exception(std::string("Could not start the referee.").c_str());
			}

			logString = "Referee " + refereeCmd;
			logString += " started with " + (args == "" ? "no args" : args);
			logString += ".";
			logger.addLog(Level::VERBOSE, logString);

			bool error = false;
			std::string data = "";
			std::string fullOut = "";

			std::vector<int> scores;

			for (int pi = 0; pi < playersCount; ++pi) {
				int i = swap ? (pi + seedRotate[1]) % playersCount : pi;
				if (referee.isPipeEmpty(referee.getHandle(Process::OUTPUT))) {
					if (hasNextInt(this->referee.getHandle(Process::OUTPUT)))
					{
						scores[i] = toInteger(referee.readPipe(Process::OUTPUT));
					}
					else
					{
						while (!hasNextInt(referee.getHandle(Process::OUTPUT)) && hasNext(referee.getHandle(Process::OUTPUT)))
						{
							std::string line = referee.readPipe(Process::OUTPUT);
							fullOut += line + "\n";
						}

						// Try again after referee messages are out of the way
						if (hasNextInt(referee.getHandle(Process::OUTPUT))) scores[i] = toInteger(referee.readPipe(Process::OUTPUT));
					}

					if (scores[i] < 0) {
						error = true;
						logString = "Negative score during game " + game;
						logString += " p" + i;
						logString += ":" + scores[i];
						logger.addLog(Level::ERR, logString);
					}
				}
				else {
					logString = " Problem with referee output in game" + game;
					logString += ". No output content. Maybe try Old Mode?";
					logger.addLog(Level::FATAL, logString);
					keepRunning = false;
					setFinished();
				}
			}

			while (hasNextLine(referee.getHandle(Process::OUTPUT))) {
				std::string line = referee.readPipe(Process::OUTPUT);
				data += line + "\n";
			}

			if (fullOut.length() < 0) {
				logString = "Problem with referee output in game" + game;
				logString += ". Output content:" + fullOut;
				logger.addLog(Level::ERR, logString);
			}

			if (hasNextLine(referee.getHandle(Process::ERR))) {
				error = true;
				logString = "Error during game " + game;

				while (hasNextLine(referee.getHandle(Process::ERR))) {
					logString += "\n";
					std::string line = referee.readPipe(Process::ERR);
					logString += line;
				}

				logger.addLog(Level::ERR, logString);
			}

			if (error) {
				logString = "If you want to replay and see this game, use the following command line: ";
				logString += joinString<std::string>(command, command.begin(), command.end(), " ");
				logString += " -s";
				if (data.length() > 0) logString += " -d " + data;
				logger.addLog(Level::INFO, logString);
			}

			playerStats.get().add(scores);

			//log end of game
			std::string logLine = "End of game " + game;
			//logLine += ": " + line.substr(7);
			logLine += "\t" + playerStats.get().toString();
			this->logger.addLog(Level::INFO, "Referee: " + logLine);
		}
		catch (std::exception& e) {
			//something went really wrong, lets log it. 
			logString = "Exception in game " + std::to_string(game) + ": " + e.what();
			logger.addLog(Level::FATAL, logString);
		}
	}
}