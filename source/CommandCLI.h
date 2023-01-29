#ifndef COMMANDCLI_H
#define COMMANDCLI_H

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

namespace CommandCLI {    
    /**
     * @brief Class representing command line options
     */
    class Options {
    private:
        std::unordered_map<std::string, std::string> options; //< map of option keys to option values
        std::unordered_map<std::string, bool> has_options;    //< map of option keys to whether the option has a value or not
        std::vector<std::string> order;                       //< vector of option keys in the order they were added
    public:

        /**
         * @brief Constructs an empty Options object
         */
        Options();

        /**
         * @brief Destructs the Options object and clears the options, has_options, and order member variables
         */
        ~Options();

        /**
         * @brief Adds an option to the Options object
         *
         * @param key      the key for the option
         * @param hasOpt   whether the option has a value or not
         * @param value    the value for the option (if hasOpt is true)
         */
        void Add(std::string key, bool hasOpt, std::string value);

        /**
         * @brief Prints the help message for the options
         *
         * @param name     the name of the program
         */
        void PrintHelp(std::string name);

        /**
         * @brief Returns whether the specified option has a value or not
         *
         * @param key      the key for the option
         * @return true if the option has a value, false otherwise
         */
        bool hasOptions(std::string key);
    };

    /**
     * @brief Class for parsing command line options using the Options class
     */
    class DefaultParser {
    private:
        std::unordered_map<std::string, std::string> cmd_options; //< map of option keys to option values
        /**
         * @brief Parses the command line options and stores them in the cmd_options map
         *
         * @param argc     the number of command line arguments
         * @param argv     the array of command line arguments
         * @param opt      the Options object to use for parsing
         */
        void parseOptions(int argc, char* argv[], Options& opt);
    public:
        /**
         * @brief Constructs a DefaultParser object and parses the command line options
         *
         * @param argc     the number of command line arguments
         * @param argv     the array of command line arguments
         * @param opt      the Options object to use for parsing
         */
        DefaultParser(int argc, char* argv[], Options& opt);

        /**
         * @brief Destructs the DefaultParser object and clears the cmdOptions member variable
         */
        ~DefaultParser();

        /**
         * @brief Returns whether the specified option was found in the command line arguments
         *
         * @param key      the key for the option
         * @return true if the option was found, false otherwise
         */
        bool hasOption(std::string key);

        /**
         * @brief Returns the value for the specified option, if it was found in the command line arguments
         *
         * @param key      the key for the option
         * @return the value for the option, or an empty string if the option was not found
         */
        std::string getOptionValue(std::string key);
    };

};

#endif