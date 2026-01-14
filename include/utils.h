// utils.h - Simple utility functions
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
using namespace std;

// Function declarations only
string getCurrentTime();
vector<string> split(const string& str, char delimiter);
vector<string> parseInput(const string& input);

//fxns which i want to declare outside the filesystem class
void clearTerminal();

void timedate(string cmd);
void sysinfo();
void showCommandHelp(const string& cmd);
void showHelp();
void showBanner();



#endif