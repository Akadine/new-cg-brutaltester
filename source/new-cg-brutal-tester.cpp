#include "Mutable.h"
#include "CommandCLI.h"
#include "Logger.h"
#include "SeedGenerator.h"
#include "PlayerStats.h"
#include "OldGameThread.h"
#include "GameThread.h"

LONG WINAPI GlobalSEHHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    std::cout << "Exception occurred" << std::endl;
    return EXCEPTION_EXECUTE_HANDLER;
}

using namespace CommandCLI;

void finished(PlayerStats stats, Logger log) {
    stats.print();
    bool saved = false;
    if (log.getPath() != "" && log.getFile() != "") saved = log.SaveLogs();
    if (!saved) {
        std::cout << "Warning: Logs not Saved!" << std::endl;
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    SetUnhandledExceptionFilter(GlobalSEHHandler);

    Options opt = Options();
    opt.Add("-h", false, "Displays this help.");
    opt.Add("-r", true, "Required. Referee command line.");
    opt.Add("-p1", true, "Required. Player 1 command line.");
    opt.Add("-p2", true, "Required. Player 2 command line.");
    opt.Add("-P3", true, "Player 3 command line.");
    opt.Add("-P4", true, "Player 4 command line.");
    opt.Add("-v", true, "Visualizer command line. Not implemented, hard sets threads to 1.");
    opt.Add("-t", true, "Number of threads to spawn for the games. Default 1.");
    opt.Add("-n", true, "Number of games to play. Default 1.");
    opt.Add("-s", false, "Swap player positions.");
    opt.Add("-i", true, "Initial seed. For repetable tests");
    opt.Add("-d", true, "Log directory.");
    opt.Add("-l", true, "Log level. 0 verbose, 1 info, 2 warning (default), 3 error, and 4 fatal. Only logs that level and higher. ex. if set to 3, only error and fatal levels logs are created.");
    opt.Add("-o", false, "Old mode");

    DefaultParser cmd = DefaultParser(argc, argv, opt);

    // Need help ?
    if (cmd.hasOption("-h") || !cmd.hasOption("-r") || !cmd.hasOption("-p1") || !cmd.hasOption("-p2")) {
        opt.PrintHelp(argv[0]);
        exit(0);
    }

    //setup logging
    Logger logger = cmd.hasOption("-l") ? Logger((Level)std::stoi(cmd.getOptionValue("-l"))) : Logger();

    std::string level = Logger::levelToString(logger.getVerbosity());

    std::string logString = "Logger set up with level ";
    logString += level + " logging.";
    logger.addLog(Level::INFO, logString);

    std::string refereeCmd = cmd.getOptionValue("-r");

    // Players command lines
    std::vector<std::string> playersCmd;
    std::string test = "";
    for (int i = 1; i <= 4; ++i) {
        test = "-p" + std::to_string(i);
        if (cmd.hasOption(test)) {
            playersCmd.push_back(cmd.getOptionValue(test));

            logString = "Player ";
            logString += std::to_string(i) + " file set to ";
            logString += playersCmd.back() + ".";
            logger.addLog(Level::INFO, logString);
        }
    }

    // Games count
    int n = cmd.hasOption("-n") ? std::stoi(cmd.getOptionValue("-n")) : 1;

    logString = "Number of games to play: ";
    logString += std::to_string(n) + ".";
    logger.addLog(Level::INFO, logString);

    //Visualizer File
    std::string visualizerFile = cmd.hasOption("-v") ? cmd.getOptionValue("-v") : "";

    if (visualizerFile != "") {
        logString = "Visualizer File set to: ";
        logString += visualizerFile + ". Visualizer use not implemented.";
        logger.addLog(Level::WARN, logString);
    }

    // Thread count
    int t = (cmd.hasOption("-n") && !cmd.hasOption("-v")) ? std::stoi(cmd.getOptionValue("-t")) : 1;

    logString = "Number of threads to spawn: ";
    logString = logString + std::to_string(t);
    logString = logString + ".";
    logger.addLog(Level::INFO, logString);

    // Logs Directory
    std::string dir = cmd.hasOption("-d") ? cmd.getOptionValue("-d") : "";

    if (dir == "") {
        logger.addLog(Level::WARN, "No output directory set, logs will not be saved");
    }
    else {
        logger.setOutputPath(dir);
        logger.setOutputFile("MasterLog.txt");
        logString = "Log Directory: ";
        logString = logString + dir;
        logString = logString + ".";
        logger.addLog(Level::INFO, logString);
    }

    // swap?
    bool swap = cmd.hasOption("-s");

    logString = "Swap flag set to: ";
    logString = logString + ((swap) ? "true" : "false");
    logString = logString + ".";
    logger.addLog(Level::INFO, logString);

    //Seed Initialization
    Mutable<SeedGenerator> seeder = Mutable<SeedGenerator>(SeedGenerator());
    SeedGenerator& mSeeder = seeder.get();
    if (cmd.hasOption("-i")) {
        long newSeed = std::stoi(cmd.getOptionValue("i"));
        mSeeder.initialSeed(newSeed);

        logString = "Initial seed: ";
        logString = logString + std::to_string(newSeed);
        logString = logString + ".";
        logger.addLog(Level::INFO, logString);
    }
    else {
        mSeeder.noInitialSeed();
        logger.addLog(Level::INFO, "No initial seed");
    }

    //old mode?
    bool old = cmd.hasOption("-o");

    logString = "Old flag set to: ";
    logString = logString + ((old) ? "true" : "false");
    logString = logString + ".";
    logger.addLog(Level::INFO, logString);

    // Prepare stats objects
    int size = (int)playersCmd.size();
    Mutable<PlayerStats> playerStats = Mutable<PlayerStats>(PlayerStats(size));
    Mutable<int> count = Mutable<int>(0);

    logString = "Player Stats initialized with size: ";
    logString = logString + std::to_string(size);
    logString = logString + ".";
    logger.addLog(Level::INFO, logString);

    bool allDone = false;

    if (old) {
        std::vector<OldGameThread*> threads;
        for (int i = 0; i < t; ++i) {
            threads.push_back(new OldGameThread(i + 1, refereeCmd, playersCmd, count, playerStats, seeder, n, swap, logger.getVerbosity(), dir, "GameLog.txt"));
        }
        for (int i = 0; i < t; ++i) {
            if (!threads[i]->start()) {
                logger.addLog(Level::FATAL, "Unable to start game.");
                finished(playerStats.get(), logger);
            }
        }
        while (!allDone) {
            bool done = true; //assume the threads are done
            for (int i = 0; i < t; ++i) {
                if (!threads[i]->isFinished()) done = false; //if one is not, we are not done. 
            }
            allDone = done; //set allDone flag
        }
        for (int i = 0; i < t; ++i) {
            logger.appendLogs(threads[i]->getLog()); //append the logs from the threads
            
        }
        for (int i = 0; i < t; ++i) {
            delete threads[i]; //cleanup        
        }
        threads.clear();
    }
    else {
        std::vector<GameThread*> threads;
        for (int i = 0; i < t; ++i) {
            threads.push_back(new GameThread(i + 1, refereeCmd, playersCmd, count, playerStats, seeder, n, swap, logger.getVerbosity(), dir, "GameLog.txt"));
            threads[i]->start();
            logger.addLog(Level::INFO, "Referee thread started started");
        }
        while (!allDone) {
            bool done = true; //assume the threads are done
            for (int i = 0; i < t; ++i) {
                if (!threads[i]->isFinished()) done = false;  //if one is not, we are not done. 
            }
            allDone = done;  //set allDone flag
        }
        for (int i = 0; i < t; ++i) {
            logger.appendLogs(threads[i]->getLog()); //append the logs from the threads
        }
        for (int i = 0; i < t; ++i) {
            delete threads[i]; //cleanup
        }
        threads.clear();
    }
    finished(playerStats.get(), logger);
}
