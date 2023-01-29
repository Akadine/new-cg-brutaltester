
#include "CommandCLI.h"

namespace CommandCLI {
    Options::Options() { }
    Options::~Options() { options.clear(); order.clear(); has_options.clear(); }
    void Options::Add(std::string key, bool hasOpt, std::string value) {
        this->options[key] = value;
        this->has_options[key] = hasOpt;
        this->order.push_back(key);
    }
    void Options::PrintHelp(std::string name) {
        std::cout << name << std::endl;
        for (std::string key : this->order) {
            std::string value = options[key];
            std::cout << "\t" << key << "\t" << value << std::endl;
        }
    }
    bool Options::hasOptions(std::string key) {
        return has_options[key];
    }

    //Default Parser
    DefaultParser::DefaultParser(int argc, char* argv[], Options& opt) : cmd_options() {
        parseOptions(argc, argv, opt);
    }
    DefaultParser::~DefaultParser() { cmd_options.clear(); }
    bool DefaultParser::hasOption(std::string key) {
        return cmd_options.count(key);
    }
    std::string DefaultParser::getOptionValue(std::string key) {
        return cmd_options[key];
    }
    void DefaultParser::parseOptions(int argc, char* argv[], Options& opt) {
        // Parse the options
        for (int i = 1; i < argc; i++) {
            std::string cliOpt = argv[i];
            std::string value = "";
            if (opt.hasOptions(cliOpt)) {
                value = argv[++i];
            }
            // Store the option in the map
            cmd_options[cliOpt] = value;
        }
    }
};