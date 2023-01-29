#include "Logger.h"

Logger::Logger() { verbosity = Level::WARN; dir = ""; file = ""; }
Logger::Logger(Level verbose) { verbosity = verbose; dir = ""; file = ""; }
Logger::~Logger() { logs.clear(); verbose.clear(); } //< clears the logs.
void Logger::setOutputPath(std::string path) { this->dir = path; } //< Set the Output file path.
void Logger::setOutputFile(std::string file) { this->file = file; } //< Set the Output file. 
void Logger::addLog(Level level, const std::string& log) {
    // Only add and print the log if the level of verbosity is equal to or higher than the verbosity level set for the logger.
    if (level >= verbosity) {
        // Add the log and its verbosity level to the internal vectors.
        this->verbose.push_back(level);
        this->logs.push_back(log);
        // Print the log to the specified output stream.
        printToStream(std::cerr, levelToString(level) + ": " + log);
    }
}
void Logger::appendLogs(Logger& other) {
    //append the logs
    this->logs.insert(this->logs.end(), other.logs.begin(), other.logs.end());

    //append the verbosity 
    this->verbose.insert(this->verbose.end(), other.verbose.begin(), other.verbose.end());
}


void Logger::printLogs(std::ostream& stream) {
    // Iterate through the logs and print them to the specified output stream.
    for (const auto& log : logs) {
        printToStream(stream, log);
    }
}

std::string Logger::toString() {
    std::stringstream stream;
    printLogs(stream);
    return stream.str();
}

void Logger::clearLogs() {
    // Clear the internal vectors that store the logs and their verbosity levels.
    this->logs.clear();
}
bool Logger::SaveLogs() {
    // Open the file for output, truncating the contents if it already exists.
    std::ofstream out_file;

    //convert the string executable name to a Wide Char String  
    std::wstring file_l(file.c_str(), file.c_str() + file.size());

    //convert the string directory name to a Wide Char String  
    std::wstring dir_l(dir.c_str(), dir.c_str() + dir.size());

    //get the current directory
    wchar_t path_exe[MAX_PATH];
    GetModuleFileName(NULL, path_exe, MAX_PATH);
    std::wstring path = path_exe;
    path = std::filesystem::path(path).parent_path();

    std::wstring name = dir != "" ? path + L"\\" + dir_l + L"\\" + file_l : dir_l + L"\\" + file_l;
    out_file.open(name, std::ofstream::out | std::ofstream::trunc);

    // If the file could not be opened, log a warning and return false.
    if (out_file.fail()) {
        std::string logString = "Error opening log file: " + file + ". Log will not be saved.";
        this->addLog(Level::WARN, logString);
        return false;
    }

    // Write the logs to the file.
    printLogs(out_file);    

    // Close the file.
    out_file.close();

    // Return true to indicate that the logs were successfully saved.
    return true;
}
Level Logger::getVerbosity() const { return verbosity; }    //< returns the verbosity
std::string_view Logger::getPath() const { return dir; }   //< returns the log file path
std::string_view Logger::getFile() const { return file; }   //< returns the log file name

void Logger::printToStream(std::ostream& stream, std::string str) { stream << str << std::endl; } //< prints the string
std::string Logger::levelToString(Level level) {
    std::string levelString = "Verbose";
    switch (level) {
    case Level::INFO:
        levelString = "Info";
        break;
    case Level::WARN:
        levelString = "Warning";
        break;
    case Level::ERR:
        levelString = "Error";
        break;
    case Level::FATAL:
        levelString = "Fatal";
    }
    return levelString;
}
