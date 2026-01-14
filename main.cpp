#include "include/colors.h"
#include "include/file_system.h"
#include "include/utils.h"
#include "include/weather.h"
#include "include/crypto.h"
#include <iostream>

using namespace std;

int main() {
    //Initialize systems and making the neccesay files for proper simulation e.g Home , downloads , pictures 
    FileSystem fs;
    fs.CreateSystem();
    
    Weather weatherAPI("Weather_API_key_here");   // got the api from OpenweatherMap.org if u wanna test the project jsut grab an API from the site it free 
    CryptoAPI cryptoAPI;
    
    // Show welcome banner
    showBanner();
    
    string inputLine;
    while (true) {
        // Display prompt
        CommandUtils::printPrompt(fs.getPromptPath());
        
        // Get input
        if (!getline(cin, inputLine)) break;
        if (inputLine.empty()) continue;
        
        // Parse and execute command
        vector<string> args = parseInput(inputLine);
        bool shouldContinue = CommandUtils::executeCommand(fs, weatherAPI, cryptoAPI, args);
        
        if (!shouldContinue) break;
    }
    
    cout << "\nGoodbye!\n";
    return 0;
}