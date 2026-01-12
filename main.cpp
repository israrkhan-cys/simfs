#include "include/file_system.h"
#include "include/utils.h"
#include "include/colors.h"
#include "include/weather.h"
#include "include/crypto.h"

#include <iostream>
using namespace std;

int main() {
    FileSystem fs;
    fs.CreateSystem();
    
    //APIs 
    Weather weatherAPI("API_KEY_HERE");
    CryptoAPI cryptoAPI;


    string inputLine;
    cout<<"======================================="<<endl;
    cout << "=== "<<Colors::BLUE<<" Modular Linux FS Simulator"<<Colors::RESET<<"  ======" << endl;
    cout<<"======================================="<<endl;
     
    while (true) {
        cout << Colors::GREEN << "israr@cpp" << Colors::RESET << ":" 
        << Colors::BLUE << "~" << fs.getPromptPath() << Colors::RESET << "$ ";
        
        if (!getline(cin, inputLine)) break;
        if (inputLine.empty()) continue;
        
        vector<string> args = parseInput(inputLine);  // get input at terminal 
        string cmd = args[0];  

        if (cmd == "exit") break;
        else if (cmd == "help" || cmd == "--help" || cmd == "-h") {
            if (args.size() > 1) { showCommandHelp(args[1]);
                }else {
                    showHelp();}
        }
        else if (cmd == "ls") fs.ls();
        else if (cmd == "mkdir" && args.size() > 1) fs.mkdir(args[1]);
        else if (cmd == "touch" && args.size() > 1) fs.touch(args[1]);
        else if (cmd == "cd" && args.size() > 1) fs.cd(args[1]);
        else if (cmd == "rm" && args.size() > 1) fs.rm(args[1]);
        else if (cmd == "cat" && args.size() > 1) fs.cat(args[1]);
        else if (cmd == "mv" && args.size() > 2) fs.mv(args[1], args[2]);
        else if (cmd == "cp" && args.size() > 2) fs.cp(args[1], args[2]); 
        else if (cmd == "echo" && args.size() >= 3 && args[args.size()-2] == ">") {   fs.writeToFile(args.back(), args[1]);  }
        else if(cmd == "pwd"){ fs.pwd();}
        else if (cmd == "sysinfo" || cmd == "neofetch"){ sysinfo();} 
        else if (cmd == "clear") clearTerminal();
        else if (cmd == "date" || cmd== "time") timedate(cmd);
        else if (cmd == "whoami") { cout<<"israr\n";}
        else if (cmd == "weather" && args.size() > 1) {
                std::string city = args[1];
                 for (size_t i = 2; i < args.size(); i++) {
                      city += " " + args[i];
                    }
                     cout << weatherAPI.getWeather(city) << endl;
               }
        else if (cmd == "weather") {
       cout << weatherAPI.getWeather("Islamabad") << endl;  // Default city
        }
       else if (cmd == "crypto") {
       if (args.size() == 2) {
        if (args[1] == "market") {
            cout << cryptoAPI.getMarketOverview() << endl;
        }
        else if (args[1] == "top") {
            cout << cryptoAPI.getPrettyTop(5) << endl;
        }
        else {
            cout << cryptoAPI.getPrettyCrypto(args[1]) << endl;
        }
    }
    else if (args.size() == 3 && args[1] == "top") {
        try {
            int limit = std::stoi(args[2]);
            cout << cryptoAPI.getPrettyTop(limit) << endl;
        } catch (...) {
            cout << "❌ Invalid number. Usage: crypto top [number]\n";
        }
    }
    else if (args.size() >= 5 && args[1] == "convert") {
        try {
            double amount = std::stod(args[2]);
            std::string from = args[3];
            std::string to = args[5];  // args[4] should be "to"
            
            cout << cryptoAPI.convertCrypto(from, to, amount) << endl;
        } catch (...) {
            cout << "❌ Usage: crypto convert <amount> <from> to <to>\n";
            cout << "Example: crypto convert 1 bitcoin to ethereum\n";
        }
    }
    else if (args.size() >= 3 && args[1] == "search") {
        std::string query;
        for (size_t i = 2; i < args.size(); i++) {
            if (i > 2) query += " ";
            query += args[i];
        }
        // Simple search - just try to get the crypto
        cout << cryptoAPI.getPrettyCrypto(query) << endl;
    }
    else {
        // Try as single crypto with spaces in name
        std::string coin;
        for (size_t i = 1; i < args.size(); i++) {
            if (i > 1) coin += " ";
            coin += args[i];
        }
        cout << cryptoAPI.getPrettyCrypto(coin) << endl;
        }
    }
    
    else if (cmd == "find") {
    if (args.size() < 2) {  cout << "find: missing pattern\nTry: find *.cpp  or  find -type f\n"; continue; }
    string pattern = args.back();
    bool files = true, dirs = true;

    // Quick check for -type option
    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "-type" && i + 1 < args.size()) {
            if (args[i+1] == "f") dirs = false;
            if (args[i+1] == "d") files = false;
        }
    }
    auto results = fs.find(pattern, files, dirs);
    if (results.empty()) {
        cout << "No matches found.\n";
    } else {
        cout << "🔍 Found " << results.size() << " item(s):\n";
        for (const auto& path : results) {
            if (path.back() == '/') {
                cout << Colors::BLUE << "📁 " << path << Colors::RESET << endl;
            } else {
                cout << Colors::WHITE << "📄 " << path << Colors::RESET << endl;
              }
          }
      }
   }
      
     else cout << Colors::RED << "ERROR: " << Colors::RESET<<  "Command not recognized." << endl;
    }   

    return 0;
}
