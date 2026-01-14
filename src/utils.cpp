// utils.cpp - Implement helper functions
#include "../include/utils.h"
#include "../include/colors.h"
#include "../include/file_system.h"
#include "../include/weather.h"
#include "../include/crypto.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

string getCurrentTime() {
    auto t =time(nullptr);
    auto tm=*localtime(&t);
    ostringstream oss;
    oss << put_time(&tm, "%b %d %H:%M");
    return oss.str();
}

vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

vector<string> parseInput(const string& input) {
    vector<string> args;
    string current;
    bool inQuotes = false;
    for (char c : input) {
        if (c == '\"') inQuotes = !inQuotes;
        else if (c == ' ' && !inQuotes) {
            if (!current.empty()) { args.push_back(current); current.clear(); }
        } else current += c;
    }
    if (!current.empty()) args.push_back(current);
    return args;
}

void clearTerminal() {
    system("clear"); 
}


void timedate(string cmd){
     time_t now = time(nullptr);
    tm* local = localtime(&now);
    
    char buffer[100];
    if (cmd == "date") {
        strftime(buffer, sizeof(buffer), "%A, %B %d, %Y", local);
        cout << "📅 " << buffer << endl;
    } else {
        strftime(buffer, sizeof(buffer), "%I:%M:%S %p", local);
        cout << "🕐 " << buffer << endl;
    }
}

// to remove the mess from main Fxn i made this it handles all the cammands and the neccesay execution 
namespace CommandUtils {

bool executeCommand(FileSystem& fs, Weather& weather, CryptoAPI& crypto, 
                   const vector<std::string>& args) {
    if (args.empty()) return true;
    
   string cmd = args[0];
    
    // Exit command
    if (cmd == "exit" || cmd == "quit") {
        return false;
    }
    
   
    else if (cmd == "ls") fs.ls();
    else if (cmd == "mkdir" && args.size() > 1) fs.mkdir(args[1]);   // for making directory 
    else if (cmd == "touch" && args.size() > 1) fs.touch(args[1]);   // touch is used make a file 
    else if (cmd == "cd" && args.size() > 1) fs.cd(args[1]);          // change directory cammand
    else if (cmd == "rm" && args.size() > 1) fs.rm(args[1]);          // remove either a file or a directory 
    else if (cmd == "cat" && args.size() > 1) fs.cat(args[1]);        //used to write into a file like *.cpp , *.txt and stuff 
    else if (cmd == "mv" && args.size() > 2) fs.mv(args[1], args[2]); // mv used to move a directory form one place to another 
    else if (cmd == "cp" && args.size() > 2) fs.cp(args[1], args[2]);  //for copying from one place to another 
    else if (cmd == "pwd") fs.pwd();                                 // check which directory you are in 
    
    
    else if (cmd == "find") {    // find  cammand let you  check for files and directory in system 
        if (args.size() < 2) {
            cout << "find: missing pattern\nTry: find *.cpp  or  find -type f\n";
            return true;
        }
        
       string pattern = args.back();
        bool files = true, dirs = true;
        
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
    
    // Echo command
    else if (cmd == "echo" && args.size() >= 3 && args[args.size()-2] == ">") {
        fs.writeToFile(args.back(), args[1]);
    }
    
    // Weather Commands
    else if (cmd == "weather") {
        if (args.size() > 1) {
           string city = args[1];
            for (size_t i = 2; i < args.size(); i++) {
                city += " " + args[i];
            }
            cout << weather.getWeather(city) << endl;
        } else {
            cout << weather.getWeather("Islamabad") << endl;
        }
    }
    
    // Crypto Commands
    else if (cmd == "crypto") {
        handleCryptoCommand(crypto, args);
    }
    
    // System Commands
    else if (cmd == "clear") clearTerminal();
    else if (cmd == "date" || cmd == "time") timedate(cmd);
    else if (cmd == "sysinfo" || cmd == "neofetch") sysinfo();
    else if (cmd == "whoami") cout << "israr\n";
    
    // Help Command
    else if (cmd == "help" || cmd == "--help" || cmd == "-h") {
        if (args.size() > 1) {
            showCommandHelp(args[1]);
        } else {
            showHelp();
        }
    }
    
    // Unknown command
    else {
        cout << Colors::RED << "ERROR: " << Colors::RESET 
             << "Command not recognized." << endl;
    }
    
    return true;
}

void handleCryptoCommand(CryptoAPI& crypto, const vector<string>& args) {
    if (args.size() == 2) {
        if (args[1] == "market") {
            cout << crypto.getMarketOverview() << endl;
        }
        else if (args[1] == "top") {
            cout << crypto.getPrettyTop(5) << endl;
        }
        else {
            cout << crypto.getPrettyCrypto(args[1]) << endl;
        }
    }
    else if (args.size() == 3 && args[1] == "top") {
        try {
            int limit = stoi(args[2]);
            cout << crypto.getPrettyTop(limit) << endl;
        } catch (...) {
            cout << "❌ Invalid number. Usage: crypto top [number]\n";
        }
    }
    else if (args.size() >= 5 && args[1] == "convert") {
        try {
            double amount = stod(args[2]);
           string from = args[3];
           string to = args[5];
            
            cout << crypto.convertCrypto(from, to, amount) << endl;
        } catch (...) {
            cout << "❌ Usage: crypto convert <amount> <from> to <to>\n";
            cout << "Example: crypto convert 1 bitcoin to ethereum\n";
        }
    }
    else {
       string coin;
        for (size_t i = 1; i < args.size(); i++) {
            if (i > 1) coin += " ";
            coin += args[i];
        }
        cout << crypto.getPrettyCrypto(coin) << endl;
    }
}

void printPrompt(const string& path) {
    cout << Colors::GREEN << "israr@cpp" << Colors::RESET << ":" 
         << Colors::BLUE << "~" << path << Colors::RESET << "$ ";
}

} 


void showBanner() {
    cout << "\033[1;36m" << endl;
    cout << "╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║                                                        ║" << endl;
    cout << "║          ███████╗██╗███╗   ███╗███████╗███████╗        ║" << endl;
    cout << "║          ██╔════╝██║████╗ ████║██╔════╝██╔════╝        ║" << endl;
    cout << "║          ███████╗██║██╔████╔██║█████╗  ███████╗        ║" << endl;
    cout << "║          ╚════██║██║██║╚██╔╝██║██╔══╝  ╚════██║        ║" << endl;
    cout << "║          ███████║██║██║ ╚═╝ ██║██     ╗███████║        ║" << endl;
    cout << "║          ╚══════╝╚═╝╚═╝     ╚═╝╚══════╝╚══════╝        ║" << endl;
    cout << "║                                                        ║" << endl;
    cout << "║               File System Simulator v1.0               ║" << endl;
    cout << "║              Made  with  Love  By   Israr              ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
    cout << "📁 Type 'help' for commands | 'exit' to quit" << endl;
    cout << "\033[0m" << endl;
}



void sysinfo(){
    cout << "\033[1;36m" << "╔══════════════════════════════════════╗" << "\033[0m"<<endl;
    cout << "\033[1;36m" << "║      Linux Simulator v2.0            ║" << "\033[0m" <<endl;
    cout << "\033[1;36m" << "╠══════════════════════════════════════╣" << "\033[0m" <<endl;
    cout << "\033[1;33m" << "║ OS:      Zephyr Linux Simulator      ║" << "\033[0m"<<endl;
    cout << "\033[1;33m" << "║ Kernel:  6.5.0-sim                   ║" << "\033[0m" <<endl;
    cout << "\033[1;33m" << "║ Shell:   zsh-sim                     ║" << "\033[0m" <<endl;
    cout << "\033[1;33m" << "║ Terminal: Linux Simulator            ║" << "\033[0m"<<endl;
    cout << "\033[1;33m" << "║ CPU:     Simulated CPU @ 3.0GHz      ║" << "\033[0m"<<endl;
    cout << "\033[1;33m" << "║ Memory:  512MB / 1024MB              ║" << "\033[0m"<<endl;
    cout << "\033[1;33m" << "║ Disk:    15GB / 100GB                ║" << "\033[0m"<<endl;
    cout << "\033[1;36m" << "╚══════════════════════════════════════╝" << "\033[0m"<<endl;

}


void showHelp() {
    cout << "\033[1;36m" << "=== Linux FS Simulator Help ===" << "\033[0m" << endl;
    cout << "Type 'help <command>' for detailed help\n\n";
    
    cout << "\033[1;33m" << "📁 FILE SYSTEM COMMANDS:" << "\033[0m" << endl;
    cout << "  ls                    List directory contents\n";
    cout << "  cd <path>             Change directory\n";
    cout << "  mkdir <name>          Create directory\n";
    cout << "  touch <name>          Create file\n";
    cout << "  rm <name>             Remove file/directory\n";
    cout << "  cat <file>            Display file content\n";
    cout << "  mv <src> <dest>       Move/rename file\n";
    cout << "  cp <src> <dest>       Copy file\n";
    cout << "  find <pattern>        Search files/directories\n";
    
    cout << "\n\033[1;33m" << "🌤️  WEATHER COMMANDS:" << "\033[0m" << endl;
    cout << "  weather [city]        Get weather for city\n";
    cout << "  weather               default location weather\n";
    
    cout << "\n\033[1;33m" << "💰 CRYPTO COMMANDS:" << "\033[0m" << endl;
    cout << "  crypto <coin>         Get cryptocurrency price\n";
    cout << "  crypto top [n]        Top n cryptocurrencies\n";
    cout << "  crypto market         Market overview\n";
    cout << "  crypto convert <amount> <from> to <to>\n";
    
    cout << "\n\033[1;33m" << "🛠️  SYSTEM COMMANDS:" << "\033[0m" << endl;
    cout << "  clear                 Clear screen\n";
    cout << "  date/time             Show date/time\n";
    cout << "  sysinfo               System information\n";
    cout << "  help [command]        Show help\n";
    cout << "  exit                  Exit simulator\n";
    
    cout << "\n\033[1;33m" << "🎮 FUN COMMANDS:" << "\033[0m" << endl;
    cout << "  quote                 Random programming quote\n";
    cout << "  flip                  Coin toss\n";
    cout << "  random [min] [max]    Random number\n";
    
    cout << "\n\033[1;36m" << "Examples:" << "\033[0m" << endl;
    cout << "  mkdir projects        # Create directory\n";
    cout << "  weather Delhi         # Get weather\n";
    cout << "  crypto bitcoin        # Bitcoin price\n";
    cout << "  find *.cpp            # Find C++ files\n";
}

void showCommandHelp(const string& cmd) {
    cout << "\033[1;36m" << "Help: " << cmd << "\033[0m" << endl;
    cout << "────────────────────────\n";
    
    if (cmd == "ls") {
        cout << "List directory contents\n";
        cout << "Usage: ls\n";
        cout << "Shows files and directories in current location\n";
    }
    else if (cmd == "cd") {
        cout << "Change directory\n";
        cout << "Usage: cd <path>\n";
        cout << "Examples:\n";
        cout << "  cd ..               Go up one directory\n";
        cout << "  cd /home/user       Absolute path\n";
        cout << "  cd docs             Relative path\n";
    }
    else if (cmd == "mkdir") {
        cout << "Create directory\n";
        cout << "Usage: mkdir <name>\n";
        cout << "Example: mkdir projects\n";
    }
    else if (cmd == "touch") {
        cout << "Create file\n";
        cout << "Usage: touch <filename>\n";
        cout << "Example: touch main.cpp\n";
    }
    else if (cmd == "rm") {
        cout << "Remove file or directory\n";
        cout << "Usage: rm <name>\n";
        cout << "Example: rm oldfile.txt\n";
    }
    else if (cmd == "cat") {
        cout << "Display file contents\n";
        cout << "Usage: cat <filename>\n";
        cout << "Example: cat README.md\n";
    }
    else if (cmd == "mv") {
        cout << "Move or rename files\n";
        cout << "Usage: mv <source> <destination>\n";
        cout << "Examples:\n";
        cout << "  mv old.txt new.txt  # Rename\n";
        cout << "  mv file.txt docs/   # Move to directory\n";
    }
    else if (cmd == "cp") {
        cout << "Copy files\n";
        cout << "Usage: cp <source> <destination>\n";
        cout << "Example: cp file.txt backup.txt\n";
    }
    else if (cmd == "find") {
        cout << "Search for files and directories\n";
        cout << "Usage: find <pattern>\n";
        cout << "       find -type f|d <pattern>\n";
        cout << "Examples:\n";
        cout << "  find *.cpp           # All C++ files\n";
        cout << "  find -type f *.txt   # Text files only\n";
        cout << "  find -type d src     # Directories named src\n";
        cout << "\nPatterns support * wildcard\n";
    }
    else if (cmd == "weather") {
        cout << "Get weather information\n";
        cout << "Usage: weather [city]\n";
        cout << "Examples:\n";
        cout << "  weather              # Auto-detect location\n";
        cout << "  weather Delhi        # Specific city\n";
        cout << "  weather \"New York\"  # Cities with spaces\n";
        cout << "\nData from OpenWeatherMap API\n";
    }
    else if (cmd == "crypto") {
        cout << "Cryptocurrency market data\n";
        cout << "Usage:\n";
        cout << "  crypto <coin>              # Price for coin\n";
        cout << "  crypto top [n]             # Top n coins\n";
        cout << "  crypto market              # Market overview\n";
        cout << "  crypto convert 1 btc to eth # Conversion\n";
        cout << "\nExamples:\n";
        cout << "  crypto bitcoin\n";
        cout << "  crypto top 10\n";
        cout << "  crypto convert 0.5 eth to btc\n";
        cout << "\nCommon coins: bitcoin, ethereum, solana, cardano, dogecoin\n";
        cout << "Data from CoinGecko API\n";
    }
    else if (cmd == "clear") {
        cout << "Clear terminal screen\n";
        cout << "Usage: clear\n";
    }
    else if (cmd == "exit") {
        cout << "Exit the simulator\n";
        cout << "Usage: exit\n";
    }
    else {
        cout << "No detailed help available for: " << cmd << endl;
        cout << "Type 'help' to see all commands\n";
    }
    cout << "────────────────────────" << endl;
}



