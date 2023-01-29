#include "ThreadedGame.h"

ThreadedGame::ThreadedGame(int id, std::string refereeCmd, std::vector<std::string> playersCmd, Mutable<int>& count, Mutable<PlayerStats>& playerStats, Mutable<SeedGenerator>& seeder, int n, bool swap, Level verbose, std::string path, std::string file)
	:Threadable{ }, count{ count }, playerStats{ playerStats }, seeder{ seeder }, n{ n }, swap{ swap }, game{ 0 }, playersCount{ (int)playersCmd.size() },
	refereeCmd{ refereeCmd }, playersCmd{ playersCmd }, verbose{ verbose }, path{ path }, file{ file }, logger{ Logger(verbose) }, rotate{ 0 } {
	players.reserve(playersCount);
	logger.setOutputPath(path);
	logger.setOutputFile(file);
}

ThreadedGame::~ThreadedGame() {
	// Delete vector objects
	seedRotate.clear();
	players.clear();
	playersCmd.clear();
}

Logger& ThreadedGame::getLog() { return logger; }

void ThreadedGame::start() {
	// Call the start function in Threadable
	logger.addLog(Level::VERBOSE, "Threaded game, starting thread.");
	Threadable::start();
}

void ThreadedGame::log(Level v, std::string message) {
	this->logString = "[Game " + std::to_string(game) + "] ";
	logString += message;
	logger.addLog(v, logString);
}

void ThreadedGame::clearErrorStream(HANDLE& handle, std::string prefix) {
	DWORD bytesAvail = 0;
	while (PeekNamedPipe(handle, NULL, 0, NULL, &bytesAvail, NULL)) {
		std::string line;
		char buffer[Process::BUFSIZE + 1]{};
		DWORD bytesRead;
		if (!ReadFile(handle, buffer, Process::BUFSIZE, &bytesRead, NULL))
		logger.addLog(Level::ERR, prefix + buffer);
	}
}

bool ThreadedGame::hasNextInt(HANDLE& handle) {
	DWORD dwRead;
	char c;
	PeekNamedPipe(handle, &c, 1, &dwRead, NULL, NULL);
	return isdigit(c);
}

bool ThreadedGame::hasNext(HANDLE& handle) {
	DWORD dwRead;
	char c;
	PeekNamedPipe(handle, &c, 1, &dwRead, NULL, NULL);
	return c != EOF;
}

bool ThreadedGame::hasNextLine(HANDLE& handle) {
	DWORD dwRead;
	char c;
	PeekNamedPipe(handle, &c, 1, &dwRead, NULL, NULL);
	return c != EOF && c != '\n';
}


int ThreadedGame::toInteger(std::string_view s) {
	int result = 0;
	for (char c : s) {
		result = result * 10 + static_cast<int>(c) - static_cast<int>('0');
	}
	return result;
}

int ThreadedGame::toInteger(char c) {
	std::string_view s(&c, 1);
	return toInteger(s);
}

std::vector<std::string_view> ThreadedGame::splitString(std::string_view str, char delimiter) {
	std::vector<std::string_view> result;
	std::size_t start = 0;
	while (start < str.size()) {
		std::size_t end = str.find(delimiter, start);
		if (end == std::string_view::npos) {
			result.emplace_back(str.substr(start));
			break;
		}
		result.emplace_back(str.substr(start, end - start));
		start = end + 1;
	}
	return result;
}