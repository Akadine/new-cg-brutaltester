#ifndef LOGGER_H
#define LOGGER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <filesystem>

enum Level { VERBOSE, INFO, WARN, ERR, FATAL };
/**
* @brief Class representing a log
*/
class Logger {
private:
    std::vector<std::string> logs;      //< the logs
    std::vector<Level> verbose;         //<the verbosity of the log
    Level verbosity;                    //<the lowest verbosity level to save
    std::string dir;                   //<the directory to print logs to
    std::string file;                   //<the file to print logs to

    //todo Add support for timestamps

public:    
    /**
     *
     * @brief Constructs a new Logger object with the default verbosity level of WARN.
    */
    Logger();

    /**
     *
     * @brief Constructs a new Logger object with the specified verbosity level.
     * 
     * @param verbose The verbosity level for the logger. Required.
     * @param file The file to use
    */
    Logger(Level verbose);

    /**
     *
     * @brief Destructs the Logger object.
    */
    ~Logger();

    /**
     *
     * @brief Sets the Output file directory.
     *
     * @param path the path to use.
    */
    void setOutputPath(std::string path);

    /**
     *
     * @brief Sets the Output file.
     *
     * @param file the file to use.
    */
    void setOutputFile(std::string file);
    
    /**
     * @brief Adds a log to the logger with the specified level of verbosity.
     *
     * @param level The level of verbosity for the log.
     * @param log The string to be logged.
     */
    void addLog(Level level, const std::string& log);

    /**
     *
     * @brief Append the logs from another Logger object.
     *
     * @param other The other Logger object.
     */
    void appendLogs(Logger& other);
    
    /**
     *
     * @brief Prints the logs stored in the logger to the specified output stream.
     * 
     * @param stream The output stream to which the logs will be printed.
     */
    void printLogs(std::ostream& stream);

    /**
     *
     * @brief Get a string of the logs stored in the logger.
     *
     * @return The logs as a string.
     */
    std::string toString();

    /**
     *
     * @brief Clears the logs stored in the logger.
     */
    void clearLogs();

    /**
     *@brief Saves the logs to the file specified by the file member.
     *
     * @return true if the logs were successfully saved, false otherwise.
     */
    bool SaveLogs();

    /**
     *@brief Gets the Verbosity
     *
     * @return Level::VERBOSITY.
     */
    Level getVerbosity() const;

    /**
     *@brief Gets the Log file path.
     *
     * @return string log file path.
     */
    std::string_view getPath() const;

    /**
     *@brief Gets the Log file.
     *
     * @return string log file.
     */
    std::string_view getFile() const;

    /**
     *
     * @brief Prints a string to the specified output stream.
     *
     * @param stream The output stream to which the logs will be printed.
     * @param str The string to print
     */
    void static printToStream(std::ostream& stream, std::string str);

    /**
     *
     * @brief converts LEVEL to a std::string.
     *
     * @param level the level to convert.
     * 
     * @return The string of the level
     */
    std::string static levelToString(Level level);
};
#endif