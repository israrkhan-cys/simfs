// file_system.h -
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "fs_node.h"
#include <string>
using  namespace std;

class FileSystem {
private:
    FSNode* root;
    FSNode* currentDir;

    // Private helper declarations
    FSNode* findChild(FSNode* parent, const string& name);
    vector<string> split(const string& str, char delimiter);
    FSNode* resolvePath(const string& path);
    string getFullPath(FSNode* node);
    // Helper: Check if name matches pattern (supports * wildcard) for any type
    bool matchesPattern(const string& name, const string& pattern);


public:
    FileSystem();
    ~FileSystem();
    
    // Public interface
    string getPromptPath();
    void CreateSystem();

    void mkdir(const string& name);
    void touch(const string& name);
    void ls();
    void cd(const string& path);
    void rm(const string& name);
    void writeToFile(const string& name, const string& text);
    void cat(const string& name);
    void mv(const string& srcName, const string& destName);
    void cp(const string& srcName, const string& destName);
    void pwd();

    vector<string> find(const string& pattern, bool searchFiles = true, bool searchDirs = true);
    vector<string> findInDirectory(FSNode* dir, const string& pattern, const string& currentPath, bool searchFiles, bool searchDirs);

    
};

#endif