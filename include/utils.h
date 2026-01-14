// utils.h - Simple utility functions
#ifndef UTILS_H
#define UTILS_H

#include "file_system.h"
#include "weather.h"
#include "crypto.h"
#include <string>
#include <vector>
using namespace std;


string getCurrentTime();
vector<string> split(const string& str, char delimiter);
vector<string> parseInput(const string& input);
void clearTerminal();

void timedate(string cmd);
void sysinfo();
void showCommandHelp(const string& cmd);
void showHelp();
void showBanner();

// ADD THESE NEW FUNCTIONS:
namespace CommandUtils {
    
    // Main command executor
    bool executeCommand(FileSystem& fs, Weather& weather, CryptoAPI& crypto, 
                       const std::vector<std::string>& args);
    
    // Individual command handlers (optional but cleaner)
    void handleFileCommands(FileSystem& fs, const std::vector<std::string>& args);
    void handleWeatherCommand(Weather& weather, const std::vector<std::string>& args);
    void handleCryptoCommand(CryptoAPI& crypto, const std::vector<std::string>& args);
    void handleSystemCommands(const std::vector<std::string>& args);
    
    // Helpers
    void printPrompt(const std::string& path);
    
} // namespace CommandUtils


#endif